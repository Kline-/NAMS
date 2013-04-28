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
#include "h/list.h"
#include "h/server.h"

/* Core */
/**
 * @brief Dispatches the appropriate menu based on client state.
 * @param[in] client The SocketClient to process a menu request for.
 * @param[in] cmd The command sent by the SocketClient.
 * @param[in] args Any arguments to the command.
 * @retval void
 */
const void Handler::LoginHandler( SocketClient* client, const string& cmd, const string& args )
{
    UFLAGS_DE( flags );

    if ( client == NULL )
    {
        LOGSTR( flags, "Handler::LoginHandler()-> called with NULL client" );
        return;
    }

    if ( client->gState() >= SOC_STATE_ACCOUNT_MENU && client->gAccount() == NULL )
    {
        LOGSTR( flags, "Handler::LoginHandler()-> called with NULL account" );
        return;
    }

    cout << "state=[" << client->gState() << "} cmd={" << cmd << "} args={" << args <<"}" << endl;
    switch ( client->gState() )
    {
        case SOC_STATE_LOGIN_SCREEN:                LoginScreen( client, cmd, args );               break;
        case SOC_STATE_GET_OLD_PASSWORD:            GetOldPassword( client, cmd, args );            break;
        case SOC_STATE_GET_NEW_ACCOUNT:             GetNewAccount( client, cmd, args );             break;
        case SOC_STATE_GET_NEW_PASSWORD:            GetNewPassword( client, cmd, args );            break;
        case SOC_STATE_CREATE_ACCOUNT:              AttachAccount( client, cmd, args );             break;
        case SOC_STATE_LOAD_ACCOUNT:                AttachAccount( client, cmd, args );             break;
        case SOC_STATE_ACCOUNT_MENU:                AccountMenuMain( client, cmd, args );           break;
        case SOC_STATE_CHARACTER_CREATE_MENU:       CharacterCreateMenuMain( client, cmd, args );   break;
        case SOC_STATE_CHARACTER_CREATE_NAME:       CharacterCreateName( client, cmd, args );       break;
        case SOC_STATE_CHARACTER_CREATE_SEX:        CharacterCreateSex( client, cmd, args );        break;
        case SOC_STATE_CHARACTER_CREATE_FINISH:     AttachCharacter( client, cmd, args );           break;
        case SOC_STATE_CHARACTER_DELETE_MENU:       CharacterDeleteMenuMain( client, cmd, args );   break;
        case SOC_STATE_CHARACTER_DELETE_CONFIRM:    CharacterDeleteConfirm( client, cmd, args );    break;
        case SOC_STATE_CHARACTER_LOAD_MENU:         CharacterLoadMenuMain( client, cmd, args );     break;
        case SOC_STATE_LOAD_CHARACTER:              LoadCharacter( client, cmd, args );             break;
        case SOC_STATE_ENTER_GAME:                  EnterGame( client, cmd, args );                 break;
        default:                                                                                    break;
    }

    return;
}

/* Query */
/**
 * @brief Check to see if the current account is already being created.
 * @param[in] client The SocketClient to check against active connections.
 * @param[in] name The Account name to check against active connections.
 * @retval false Returned if the account is not playing or is playing but in a state greater than or equal to #SOC_STATE_ACCOUNT_MENU.
 * @retval true Returned if the account is playing and in a state less than #SOC_STATE_ACCOUNT_MENU.
 */
const bool Handler::CheckCreating( SocketClient* client, const string& name )
{
    UFLAGS_S( flags );
    ITER( list, SocketClient*, si );
    SocketClient* socket_client;

    for ( si = socket_client_list.begin(); si != socket_client_list.end(); si = client->gServer()->gSocketClientNext() )
    {
        socket_client = *si;
        client->gServer()->sSocketClientNext( ++si );

        if ( socket_client == client )
            continue;

        if ( socket_client->gState() >= SOC_STATE_ACCOUNT_MENU )
            continue;

        if ( name == socket_client->gLogin( SOC_LOGIN_NAME ) )
        {
            LOGFMT( flags, "Handler::CheckPlaying()-> player from site %s attempted to login as %s (in creation)", CSTR( client->gHostname() ), CSTR( name ) );
            return true;
        }
    }

    return false;
}

