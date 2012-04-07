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

#include "h/socketserver.h"

// Core
bool SocketServer::Bind( const uint_t port, const string addr )
{
    UFLAGS_DE( flags );
    static struct sockaddr_in sa_zero;
    struct sockaddr_in sa = sa_zero;

    if ( !Valid() )
    {
        LOGSTR( flags, "SocketServer::Bind()-> called with invalid socket" );
        return false;
    }

    sa.sin_family = AF_INET;
    sa.sin_port = htons( port );

    if ( !addr.empty() )
        sa.sin_addr.s_addr = inet_addr( CSTR( addr ) );

    if ( ::bind( m_descriptor, reinterpret_cast<sockaddr*>( &sa ), sizeof( sa ) ) < 0 )
    {
        LOGFMT( flags, "SocketServer::Bind()->bind()-> returned errno %d: %s", errno, strerror( errno ) );
        return false;
    }

    return true;
}

bool SocketServer::Listen()
{
    UFLAGS_DE( flags );

    if ( !Valid() )
    {
        LOGSTR( flags, "SocketServer::Listen()-> called with invalid socket" );
        return false;
    }

    if ( ::listen( m_descriptor, CFG_SOC_MAX_PENDING ) < 0 )
    {
        LOGFMT( flags, "SocketServer::Listen()->listen()-> returned errno %d: %s", errno, strerror( errno ) );
        return false;
    }

    return true;
}

// Query

// Manipulate

SocketServer::SocketServer()
{
    socket_server_list.push_back( this );

    return;
}

SocketServer::~SocketServer()
{
    return;
}
