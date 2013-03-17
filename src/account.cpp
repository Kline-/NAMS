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
    uint_t i = uintmin_t;
    string file( Utils::FileExt( m_name, CFG_DAT_FILE_ACT_EXT ) );
    CITER( forward_list, string, li );
    list<pair<string,string>>::const_iterator pi;

    Utils::FileOpen( ofs, file );

    if ( !ofs.good() )
    {
        LOGFMT( flags, "Account::Serialize()-> failed to open settings file: %s", CSTR( file ) );
        return false;
    }

    // First to ensure name is loaded for logging later
    KEY( ofs, "name", m_name );
    KEYLIST( ofs, "characters" );
    {
        if ( !m_characters.empty() )
        {
            for ( li = m_characters.begin(); li != m_characters.end(); li++ )
                line << *li << " ";

            value = line.str();
            value.erase( value.end() - 1 );
            ofs << value << endl;
        }
        else
            ofs << endl;
    }
    KEYLISTLOOP( ofs, "logins", i ); /** @todo Need to find a nicer way to do this */
    {
        for ( i = 0; i < MAX_ACT_LOGIN; i++ )
        {
            ofs << "logins[" << i << "]" << " = ";
            line.str( "" );

            if ( !m_logins[i].empty() )
            {
                for ( pi = m_logins[i].begin(); pi != m_logins[i].end(); pi++ )
                    line << Utils::MakePair( pi->first, pi->second ) << " ";

                value = line.str();
                value.erase( value.end() - 1 );
                ofs << value << endl;
            }
            else
                ofs << endl;
        }
    }
    KEY( ofs, "password", m_password );
    KEY( ofs, "security", m_security );

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
    UFLAGS_I( finfo );
    ifstream ifs;
    string key, value, line, arg;
    stringstream loop;
    bool found = false, maxb = false;
    pair<string,string> item;
    vector<string> token;
    ITER( vector, string, ti );
    uint_t i = uintmin_t;
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

            // First to ensure name is loaded for logging later
            Utils::KeySet( true, found, key, "name", value, m_name );
            if ( key == "characters" )
            {
                found = true;
                token = Utils::StrTokens( value, true );
                for ( ti = token.begin(); ti != token.end(); ti++ )
                    m_characters.push_front( *ti );
                m_characters.reverse();
            }
            if ( Utils::StrPrefix( "logins", key ) ) /** @todo Need to find a nicer way to do this */
            {
                for ( ; i < MAX_ACT_LOGIN; i++ )
                {
                    loop.str( "" );
                    loop << "logins[" << i << "]";

                    if ( key == loop.str() )
                    {
                        found = true;
                        break;
                    }
                }

                while ( !value.empty() )
                {
                    arg = Utils::Argument( value, "} " );
                    item = Utils::ReadPair( arg );
                    m_logins[i].push_back( pair<string,string>( item.first, item.second ) );
                }
            }
            Utils::KeySet( true, found, key, "password", value, m_password );
            Utils::KeySet( true, found, key, "security", value, m_security, MAX_ACT_SECURITY, maxb );

            if ( !found )
                LOGFMT( flags, "Account::Unserialize()->Utils::KeySet()-> key not found: %s", CSTR( key ) );

            if ( maxb )
                LOGFMT( finfo, "Account::Unserialize()->Utils::KeySet()-> account %s, key %s has illegal value %s", CSTR( m_name ), CSTR( key ), CSTR( value ) );

            break;
        }
    }

    Utils::FileClose( ifs );

    if ( m_password == gClient()->gLogin( SOC_LOGIN_PASSWORD ) )
    {
        // Use the setter as it provides sanity checking and config constraints
        aLogin( Utils::StrTime(), m_client->gHostname(), ACT_LOGIN_SUCCESS );
        return true;
    }
    else
    {
        // Track that there was a failure, force a save to ensure the data is retained
        aLogin( Utils::StrTime(), m_client->gHostname(), ACT_LOGIN_FAILURE );
        Serialize();
        gClient()->Send( CFG_STR_ACT_PASSWORD_INVALID );

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
 * @brief Returns a list of either login successes or failures based on #ACT_LOGIN.
 * @param[in] type A value from #ACT_LOGIN.
 * @retval list<pair<string,string>> A list of a string pair containing the login dates and times.
 */
const list<pair<string,string>> Account::gLogins( const uint_t& type ) const
{
    UFLAGS_DE( flags );

    if ( type < uintmin_t || type >= MAX_ACT_LOGIN )
    {
        LOGFMT( flags, "Account::gLogins()-> called with invalid type %lu", type );
        return list<pair<string,string>>();
    }

    return m_logins[type];
}

/**
 * @brief Returns the name of the account.
 * @retval string A string with the name of the account.
 */
const string Account::gName() const
{
    return m_name;
}

/**
 * @brief Returns the authorized security level of the account.
 * @retval uint_t The authorized security level of the account.
 */
const uint_t Account::gSecurity() const
{
    return m_security;
}

/* Manipulate */
/**
 * @brief Adds a hostname to the list of previous successful logins. Bumps the oldest entry.
 * @param[in] date A string of the login time.
 * @param[in] name The name to prepend to the list.
 * @param[in] type Value from #ACT_LOGIN.
 * @retval false Returned if there is an error adding the new entry.
 * @retval true Returned if the new entry was successfully added.
 */
const bool Account::aLogin( const string& date, const string& name, const uint_t& type )
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

    if ( type >= MAX_ACT_LOGIN )
    {
        LOGSTR( flags, "Account::aHost()-> called with invalid type" );
        return false;
    }

    while ( m_logins[type].size() >= CFG_ACT_LOGIN_MAX )
        m_logins[type].pop_back();

    m_logins[type].push_front( pair<string,string>( date, name ) );

    return true;
}

/**
 * @brief Set the security level of the account.
 * @param[in] security The level to set the security of the account to. From #ACT_SECURITY.
 * @retval false Returned if the security level is invalid.
 * @retval true Returned if the security level is valid.
 */
const bool Account::sSecurity( const uint_t& security )
{
    UFLAGS_DE( flags );

    if ( security < ACT_SECURITY_NONE || security >= MAX_ACT_SECURITY )
    {
        LOGFMT( flags, "Account::sSecurity()-> called with invalid security level: %lu ", security );
        return false;
    }

    m_security = security;

    return true;
}

/* Internal */
/**
 * @brief Constructor for the Account class.
 */
Account::Account()
{
    uint_t i = uintmin_t;

    m_characters.clear();
    m_client = NULL;
    for ( i = 0; i < MAX_ACT_LOGIN; i++ )
        m_logins[i].clear();
    m_name.clear();
    m_password.clear();
    m_security = ACT_SECURITY_NONE;

    return;
}

/**
 * @brief Destructor for the Account class.
 */
Account::~Account()
{
    return;
}
