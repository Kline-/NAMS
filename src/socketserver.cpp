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
 * @file socketserver.cpp
 * @brief All non-trivial member functions of the SocketServer class.
 */
#include "h/includes.h"
#include "h/class.h"

#include "h/socketserver.h"
#include "h/list.h"
#include "h/server.h"

/* Core */
/**
 * @brief Accept a new connection on the listening port.
 * @retval void
 */
const void SocketServer::Accept()
{
    UFLAGS_DE( flags );
    sint_t descriptor = 0;
    sockaddr_storage sin;
    socklen_t size = static_cast<socklen_t>( sizeof( sin ) );
    SocketClient* socket_client;

    if ( !Valid() )
    {
        LOGSTR( flags, "SocketServer::Accept()-> called with invalid socket" );
        return;
    }

    if ( ::getsockname( gDescriptor(), reinterpret_cast<sockaddr*>( &sin ), &size ) < 0 )
    {
        LOGERRNO( flags, "SocketServer::Accept()->getsockname()->" );
        return;
    }

    if ( ( descriptor = ::accept( gDescriptor(), reinterpret_cast<sockaddr*>( &sin ), &size ) ) < 0 )
    {
        LOGERRNO( flags, "SocketServer::Accept()->accept()->" );
        return;
    }

    socket_client = new SocketClient( gServer(), descriptor );

    if ( !socket_client->New() )
    {
        LOGSTR( flags, "SocketServer::Accept()->SocketClient::New()-> returned false" );
        socket_client->Delete();
        return;
    }

    return;
}

/**
 * @brief Bind a listening socket to a given port and IP address.
 * @param[in] port A #uint_t value representing the port to listen for inbound connections on.
 * @param[in] addr The IP address to listen for inbound connections on. May also be "::" for "any".
 * @retval false Returned if the socket is invalid, the address is empty, or if an error occurs during binding.
 * @retval true Returned if the socket was able to successfully bind to the port and address.
 */
const bool SocketServer::Bind( const uint_t& port, const string& addr )
{
    UFLAGS_DE( flags );
    sint_t enable = 1;
    static sockaddr_in6 sa_zero;
    sockaddr_in6 sa = sa_zero;

    if ( !Valid() )
    {
        LOGSTR( flags, "SocketServer::Bind()-> called with invalid socket" );
        return false;
    }

    if ( addr.empty() )
    {
        LOGSTR( flags, "SocketServer::Bind()-> called with empty addr" );
        return false;
    }

    sa.sin6_family = AF_INET6;
    sa.sin6_port = htons( port );
    inet_pton( AF_INET6, CSTR( addr ), &sa.sin6_addr );

    if ( ::setsockopt( gDescriptor(), SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>( &enable ), sizeof( enable ) ) < 0 )
    {
        LOGERRNO( flags, "SocketServer::Bind()->setsockopt()->" );
        return false;
    }

    if ( ::bind( gDescriptor(), reinterpret_cast<sockaddr*>( &sa ), sizeof( sa ) ) < 0 )
    {
        LOGERRNO( flags, "SocketServer::Bind()->bind()->" );
        return false;
    }

    return true;
}

/**
 * @brief Unload a server's socket from memory that was previously loaded via SocketServer::New().
 * @retval void
 */
const void SocketServer::Delete()
{
    UFLAGS_DE( flags );

    if ( !Valid() )
        return;

    if ( !m_server->sSocketClose( m_server->gSocketClose() + 1 ) )
        LOGFMT( flags, "SocketServer::Delete()->Server::sSocketClose()-> value %lu returned false", m_server->gSocketClose() + 1 );

    socket_server_list.remove( this );
    delete this;

    return;
}

/**
 * @brief Listen for new connections on an established socket.
 * @retval false Returned if the socket is invalid or if there is an error in listening.
 * @retval true Returned if the socket is able to successfully listen for new connections.
 */
const bool SocketServer::Listen()
{
    UFLAGS_DE( flags );

    if ( !Valid() )
    {
        LOGSTR( flags, "SocketServer::Listen()-> called with invalid socket" );
        return false;
    }

    if ( ::listen( gDescriptor(), CFG_SOC_MAX_PENDING ) < 0 )
    {
        LOGERRNO( flags, "SocketServer::Listen()->listen()->" );
        return false;
    }

    return true;
}

/**
 * @brief Build a socket for a server and set all attributes.
 * @retval false Returned if any errors are encountered during binding, setting hostname, or listening.
 * @retval true Returned if the socket is able to successfully bind, set hostname, and listen for new connections.
 */
const bool SocketServer::New()
{
    UFLAGS_DE( flags );

    if ( !Bind( m_server->gPort(), CFG_SOC_BIND_ADDR ) )
    {
        LOGSTR( flags, "SocketServer::New()->SocketServer::Bind()-> returned false" );
        m_server->Shutdown( EXIT_FAILURE );
    }

    if ( !sHostname( m_server->gHostname() ) )
    {
        LOGFMT( flags, "SocketServer::New()->SocketServer::sHostname()-> hostname %s returned false", CSTR( m_server->gHostname() ) );
        m_server->Shutdown( EXIT_FAILURE );
    }

    if ( !Listen() )
    {
        LOGSTR( flags, "SocketServer::New()->SocketServer::Listen()-> returned false" );
        m_server->Shutdown( EXIT_FAILURE );
    }

    return true;
}

/* Query */

/* Manipulate */
/**
 * @brief Set the owning server object that the socket is actually connected to.
 * @param[in] server A pointer to an instance of a Server object. By default this is  the server instance which initially spawned the socket.
 * @retval false Returned if the server is either invalid (NULL) or shutdown.
 * @retval true Returned if owning server is successfully set.
 */
const bool SocketServer::sServer( Server* server )
{
    UFLAGS_DE( flags );

    if ( !server )
    {
        LOGSTR( flags, "SocketServer::sServer()-> called with NULL server" );
        return false;
    }

    if ( !server->Running() )
    {
        LOGSTR( flags, "SocketServer::sServer()-> called with offline server" );
        return false;
    }

    m_server = server;

    return true;
}

/* Internal */
/**
 * @brief Constructor for the SocketServer class.
 * @param[in] server A pointer to an instance of a Server object.
 * @param[in] descriptor A #sint_t value of the file descriptor that has been opened for the socket.
 */
SocketServer::SocketServer( Server* server, const sint_t& descriptor ) : Socket( server, descriptor )
{
    UFLAGS_DE( flags );

    sServer( server );

    m_server->sSocketOpen( m_server->gSocketOpen() + 1 );
    socket_server_list.push_back( this );

    return;
}

/**
 * @brief Destructor for the SocketServer class.
 */
SocketServer::~SocketServer()
{
    return;
}
