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
 * @file handler.cpp
 * @brief All non-template member functions of the Handler namespace.
 *
 * The Handler namespace contains all non-class process control functions.
 * To pointedly avoid large complex functions such as "nanny()" from Diku
 * heritage, process controls should be implemented as individual functions
 * within this namespace.
 */
#include "h/includes.h"
#include "h/class.h"

#include "h/handler.h"
#include "h/account.h"
#include "h/command.h"
#include "h/server.h"

/* Core */
/**
 * @brief Handle SocketClient objects who have not fully logged into the game yet.
 * @param[in] client The SocketClient to process a login request for.
 * @param[in] cmd The command sent by the SocketClient.
 * @param[in] args Any arguments to the command.
 * @retval void
 */
const void Handler::ProcessLogin( SocketClient* client, const string& cmd, const string& args )
{
    UFLAGS_DE( flags );

    if ( client == NULL )
    {
        LOGSTR( flags, "Handler::ProcessLogin()-> called with NULL client" );
        return;
    }

    cout << "state={" << client->gState() << "} cmd={" << cmd << "} args={" << args << "}" << endl;
    switch ( client->gState() )
    {
        case SOC_STATE_LOGIN_SCREEN:
            LoginScreen( client, cmd, args );
        break;

        case SOC_STATE_GET_OLD_PASSWORD:
            GetOldPassword( client, cmd, args );
        break;

        case SOC_STATE_GET_NEW_ACCOUNT:
            //Pass the requested account name as args so it can be re-used for output messages
            GetNewAccount( client, cmd, client->gLogin( SOC_LOGIN_ARG ) );
        break;

        case SOC_STATE_GET_NEW_PASSWORD:
            GetNewPassword( client, cmd, client->gLogin( SOC_LOGIN_PWD ) );
        break;

        case SOC_STATE_CREATE_ACCOUNT:
            CreateAccount( client, cmd, args );
        break;

        default:
        break;
    }

    return;
}

/* Query */

/* Manipulate */

/* Internal */
/**
 * @brief Create a new account object with a name and password.
 * @param[in] client The SocketClient to process a login request for.
 * @param[in] cmd The command sent by the SocketClient.
 * @param[in] args Any arguments to the command.
 * @retval void
 */
const void Handler::CreateAccount( SocketClient* client, const string& cmd, const string& args )
{
    client->Send( "And we danced!" CRLF );
    return;
}

/**
 * @brief Select a new account name.
 * @param[in] client The SocketClient to process a login request for.
 * @param[in] cmd The command sent by the SocketClient.
 * @param[in] args Any arguments to the command.
 * @retval void
 */
const void Handler::GetNewAccount( SocketClient* client, const string& cmd, const string& args )
{
    UFLAGS_DE( flags );

    if ( client == NULL )
    {
        LOGSTR( flags, "Handler::GetNewAccount()-> called with NULL client" );
        return;
    }

    //Initial entry or an invalid selection
    if ( cmd.empty() && !args.empty() )
    {
        client->Send( Utils::FormatString( 0, CFG_STR_ACT_CONFIRM_NAME, CSTR( args ) ) );
        return;
    }

    if ( Utils::StrPrefix( cmd, "yes", true ) )
        client->sState( SOC_STATE_GET_NEW_PASSWORD );
    else if ( Utils::StrPrefix( cmd, "no", true ) )
        client->sState( SOC_STATE_LOGIN_SCREEN );
    else
        client->Send( CFG_STR_SEL_INVALID );

    //Return to the main handler
    ProcessLogin( client );

    return;
}

/**
 * @brief Create a new account.
 * @param[in] client The SocketClient to process a login request for.
 * @param[in] cmd The command sent by the SocketClient.
 * @param[in] args Any arguments to the command.
 * @retval void
 */
