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
 * @brief All non-trivial member functions of the Command class.
 * @todo Commands need to contain additional fields for function name
 *  and the location of the cpp library file they will reside in. Implement
 *  method to self-compile and reload libraries externally with dlsym.
 *  Create abstract classes to treat as an API layer to the shared objects.
 */
#include "h/includes.h"
#include "h/class.h"

#include "h/command.h"
#include "h/list.h"

/** @name Core */ /**@{*/
/**
 * @brief Unload a command from memory that was previously loaded via Command::New().
 * @retval void
 */
const void Command::Delete()
{
    UFLAGS_DE( flags );
    MITER( multimap, const char,Command*, mi );

    m_plg_delete( m_plg );
    ::dlerror();

    if ( ::dlclose( m_plg_handle ) )
        LOGFMT( flags, "Command::Delete()->dlclose() returned error: %s", ::dlerror() );

    for ( mi = command_list.begin(); mi != command_list.end(); )
    {
        if ( mi->second == this )
        {
            command_list.erase( mi++ );
            break;
        }
        else
            ++mi;
    }
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

    // Ensure there is a valid file to open
    if ( !Utils::iFile( path ) )
    {
        LOGFMT( flags, "Command::New()->Utils::iFile()-> returned false for path: %s", CSTR( path ) );
        return false;
    }

    if ( ( m_plg_handle = ::dlopen( CSTR( path ), RTLD_LAZY | RTLD_GLOBAL ) ) == NULL )
    {
        LOGFMT( flags, "Command::New()->dlopen returned error: %s", ::dlerror() );
        return false;
    }
    else
    {
        m_plg_delete = (DeletePlugin*) ::dlsym( m_plg_handle, "Delete" );
        m_plg_new = (NewPlugin*) ::dlsym( m_plg_handle, "New" );
        m_plg = m_plg_new();
    }

    if ( CFG_GAM_CMD_IGNORE_CASE )
        command_list.insert( pair<const char,Command*>( Utils::Lower( gName() )[0], this ) );
    else
        command_list.insert( pair<const char,Command*>( gName()[0], this ) );

    return true;
}
/**@}*/

/** @name Query */
/**@}*/

/** @name Manipulate */
/**@}*/

/** @name Internal */
/**
 * @brief Constructor for the Command class.
 */
Command::Command()
{
    m_level = uintmin_t;
    m_plg = NULL;
    m_plg_delete = NULL;
    m_plg_handle = NULL;
    m_plg_new = NULL;
    m_preempt = false;

    return;
}

/**
 * @brief Destructor for the Command class.
 */
Command::~Command()
{
    return;
}
/**@}*/
