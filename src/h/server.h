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
#ifndef DEC_SERVER_H
#define DEC_SERVER_H

#include <fcntl.h>

#include "socketclient.h"
#include "socketserver.h"
#include "list.h"

class Server {
    public:
        Server();
        ~Server();

        // Core
        bool InitSocket( SocketServer* socket_server );
        bool LoadCommands();
        const void NewConnection();
        bool PollSockets();
        bool ProcessInput();
        bool Running() const { return !m_shutdown; }
        const void Startup();
        const void Shutdown( const sint_t status );
        const void Update();

        // Query
        string gHostname();
        uint_t gPort() const { return m_port; }
        uint_t gPulseRate() const { return m_pulse_rate; }
        string gTimeBoot() const;
        string gTimeCurrent() const;
        SocketServer* gSocket() const { return m_socket; }
        uint_t gSocketClose() const { return m_socket_close; }

        // Manipulate
        bool sPort( const uint_t port );
        bool sPulseRate( const uint_t rate );
        bool sSocketClose( const uint_t amount );
        const void sTimeBoot();
        const void sTimeCurrent();

    private:
        uint_t  m_dir_close;
        uint_t  m_dir_open;
        uint_t  m_port;
        uint_t  m_pulse_rate;
        bool    m_shutdown;
        SocketServer* m_socket;
        ITER( list, SocketClient*, m_socket_client_next );
        uint_t  m_socket_close;
        uint_t  m_socket_open;
        time_t  m_time_boot;
        time_t  m_time_current;
};

#endif