/**
 * @brief Checks to see if a name is on the server's prohibited list.
 * @param[in] client The SocketClient to requesting to use the name.
 * @param[in] name The name to check against the prohibited list.
 * @param[in] type The type of prohibited names list to check.
 * @retval false Returned if the name is not prohibited.
 * @retval true Returned if the name is prohibited.
 */
const bool Handler::CheckProhibited( SocketClient* client, const string& name, const uint_t& type )
{
    ITER( forward_list, string, fi );
    forward_list<string> search;
    string comp;

    // Search for prohibited names
    search = client->gServer()->gConfig()->gProhibitedNames( type );
    for ( fi = search.begin(); fi != search.end(); fi++ )
    {
        comp = *fi;
        if ( comp == name )
            return true;
    }

    return false;
}

/* Manipulate */
/**
 * @brief Create a new account object, or load an existing one, with a name and password.
 * @param[in] client The SocketClient to process a login request for.
 * @param[in] cmd The command sent by the SocketClient.
 * @param[in] args Any arguments to the command.
 * @retval void
 */
const void Handler::AttachAccount( SocketClient* client, const string& cmd, const string& args )
{
    UFLAGS_DE( flags );
    Account* account = NULL;
    bool exists = false;

    if ( client == NULL )
    {
        LOGSTR( flags, "Handler::AttachAccount()-> called with NULL client" );
        return;
    }

    if ( client->gState() == SOC_STATE_CREATE_ACCOUNT )
        exists = false;
    else if ( client->gState() == SOC_STATE_LOAD_ACCOUNT )
        exists = true;
    else
    {
        LOGFMT( flags, "Handler::AttachAccount()-> called with invalid client state: %lu", client->gState() );
        return;
    }

    Telopt::Negotiate( client, SOC_TELOPT_ECHO, false );

    account = new Account();
    if ( !account->New( client, exists ) )
    {
        account->Delete();

        if ( exists )
            client->gServer()->FindCommand( "quit" )->Run( client );
        else
        {
            client->sLogin( SOC_LOGIN_NAME, "" );
            client->sLogin( SOC_LOGIN_PASSWORD, "" );
            client->sState( SOC_STATE_LOGIN_SCREEN );
            client->Send( CFG_STR_ACT_NEW_ERROR );
            LoginHandler( client );
        }

        return;
    }

    // All went well, off to the account menu
    client->sState( SOC_STATE_ACCOUNT_MENU );
    LoginHandler( client );

    return;
}

/**
 * @brief Attempt to write a new character to disk after creation was completed.
 * @param[in] client The SocketClient to process a login request for.
 * @param[in] cmd The command sent by the SocketClient.
 * @param[in] args Any arguments to the command.
 * @retval void
 */
const void Handler::AttachCharacter( SocketClient* client, const string& cmd, const string& args )
{
    UFLAGS_DE( flags );

    if ( client == NULL )
    {
        LOGSTR( flags, "Handler::AttachCharacter()-> called with NULL client" );
        return;
    }

    if ( client->gAccount() == NULL )
    {
        LOGSTR( flags, "Handler::AttachCharacter()-> called with NULL account" );
        return;
    }

    if ( client->gAccount()->gCharacter() == NULL )
    {
        LOGSTR( flags, "Handler::AttachCharacter()-> called with NULL character" );
        return;
    }

    // Save the new character then the delete the in-memory copy and reset the active account character
    client->gAccount()->aCharacter( client->gAccount()->gCharacter()->gName() );
    client->gAccount()->Serialize();
    client->gAccount()->gCharacter()->Serialize();
    client->gAccount()->gCharacter()->Delete();
    client->gAccount()->ClearCharacter();

    // All went well, off to the account menu
    client->sState( SOC_STATE_ACCOUNT_MENU );
    LoginHandler( client );

    return;
}

/* Internal */
/**
 * @brief Send initial account interface menu.
 * @param[in] client The SocketClient to process a login request for.
 * @param[in] cmd The command sent by the SocketClient.
 * @param[in] args Any arguments to the command.
 * @retval void
 */
