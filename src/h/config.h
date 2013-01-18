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
 * @file config.h
 * @brief NAMS configuration options.
 *
 *  All NAMS configuration options are kept within this file. This includes
 *  defines for specific folders, memory tuning, frequently used string
 *  messages, etc.
 */
#ifndef DEC_CONFIG_H
#define DEC_CONFIG_H

/***************************************************************************
 *                              DATA OPTIONS                               *
 ***************************************************************************/
/** @name Data Options */ /**@{*/
/**
 * @def CFG_DAT_DIR_COMMAND
 * @brief Directory for commands to be loaded from.
 * @par Default: "command"
 */
#define CFG_DAT_DIR_COMMAND     "command"

/**
 * @def CFG_DAT_DIR_ETC
 * @brief Directory for misc files to be written to and loaded from.
 * @par Default: "etc"
 */
#define CFG_DAT_DIR_ETC         "etc"

/**
 * @def CFG_DAT_DIR_LOG
 * @brief Directory for logfiles to be written to.
 * @par Default: "log"
 */
#define CFG_DAT_DIR_LOG         "log"

/**
 * @def CFG_DAT_DIR_OBJ
 * @brief Directory for plugins to be loaded from.
 * @par Default: "obj"
 */
#define CFG_DAT_DIR_OBJ         "obj"

/**
 * @def CFG_DAT_FILE_REBOOT
 * @brief File for reboot data to be temporarily stored in.
 * @par Default: CFG_DAT_DIR_ETC "/reboot.dat"
 */
#define CFG_DAT_FILE_REBOOT     CFG_DAT_DIR_ETC "/reboot.dat"
/**@}*/

/***************************************************************************
 *                              GAME OPTIONS                               *
 ***************************************************************************/
/** @name Game Options */ /**@{*/
/**
 * @def CFG_GAM_CMD_IGNORE_CASE
 * @brief Ignore case sensitivity on input from sockets.
 * @par Default: true
 */
#define CFG_GAM_CMD_IGNORE_CASE true

/**
 * @def CFG_GAM_PULSE_RATE
 * @brief How many cycles per second should be processed.
 * @par Default: 100
 */
#define CFG_GAM_PULSE_RATE      100
/**@}*/

/***************************************************************************
 *                              LOG OPTIONS                                *
 ***************************************************************************/
/** @name Log Options */ /**@{*/
/**
 * @def CFG_LOG_SERVER
 * @brief Default log for boot / shutdown and general information.
 * @par Default: "server.log"
 */
#define CFG_LOG_SERVER          "server.log"
/**@}*/

/***************************************************************************
 *                              PLUGIN OPTIONS                             *
 ***************************************************************************/
/** @name Plugin Options */ /**@{*/
/**
 * @def CFG_PLG_BUILD_CMD
 * @brief The compiler command to use when building plugins.
 * @par Default: "g++"
 */
#define CFG_PLG_BUILD_CMD       "g++"

/**
 * @def CFG_PLG_BUILD_OPT
 * @brief All build options passed during compiling a plugin.
 * @par Default: "-Isrc/h -fpic -ldl -rdynamic -shared 2>&1"
 */
#define CFG_PLG_BUILD_OPT       "-Isrc/h -fpic -ldl -rdynamic -shared 2>&1"

/**
 * @def CFG_PLG_BUILD_EXT_IN
 * @brief File extension for files to be compiled.
 * @par Default: "cpp"
 */
#define CFG_PLG_BUILD_EXT_IN    "cpp"

/**
 * @def CFG_PLG_BUILD_EXT_OUT
 * @brief File extension for files after they are compiled.
 * @par Default: "so"
 */
#define CFG_PLG_BUILD_EXT_OUT   "so"

/**
 * @def CFG_PLG_MAX_ARR
 * @brief The maximum number of elements for each data type array in a plugin.
 * @par Default: 5
 */
#define CFG_PLG_MAX_ARR         5
/**@}*/

/***************************************************************************
 *                              MEMORY OPTIONS                             *
 ***************************************************************************/
/** @name Memory Options */ /**@{*/
/**
 * @def CFG_MEM_MAX_BITSET
 * @brief Maximum size of all bitset elements within the server.
 * @par Default: 128
 */
#define CFG_MEM_MAX_BITSET      128
/**@}*/

/***************************************************************************
 *                              SOCKET OPTIONS                             *
 ***************************************************************************/
/** @name Socket Options */ /**@{*/
/**
 * @def CFG_SOC_BIND_ADDR
 * @brief IP address to bind the listening server socket to.
 * @par Default: "::"
 */
