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

#include "h/socket.h"

// Core

// Query

// Manipulate
bool Socket::sBytesRecvd( const uint_t amount )
{
    UFLAGS_DE( flags );

    if ( amount < 1 || ( ( m_bytes_recvd + amount ) >= uintmax_t ) )
    {
        LOGFMT( flags, "Socket::sBytesRecvd()-> amount %ld would overflow m_bytes_recvd: %ld", amount, m_bytes_recvd );
        return false;
    }

    m_bytes_recvd += amount;

    return true;
}

bool Socket::sBytesSent( const uint_t amount )
{
    UFLAGS_DE( flags );

    if ( amount < 1 || ( ( m_bytes_sent + amount ) >= uintmax_t ) )
    {
        LOGFMT( flags, "Socket::sBytesSent()-> amount %ld would overflow m_bytes_sent: %ld", amount, m_bytes_sent );
        return false;
    }

    m_bytes_sent += amount;

    return true;
}

bool Socket::sDescriptor( const sint_t descriptor )
{
    UFLAGS_DE( flags );

    if ( descriptor < 0 || descriptor >= sintmax_t )
    {
        LOGFMT( flags, "Socket::sDescriptor()-> called with invalid descriptor: %ld", descriptor );
        return false;
    }

    m_descriptor = descriptor;

    return true;
}

bool Socket::sHost( const string host )
{
    UFLAGS_DE( flags );

    if ( host.empty() )
    {
        LOGSTR( flags, "Socket::sHost()-> called with empty host" );
        return false;
    }

    m_host = host;

    return true;
}

bool Socket::sPort( const uint_t port )
{
    UFLAGS_DE( flags );

    if ( port <= uintmin_t || port >= uintmax_t )
    {
        LOGFMT( flags, "Socket::sPort()-> called with invalid port: %lu", port );
        return false;
    }

    m_port = port;

    return true;
}

Socket::Socket()
{
    m_bytes_recvd = 0;
    m_bytes_sent = 0;
    m_descriptor = 0;
    m_host.clear();
    m_port = 0;

    return;
}

Socket::~Socket()
{
    if ( Valid() )
        ::close( m_descriptor );

    return;
}
