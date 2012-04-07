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
#include "h/globals.h"
#include "h/directory.h"

// Core
const void Directory::Close()
{
    return;
}

bool Directory::Open( const string path )
{
    UFLAGS_DE( flags );

    if ( path.empty() )
    {
        LOGSTR( flags, "Directory::Open()-> called with empty path" );
        return false;
    }

    if ( ( m_handle = ::opendir( CSTR( path ) ) ) == NULL )
    {
        LOGFMT( flags, "Directory::Open()->opendir()-> returned errno %d: %s", errno, strerror( errno ) );
        return false;
    }

    return true;
}

// Query

// Manipulate

Directory::Directory()
{
    m_handle = NULL;

    return;
}

Directory::~Directory()
{
    if ( iOpen() )
        ::closedir( m_handle );

    return;
}
