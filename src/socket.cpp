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
 * @file socket.cpp
 * @brief All non-template member functions of the Socket class.
 *
 * The Socket class is the base class for both the SocketClient and SocketServer
 * classes. Common parts of both classes are implemented within this class when
 * possible for efficiency and ease of future maintenance.
 */
#include "h/includes.h"
#include "h/class.h"

#include "h/socket.h"

/* Core */
/**
 * @brief Return if the Socket is valid for use.
 * @retval false Returned if the file descriptor associated to the Socket is < 1.
 * @retval true Returned if the file descriptor associated to the Socket is > 0.
 */
const bool Socket::Valid() const
{
    return m_descriptor > 0;
}

/* Query */
/**
 * @brief Returns the number of bytes received by the Socket.
 * @retval uint_t The number of bytes received by the Socket.
 */
const uint_t Socket::gBytesRecvd() const
{
    return m_bytes_recvd;
}

/**
 * @brief Returns the number of bytes sent by the Socket.
 * @retval uint_t The number of bytes sent by the Socket.
 */
const uint_t Socket::gBytesSent() const
{
    return m_bytes_sent;
}

/**
 * @brief Returns the file descriptor associated to the Socket.
 * @retval sint_t The file descriptor associated to the Socket.
 */
const sint_t Socket::gDescriptor() const
{
    return m_descriptor;
}

/**
 * @brief Returns the hostname associated to the Socket.
 * @retval string The hostname associated to the Socket.
 */
const string Socket::gHostname() const
{
    return m_hostname;
}

/**
 * @brief Returns the port associated to the Socket.
 * @retval uint_t The port associated to the Socket.
 */
const uint_t Socket::gPort() const
{
    return m_port;
}

/* Manipulate */
/**
 * @brief Increment the total count of bytes received by the socket.
 * @param[in] amount A #uint_t value to increase the byte count by.
 * @retval false Returned if the amount is outside the limits for a #uint_t variable or if the existing counter would overflow.
 * @retval true Returned if the byte counter was successfully incremented.
 */
const bool Socket::aBytesRecvd( const uint_t& amount )
{
    UFLAGS_DE( flags );

    if ( amount < 1 || ( ( m_bytes_recvd + amount ) >= uintmax_t ) )
    {
        LOGFMT( flags, "Socket::aBytesRecvd()-> called with m_bytes_recvd overflow: %lu + %lu", m_bytes_recvd, amount );
        return false;
    }

    m_bytes_recvd += amount;

    return true;
}

/**
 * @brief Increment the total count of bytes sent by the socket.
 * @param[in] amount A #uint_t value to increase the byte count by.
 * @retval false Returned if the amount is outside the limits for a #uint_t variable or if the existing counter would overflow.
 * @retval true Returned if the byte counter was successfully incremented.
 */
const bool Socket::aBytesSent( const uint_t& amount )
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

/**
 * @brief Sets the file descriptor that was opened for the socket.
 * @param[in] descriptor A #sint_t value that corresponds to an open file descriptor.
 * @retval false Returned if the descriptor is outside the limits for a #sint_t variable.
 * @retval true Returned if the descriptor was successfully set.
 */
const bool Socket::sDescriptor( const sint_t& descriptor )
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

/**
 * @brief Sets the hostname of the socket.
 * @param[in] hostname A string value that equal to the hostname of the socket.
 * @retval false Returned if the hostname is empty.
 * @retval true Returned if the hostname was successfully set.
 */
const bool Socket::sHostname( const string& hostname )
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

/**
 * @brief Sets the port that was opened for the socket.
 * @param[in] port A #uint_t value that corresponds to an open port.
 * @retval false Returned if the port is outside the limits for a #uint_t variable.
 * @retval true Returned if the port was successfully set.
 */
const bool Socket::sPort( const uint_t& port )
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

/* Internal */
/**
 * @brief Constructor for the Socket class.
 * @param[in] server A pointer to an instance of a Server object.
 * @param[in] descriptor A #sint_t value of the file descriptor that has been opened for the socket.
 */
Socket::Socket( Server* server, const sint_t& descriptor )
{
    m_bytes_recvd = 0;
    m_bytes_sent = 0;
    sDescriptor( descriptor );
    m_hostname.clear();
    m_port = 0;

    return;
}

/**
 * @brief Destructor for the Socket class.
 */
Socket::~Socket()
{
    UFLAGS_DE( flags );

    if ( ::close( m_descriptor ) < 0 )
        LOGERRNO( flags, "Socket::~Socket()->close()->" );

    return;
}
