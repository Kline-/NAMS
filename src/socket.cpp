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
bool Socket::sBytesRecvd( const uint_t& amount )
{
    UFLAGS_DE( flags );

    if ( amount < 1 || ( ( m_bytes_recvd + amount ) >= uintmax_t ) )
    {
        LOGFMT( flags, "Socket::sBytesRecvd()-> called with m_bytes_recvd overflow: %lu + %lu", m_bytes_recvd, amount );
        return false;
    }

    m_bytes_recvd += amount;

    return true;
}

bool Socket::sBytesSent( const uint_t& amount )
{
    UFLAGS_DE( flags );

    if ( amount < 1 || ( ( m_bytes_sent + amount ) >= uintmax_t ) )
    {
        LOGFMT( flags, "Socket::sBytesSent()-> called with m_bytes_sent overflow: %lu + %lu", m_bytes_sent, amount );
        return false;
    }

    m_bytes_sent += amount;

    return true;
}

bool Socket::sDescriptor( const sint_t& descriptor )
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

bool Socket::sHostname( const string& hostname )
{
    UFLAGS_DE( flags );

    if ( hostname.empty() )
    {
        LOGSTR( flags, "Socket::sHostname()-> called with empty hostname" );
        return false;
    }

    m_hostname = hostname;

    return true;
}

bool Socket::sPort( const uint_t& port )
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

Socket::Socket( sint_t& descriptor )
{
    m_bytes_recvd = 0;
    m_bytes_sent = 0;
    m_hostname.clear();
    m_port = 0;

    sDescriptor( descriptor );

    return;
}

Socket::~Socket()
{
    if ( Valid() )
        ::close( m_descriptor );

    return;
}
