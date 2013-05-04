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
 *                             ACCOUNT OPTIONS                             *
 ***************************************************************************/
/** @name Account Options */ /**@{*/
/**
 * @def CFG_ACT_CHARACTER_MAX
 * @brief The maximum number of characters an account may have.
 * @par Default: 10
 */
#define CFG_ACT_CHARACTER_MAX 10

/**
 * @def CFG_ACT_LOGIN_MAX
 * @brief The number of previous hosts to track for login history.
 * @par Default: 3
 */
#define CFG_ACT_LOGIN_MAX 3

/**
 * @def CFG_ACT_NAME_MAX_LEN
 * @brief The maximum allowable length for an account name.
 * @par Default: 32
 */
#define CFG_ACT_NAME_MAX_LEN 32

/**
 * @def CFG_ACT_NAME_MIN_LEN
 * @brief The minimum allowable length for an account name.
 * @par Default: 4
 */
#define CFG_ACT_NAME_MIN_LEN 4

/**
 * @def CFG_ACT_PASSWORD_MAX_LEN
 * @brief The maximum allowable length for an account password.
 * @par Default: 32
 */
#define CFG_ACT_PASSWORD_MAX_LEN 32

/**
 * @def CFG_ACT_PASSWORD_MIN_LEN
 * @brief The minimum allowable length for an account password.
 * @par Default: 4
 */
#define CFG_ACT_PASSWORD_MIN_LEN 4
/**@}*/

/***************************************************************************
 *                            CHARACTER OPTIONS                            *
 ***************************************************************************/
/** @name Character Options */ /**@{*/
/**
 * @def CFG_CHR_MAX_IDLE
 * @brief (Minutes / tick rate) before disconnecting an idle Character detached from an Account. Approx 5 minutes.
 * @par Default: ( ( 5 * 60 * USLEEP_MAX ) / ( USLEEP_MAX / CFG_GAM_PULSE_RATE ) )
 */
#define CFG_CHR_MAX_IDLE ( ( 5 * 60 * USLEEP_MAX ) / ( USLEEP_MAX / CFG_GAM_PULSE_RATE ) )
/**@}*/

/***************************************************************************
 *                              DATA OPTIONS                               *
 ***************************************************************************/
/** @name Data Options */ /**@{*/
/**
 * @def CFG_DAT_CHR_UNLINK
 * @brief If true, will unlink character files on deletion. If false, character files will be retained on disk and only de-associated from the account.
 * @par Default: false
 */
#define CFG_DAT_CHR_UNLINK false

/**
 * @def CFG_DAT_DIR_ACCOUNT
 * @brief Directory for accounts to be loaded from.
 * @par Default: "account"
 */
#define CFG_DAT_DIR_ACCOUNT "account"

/**
 * @def CFG_DAT_DIR_COMMAND
 * @brief Directory for commands to be loaded from.
 * @par Default: "command"
 */
#define CFG_DAT_DIR_COMMAND "command"

/**
 * @def CFG_DAT_DIR_ETC
 * @brief Directory for misc files to be written to and loaded from.
 * @par Default: "etc"
 */
#define CFG_DAT_DIR_ETC "etc"

/**
 * @def CFG_DAT_DIR_LOG
 * @brief Directory for logfiles to be written to.
 * @par Default: "log"
 */
#define CFG_DAT_DIR_LOG "log"

/**
 * @def CFG_DAT_DIR_OBJ
 * @brief Directory for plugins to be loaded from.
 * @par Default: "obj"
 */
#define CFG_DAT_DIR_OBJ "obj"

/**
 * @def CFG_DAT_DIR_VAR
 * @brief Directory for temporary files.
 * @par Default: "var"
 */
#define CFG_DAT_DIR_VAR "var"

/**
 * @def CFG_DAT_DIR_WORLD
 * @brief Directory for game world files: rooms, objects, NPCs, etc.
 * @par Default: "world"
 */
#define CFG_DAT_DIR_WORLD "world"

