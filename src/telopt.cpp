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
 * @brief Send initial options to negotiate for, then the login string.
 * @param[in] client The client to begin negotiations with.
 * @retval false Returned if there was an error sending data to the client.
 * @retval true Returned if data was successfully sent to the client.
 */
const bool Telopt::InitialNegotiation( SocketClient* client )
{
    UFLAGS_DE( flags );

    if ( client == NULL )
    {
        LOGSTR( flags, "Telopt::InitialNegotiation()-> called with NULL client" );
        return false;
    }

    // negotiate some options here, then send the login string
    Telopt::Negotiate( client, SOC_TELOPT_ECHO, false );

    if ( !client->Send( CFG_STR_LOGIN ) )
    {
        LOGSTR( flags, "Telopt::InitialNegotiation()-> SocketClient::Send()-> returned false" );
        return false;
    }

    return true;
}

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

    if ( client == NULL )
    {
        LOGSTR( flags, "Telopt::ProcessInput()-> called with NULL client" );
        return output;
    }

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
                case (char)IAC: // IAC IAC will result in IAC being passed as data
                    output[o++] = (char)IAC;
                    i++;
                break;

                case (char)DO:
                case (char)DONT:
                case (char)WILL:
                case (char)WONT:
                    Handshake( client, data[i+1], data[i+2] );
                    i += 2; // data[i] == IAC, data[i+1] = cmd, data[i+2] = opt
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

    if ( client == NULL )
    {
        LOGSTR( flags, "Telopt::ProcessOutput()-> called with NULL client" );
        return output;
    }

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
/**
 * @brief Begins a telnet negotiation handshake for requested options.
 * @param[in] client The client requesting a negotiation.
 * @param[in] cmd The telnet command such as IAC, DO, WILL, WONT.
 * @param[in] opt The option following the command.
 * @retval void
 */
const void Telopt::Handshake( SocketClient* client, const char& cmd, const char& opt )
{
    UFLAGS_DE( flags );

    if ( client == NULL )
    {
        LOGSTR( flags, "Telopt::Handshake()-> called with NULL client" );
        return;
    }

    switch ( opt )
    {
        case (char)TELOPT_ECHO:
            if ( cmd == (char)DO )
            {   // PuTTY likes to send IAC DO ECHO on connect which doesn't play nicely with
                // allowing the account name to echo then flipping it for the password
                if ( client->gState() > SOC_STATE_LOGIN_SCREEN )
                    Negotiate( client, SOC_TELOPT_ECHO, true, true );
                else
                    Negotiate( client, SOC_TELOPT_ECHO, false, true );

                client->gTermInfo()->sTelopt( SOC_TELOPT_ECHO, true );
            }
            else if ( cmd == (char)DONT )
            {
                Negotiate( client, SOC_TELOPT_ECHO, false, client->gTermInfo()->gTelopt( SOC_TELOPT_ECHO ) );
                client->gTermInfo()->sTelopt( SOC_TELOPT_ECHO, false );
            }
            else if ( cmd == (char)WILL )
                Telopt::Send( client, (char)DONT, opt );
        break;

        default:
            if ( cmd == (char)WILL )
                Telopt::Send( client, DONT, opt );
            else if ( cmd == (char)DO )
                Telopt::Send( client, WONT, opt );
        break;

    }

    return;
}

/**
 * @brief Negotiate requested options.
 * @param[in] client The client requesting a negotiation.
 * @param[in] opt The option to check from #SOC_TELOPT.
 * @param[in] val The value to check the option for.
 * @param[in] reply If true, a reply will be sent upon completion.
 * @retval void
 */
const void Telopt::Negotiate( SocketClient* client, const uint_t& opt, const bool& val, const bool& reply )
{
    UFLAGS_DE( flags );

    if ( client == NULL )
    {
        LOGSTR( flags, "Telopt::Negotiate()-> called with NULL client" );
        return;
    }

    if ( opt < uintmin_t || opt >= MAX_SOC_TELOPT )
    {
        LOGFMT( flags, "Telopt::Negotiate()-> called with invalid opt: %lu", opt );
        return;
    }

    // Check to see if anything has changed
    if ( client->gTermInfo()->gNegotiated( opt ) != val )
    {
        client->gTermInfo()->sNegotiated( opt, val );

        if ( reply )
        {
            switch ( opt )
            {
                case SOC_TELOPT_ECHO:
                    Telopt::Send( client, val ? WILL : WONT, TELOPT_ECHO );
                break;

                default:
                break;
            }
        }
    }

    return;
}

/**
 * @brief Sends a formatted telnet negotiation command to the client.
 * @param[in] client The client to send data to.
 * @param[in] cmd The command to be sent.
 * @param[in] opt The option to be sent.
 * @retval void
 */
const void Telopt::Send( SocketClient* client, const char& cmd, const char& opt )
{
    UFLAGS_DE( flags );
    char output[4] = {'\0'};

    if ( client == NULL )
    {
        LOGSTR( flags, "Telopt::Send()-> called with NULL client" );
        return;
    }

    output[0] = (char)IAC;
    output[1] = cmd;
    output[2] = opt;
    output[3] = '\0';

    client->Send( output );

    return;
 }
