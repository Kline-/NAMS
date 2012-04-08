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
#ifndef DEC_CONFIG_H
#define DEC_CONFIG_H

/***************************************************************************
 *                              DATA OPTIONS                               *
 ***************************************************************************/
// CFG_DAT_DIR_COMMAND -- Directory for commands to be loaded from; should contain subdirs a-z
//  Default: "command"
#define CFG_DAT_DIR_COMMAND     "command"

/***************************************************************************
 *                              GAME OPTIONS                               *
 ***************************************************************************/
// CFG_GAM_PULSE_RATE -- How many cycles per second should be processed.
//  Default: 100
#define CFG_GAM_PULSE_RATE      100

/***************************************************************************
 *                              MEMORY OPTIONS                             *
 ***************************************************************************/
// CFG_MEM_MAX_BITSET -- Maximum size of all bitset elements within the server.
//  Default: 128
#define CFG_MEM_MAX_BITSET      128

/***************************************************************************
 *                              SOCKET OPTIONS                             *
 ***************************************************************************/
// CFG_SOC_BIND_ADDR -- IP address to bind the listening server socket to.
//  Default: "0.0.0.0"
#define CFG_SOC_BIND_ADDR       "0.0.0.0"

// CFG_SOC_MIN_PORTNUM -- The listening server socket must be above this port.
//  Default: 1024
#define CFG_SOC_MIN_PORTNUM     1024

// CFG_SOC_MAX_IDLE -- (Minutes / tick rate) before disconneting an idle socket. Approx 30 minutes.
//  Default: ( ( 30 * 60 * USLEEP_MAX ) / ( USLEEP_MAX / CFG_GAM_PULSE_RATE ) )
#define CFG_SOC_MAX_IDLE        ( ( 30 * 60 * USLEEP_MAX ) / ( USLEEP_MAX / CFG_GAM_PULSE_RATE ) )

// CFG_SOC_MAX_PENDING -- The maximum number of pending connections to allow in the listen backlog.
//  Default: 5
#define CFG_SOC_MAX_PENDING     5

// CFG_SOC_MAX_PORTNUM -- The listening server socket must be below this port.
//  Default: 65536
#define CFG_SOC_MAX_PORTNUM     65536

// CFG_SOC_PORTNUM -- Port number to listen on if not specified on the command line.
//  Default: 4321
#define CFG_SOC_PORTNUM         4321

/***************************************************************************
 *                              STRING OPTIONS                             *
 ***************************************************************************/
// CFG_STR_IDLE -- String sent on disconnecting an idle socket.
//  Default: CRLF "Idle limit reached. Goodbye."
#define CFG_STR_IDLE            CRLF "Idle limit reached. Goodbye." CRLF
// CFG_STR_LOGIN -- String sent on initial socket connection.
//  Default: "For a list of commands type 'help'." CRLF "Please enter your name: "
#define CFG_STR_LOGIN           "For a list of commands type 'help'." CRLF "Please enter your name: "

// CFG_STR_MAX_BUFLEN -- Maximum length of all char type buffers.
//  Default: 16384
#define CFG_STR_MAX_BUFLEN      16384

// CFG_STR_UTILS_ERROR -- String to prepend to logs flagged UTILS_TYPE_ERROR.
//  Default: "[ERROR ] "
#define CFG_STR_UTILS_ERROR     "[ERROR] "

// CFG_STR_UTILS_INFO -- String to prepend to logs flagged UTILS_TYPE_INFO.
//  Default: "[INFO  ] "
#define CFG_STR_UTILS_INFO      "[INFO  ] "

// CFG_STR_UTILS_SOCKET -- String to prepend to logs flagged UTILS_TYPE_SOCKET.
//  Default: "[SOCKET] "
#define CFG_STR_UTILS_SOCKET    "[SOCKET] "

// CFG_STR_VERSION -- The current server version.
//  Default: "NAMS 1.0.0"
#define CFG_STR_VERSION         "NAMS 1.0.0"

#endif