/**
 * @def CFG_DAT_FILE_ACT_EXT
 * @brief File extension to use for account setting files.
 * @par Default: "act"
 */
#define CFG_DAT_FILE_ACT_EXT "act"

/**
 * @def CFG_DAT_FILE_PLR_EXT
 * @brief File extension to use for player character files.
 * @par Default: "plr"
 */
#define CFG_DAT_FILE_PLR_EXT "plr"

/**
 * @def CFG_DAT_FILE_LOC_EXT
 * @brief File extension to use for location files.
 * @par Default: "loc"
 */
#define CFG_DAT_FILE_LOC_EXT "loc"

/**
 * @def CFG_DAT_FILE_REBOOT
 * @brief File for reboot data to be temporarily stored in.
 * @par Default: "reboot.dat"
 */
#define CFG_DAT_FILE_REBOOT "reboot.dat"

/**
 * @def CFG_DAT_FILE_SETTINGS
 * @brief File for runtime and account settings.
 * @par Default: "settings.dat"
 */
#define CFG_DAT_FILE_SETTINGS "settings.dat"

/**
 * @def CFG_DAT_STR_CTR_A
 * @brief Delimeter to use before writing a container wrapped string.
 * @par Default: "{\""
 */
#define CFG_DAT_STR_CTR_A "{\""

/**
 * @def CFG_DAT_STR_CTR_B
 * @brief Delimeter to use when writing an object pair of data.
 * @par Default: "\":\""
 */
#define CFG_DAT_STR_CTR_B "\":\""

/**
 * @def CFG_DAT_STR_CTR_C
 * @brief Delimeter to use after writing a container wrapped string.
 * @par Default: "\"}"
 */
#define CFG_DAT_STR_CTR_C "\"}"
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
#define CFG_GAM_PULSE_RATE 100
/**@}*/

/***************************************************************************
 *                             LOCATION OPTIONS                            *
 ***************************************************************************/
/** @name Location Options */ /**@{*/
/**
 * @def CFG_LOC_ID_START
 * @brief The initial location to place characters in upon first entering the game.
 * @par Default: "sys.entrypoint"
 */
#define CFG_LOC_ID_START "sys.entrypoint"
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
#define CFG_LOG_SERVER "server.log"
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
#define CFG_PLG_BUILD_CMD "g++"

/**
 * @def CFG_PLG_BUILD_OPT
 * @brief All build options passed during compiling a plugin.
 * @par Default: "-std=c++0x -Isrc/h -fpic -ldl -rdynamic -shared 2>&1"
 */
#define CFG_PLG_BUILD_OPT "-std=c++0x -Isrc/h -fpic -ldl -rdynamic -shared 2>&1"

/**
 * @def CFG_PLG_BUILD_EXT_IN
 * @brief File extension for files to be compiled.
 * @par Default: "cpp"
 */
#define CFG_PLG_BUILD_EXT_IN "cpp"

/**
 * @def CFG_PLG_BUILD_EXT_OUT
 * @brief File extension for files after they are compiled.
 * @par Default: "so"
 */
#define CFG_PLG_BUILD_EXT_OUT "so"

/**
 * @def CFG_PLG_MAX_ARR
 * @brief The maximum number of elements for each data type array in a plugin.
 * @par Default: 5
 */
#define CFG_PLG_MAX_ARR 5
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
#define CFG_MEM_MAX_BITSET 128
/**@}*/

/***************************************************************************
 *                            SECURITY OPTIONS                             *
 ***************************************************************************/
/** @name Security Options */ /**@{*/
/**
 * @def CFG_SEC_CRYPT_METHOD
 * @brief The algorithm to use with crypt(). See man 3 crypt for details.
 * @par Default: 6
 */
#define CFG_SEC_CRYPT_METHOD 6

/**
 * @def CFG_SEC_CRYPT_SALT
 * @brief The initial salt string to use with crypt().
 * @par Default: "$" CFG_SEC_CRYPT_METHOD "$"
 */