const void Handler::GetNewPassword( SocketClient* client, const string& cmd, const string& args )
{
    UFLAGS_DE( flags );

    if ( client == NULL )
    {
        LOGSTR( flags, "Handler::GetNewPassword()-> called with NULL client" );
        return;
    }

    //Initial entry
    if ( cmd.empty() && args.empty() )
    {
        client->Send( CFG_STR_ACT_NEW );
        client->Send( CFG_STR_ACT_GET_PASSWORD );
        return;
    }

    if ( cmd.length() < CFG_ACT_MIN_PASSWORD_LEN || cmd.length() > CFG_ACT_MAX_PASSWORD_LEN )
    {
        client->Send( CFG_STR_ACT_INVALID_PASSWORD );
        client->Send( CFG_STR_ACT_LENGTH_PASSWORD );
        client->Send( CFG_STR_ACT_GET_PASSWORD );
        return;
    }

    if ( client->gLogin( SOC_LOGIN_PWD ).empty() )
    {
        client->sLogin( SOC_LOGIN_PWD, cmd );
        client->Send( CFG_STR_ACT_CONFIRM_PASSWORD );
        return;
    }

    if ( cmd.compare( client->gLogin( SOC_LOGIN_PWD ) ) == 0 )
        client->sState( SOC_STATE_CREATE_ACCOUNT );
    else
    {
        client->sLogin( SOC_LOGIN_PWD, "" );
        client->Send( CFG_STR_ACT_PASSWORD_MISMATCH );
        client->Send( CFG_STR_ACT_GET_PASSWORD );
        return;
    }

    //Return to the main handler
    ProcessLogin( client );

    return;
}

/**
 * @brief Compare a received password against an existing account.
 * @param[in] client The SocketClient to process a login request for.
 * @param[in] cmd The command sent by the SocketClient.
 * @param[in] args Any arguments to the command.
 * @retval void
 */
const void Handler::GetOldPassword( SocketClient* client, const string& cmd, const string& args )
{
    UFLAGS_DE( flags );

    if ( client == NULL )
    {
        LOGSTR( flags, "Handler::GetOldPassword()-> called with NULL client" );
        return;
    }

    //Initial hit prompting for password
    if ( cmd.empty() )
    {
        client->Send( CFG_STR_ACT_GET_PASSWORD );
        return;
    }

    //Return to the main handler
    ProcessLogin( client );

    return;
}

/**
 * @brief Send initial greeting and request for account name.
 * @param[in] client The SocketClient to process a login request for.
 * @param[in] cmd The command sent by the SocketClient.
 * @param[in] args Any arguments to the command.
 * @retval void
 */
const void Handler::LoginScreen( SocketClient* client, const string& cmd, const string& args )
{
    UFLAGS_DE( flags );
    stringstream account;

    if ( client == NULL )
    {
        LOGSTR( flags, "Handler::LoginScreen()-> called with NULL client" );
        return;
    }

    // Initial connection with no input yet received or previous name was invalid
    if ( cmd.empty() && args.empty() )
    {
        client->Send( CFG_STR_ACT_GET_NAME );
        return;
    }

    if ( cmd.length() < CFG_ACT_MIN_NAME_LEN || cmd.length() > CFG_ACT_MAX_NAME_LEN )
    {
        client->Send( CFG_STR_ACT_INVALID_NAME );
        client->Send( CFG_STR_ACT_LENGTH_NAME );
    }
    else
    {
        account << CFG_DAT_DIR_ACCOUNT << "/" << cmd;
        switch (  Utils::DirExists( account.str() ) )
        {
            case UTILS_RET_FALSE:
                client->sState( SOC_STATE_GET_NEW_ACCOUNT );
            break;

            case UTILS_RET_TRUE:
                client->sState( SOC_STATE_GET_OLD_PASSWORD );
            break;

            case UTILS_RET_ERROR:
            default:
                client->Send( CFG_STR_ACT_INVALID_NAME );
                LOGFMT( flags, "Handler::LoginScreen()->Utils::DirExists()-> returned UTILS_RET_ERROR for name: %s", CSTR( cmd ) );
            break;
        }
    }

    //Return to the main handler
    client->sLogin( SOC_LOGIN_ARG, cmd );
    ProcessLogin( client );

    return;
}
