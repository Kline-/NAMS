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
 * @file socketclient.h
 * @brief The SocketClient class.
 *
 *  This file contains the SocketClient class, templates, and trivial member functions.
 */
#ifndef DEC_SOCKETCLIENT_H
#define DEC_SOCKETCLIENT_H

#include <deque>
#include <netdb.h>

#include "socket.h"
#include "server.h"

using namespace std;

class SocketClient : public Socket {
    public:
        /** @name Core */ /**@{*/
        const void Delete();
        const bool New();
        const bool PendingCommand() const { return !m_command_queue.empty(); }
        const bool PendingOutput() const { return !m_output.empty(); }
        const bool ProcessCommand();
        const bool ProcessInput();
        const bool QueueCommand( const string& command );
        const bool Recv();
        const void ResolveHostname();
        const bool Send();
        const bool Send( const string& msg );
        /**@}*/

        /** @name Query */ /**@{*/
        const uint_t gIdle() const { return m_idle; }
        Server* gServer() const { return m_server; }
        const uint_t gState() const { return m_state; }
        /**@}*/

        /** @name Manipulate */ /**@{*/
        const bool sIdle( const uint_t& idle );
        static void* tResolveHostname( void* data );
        const bool sServer( Server* server );
        const bool sState( const uint_t& state );
        /**@}*/

        /** @name Internal */ /**@{*/
        SocketClient( Server* server, const sint_t& descriptor );
        ~SocketClient();
        /**@}*/

    private:
        deque<string> m_command_queue;
        uint_t  m_idle;
        string  m_input;
        string  m_output;
        Server* m_server;
        uint_t  m_state;
};

#endif
