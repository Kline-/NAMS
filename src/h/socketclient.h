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

#include "socket.h"

using namespace std;

/**
 * @brief Client / remote host specific functions that extend the Socket class.
 */
class SocketClient : public Socket {
    /**
     * @brief Remote terminal information and configuration.
     */
    class TermInfo {
        public:
            /** @name Core */ /**@{*/
            /**@}*/

            /** @name Query */ /**@{*/
            const bool gNegotiated( const uint_t& opt ) const;
            const bool gTelopt( const uint_t& opt ) const;
            /**@}*/

            /** @name Manipulate */ /**@{*/
            const bool sNegotiated( const uint_t& opt, const bool& val );
            const bool sTelopt( const uint_t& opt, const bool& val );
            /**@}*/

            /** @name Internal */ /**@{*/
            TermInfo();
            ~TermInfo();
            /**@}*/

        private:
            bool m_negotiated[MAX_SOC_TELOPT]; /**< Telnet options that have been negotiated previously. */
            bool m_telopt[MAX_SOC_TELOPT]; /**< Supported telnet options. */
    };

    public:
        /** @name Core */ /**@{*/
        const void Delete();
        const bool New( Server* server, const sint_t& descriptor, const bool& reboot = false );
        const bool PendingCommand() const;
        const bool PendingOutput() const;
        const bool ProcessCommand();
        const bool ProcessInput();
        const bool QueueCommand( const string& command );
        const void Quit();
        const bool Quitting() const;
        const bool Recv();
        const void ResolveHostname();
        const bool Send();
        const bool Send( const string& msg );
        const string Serialize() const;
        /**@}*/

        /** @name Query */ /**@{*/
        Account* gAccount() const;
        const uint_t gIdle() const;
        const string gLogin( const uint_t& key ) const;
        const uint_t gState() const;
        SocketClient::TermInfo* gTermInfo() const;
        /**@}*/

        /** @name Manipulate */ /**@{*/
        const bool sAccount( Account* account );
        const bool sIdle( const uint_t& idle );
        const bool sLogin( const uint_t& key, const string& val);
        static void* tResolveHostname( void* data );
        const bool sState( const uint_t& state );
        /**@}*/

        /** @name Internal */ /**@{*/
        SocketClient();
        ~SocketClient();
        /**@}*/

    private:
        Account* m_account; /**< The account associated with the client. */
        deque< pair<string,string> > m_command_queue; /**< FIFO queue for commands unless the command specifies preempt. */
        uint_t m_idle; /**< Amount of time since data was last received from the client. */
        string m_input; /**< Data received from the client that hasn't been processed yet. */
        string m_login[MAX_SOC_LOGIN]; /**< Used to pass login process commands and arguments between handler functions. */
        string m_output; /**< Data buffered for sending to the client. */
        bool m_quitting; /**< Flag the client as quitting to escape nested loops. */
        uint_t m_state; /**< Connection state of the client from #SOC_STATE. */
        SocketClient::TermInfo* m_terminfo; /**< Terminal options and status. */
};

#endif
