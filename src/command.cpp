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
    ifstream cmd( CSTR( file ), ifstream::in );
    string path = CFG_DAT_DIR_COMMAND;
    string line;

    LOGFMT( flags, "Received file: %s", CSTR( file ) );

    // Ensure a trailing slash is present to properly recurse
    if ( path.compare( path.length() - 1, 1, "/" ) != 0 )
        path += "/";

    // Now append the first letter of the file recieved as a subdir
    path += file[0]; path += "/";

    // Finally add the filename
    path += file;

    while( cmd.is_open() && cmd.good() && getline( cmd, line ) )
    {
        LOGFMT( flags, "Read: %s", CSTR( line ) );
    }
m_name = file;
    LOGFMT( flags, "Built path: %s", CSTR( path ) );

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
