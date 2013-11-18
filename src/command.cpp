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
 * @file command.cpp
 * @brief All non-template member functions of the Command class.
 *
 * Command objects are built as independently loaded Plugin classes. The
 * Server will compile any file within #CFG_DAT_DIR_COMMAND and ending in
 * #CFG_PLG_BUILD_EXT_IN to a shared object file stored in #CFG_DAT_DIR_OBJ.
 *
 * Each object file is a unique class derived from Plugin. The Command class
 * then acts as an abstraction layer for interacting with thse objects in a
 * more specialized manner, as the Plugin class is broadly generic to support
 * other implementation uses.
 */
#include "h/includes.h"
#include "h/command.h"

#include "h/character.h"
#include "h/list.h"
#include "h/plugin.h"
#include "h/socketclient.h"
#include "h/account.h"

/* Core */
/**
 * @brief Check if sec is >= m_security for permission to call Command::Run().
 * @param[in] sec The security level of the caller.
 * @retval false Returned if sec < m_security.
 * @retval true Returned if sec >= m_security.
 */
const bool Command::Authorized( const uint_t& sec ) const
{
    if ( sec < m_security )
        return false;

    /** @todo Consider adding logging here to catch denials. */

    return true;
}

/**
 * @brief Unload a command from memory that was previously loaded via Command::New().
 * @retval void
 */
const void Command::Delete()
{
    UFLAGS_DE( flags );

    m_plg_delete( m_plg );
    ::dlerror();

    if ( ::dlclose( m_plg_handle ) )
        LOGFMT( flags, "Command::Delete()->dlclose() returned error: %s", ::dlerror() );

    if ( find( command_list.begin(), command_list.end(), this ) != command_list.end() )
        command_list.erase( find( command_list.begin(), command_list.end(), this ) );

    delete this;

   return;
}

/**
 * @brief Load a plugin command from #CFG_DAT_DIR_OBJ.
 * @param[in] file The filename to load without any path prepended to it.
 * @retval false Returned if the command in file was not found or unable to be loaded.
 * @retval true Returned if the command in file was successfully loaded.
 */
const bool Command::New( const string& file )
{
    UFLAGS_DE( flags );
    string path( Utils::DirPath( CFG_DAT_DIR_OBJ, file, CFG_PLG_BUILD_EXT_OUT ) );
    vector<string> disabled = g_config->gDisabledCommands();

    // Ensure there is a valid file to open
    if ( !Utils::iFile( path ) )
    {
        LOGFMT( flags, "Command::New()->Utils::iFile()-> returned false for path: %s", CSTR( path ) );
        return false;
    }

    if ( ( m_plg_handle = ::dlopen( CSTR( path ), RTLD_LAZY | RTLD_GLOBAL ) ) == NULL )
    {
        LOGFMT( flags, "Command::New()->dlopen() returned error: %s", ::dlerror() );
        return false;
    }
    else
    {
        // Register the handlers and create a new instance of the Plugin's class
        m_plg_delete = (PluginDelete*) ::dlsym( m_plg_handle, "Delete" );
        m_plg_file = file;
        m_plg_new = (PluginNew*) ::dlsym( m_plg_handle, "New" );
        m_plg = m_plg_new();
        m_plg->sCaller( this );

        // Set specific values unique to a Command object that the Plugin can specify
        m_preempt = m_plg->gBool( PLG_TYPE_COMMAND_BOOL_PREEMPT );
        m_security = m_plg->gUint( PLG_TYPE_COMMAND_UINT_SECURITY );

        // Check to see if the Command has been disabled
        if ( find( disabled.begin(), disabled.end(), gName() ) != disabled.end() )
            m_disabled = true;
    }

    command_list.push_back( this );

    return true;
}

/**
 * @brief Execute a Plugin object's primary function.
 * @param[in] character If called from a Character, the caller is passed through to the Plugin for reference.
 * @param[in] cmd If called from a Character, the command from the client is passed through.
 * @param[in] arg If called from a Character, the arguments from the client are passed through.
 * @retval void
 */
const void Command::Run( Character* character, const string& cmd, const string& arg ) const
{
    if ( character )
    {
        if ( m_disabled && character->gBrain()->gAccount()->gSecurity() < ACT_SECURITY_ADMIN )
            character->Send( CFG_STR_CMD_DISABLED );
        else
            m_plg->Run( character, cmd, arg );
    }

    return;
}

/**
 * @brief Execute a Plugin object's primary function.
 * @param[in] client If called from a SocketClient, the caller is passed through to the Plugin for reference.
 * @param[in] cmd If called from a SocketClient, the command from the client is passed through.
 * @param[in] arg If called from a SocketClient, the arguments from the client are passed through.
 * @retval void
 */
const void Command::Run( SocketClient* client, const string& cmd, const string& arg ) const
{
    if ( client )
    {
        if ( m_disabled && client->gAccount()->gSecurity() < ACT_SECURITY_ADMIN )
            client->Send( CFG_STR_CMD_DISABLED );
        else
            m_plg->Run( client, cmd, arg );
    }

    return;
}

/* Query */
/**
 * @brief Return a pointer back to a Command object associated with the loaded Plugin.
 * @retval void* A pointer back to a Command object associated with the loaded Plugin.
 */
void* Command::gCaller() const
{
    return m_plg->gCaller();
}

/**
 * @brief Return the filename of the associated Plugin object.
 * @retval string A string containing the filename of the associated Plugin object.
 */
const string Command::gFile() const
{
    return m_plg_file;
}

/**
 * @brief Return the name of the associated Plugin object.
 * @retval string A string containing the name of the associated Plugin object.
 */
const string Command::gName() const
{
    return m_plg->gName();
}

/**
 * @brief Return the preempt status of the associated Plugin object.
 * @retval false Returned if the associated Plugin doesn't preempt.
 * @retval true Returned if the associated Plugin does preempt.
 */
const bool Command::gPreempt() const
{
    return m_preempt;
}

/* Manipulate */
/**
 * @brief Toggles the disabled state of a Command.
 * @retval void
 */
const void Command::ToggleDisable()
{
    m_disabled = !m_disabled;

    return;
}

/* Internal */
/**
 * @brief Constructor for the Command class.
 */
Command::Command()
{
    m_disabled = false;
    m_plg = NULL;
    m_plg_delete = NULL;
    m_plg_file.clear();
    m_plg_handle = NULL;
    m_plg_new = NULL;
    m_preempt = false;
    m_security = ACT_SECURITY_NONE;

    return;
}

/**
 * @brief Destructor for the Command class.
 */
Command::~Command()
{
    return;
}
