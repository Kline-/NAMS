/***************************************************************************
 * NAMS - Not Another MUD Server                                           *
 * Copyright (C) 2012 Matthew Goff (matt@goff.cc) <http://www.ackmud.net/> *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 ***************************************************************************/
/**
 * @file account.cpp
 * @brief All non-template member functions of the Account class.
 *
 * Security structure to maintain characters, roles, and permissions.
 */
#include "h/includes.h"
#include "h/class.h"

#include "h/account.h"
#include "h/socketclient.h"

/* Core */
/**
 * @brief Unload an account from memory that was previously loaded via Account::New().
 * @retval void
 */
const void Account::Delete()
{
    delete this;

    return;
}

/**
 * @brief Create a new account.
 * @param[in] client The SocketClient requesting an account.
 * @param[in] exists True if the account should be loaded, false if a new one need be created.
 * @retval false Returned if a new Account was successfully created or loaded.
 * @retval true Returned if a new Account was unable to be created.
 */
const bool Account::New( SocketClient* client, const bool& exists )
{
    UFLAGS_DE( flags );

    if ( client == NULL )
    {
        LOGSTR( flags, "Account::New()-> called with NULL client" );
        return false;
    }

    if ( client->gLogin( SOC_LOGIN_NAME ).empty() )
    {
        LOGSTR( flags, "Account::New()->SocketClient::gLogin()-> called with empty SOC_LOGIN_NAME" );
        return false;
    }

    if ( client->gLogin( SOC_LOGIN_PASSWORD ).empty() )
    {
        LOGSTR( flags, "Account::New()->SocketClient::gLogin()-> called with empty SOC_LOGIN_PASSWORD" );
        return false;
    }

    m_client = client;

    if ( exists )
    {
        if ( !Unserialize() )
        {
            LOGSTR( flags, "Account::new()->Account::Unserialize()-> returned false" );
            return false;
        }
    }
    else
    {
        m_name = client->gLogin( SOC_LOGIN_NAME );
        m_password = client->gLogin( SOC_LOGIN_PASSWORD );

        if ( ::mkdir( CSTR( Utils::DirPath( CFG_DAT_DIR_ACCOUNT, m_name ) ), CFG_SEC_DIR_MODE ) < 0 )
        {
            LOGERRNO( flags, "Account::New()->mkdir()->" );
            return false;
        }

        if ( !Serialize() )
        {
            LOGSTR( flags, "Account::New()->Account::Serialize()-> returned false" );

            if ( ::rmdir( CSTR( Utils::DirPath( CFG_DAT_DIR_ACCOUNT, m_name ) ) ) < 0 )
                LOGERRNO( flags, "Account::New()->rmdir->" );

            return false;
        }
    }

    // Use the setter as it provides sanity checking and config constraints
    aHost( Utils::StrTime(), m_client->gHostname() );

    if ( !client->sAccount( this ) )
    {
        LOGSTR( flags, "Account::New()->SocketClient::sAccount()-> returned false" );
        return false;
    }

    return true;
}

/**
 * @brief Serialize the account data and write them to #CFG_DAT_FILE_SETTINGS.
 * @retval false Returned if there was an error serializing settings.
 * @retval true Returned if settings were serialized successfully.
 */
const bool Account::Serialize() const
{
    UFLAGS_DE( flags );
    ofstream ofs;
    string value;
    stringstream line;
    string file( Utils::FileExt( m_name, CFG_DAT_FILE_ACT_EXT ) );
    list<pair<string,string>>::const_iterator li;

    Utils::FileOpen( ofs, file );

    if ( !ofs.good() )
    {
        LOGFMT( flags, "Account::Serialize()-> failed to open settings file: %s", CSTR( file ) );
        return false;
    }

    KEYLIST( ofs, "host" );
    {
        if ( !m_host.empty() )
        {
            for ( li = m_host.begin(); li != m_host.end(); li++ )
                line << Utils::MakePair( li->first, li->second ) << " ";

            value = line.str();
            value.erase( value.end() - 1 );
            ofs << value << endl;
        }
        else
            ofs << endl;
    }
    KEY( ofs, "name", m_name );
    KEY( ofs, "password", m_password );

    Utils::FileClose( ofs, Utils::DirPath( CFG_DAT_DIR_ACCOUNT, m_name ), CSTR( file ) );

    return true;
}