#define CFG_SEC_CRYPT_SALT "$" SX( CFG_SEC_CRYPT_METHOD ) "$"

/**
 * @def CFG_SEC_DIR_MODE
 * @brief The chmod mode to set on directories (accounts) created by the server.
 * @par Default: 0755
 */
#define CFG_SEC_DIR_MODE 0755
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
#define CFG_SOC_BIND_ADDR "::"

/**
 * @def CFG_SOC_MIN_PORTNUM
 * @brief The listening server socket must be above this port.
 * @par Default: 1024
 */
#define CFG_SOC_MIN_PORTNUM 1024

/**
 * @def CFG_SOC_MAX_IDLE
 * @brief (Minutes / tick rate) before disconnecting an idle socket. Approx 30 minutes.
 * @par Default: ( ( 30 * 60 * USLEEP_MAX ) / ( USLEEP_MAX / CFG_GAM_PULSE_RATE ) )
 */
#define CFG_SOC_MAX_IDLE ( ( 30 * 60 * USLEEP_MAX ) / ( USLEEP_MAX / CFG_GAM_PULSE_RATE ) )

/**
 * @def CFG_SOC_MAX_PENDING
 * @brief The maximum number of pending connections to allow in the listen backlog.
 * @par Default: 5
 */
#define CFG_SOC_MAX_PENDING 5

/**
 * @def CFG_SOC_MAX_PORTNUM
 * @brief The listening server socket must be below this port.
 * @par Default: 65536
 */
#define CFG_SOC_MAX_PORTNUM 65536

/**
 * @def CFG_SOC_PORTNUM
 * @brief Port number to listen on if not specified on the command line.
 * @par Default: 4321
 */
#define CFG_SOC_PORTNUM 4321
/**@}*/

/***************************************************************************
 *                              STRING OPTIONS                             *
 ***************************************************************************/
/** @name String Options */ /**@{*/
/**
 * @def CFG_STR_ACT_CHR_LIMIT
 * @brief String sent if the account character limit has been reached.
 * @par Default: "Accounts are limited to %d characters. Please delete an existing character to create a new one." CRLF
 */
#define CFG_STR_ACT_CHR_LIMIT "Accounts are limited to %d characters. Please delete an existing character to create a new one." CRLF

/**
 * @def CFG_STR_ACT_CHR_NONE
 * @brief String sent if the account has no characters to delete.
 * @par Default: "There are no characters associated with this account." CRLF
 */
#define CFG_STR_ACT_CHR_NONE "There are no characters associated with this account." CRLF

/**
 * @def CFG_STR_ACT_NAME_ALNUM
 * @brief String additionally sent if an account name is invalid due to non-alphanumeric characters.
 * @par Default: "Account name must be alphanumeric characters only." CRLF
 */
#define CFG_STR_ACT_NAME_ALNUM "Account name must be alphanumeric characters only." CRLF

/**
 * @def CFG_STR_ACT_NAME_CONFIRM
 * @brief String sent to request the user confirms their new account name.
 * @par Default: "Do you wish to create a new account named %s (Y/N)? "
 */
#define CFG_STR_ACT_NAME_CONFIRM "Do you wish to create a new account named %s (Y/N)? "

/**
 * @def CFG_STR_ACT_NAME_GET
 * @brief String sent to request the user enters their account name.
 * @par Default: "Please enter your account name: "
 */
#define CFG_STR_ACT_NAME_GET "Please enter your account name: "

/**
 * @def CFG_STR_ACT_NAME_INVALID
 * @brief String sent when an invalid account name is received.
 * @par Default: "Invalid account name." CRLF
 */
#define CFG_STR_ACT_NAME_INVALID "Invalid account name." CRLF

/**
 * @def CFG_STR_ACT_NAME_LENGTH
 * @brief String additionally sent when an account name is invalid due to length.
 * @par Default: "Account name must be between %d and %d characters." CRLF
 */
#define CFG_STR_ACT_NAME_LENGTH "Account name must be between %d and %d characters." CRLF