const void Handler::AccountMenuMain( SocketClient* client, const string& cmd, const string& args )
{
    UFLAGS_DE( flags );
    uint_t val = uintmin_t;

    if ( client == NULL )
    {
        LOGSTR( flags, "Handler::MenuScreen()-> called with NULL client" );
        return;
    }

    if ( client->gAccount() == NULL )
    {
        LOGSTR( flags, "Handler::MenuScreen()-> called with NULL account" );
        return;
    }

    //Initial entry
    if ( cmd.empty() )
    {
        client->Send( Telopt::opt_cursor_home );
        client->Send( Telopt::opt_erase_screen );
        client->Send( "Account Menu" CRLF CFG_STR_SEL_OPTIONS );
        client->Send( Utils::FormatString( 0, "%5d) Load an existing character" CRLF, ACT_MENU_MAIN_CHARACTER_LOAD ) );
        client->Send( Utils::FormatString( 0, "%5d) Create a new character" CRLF, ACT_MENU_MAIN_CHARACTER_CREATE ) );
        client->Send( Utils::FormatString( 0, "%5d) Delete an existing character" CRLF, ACT_MENU_MAIN_CHARACTER_DELETE ) );
        client->Send( Utils::FormatString( 0, "%5d) Quit" CRLF, ACT_MENU_MAIN_QUIT ) );
        client->Send( CFG_STR_SEL_PROMPT );
        return;
    }

    // Safer than ::stoi(), will output 0 for anything invalid
    stringstream( cmd ) >> val;

    switch( val )
    {
        case ACT_MENU_MAIN_CHARACTER_LOAD:
            if ( !client->gAccount()->gCharacters().empty() )
            {
                client->sState( SOC_STATE_CHARACTER_LOAD_MENU );
                LoginHandler( client );
            }
            else
            {
                client->Send( CFG_STR_ACT_CHR_NONE );
                client->Send( CFG_STR_SEL_PROMPT );
            }
        break;

        case ACT_MENU_MAIN_CHARACTER_CREATE:
            if ( client->gAccount()->gCharacters().size() < CFG_ACT_CHARACTER_MAX )
            {
                client->sState( SOC_STATE_CHARACTER_CREATE_MENU );
                LoginHandler( client );
            }
            else
            {
                client->Send( Utils::FormatString( 0, CFG_STR_ACT_CHR_LIMIT, CFG_ACT_CHARACTER_MAX ) );
                client->Send( CFG_STR_SEL_PROMPT );
            }
        break;

        case ACT_MENU_MAIN_CHARACTER_DELETE:
            if ( !client->gAccount()->gCharacters().empty() )
            {
                client->sState( SOC_STATE_CHARACTER_DELETE_MENU );
                LoginHandler( client );
            }
            else
            {
                client->Send( CFG_STR_ACT_CHR_NONE );
                client->Send( CFG_STR_SEL_PROMPT );
            }
        break;

        case ACT_MENU_MAIN_QUIT:
            client->Quit();
        break;

        case ACT_MENU_MAIN_INVALID:
        default:
            client->Send( CFG_STR_SEL_INVALID );
            client->Send( CFG_STR_SEL_PROMPT );
        break;
    }

    return;
}

/**
 * @brief Main Character creation menu.
 * @param[in] client The SocketClient to process a login request for.
 * @param[in] cmd The command sent by the SocketClient.
 * @param[in] args Any arguments to the command.
 * @retval void
 */
