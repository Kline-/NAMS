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
#ifndef DEC_SOCKETCLIENT_H
#define DEC_SOCKETCLIENT_H

#include <netdb.h>

#include "socket.h"
#include "server.h"

class SocketClient : public Socket {
    public:
        SocketClient( Server* server, sint_t& descriptor );
        ~SocketClient();

        // Core
        const void Disconnect( const string& msg = "" );
        bool PendingCommand() const { return !m_command_queue.empty(); }
        bool PendingOutput() const { return !m_output.empty(); }
        bool ProcessCommand();
        bool ProcessInput();
        bool QueueCommand( const string& command );
        bool Recv();
        const void ResolveHostname();
        const void Send( const string& msg );
        bool Send();

        // Query
        uint_t gIdle() const { return m_idle; }
        Server* gServer() const { return m_server; }
        uint_t gState() const { return m_state; }

        // Manipulate
        bool sIdle( const uint_t& idle );
        static void* tResolveHostname( void* data );
        bool sServer( Server* server );
        bool sState( const uint_t& state );

    private:
        vector<string> m_command_queue;
        uint_t  m_idle;
        string  m_input;
        string  m_output;
        Server* m_server;
        uint_t  m_state;
};

#endif
