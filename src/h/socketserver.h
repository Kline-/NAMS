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
 * @file socketserver.h
 * @brief The SocketServer class.
 *
 *  This file contains the SocketServer class and template functions.
 */
#ifndef DEC_SOCKETSERVER_H
#define DEC_SOCKETSERVER_H

#include "socket.h"

using namespace std;

/**
 * @brief SocketServer extends the Socket class to implement server-side socket functions.
 */
class SocketServer : public Socket {
    public:
        /** @name Core */ /**@{*/
        const void Accept();
        const bool Bind( const uint_t& port, const string& addr );
        const void Delete();
        const bool Listen();
        const bool New( const bool& reboot = false );
        /**@}*/

        /** @name Query */ /**@{*/
        Server* gServer() const;
        /**@}*/

        /** @name Manipulate */ /**@{*/
        const bool sServer( Server* server );
        /**@}*/

        /** @name Internal */ /**@{*/
        SocketServer( Server* server, const sint_t& descriptor );
        ~SocketServer();
        /**@}*/

    private:
        Server* m_server;   /**< The Server object associated to this server-side socket. */
};

#endif