const void Handler::CharacterCreateMenuMain( SocketClient* client, const string& cmd, const string& args )
{
    UFLAGS_DE( flags );
    uint_t i = uintmin_t, val = uintmin_t;
    stringstream menu1, menu2;
    bool done = true;

    if ( client == NULL )
    {
        LOGSTR( flags, "Handler::CharacterCreateMenu()-> called with NULL client" );
        return;
    }

    if ( client->gAccount() == NULL )
    {
        LOGSTR( flags, "Handler::CharacterCreateMenu()-> called with NULL account" );
        return;
    }

    //Initial entry
    if ( cmd.empty() )
    {
        client->Send( Telopt::opt_cursor_home );
        client->Send( Telopt::opt_erase_screen );
        client->Send( "Account Menu > Create a new character" CRLF CFG_STR_SEL_OPTIONS );

        // If we're mid-creation, update the menu items to show current selections
        if ( client->gAccount()->gCharacter() != NULL )
        {
            if ( client->gAccount()->gCharacter()->gCreation( CHR_CREATION_NAME ) )
                menu1 << "(is: " << client->gAccount()->gCharacter()->gName() << ")";
            if ( client->gAccount()->gCharacter()->gCreation( CHR_CREATION_SEX ) )
                switch ( client->gAccount()->gCharacter()->gSex() )
                {
                    case CHR_SEX_NEUTRAL: menu2 << "(is: neutral)"; break;
                    case CHR_SEX_FEMALE:  menu2 << "(is: female)";  break;
                    case CHR_SEX_MALE:    menu2 << "(is: male)";    break;
                }
            // Ensure creation was completed
            for ( i = 0; i < MAX_CHR_CREATION; i++ )
                if ( !client->gAccount()->gCharacter()->gCreation( i ) )
                {
                    done = false;
                    break;
                }
        }
        else
            done = false;

        client->Send( Utils::FormatString( 0, "%5d) Set name %s" CRLF, ACT_MENU_CHARACTER_CREATE_NAME, CSTR( menu1.str() ) ) );
        client->Send( Utils::FormatString( 0, "%5d) Set sex %s" CRLF, ACT_MENU_CHARACTER_CREATE_SEX, CSTR( menu2.str() ) ) );
        if ( done )
            client->Send( Utils::FormatString( 0, "%5d) Finish Creation" CRLF, ACT_MENU_CHARACTER_CREATE_FINISH ) );
        client->Send( Utils::FormatString( 0, "%5d) Back" CRLF, ACT_MENU_CHARACTER_CREATE_BACK ) );
        client->Send( CFG_STR_SEL_PROMPT );
        return;
    }

    // Safer than ::stoi(), will output 0 for anything invalid
    stringstream( cmd ) >> val;

    switch( val )
    {
        case ACT_MENU_CHARACTER_CREATE_NAME:
            client->sState( SOC_STATE_CHARACTER_CREATE_NAME );
            LoginHandler( client );
        break;

        case ACT_MENU_CHARACTER_CREATE_SEX:
            client->sState( SOC_STATE_CHARACTER_CREATE_SEX );
            LoginHandler( client );
        break;

        case ACT_MENU_CHARACTER_CREATE_FINISH:
            if ( done )
            {
                client->sState( SOC_STATE_CHARACTER_CREATE_FINISH );
                LoginHandler( client );
            }
            else
            {
                client->Send( CFG_STR_SEL_INVALID );
                client->Send( CFG_STR_SEL_PROMPT );
            }
        break;

        case ACT_MENU_CHARACTER_CREATE_BACK:
            // Clear out any partially created characters
            if ( client->gAccount()->gCharacter() != NULL )
            {
                client->gAccount()->gCharacter()->Delete();
                client->gAccount()->ClearCharacter();
            }
            client->sState( SOC_STATE_ACCOUNT_MENU );
            LoginHandler( client );
        break;

        case ACT_MENU_CHARACTER_CREATE_INVALID:
        default:
            client->Send( CFG_STR_SEL_INVALID );
            client->Send( CFG_STR_SEL_PROMPT );
        break;
    }

    return;
}

/**
 * @brief Select a new Character name.
 * @param[in] client The SocketClient to process a login request for.
 * @param[in] cmd The command sent by the SocketClient.
 * @param[in] args Any arguments to the command.
 * @retval void
 */