/**
 * @def CFG_STR_ACT_NAME_PROHIBITED
 * @brief String additionally sent when an account name matches the prohibited names list.
 * @par Default: "That account name has been prohibited from use." CRLF
 */
#define CFG_STR_ACT_NAME_PROHIBITED "That account name has been prohibited from use." CRLF

/**
 * @def CFG_STR_ACT_NEW
 * @brief String sent when a new account will be created.
 * @par Default: "New account." CRLF
 */
#define CFG_STR_ACT_NEW "New account." CRLF

/**
 * @def CFG_STR_ACT_NEW_ERROR
 * @brief String sent if an error occurs when first creating a new account.
 * @par Default: "An error occurred while attempting to create that account." CRLF
 */
#define CFG_STR_ACT_NEW_ERROR "An error occurred while attempting to create that account." CRLF

/**
 * @def CFG_STR_ACT_PASSWORD_CONFIRM
 * @brief String sent to request the user confirms their new account password.
 * @par Default: CRLF "Please confirm password: "
 */
#define CFG_STR_ACT_PASSWORD_CONFIRM CRLF "Please confirm password: "

/**
 * @def CFG_STR_ACT_PASSWORD_GET
 * @brief String sent to request the user inputs their password.
 * @par Default: "Password: "
 */
#define CFG_STR_ACT_PASSWORD_GET "Password: "

/**
 * @def CFG_STR_ACT_PASSWORD_INVALID
 * @brief String sent when an invalid account password is received.
 * @par Default: CRLF "Invalid account password." CRLF
 */
#define CFG_STR_ACT_PASSWORD_INVALID CRLF "Invalid account password." CRLF

/**
 * @def CFG_STR_ACT_PASSWORD_LENGTH
 * @brief String additionally sent when an account password is invalid due to length.
 * @par Default: "Account password must be between %d and %d characters." CRLF
 */
#define CFG_STR_ACT_PASSWORD_LENGTH "Account password must be between %d and %d characters." CRLF

/**
 * @def CFG_STR_ACT_PASSWORD_MISMATCH
 * @brief String sent when a new password confirmation fails.
 * @par Default: CRLF "Passwords don't match." CRLF
 */
#define CFG_STR_ACT_PASSWORD_MISMATCH CRLF "Passwords don't match." CRLF

/**
 * @def CFG_STR_CHR_DELETE_CONFIRM
 * @brief String sent to confirm the deletion of a character.
 * @par Default: "To confirm deletion enter 'yes'; to abort, enter anything else." CRLF "Do you wish to delete %s? "
 */
#define CFG_STR_CHR_DELETE_CONFIRM "To confirm deletion enter 'yes'; to abort, enter anything else." CRLF "Do you wish to delete %s? "

/**
 * @def CFG_STR_CHR_NAME_ALNUM
 * @brief String additionally sent if a character name is invalid due to non-alphanumeric characters.
 * @par Default: "Name must be alphanumeric characters only." CRLF
 */
#define CFG_STR_CHR_NAME_ALNUM "Name must be alphanumeric characters only." CRLF

/**
 * @def CFG_STR_CHR_NAME_EXISTS
 * @brief String sent if an account tries to create a duplicate character name.
 * @par Default: "That character already exists in this account." CRLF
 */
#define CFG_STR_CHR_NAME_EXISTS "That character already exists in this account." CRLF

/**
 * @def CFG_STR_CHR_NAME_GET
 * @brief String sent to request the user enters their character name.
 * @par Default: "Name: "
 */
#define CFG_STR_CHR_NAME_GET "Name: "

/**
 * @def CFG_STR_CHR_NAME_INVALID
 * @brief String sent when an invalid character name is received.
 * @par Default: "Invalid name." CRLF
 */
#define CFG_STR_CHR_NAME_INVALID "Invalid name." CRLF

/**
 * @def CFG_STR_CHR_NAME_LENGTH
 * @brief String additionally sent when a character name is invalid due to length.
 * @par Default: "Name must be between %d and %d characters." CRLF
 */
