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

/**
 * @brief Functions and interfaces that are common to both SocketClient and SocketServer.
 */
class Socket {
    public:
        /** @name Core */ /**@{*/
        /**
         * @brief Destroy a Socket previously created via Socket::New().
         * @retval void
         */
        virtual const void Delete() = 0;
        /**
         * @brief Build a Socket for either a new SocketClient or SocketServer.
         * @param[in] reboot Mark if the server is undergoing a reboot or not.
         * @retval false Returned if there is an error in creating the Socket.
         * @retval true Returned if the Socket is successfully created.
         */
        virtual const bool New( const bool& reboot = false ) = 0;
        virtual const string Serialize() const = 0;
        const bool Valid() const;
        /**@}*/

        /** @name Query */ /**@{*/
        const uint_t gBytesRecvd() const;
        const uint_t gBytesSent() const;
        const sint_t gDescriptor() const;
        const string gHostname() const;
        const uint_t gPort() const;
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
        uint_t m_bytes_recvd;   /**< Number of bytes received. */
        uint_t m_bytes_sent;    /**< Number of bytes sent. */
        sint_t m_descriptor;    /**< File descriptor assigned from the host OS. */
        string m_hostname;      /**< Hostname of the host OS or remote client. */
        uint_t m_port;          /**< Port number being used. */
};

#endif