const void Handler::CharacterCreateName( SocketClient* client, const string& cmd, const string& args )
{
    UFLAGS_DE( flags );
    Character* chr = NULL;
    vector<string> clist;
    vector<string>::iterator ci;
    stringstream cid;

    if ( client == NULL )
    {
        LOGSTR( flags, "Handler::CharacterCreateName()-> called with NULL client" );
        return;
    }

    if ( client->gAccount() == NULL )
    {
        LOGSTR( flags, "Handler::CharacterCreateName()-> called with NULL account" );
        return;
    }

    //Initial entry
    if ( cmd.empty() )
    {
        client->Send( CFG_STR_CHR_NAME_GET );
        return;
    }

    // Only allow alphanumerics for the character name itself
    if ( !Utils::iAlNum( cmd ) )
    {
        client->Send( CFG_STR_CHR_NAME_INVALID );
        client->Send( CFG_STR_CHR_NAME_ALNUM );
        LoginHandler( client );

        return;
    }

    //Prevent prohibited names based on Server runtime configuration
    if ( Handler::CheckProhibited( client, cmd, SVR_CFG_PROHIBITED_NAMES_CHARACTER ) )
    {
        client->Send( CFG_STR_CHR_NAME_INVALID );
        client->Send( CFG_STR_CHR_NAME_PROHIBITED );
        LoginHandler( client );

        return;
    }

    if ( cmd.length() < CFG_THG_NAME_MIN_LEN || cmd.length() > CFG_THG_NAME_MAX_LEN )
    {
        client->Send( CFG_STR_CHR_NAME_INVALID );
        client->Send( Utils::FormatString( 0, CFG_STR_CHR_NAME_LENGTH, CFG_THG_NAME_MIN_LEN, CFG_THG_NAME_MAX_LEN ) );
        LoginHandler( client );

        return;
    }

    // Does this account already have a character with this name?
    clist = client->gAccount()->gCharacters();
    for ( ci = clist.begin(); ci != clist.end(); ci++ )
    {
        if ( cmd == *ci )
        {
            client->Send( CFG_STR_CHR_NAME_EXISTS );
            LoginHandler( client );
            return;
        }
    }

    if ( client->gAccount()->gCharacter() == NULL )
    {
        chr = new Character();
        chr->sAccount( client->gAccount() );

        if ( !chr->New( client->gServer(), cmd, false ) )
        {
            client->Send( CFG_STR_CHR_NEW_ERROR );
            chr->Delete();
            return;
        }

        client->gAccount()->sCharacter( chr );
    }
    else
        chr = client->gAccount()->gCharacter();

    chr->sName( cmd );
    // Id for characters owned by accounts is account_name.character_name
    cid << client->gAccount()->gName() << "." << chr->gName();
    chr->sId( cid.str() );

    //All went well, generate the next input prompt
    chr->sCreation( CHR_CREATION_NAME, true );
    client->sState( SOC_STATE_CHARACTER_CREATE_MENU );
    LoginHandler( client );

    return;
}

/**
 * @brief Select a new Character sex.
 * @param[in] client The SocketClient to process a login request for.
 * @param[in] cmd The command sent by the SocketClient.
 * @param[in] args Any arguments to the command.
 * @retval void
 */
const void Handler::CharacterCreateSex( SocketClient* client, const string& cmd, const string& args )
{
    UFLAGS_DE( flags );
    uint_t val = uintmin_t;

    if ( client == NULL )
    {
        LOGSTR( flags, "Handler::CharacterCreateSex()-> called with NULL client" );
        return;
    }

    if ( client->gAccount() == NULL )
    {
        LOGSTR( flags, "Handler::CharacterCreateSex()-> called with NULL account" );
        return;
    }

    //Initial entry
    if ( cmd.empty() )
    {
        client->Send( CFG_STR_SEL_OPTIONS );
        client->Send( Utils::FormatString( 0, "%5d) Neutral" CRLF, CHR_SEX_NEUTRAL ) );
        client->Send( Utils::FormatString( 0, "%5d) Female" CRLF, CHR_SEX_FEMALE ) );
        client->Send( Utils::FormatString( 0, "%5d) Male" CRLF, CHR_SEX_MALE ) );
        client->Send( CFG_STR_SEL_PROMPT );

        return;
    }

    // Safer than ::stoi(), will output 0 for anything invalid
    stringstream( cmd ) >> val;

    if ( val == CHR_SEX_NONE )
    {
        client->Send( CFG_STR_SEL_INVALID );
        client->Send( CFG_STR_SEL_PROMPT );

        return;
    }

    client->gAccount()->gCharacter()->sSex( val );

    //All went well, generate the next input prompt
    client->gAccount()->gCharacter()->sCreation( CHR_CREATION_SEX, true );
    client->sState( SOC_STATE_CHARACTER_CREATE_MENU );
    LoginHandler( client );

    return;
}

/**
 * @brief Main Character deletion confirmation.
 * @param[in] client The SocketClient to process a login request for.
 * @param[in] cmd The command sent by the SocketClient.
 * @param[in] args Any arguments to the command.
 * @retval void
 */
const void Handler::CharacterDeleteConfirm( SocketClient* client, const string& cmd, const string& args )
{
    UFLAGS_DE( flags );

    if ( client == NULL )
    {
        LOGSTR( flags, "Handler::CharacterDeleteConfirm()-> called with NULL client" );
        return;
    }

    if ( client->gAccount() == NULL )
    {
        LOGSTR( flags, "Handler::CharacterDeleteConfirm()-> called with NULL account" );
        return;
    }

    //Initial entry
    if ( cmd.empty() )
    {
        client->Send( Utils::FormatString( 0, CFG_STR_CHR_DELETE_CONFIRM, CSTR( client->gLogin( SOC_LOGIN_CHARACTER ) ) ) );
        return;
    }

    if ( Utils::Lower( cmd ) == "yes" && client->gAccount()->dCharacter( client->gLogin( SOC_LOGIN_CHARACTER ) ) )
            client->gAccount()->Serialize();

    client->sLogin( SOC_LOGIN_CHARACTER, "" );

    //Generate the next input prompt
    client->sState( SOC_STATE_ACCOUNT_MENU );
    LoginHandler( client );

    return;
}