#define CFG_STR_CHR_NAME_LENGTH "Name must be between %d and %d characters." CRLF

/**
 * @def CFG_STR_CHR_NAME_PROHIBITED
 * @brief String additionally sent when a character name matches the prohibited names list.
 * @par Default: "That name has been prohibited from use." CRLF
 */
#define CFG_STR_CHR_NAME_PROHIBITED "That name has been prohibited from use." CRLF

/**
 * @def CFG_STR_CHR_NEW_ERROR
 * @brief String sent if an error occurs when first creating a new character.
 * @par Default: "An error occurred while attempting to create that character." CRLF
 */
#define CFG_STR_CHR_NEW_ERROR "An error occurred while attempting to create that character." CRLF

/**
 * @def CFG_STR_CHR_RECONNECTED
 * @brief String sent upon resuming a linkdead character.
 * @par Default: "You take over your body, which was already in use." CRLF
 */
#define CFG_STR_CHR_RECONNECTED "You take over your body, which was already in use." CRLF

/**
 * @def CFG_STR_CMD_INVALID
 * @brief String sent when an invalid command is received.
 * @par Default: "Invalid command." CRLF
 */
#define CFG_STR_CMD_INVALID "Invalid command." CRLF

/**
 * @def CFG_STR_EXIT_FAILURE
 * @brief String to write to log on EXIT_FAILURE.
 * @par Default: "Server terminated."
 */
#define CFG_STR_EXIT_FAILURE "Server terminated."

/**
 * @def CFG_STR_EXIT_SUCCESS
 * @brief String to write to log on EXIT_SUCCESS.
 * @par Default: "Normal termination of server."
 */
#define CFG_STR_EXIT_SUCCESS "Normal termination of server."

/**
 * @def CFG_STR_FILE_COMMAND_READ
 * @brief String to output prior to loading command plugins.
 * @par Default: "Loading commands..."
 */
#define CFG_STR_FILE_COMMAND_READ "Loading commands..."

/**
 * @def CFG_STR_FILE_DONE
 * @brief String to output after file read/write operations are complete.
 * @par Default: "Done."
 */
#define CFG_STR_FILE_DONE "Done."

/**
 * @def CFG_STR_FILE_LOCATION_READ
 * @brief String to output prior to loading location files.
 * @par Default: "Loading locations..."
 */
#define CFG_STR_FILE_LOCATION_READ "Loading locations..."

/**
 * @def CFG_STR_FILE_SETTINGS_READ
 * @brief String to output prior to loading settings files.
 * @par Default: "Loading settings..."
 */
#define CFG_STR_FILE_SETTINGS_READ "Loading settings..."

/**
 * @def CFG_STR_FILE_SETTINGS_WRITE
 * @brief String to output prior to writing settings files.
 * @par Default: "Saving settings..."
 */
#define CFG_STR_FILE_SETTINGS_WRITE "Saving settings..."

/**
 * @def CFG_STR_GAME_ENTER
 * @brief String sent when a Character first enters the game world.
 * @par Default "Welcome, please enjoy your stay." CRLF
 */
#define CFG_STR_GAME_ENTER "Welcome, please enjoy your stay." CRLF

/**
 * @def CFG_STR_GAME_ENTER_ERROR
 * @brief String sent when a server fault prevents a Character from entering the game world.
 * @par Default "An error occurred while attempting to enter the game world." CRLF
 */
#define CFG_STR_GAME_ENTER_ERROR "An error occurred while attempting to enter the game world." CRLF

/**
 * @def CFG_STR_IDLE
 * @brief String sent on disconnecting an idle socket.
 * @par Default: CRLF "Idle limit reached. Goodbye." CRLF
 */
#define CFG_STR_IDLE CRLF "Idle limit reached. Goodbye." CRLF

/**
 * @def CFG_STR_LOGIN
 * @brief String sent on initial socket connection.
 * @par Default: "Welcome to " CFG_STR_VERSION CRLF "For a list of commands type 'help'." CRLF
 */
