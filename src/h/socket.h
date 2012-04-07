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
#ifndef DEC_SOCKET_H
#define DEC_SOCKET_H

#include "lists.h"
#include "enum.h"
#include "server.h"

class Socket : public Utils {
    public:
        Socket();
        ~Socket();

        // Core
        bool Valid() const { return m_descriptor > 0; }

        // Query
        sint_t gDescriptor() const { return m_descriptor; }
        string gHost() const { return m_host; }
        uint_t gPort() const { return m_port; }

        // Manipulate
        bool sDescriptor( const sint_t descriptor );
        bool sHost( const string host );
        bool sPort( const uint_t port );

    protected:
        uint_t  m_bytes_recvd;
        uint_t  m_bytes_sent;
        sint_t  m_descriptor;
        string  m_host;

    private:
        uint_t  m_port;
};

class SocketClient : public Socket {
    public:
        SocketClient();
        ~SocketClient();

        // Core
        const void Disconnect( const string msg = "" );
        bool PendingCommand() const { return !m_command_queue.empty(); }
        bool PendingOutput() const { return !m_output.empty(); }
        bool ProcessCommand();
        bool ProcessInput();
        bool QueueCommand( const string command );
        bool Recv();
        const void ResolveHostname();
        const void Send( const string msg );
        bool Send();

        // Query
        uint_t gIdle() const { return m_idle; }
        Server* gServer() const { return m_server; }
        uint_t gState() const { return m_state; }

        // Manipulate
        bool sIdle( const uint_t idle );
        static void* tResolveHostname( void* data );
        bool sServer( Server* server );
        bool sState( const uint_t state );

    private:
        vector<string> m_command_queue;
        uint_t  m_idle;
        string  m_input;
        string  m_output;
        Server* m_server;
        uint_t  m_state;
};

class SocketServer : public Socket {
    public:
        SocketServer();
        ~SocketServer();

        bool Bind( const uint_t port, const string addr );
        bool Listen();

    private:
};

#endif