/**
 * @brief Main Character deletion menu.
 * @param[in] client The SocketClient to process a login request for.
 * @param[in] cmd The command sent by the SocketClient.
 * @param[in] args Any arguments to the command.
 * @retval void
 */
const void Handler::CharacterDeleteMenuMain( SocketClient* client, const string& cmd, const string& args )
{
    UFLAGS_DE( flags );
    uint_t i = uintmin_t, val = uintmin_t;

    if ( client == NULL )
    {
        LOGSTR( flags, "Handler::CharacterDeleteMenuMain()-> called with NULL client" );
        return;
    }

    if ( client->gAccount() == NULL )
    {
        LOGSTR( flags, "Handler::CharacterDeleteMenuMain()-> called with NULL account" );
        return;
    }

    //Initial entry
    if ( cmd.empty() )
    {
        client->Send( Telopt::opt_cursor_home );
        client->Send( Telopt::opt_erase_screen );
        client->Send( "Account Menu > Delete an existing character" CRLF CFG_STR_SEL_OPTIONS );
        for ( i = 0; i < client->gAccount()->gCharacters().size(); i++ )
            client->Send( Utils::FormatString( 0, "%5d) %s" CRLF, i+1, CSTR( client->gAccount()->gCharacters()[i] ) ) );
        client->Send( Utils::FormatString( 0, "%5d) Back" CRLF, ACT_MENU_CHARACTER_DELETE_BACK ) );
        client->Send( CFG_STR_SEL_PROMPT );
        return;
    }

    // Safer than ::stoi(), will output 0 for anything invalid
    stringstream( cmd ) >> val;

    // Handle 1 through CFG_ACT_CHARACTER_MAX dynamically
    if ( val >= 1 && val <= CFG_ACT_CHARACTER_MAX )
    {
        client->sLogin( SOC_LOGIN_CHARACTER, client->gAccount()->gCharacters()[val-1] );
        client->sState( SOC_STATE_CHARACTER_DELETE_CONFIRM );
        LoginHandler( client );

        return;
    }

    switch( val )
    {
        case ACT_MENU_CHARACTER_DELETE_BACK:
            client->sState( SOC_STATE_ACCOUNT_MENU );
            LoginHandler( client );
        break;

        case ACT_MENU_CHARACTER_DELETE_INVALID:
        default:
            client->Send( CFG_STR_SEL_INVALID );
            client->Send( CFG_STR_SEL_PROMPT );
        break;
    }

    return;
}

/**
 * @brief Main Character load menu.
 * @param[in] client The SocketClient to process a login request for.
 * @param[in] cmd The command sent by the SocketClient.
 * @param[in] args Any arguments to the command.
 * @retval void
 */
const void Handler::CharacterLoadMenuMain( SocketClient* client, const string& cmd, const string& args )
{
    UFLAGS_DE( flags );
    uint_t i = uintmin_t, val = uintmin_t;

    if ( client == NULL )
    {
        LOGSTR( flags, "Handler::CharacterLoadMenuMain()-> called with NULL client" );
        return;
    }

    if ( client->gAccount() == NULL )
    {
        LOGSTR( flags, "Handler::CharacterLoadMenuMain()-> called with NULL account" );
        return;
    }

    //Initial entry
    if ( cmd.empty() )
    {
        client->Send( Telopt::opt_cursor_home );
        client->Send( Telopt::opt_erase_screen );
        client->Send( "Account Menu > Load an existing character" CRLF CFG_STR_SEL_OPTIONS );
        for ( i = 0; i < client->gAccount()->gCharacters().size(); i++ )
            client->Send( Utils::FormatString( 0, "%5d) %s" CRLF, i+1, CSTR( client->gAccount()->gCharacters()[i] ) ) );
        client->Send( Utils::FormatString( 0, "%5d) Back" CRLF, ACT_MENU_CHARACTER_DELETE_BACK ) );
        client->Send( CFG_STR_SEL_PROMPT );
        return;
    }

    // Safer than ::stoi(), will output 0 for anything invalid
    stringstream( cmd ) >> val;

    // Handle 1 through CFG_ACT_CHARACTER_MAX dynamically
    if ( val >= 1 && val <= CFG_ACT_CHARACTER_MAX )
    {
        client->sLogin( SOC_LOGIN_CHARACTER, client->gAccount()->gCharacters()[val-1] );
        client->sState( SOC_STATE_LOAD_CHARACTER );
        LoginHandler( client );

        return;
    }

    switch( val )
    {
        case ACT_MENU_CHARACTER_LOAD_BACK:
            client->sState( SOC_STATE_ACCOUNT_MENU );
            LoginHandler( client );
        break;

        case ACT_MENU_CHARACTER_LOAD_INVALID:
        default:
            client->Send( CFG_STR_SEL_INVALID );
            client->Send( CFG_STR_SEL_PROMPT );
        break;
    }

    return;
}

