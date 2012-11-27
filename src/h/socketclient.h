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
 *  This file contains the SocketClient class and template functions.
 */
#ifndef DEC_SOCKETCLIENT_H
#define DEC_SOCKETCLIENT_H

#include <deque>
#include <netdb.h>

#include "socket.h"
#include "server.h"

using namespace std;

/**
 * @brief Client / remote host specific functions that extend the Socket class.
 */
class SocketClient : public Socket {
    public:
        /** @name Core */ /**@{*/
        const void Delete();
        const bool New();
        const bool PendingCommand() const;
        const bool PendingOutput() const;
        const bool ProcessCommand();
        const bool ProcessInput();
        const bool QueueCommand( const string& command );
        const bool Recv();
        const void ResolveHostname();
        const bool Send();
        const bool Send( const string& msg );
        /**@}*/

        /** @name Query */ /**@{*/
        const uint_t gIdle() const;
        Server* gServer() const;
        const uint_t gState() const;
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
        deque< pair<string,string> > m_command_queue; /**< FIFO queue for commands unless the command specifies preempt. */
        uint_t m_idle;                              /**< Amount of time since data was last received from the client. */
        string m_input;                             /**< Data received from the client that hasn't been processed yet. */
        string m_output;                            /**< Data buffered for sending to the client. */
        Server* m_server;                           /**< The Server object associated to this client. */
        uint_t m_state;                             /**< Connection state of the client from #SOC_STATE. */
};

#endif