#define CFG_STR_LOGIN "Welcome to " CFG_STR_VERSION CRLF "For a list of commands type 'help'." CRLF

/**
 * @def CFG_STR_MAX_BUFLEN
 * @brief Maximum length of all char type buffers.
 * @par Default: 16384
 */
#define CFG_STR_MAX_BUFLEN 16384

/**
 * @def CFG_STR_QUIT_LINKDEAD
 * @brief String sent to the client when they are disconnected due to a new login session on the same Character.
 * @par Default: CRLF "This body has been taken over!" CRLF
 */
#define CFG_STR_QUIT_LINKDEAD CRLF "This body has been taken over!" CRLF

/**
 * @def CFG_STR_QUIT_PLAYING
 * @brief String sent to the client when they quit.
 * @par Default: "Goodbye." CRLF
 */
#define CFG_STR_QUIT_PLAYING "Goodbye." CRLF

/**
 * @def CFG_STR_SEL_INVALID
 * @brief String send to the client after an invalid selection is made during an input request.
 * @par Default: "Invalid selection." CRLF
 */
#define CFG_STR_SEL_INVALID "Invalid selection." CRLF

/**
 * @def CFG_STR_SEL_OPTIONS
 * @brief String sent to prompt for a selection of menu options.
 * @par Default: "Please select one of the following options:" CRLF
 */
#define CFG_STR_SEL_OPTIONS "Please select one of the following options:" CRLF

/**
 * @def CFG_STR_SEL_PROMPT
 * @brief String send to the client to prompt an input selection.
 * @par Default: "Option: "
 */
#define CFG_STR_SEL_PROMPT "Option: "

/**
 * @def CFG_STR_SHUTDOWN
 * @brief String broadcast to all clients when the server is shutting down.
 * @par Default: "Server shutting down." CRLF
 */
#define CFG_STR_SHUTDOWN "Server shutting down." CRLF

/**
 * @def CFG_STR_UTILS_ERROR
 * @brief String to prepend to logs flagged UTILS_TYPE_ERROR.
 * @par Default: "[ERROR ] "
 */
#define CFG_STR_UTILS_ERROR "[ERROR] "

/**
 * @def CFG_STR_UTILS_INFO
 * @brief String to prepend to logs flagged UTILS_TYPE_INFO.
 * @par Default: "[INFO  ] "
 */
#define CFG_STR_UTILS_INFO "[INFO  ] "

/**
 * @def CFG_STR_UTILS_SOCKET
 * @brief String to prepend to logs flagged UTILS_TYPE_SOCKET.
 * @par Default: "[SOCKET] "
 */
#define CFG_STR_UTILS_SOCKET "[SOCKET] "

/**
 * @def CFG_STR_VERSION
 * @brief The current server version.
 * @par Default: "NAMS Development 0.0.0"
 */
#define CFG_STR_VERSION "NAMS Development 0.0.0"
/**@}*/

/***************************************************************************
 *                              THING OPTIONS                              *
 ***************************************************************************/
/** @name Thing Options */ /**@{*/
/**
 * @def CFG_THG_ID_MAX_LEN
 * @brief The maximum allowable length for a thing id.
 * @par Default: 128
 */
#define CFG_THG_ID_MAX_LEN 128

/**
 * @def CFG_THG_ID_MIN_LEN
 * @brief The minimum allowable length for a thing id.
 * @par Default: 1
 */
#define CFG_THG_ID_MIN_LEN 1

/**
 * @def CFG_THG_NAME_MAX_LEN
 * @brief The maximum allowable length for a thing name when the thing is associated to an account.
 * @par Default: 16
 */
#define CFG_THG_NAME_MAX_LEN 16

/**
 * @def CFG_THG_NAME_MIN_LEN
 * @brief The minimum allowable length for a thing name when the thing is associated to an account.
 * @par Default: 128
 */
#define CFG_THG_NAME_MIN_LEN 4
/**@}*/

#endif