/**
 * @brief Places the character loaded against the account into the game world.
 * @param[in] client The SocketClient to process a login request for.
 * @param[in] cmd The command sent by the SocketClient.
 * @param[in] args Any arguments to the command.
 * @retval void
 */
const void Handler::EnterGame( SocketClient* client, const string& cmd, const string& args )
{
    UFLAGS_DE( flags );

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
    if ( cmd.empty() && !client->gLogin( SOC_LOGIN_NAME ).empty() )
    {
        client->Send( Utils::FormatString( 0, CFG_STR_ACT_NAME_CONFIRM, CSTR( client->gLogin( SOC_LOGIN_NAME ) ) ) );
        return;
    }

    if ( Utils::StrPrefix( cmd, "no", true ) )
    {
        client->sLogin( SOC_LOGIN_NAME, "" );
        client->sState( SOC_STATE_LOGIN_SCREEN );
    }
    else if ( Utils::StrPrefix( cmd, "yes", true ) )
        client->sState( SOC_STATE_GET_NEW_PASSWORD );
    else
        client->Send( CFG_STR_SEL_INVALID );

    //Generate the next input prompt
    LoginHandler( client );

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
    if ( cmd.empty() )
    {
        Telopt::Negotiate( client, SOC_TELOPT_ECHO, true );
        client->Send( CFG_STR_ACT_NEW );
        client->Send( CFG_STR_ACT_PASSWORD_GET );

        return;
    }

    if ( cmd.length() < CFG_ACT_PASSWORD_MIN_LEN || cmd.length() > CFG_ACT_PASSWORD_MAX_LEN )
    {
        client->sLogin( SOC_LOGIN_PASSWORD, "" );
        client->Send( CFG_STR_ACT_PASSWORD_INVALID );
        client->Send( Utils::FormatString( 0, CFG_STR_ACT_PASSWORD_LENGTH, CFG_ACT_PASSWORD_MIN_LEN, CFG_ACT_PASSWORD_MAX_LEN ) );
        client->Send( CFG_STR_ACT_PASSWORD_GET );

        return;
    }

    if ( client->gLogin( SOC_LOGIN_PASSWORD ).empty() )
    {
        client->sLogin( SOC_LOGIN_PASSWORD, crypt( CSTR( cmd ), CSTR( Utils::Salt( client->gLogin( SOC_LOGIN_NAME ) ) ) ) );
        client->Send( CFG_STR_ACT_PASSWORD_CONFIRM );

        return;
    }

    if ( CKPASSWORD( client->gLogin( SOC_LOGIN_NAME ), cmd, client->gLogin( SOC_LOGIN_PASSWORD ) ) )
        client->sState( SOC_STATE_CREATE_ACCOUNT );
    else
    {
        client->sLogin( SOC_LOGIN_PASSWORD, "" );
        client->Send( CFG_STR_ACT_PASSWORD_MISMATCH );
        client->Send( CFG_STR_ACT_PASSWORD_GET );

        return;
    }

    //Generate the next input prompt
    LoginHandler( client );

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

    //Initial entry
    if ( cmd.empty() )
    {
        Telopt::Negotiate( client, SOC_TELOPT_ECHO, true );
        client->Send( CFG_STR_ACT_PASSWORD_GET );
        return;
    }

    client->sLogin( SOC_LOGIN_PASSWORD, crypt( CSTR( cmd ), CSTR( Utils::Salt( client->gLogin( SOC_LOGIN_NAME ) ) ) ) );
    client->sState( SOC_STATE_LOAD_ACCOUNT );

    //Generate the next input prompt
    LoginHandler( client );

    return;
}

