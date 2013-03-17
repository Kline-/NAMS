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
 * @file server.h
 * @brief The Server class.
 *
 *  This file contains the Server class and template functions.
 */
#ifndef DEC_SERVER_H
#define DEC_SERVER_H

#include <fcntl.h>

#include "socketclient.h"
#include "socketserver.h"

using namespace std;

/**
 * @brief The core of the game server.
 */
class Server {
    /**
     * @brief Runtime settings.
     */
    class Config {
        public:
            /** @name Core */ /**@{*/
            /**@}*/

            /** @name Query */ /**@{*/
            forward_list<string> gProhibitedNames( const uint_t& type ) const;
            /**@}*/

            /** @name Manipulate */ /**@{*/
            const bool Serialize() const;
            const bool Unserialize();
            /**@}*/

            /** @name Internal */ /**@{*/
            Config();
            ~Config();
            /**@}*/

        private:
            forward_list<string> m_prohibited_names[MAX_SVR_CFG_PROHIBITED_NAMES]; /**< Names that are illegal to use as an account name. */
    };

    public:
        /** @name Core */ /**@{*/
        const void Broadcast( const string& msg );
        const bool BuildPlugin( const string& file, const bool& force = false );
        Command* FindCommand( const string& name ) const;
        const bool LoadCommands();
        const bool PollSockets();
        const void ProcessEvents();
        const void ProcessInput();
        const void RebootRecovery( const bool& reboot );
        const bool ReloadCommand( const string& name );
        const bool Running() const;
        const void Startup( const sint_t& desc = 0 );
        const void Shutdown( const sint_t& status );
        const void Update();
        /**@}*/

        /** @name Query */ /**@{*/
        Server::Config* gConfig() const;
        const string gHostname() const;
        const uint_t gPort() const;
        SocketServer* gSocket() const;
        list<SocketClient*>::iterator gSocketClientNext() const;
        const uint_t gSocketClose() const;
        const uint_t gSocketOpen() const;
        const string gStatus() const;
        /**@}*/

        /** @name Manipulate */ /**@{*/
        const bool sPort( const uint_t& port );
        const void sSocketClientNext( list<SocketClient*>::iterator next );
        const bool sSocketClose( const uint_t& amount );
        const bool sSocketOpen( const uint_t& amount );
        /**@}*/

        /** @name Internal */ /**@{*/
        Server();
        ~Server();
        /**@}*/

    private:
        Server::Config* m_config; /**< Runtime configuration settings. */
        uint_t m_dir_close; /**< Total number of directories closed by the Server. */
        uint_t m_dir_open; /**< Total number of directories opened by the Server. */
        uint_t m_port; /**< Port number to be passed to the associated SocketServer. */
        bool m_shutdown; /**< Shutdown state of the Server. */
        SocketServer* m_socket; /**< Pointer to the associated SocketServer object. */
        list<SocketClient*>::iterator m_socket_client_next; /**< Used as the next iterator in all loops dealing with SocketClient objects to prevent nested processing loop problems. */
        uint_t m_socket_close; /**< Total number of SocketClient and SocketServer objects closed by the Server. */
        uint_t m_socket_open; /**< Total number of SocketClient and SocketServer objects opened by the Server. */
        chrono::high_resolution_clock::time_point m_time_boot; /**< Time the Server was first booted. */
        chrono::high_resolution_clock::time_point m_time_current; /**< Current time from the host OS. */
};

#endif