/**
 * @brief Unserialize the account data from #CFG_DAT_FILE_SETTINGS.
 * @retval false Returned if there was an error unserializing settings.
 * @retval true Returned if settings were unserialized successfully.
 */
const bool Account::Unserialize()
{
    UFLAGS_DE( flags );
    ifstream ifs;
    string key, value, line, token;
    bool found = false;
    pair<string,string> item;
    string file( Utils::FileExt( m_client->gLogin( SOC_LOGIN_NAME ), CFG_DAT_FILE_ACT_EXT ) );

    Utils::FileOpen( ifs, Utils::DirPath( CFG_DAT_DIR_ACCOUNT, m_client->gLogin( SOC_LOGIN_NAME ) ), file );

    if ( !ifs.good() )
    {
        LOGFMT( flags, "Account::Unserialize()-> failed to open settings file: %s", CSTR( file ) );
        return false;
    }

    while ( getline( ifs, line ) )
    {
        if ( !Utils::KeyValue( key, value, line) )
        {
            LOGFMT( flags, "Account::Unserialize()-> error reading line: %s", CSTR( line ) );
            continue;
        }

        for ( ;; )
        {
            found = false;

            if ( key == "host" )
            {
                found = true;
                while ( !value.empty() )
                {
                    token = Utils::Argument( value, "} " );
                    item = Utils::ReadPair( token );
                    m_host.push_back( pair<string,string>( item.first, item.second ) );
                }
            }
            Utils::KeySet( true, found, key, "Name", value, m_name );
            Utils::KeySet( true, found, key, "Password", value, m_password );

            if ( !found )
                LOGFMT( flags, "Account::Unserialize()->Utils::KeySet()-> key not found: %s", CSTR( key ) );

            break;
        }
    }

    Utils::FileClose( ifs );

    if ( m_password == gClient()->gLogin( SOC_LOGIN_PASSWORD ) )
        return true;
    else
    {
        gClient()->Send( CFG_STR_ACT_PASSWORD_INVALID );
        // Ensure the output goes before the client gets dropped
        gClient()->Send();

        return false;
    }
}

/* Query */
/**
 * @brief Returns the associated SocketClient.
 * @retval SocketClient* A pointer to the associated SocketClient.
 */
SocketClient* Account::gClient() const
{
    return m_client;
}

/**
 * @brief Returns the name of the account.
 * @retval string A string with the name of the account.
 */
const string Account::gName() const
{
    return m_name;
}

/* Manipulate */
/**
 * @brief Adds a hostname to the list of previous hosts. Bumps the oldest entry.
 * @param[in] date A string of the login time.
 * @param[in] name The name to prepend to the list.
 * @retval false Returned if there is an error adding the new entry.
 * @retval true Returned if the new entry was successfully added.
 */
const bool Account::aHost( const string& date, const string& name )
{
    UFLAGS_DE( flags );

    if ( date.empty() )
    {
        LOGSTR( flags, "Account::aHost()-> called with empty date" );
        return false;
    }

    if ( name.empty() )
    {
        LOGSTR( flags, "Account::aHost()-> called with empty name" );
        return false;
    }

    m_host.push_front( pair<string,string>( date, name ) );

    if ( m_host.size() > CFG_ACT_HOST_MAX )
        m_host.pop_back();

    return true;
}

/* Internal */
/**
 * @brief Constructor for the Account class.
 */
Account::Account()
{
    m_client = NULL;
    m_host.clear();
    m_name.clear();
    m_password.clear();

    return;
}

/**
 * @brief Destructor for the Account class.
 */
Account::~Account()
{
    return;
}