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
 * @file telopt.cpp
 * @brief All non-template member functions of the Telopt namespace.
 *
 * The Telopt namespace contains telnet option negotiation functions.
 */
#include "h/includes.h"
#include "h/class.h"

/* Core */
/**
 * @brief Processes data received from a SocketClient for telnet sequences.
 * @param[in] client The SocketClient to process data for.
 * @param[in] data The received data to be processed.
 * retval string A string that has been processed for telnet sequences.
 */
const string Telopt::ProcessInput( SocketClient* client, const string& data )
{
    UFLAGS_DE( flags );
    uint_t i = uintmin_t, o = uintmin_t;
    char output[CFG_STR_MAX_BUFLEN] = {'\0'};

    if ( data.empty() )
        return output;

    if ( data.length() >= CFG_STR_MAX_BUFLEN )
        LOGFMT( flags, "Telopt::ProcessInput()-> data length of %lu exceeds buffer length of %lu, truncating", data.length(), CFG_STR_MAX_BUFLEN );

    for ( i = 0; i < data.length() && i < CFG_STR_MAX_BUFLEN; i++ )
    {
        if ( data[i] == (char)IAC )
        {
            switch ( data[i+1] )
            {
                case (char)IAC:
                    output[o++] = (char)IAC;
                    i++;
                break;

                default:
                    i += 2;
                break;
            }
        }
        else
            output[o++] = data[i];
    }

    return output;
}

/**
 * @brief Processes data to be sent to a SocketClient for telnet sequences.
 * @param[in] client The SocketClient to process data for.
 * @param[in] data The received data to be processed.
 * @retval string A string that has been processed for telnet sequences.
 */
const string Telopt::ProcessOutput( SocketClient* client, const string& data )
{
    UFLAGS_DE( flags );
    uint_t i = uintmin_t, o = uintmin_t;
    char output[CFG_STR_MAX_BUFLEN] = {'\0'};

    if ( data.empty() )
        return output;

    if ( data.length() >= CFG_STR_MAX_BUFLEN )
        LOGFMT( flags, "Telopt::ProcessOutput()-> data length of %lu exceeds buffer length of %lu, truncating", data.length(), CFG_STR_MAX_BUFLEN );

    for ( i = 0; i < data.length() && i < CFG_STR_MAX_BUFLEN; i++ )
    {
        output[o++] = data[i];
    }

    return output;
}

/* Query */

/* Manipulate */

/* Internal */
