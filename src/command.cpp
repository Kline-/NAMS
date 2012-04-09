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
#include "h/includes.h"
#include "h/class.h"

#include "h/command.h"
#include "h/list.h"

// Core
bool Command::Load( const string& file )
{
    UFLAGS_DE( flags );
    string path(  Utils::DirPath( CFG_DAT_DIR_COMMAND, file ) );
    string line, key, value;
    bool found = false;
    ifstream cmd;

    // Ensure there is a valid file to open
    if ( !Utils::iFile( path ) )
    {
        LOGFMT( flags, "Command::Load()->Utils::iFile()-> returned false for path: %s", CSTR( path ) );
        return false;
    }

    cmd.open( CSTR( path ), ifstream::in );
    while( cmd.is_open() && cmd.good() && getline( cmd, line ) )
    {
        // Find the key = value split
        if ( !Utils::KeyValue( key, value, line ) )
        {
            LOGFMT( flags, "Command::Load()->getline()-> invalid line format: %s", CSTR( line ) );
            continue;
        }

        // If the contents of arg3 == arg4 then assign arg5 == arg6
        for ( ;; )
        {
            found = false;

            Utils::KeySet( true, found, key, "Level",   value, m_level   );
            Utils::KeySet( true, found, key, "Name",    value, m_name    );
            Utils::KeySet( true, found, key, "Preempt", value, m_preempt );

            if ( !found )
                LOGFMT( flags, "Command::Load()->Utils::KeySet()-> key not found: %s", CSTR( key ) );

            break;
        }
    }

    command_list.insert( pair<const char,Command*>( m_name[0], this ) );
clog << m_level << endl << m_name << endl << m_preempt << endl;
    return true;
}

const void Command::Unload()
{
    delete this;

   return;
}

// Query

// Manipulate

Command::Command()
{
    m_level = 0;
    m_name.clear();
    m_preempt = false;

    return;
}

Command::~Command()
{
    return;
}
