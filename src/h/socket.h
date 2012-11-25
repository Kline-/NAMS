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
 * @file socket.h
 * @brief The Socket class.
 *
 *  This file contains the Socket class and template functions.
 */
#ifndef DEC_SOCKET_H
#define DEC_SOCKET_H

using namespace std;

class Socket {
    public:
        /** @name Core */ /**@{*/
        virtual const void Delete() = 0;
        virtual const bool New() = 0;
        const bool Valid() const { return m_descriptor > 0; }
        /**@}*/

        /** @name Query */ /**@{*/
        const uint_t gBytesRecvd() const { return m_bytes_recvd; }
        const uint_t gBytesSent() const { return m_bytes_sent; }
        const sint_t gDescriptor() const { return m_descriptor; }
        const string gHostname() const { return m_hostname; }
        const uint_t gPort() const { return m_port; }
        /**@}*/

        /** @name Manipulate */ /**@{*/
        const bool aBytesRecvd( const uint_t& amount );
        const bool aBytesSent( const uint_t& amount );
        const bool sDescriptor( const sint_t& descriptor );
        const bool sHostname( const string& hostname );
        const bool sPort( const uint_t& port );
        /**@}*/

        /** @name Internal */ /**@{*/
        Socket( Server* server, const sint_t& descriptor );
        ~Socket();
        /**@}*/

    private:
        uint_t  m_bytes_recvd;
        uint_t  m_bytes_sent;
        sint_t  m_descriptor;
        string  m_hostname;
        uint_t  m_port;
};

#endif
