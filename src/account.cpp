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
    return;
}

/**
 * @brief Create a new account.
 * @param[in] client The SocketClient requesting an account.
 * @retval false Returned if a new Account was successfully created or loaded.
 * @retval true Returned if a new Account was unable to be created.
 */
const bool Account::New( SocketClient* client )
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

    if ( !client->sAccount( this ) )
    {
        LOGSTR( flags, "Account::New()->SocketClient::sAccount()-> returned false" );
        return false;
    }

    return true;
}

/* Query */
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
 * @brief Serialize the account data and write them to #CFG_DAT_FILE_SETTINGS.
 * @retval false Returned if there was an error serializing settings.
 * @retval true Returned if settings were serialized successfully.
 */
const bool Account::Serialize() const
{
    UFLAGS_DE( flags );
    ofstream ofs;
    string value;
    stringstream file;
    CITER( forward_list, string, li );

    file << m_name << "." << CFG_DAT_FILE_ACT_EXT;
    Utils::FileOpen( ofs, file.str() );

    if ( !ofs.good() )
    {
        LOGFMT( flags, "Account::Serialize()-> failed to open settings file: %s", CSTR( file.str() ) );
        return false;
    }

    KEY( ofs, "name", m_name );
    KEY( ofs, "password", m_password );

    Utils::FileClose( ofs, Utils::DirPath( CFG_DAT_DIR_ACCOUNT, m_name ), CSTR( file.str() ) );

    return true;
}

/**
 * @brief Unserialize the account data from #CFG_DAT_FILE_SETTINGS.
 * @retval false Returned if there was an error unserializing settings.
 * @retval true Returned if settings were unserialized successfully.
 */
const bool Account::Unserialize()
{/*
    UFLAGS_DE( flags );
    ifstream ifs;
    string key, value, line;
    vector<string> token;
    ITER( vector, string, ti );

    LOGSTR( 0, CFG_STR_FILE_SETTINGS_READ );
    Utils::FileOpen( ifs, CFG_DAT_DIR_ETC, CFG_DAT_FILE_SETTINGS );

    if ( !ifs.good() )
    {
        LOGFMT( flags, "Server::Config::Unserialize()-> failed to open settings file: %s", CFG_DAT_FILE_SETTINGS );
        return false;
    }

    while ( getline( ifs, line ) )
    {
        if ( !Utils::KeyValue( key, value, line) )
        {
            LOGFMT( flags, "Server::Config::Unserialize()-> error reading line: %s", CSTR( line ) );
            continue;
        }

        if ( key.compare( "account_prohibited_names" ) == 0 )
        {
            token = Utils::StrTokens( value, true );
            for ( ti = token.begin(); ti != token.end(); ti++ )
                m_account_prohibited_names.push_front( *ti );
            m_account_prohibited_names.reverse();
        }
    }

    Utils::FileClose( ifs );
    LOGSTR( 0, CFG_STR_FILE_DONE );
*/
    return true;
}

/* Internal */
/**
 * @brief Constructor for the Account class.
 */
Account::Account()
{
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