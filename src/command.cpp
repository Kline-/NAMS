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
bool Command::Load( const string file )
{
    UFLAGS_DE( flags );
    ifstream cmd;
    string path = CFG_DAT_DIR_COMMAND;
    string line, key, value;
    size_t pos = 0;

    LOGFMT( flags, "Received file: %s", CSTR( file ) );

    // Ensure a trailing slash is present to properly recurse
    if ( path.compare( path.length() - 1, 1, "/" ) != 0 )
        path += "/";

    // Now append the first letter of the file recieved as a subdir
    path += file[0]; path += "/";

    // Finally add the filename
    path += file;

    cmd.open( CSTR( path ), ifstream::in );
    while( cmd.is_open() && cmd.good() && getline( cmd, line ) )
    {
        if ( ( pos = line.find( "=" ) ) == string::npos )
        {
            LOGFMT( flags, "Command::Load()->getline()-> invalid line: %s", CSTR( line ) );
            continue;
        }
        else
        {
            key = line.substr( 0, pos - 1 );
            value = line.substr( pos + 1, line.length() );
            key = Utils::rmSpaces( key );
            value = Utils::rmSpaces( value );
            LOGFMT( flags, "%s:%s", CSTR( key ), CSTR( value ) );
        }

    }


    m_name = file;

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
    m_name.clear();
    m_preempt = false;

    return;
}

Command::~Command()
{
    return;
}