#define CFG_SOC_BIND_ADDR       "::"

/**
 * @def CFG_SOC_MIN_PORTNUM
 * @brief The listening server socket must be above this port.
 * @par Default: 1024
 */
#define CFG_SOC_MIN_PORTNUM     1024

/**
 * @def CFG_SOC_MAX_IDLE
 * @brief (Minutes / tick rate) before disconneting an idle socket. Approx 30 minutes.
 * @par Default: ( ( 30 * 60 * USLEEP_MAX ) / ( USLEEP_MAX / CFG_GAM_PULSE_RATE ) )
 */
#define CFG_SOC_MAX_IDLE        ( ( 30 * 60 * USLEEP_MAX ) / ( USLEEP_MAX / CFG_GAM_PULSE_RATE ) )

/**
 * @def CFG_SOC_MAX_PENDING
 * @brief The maximum number of pending connections to allow in the listen backlog.
 * @par Default: 5
 */
#define CFG_SOC_MAX_PENDING     5

/**
 * @def CFG_SOC_MAX_PORTNUM
 * @brief The listening server socket must be below this port.
 * @par Default: 65536
 */
#define CFG_SOC_MAX_PORTNUM     65536

/**
 * @def CFG_SOC_PORTNUM
 * @brief Port number to listen on if not specified on the command line.
 * @par Default: 4321
 */
#define CFG_SOC_PORTNUM         4321
/**@}*/

/***************************************************************************
 *                              STRING OPTIONS                             *
 ***************************************************************************/
/** @name String Options */ /**@{*/
/**
 * @def CFG_STR_IDLE
 * @brief String sent on disconnecting an idle socket.
 * @par Default: CRLF "Idle limit reached. Goodbye." CRLF
 */
#define CFG_STR_IDLE            CRLF "Idle limit reached. Goodbye." CRLF

/**
 * @def CFG_STR_LOGIN
 * @brief String sent on initial socket connection.
 * @par Default: "For a list of commands type 'help'." CRLF "Please enter your name: "
 */
#define CFG_STR_LOGIN           "For a list of commands type 'help'." CRLF "Please enter your name: "

/**
 * @def CFG_STR_CMD_INVALID
 * @brief String sent when an invalid command is received.
 * @par Default: "Invalid command." CRLF
 */
#define CFG_STR_CMD_INVALID     "Invalid command." CRLF

/**
 * @def CFG_STR_MAX_BUFLEN
 * @brief Maximum length of all char type buffers.
 * @par Default: 16384
 */
#define CFG_STR_MAX_BUFLEN      16384

/**
 * @def CFG_STR_EXIT_FAILURE
 * @brief String to write to log on EXIT_FAILURE.
 * @par Default: "Server terminated."
 */
#define CFG_STR_EXIT_FAILURE    "Server terminated."

/**
 * @def CFG_STR_EXIT_SUCCESS
 * @brief String to write to log on EXIT_SUCCESS.
 * @par Default: "Normal termination of server."
 */
#define CFG_STR_EXIT_SUCCESS    "Normal termination of server."

/**
 * @def CFG_STR_QUIT
 * @brief String sent to the client when they quit.
 * @par Default: "Goodbye." CRLF
 */
#define CFG_STR_QUIT            "Goodbye." CRLF

/**
 * @def CFG_STR_SHUTDOWN
 * @brief String broadcast to all clients when the server is shutting down.
 * @par Default: "Server shutting down." CRLF
 */
#define CFG_STR_SHUTDOWN        "Server shutting down." CRLF

/**
 * @def CFG_STR_UTILS_ERROR
 * @brief String to prepend to logs flagged UTILS_TYPE_ERROR.
 * @par Default: "[ERROR ] "
 */
#define CFG_STR_UTILS_ERROR     "[ERROR] "

/**
 * @def CFG_STR_UTILS_INFO
 * @brief String to prepend to logs flagged UTILS_TYPE_INFO.
 * @par Default: "[INFO  ] "
 */
#define CFG_STR_UTILS_INFO      "[INFO  ] "

/**
 * @def CFG_STR_UTILS_SOCKET
 * @brief String to prepend to logs flagged UTILS_TYPE_SOCKET.
 * @par Default: "[SOCKET] "
 */
#define CFG_STR_UTILS_SOCKET    "[SOCKET] "

/**
 * @def CFG_STR_VERSION
 * @brief The current server version.
 * @par Default: "NAMS Development 0.0.0"
 */
#define CFG_STR_VERSION         "NAMS Development 0.0.0"
/**@}*/

#endif