/**
 * @brief Load a character from the account to enter into the game with.
 * @param[in] client The SocketClient to process a login request for.
 * @param[in] cmd The command sent by the SocketClient.
 * @param[in] args Any arguments to the command.
 * @retval void
 */
const void Handler::LoadCharacter( SocketClient* client, const string& cmd, const string& args )
{
    UFLAGS_DE( flags );
    Character* chr = NULL;
    stringstream id;

    if ( client == NULL )
    {
        LOGSTR( flags, "Handler::LoadCharacter()-> called with NULL client" );
        return;
    }

    if ( client->gAccount() == NULL )
    {
        LOGSTR( flags, "Handler::LoadCharacter()-> called with NULL account" );
        return;
    }

    chr = new Character();
    chr->sAccount( client->gAccount() );
    // Id for characters owned by accounts is account_name.character_name
    id << client->gAccount()->gName() << "." << client->gLogin( SOC_LOGIN_CHARACTER );

    if ( !chr->New( client->gServer(), Utils::FileExt( id.str(), CFG_DAT_FILE_PLR_EXT ), true ) )
    {
        LOGFMT( flags, "Handler::LoadCharacter()->Character::New()-> returned false for character %s", CSTR( id.str() ) );
        delete chr;
        return;
    }

    client->gAccount()->sCharacter( chr );
    client->sState( SOC_STATE_ENTER_GAME );

    //Generate the next input prompt
    LoginHandler( client );

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

    if ( client == NULL )
    {
        LOGSTR( flags, "Handler::LoginScreen()-> called with NULL client" );
        return;
    }

    // Initial connection with no input yet received or previous name was invalid
    if ( cmd.empty() )
    {
        client->Send( CFG_STR_ACT_NAME_GET );
        return;
    }

    // Only allow alphanumerics for the account name itself
    if ( !Utils::iAlNum( cmd ) )
    {
        client->Send( CFG_STR_ACT_NAME_INVALID );
        client->Send( CFG_STR_ACT_NAME_ALNUM );
        LoginHandler( client );

        return;
    }

    //Prevent prohibited names based on Server runtime configuration
    if ( Handler::CheckProhibited( client, cmd, SVR_CFG_PROHIBITED_NAMES_ACCOUNT ) )
    {
        client->Send( CFG_STR_ACT_NAME_INVALID );
        client->Send( CFG_STR_ACT_NAME_PROHIBITED );
        LoginHandler( client );

        return;
    }

    //Prevent two new accounts being created with the same name
    if ( Handler::CheckCreating( client, cmd ) )
    {
        client->Send( CFG_STR_ACT_NAME_INVALID );
        client->Send( CFG_STR_ACT_NAME_PROHIBITED );
        LoginHandler( client );

        return;
    }

    if ( cmd.length() < CFG_ACT_NAME_MIN_LEN || cmd.length() > CFG_ACT_NAME_MAX_LEN )
    {
        client->Send( CFG_STR_ACT_NAME_INVALID );
        client->Send( Utils::FormatString( 0, CFG_STR_ACT_NAME_LENGTH, CFG_ACT_NAME_MIN_LEN, CFG_ACT_NAME_MAX_LEN ) );
        LoginHandler( client );

        return;
    }

    if ( client->gLogin( SOC_LOGIN_NAME ).empty() )
    {
        client->sLogin( SOC_LOGIN_NAME, cmd );

        switch (  Utils::DirExists( Utils::DirPath( CFG_DAT_DIR_ACCOUNT, cmd ) ) )
        {
            case UTILS_RET_FALSE:
                client->sState( SOC_STATE_GET_NEW_ACCOUNT );
            break;

            case UTILS_RET_TRUE:
                client->sState( SOC_STATE_GET_OLD_PASSWORD );
            break;

            case UTILS_RET_ERROR:
            default:
                client->Send( CFG_STR_ACT_NAME_INVALID );
                LOGFMT( flags, "Handler::LoginScreen()->Utils::DirExists()-> returned UTILS_RET_ERROR for name: %s", CSTR( cmd ) );
            break;
        }
    }

    //Generate the next input prompt
    LoginHandler( client );

    return;
}
