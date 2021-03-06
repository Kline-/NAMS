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
 * @file telopt.h
 * @brief The telopt namespace.
 *
 * This file contains the Telopt namespace and template functions.
 */
#ifndef DEC_TELOPT_H
#define DEC_TELOPT_H

using namespace std;

/**
 * @brief The Telopt namespace contains telnet option negotiation functions.
 */
namespace Telopt
{
    /** @name Core */ /**@{*/
    #define TELOPT_MSP 90
    static const string opt_telopt_esc = "\e";
    const bool InitialNegotiation( SocketClient* client );
    const string ProcessInput( SocketClient* client, const string& data );
    const string ProcessOutput( SocketClient* client, const string& data );
    /**@}*/

    /** @name Query */ /**@{*/
    /**@}*/

    /** @name Manipulate */ /**@{*/
    static const string opt_cursor_home = opt_telopt_esc + "[H";
    static const string opt_erase_screen = opt_telopt_esc + "[2J";
    /**@}*/

    /** @name Internal */ /**@{*/
    const void Handshake( SocketClient* client, const char& cmd, const char& opt );
    const void Music( SocketClient* client, const string& music, const string& v = "", const string& l = "", const string& c = "", const string& t = "", const string& u = "" );
    const void Negotiate( SocketClient* client, const uint_t& opt, const bool& val, const bool& reply = true );
    const void Send( SocketClient* client, const char& cmd, const char& opt );
    const void Sound( SocketClient* client, const string& sound, const string& v = "", const string& l = "", const string& p = "", const string& t = "", const string& u = "" );
    /**@}*/
};

#endif
