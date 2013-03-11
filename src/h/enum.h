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
 * @file enum.h
 * @brief All numerical and boolean defines for every class and namespace.
 *
 *  This file contains all numerical and boolean defines for each class and
 *  namespace. All items of similar purpose are grouped together with a
 *  comment to denote the class or namespace they are significant to.
 */
#ifndef DEC_ENUM_H
#define DEC_ENUM_H

/** @name Event */ /**@{*/
/**
 * @enum EVENT_TYPE
 */
enum EVENT_TYPE {
    EVENT_TYPE_COMMAND = 0,  /**< An Event to execute a game command. */
    EVENT_TYPE_RELOAD  = 1,  /**< An Event to reload a game command. */
    MAX_EVENT_TYPE     = 1   /**< Safety limit for looping. */
};
/**@}*/

/** @name Plugin */ /**@{*/
/**
 * @enum PLG_TYPE
 */
enum PLG_TYPE {
    PLG_TYPE_COMMAND = 0,   /**< A Plugin that specifies a game command. */
    MAX_PLG_TYPE     = 1    /**< Safety limit for looping. */
};

/**
 * @enum PLG_TYPE_COMMAND_BOOL
 */
enum PLG_TYPE_COMMAND_BOOL {
    PLG_TYPE_COMMAND_BOOL_PREEMPT = 0,  /**< Ties to Command->m_preempt. */
    MAX_PLG_TYPE_COMMAND_BOOL     = 1   /**< Safety limit for looping. */
};

/**
 * @enum PLG_TYPE_COMMAND_UINT
 */
enum PLG_TYPE_COMMAND_UINT {
    PLG_TYPE_COMMAND_UINT_SECURITY = 0, /**< Ties to Command->m_security. */
    MAX_PLG_TYPE_COMMAND_UINT      = 1  /**< Safety limit for looping. */
};
/**@}*/

/** @name Socket */ /**@{*/
/**
 * @enum SOC_STATE
 */
enum SOC_STATE {
    SOC_STATE_DISCONNECTED     = 0,   /**< A Socket in a disconnected state. */
    SOC_STATE_LOGIN_SCREEN     = 1,   /**< A Socket waiting at the login screen. */
    SOC_STATE_GET_OLD_PASSWORD = 2,   /**< A Socket using an existing account entering the password. */
    SOC_STATE_GET_NEW_ACCOUNT  = 3,   /**< A Socket selecting a new account name. */
    SOC_STATE_GET_NEW_PASSWORD = 4,   /**< A Socket selecting a new account password. */
    SOC_STATE_CREATE_ACCOUNT   = 5,   /**< An internal state where a new account is created after confirming a name and password. */
    SOC_STATE_ACCOUNT_MENU     = 6,   /**< A Socket at the main account menu. */

    SOC_STATE_PLAYING          = 100, /**< A Socket fully within the game world and actively playing. */
    MAX_SOC_STATE              = 101  /**< Safety limit for looping. */
};
/**@}*/

/** @name SocketClient */ /**@{*/
/**
 * @enum SOC_LOGIN
 */
enum SOC_LOGIN {
    SOC_LOGIN_CMD = 0, /**< Command received from the socket during the login process. */
    SOC_LOGIN_ARG = 1, /**< Any arguments received from the socket during the login process. */
    SOC_LOGIN_PWD = 2, /**< Initial password to match against when creating a new account. */
    MAX_SOC_LOGIN = 3  /**< Safety limit for looping. */
};

/**
 * @enum SOC_SECURITY
 */
enum SOC_SECURITY {
    SOC_SECURITY_NONE      = 0, /**< A SocketClient that has not logged in. Login screen commands. */
    SOC_SECURITY_AUTH_USER = 1, /**< A SocketClient that has logged in as a normal user account. */
    SOC_SECURITY_ADMIN     = 2, /**< A SocketClient that has logged in as an admin account. */
    MAX_SOC_SECURITY       = 3  /**< Safety limit for looping. */
};
/**@}*/

/** @name Utils */ /**@{*/
/**
 * @enum UTILS_OPTS
 */
enum UTILS_OPTS {
    UTILS_DEBUG       = 0, /**< Enables debugging output in Utils::_Logger(). This typically includes the calling function's file and line number. */
    UTILS_RAW         = 1, /**< Raw data output. Skips prepending / appending anything on Utils::_Logger writes (no timestamp, etc). */
    UTILS_TYPE_ERROR  = 2, /**< Indicates an error and prepends #CFG_STR_UTILS_ERROR to Utils::_Logger() output. */
    UTILS_TYPE_INFO   = 3, /**< Indicates an info message and prepends #CFG_STR_UTILS_INFO to Utils::_Logger() output. */
    UTILS_TYPE_SOCKET = 4, /**< Indicates a socket related message and prepends #CFG_STR_UTILS_SOCKET to Utils::_Logger() output. */
    UTILS_RET_ERROR   = 5, /**< Returned to indicate an error that the calling function should handle. */
    UTILS_RET_FALSE   = 6, /**< Returned to indicate a false value that the calling function should handle. */
    UTILS_RET_TRUE    = 7, /**< Returned to indicate a true value that the calling function should handle. */
    MAX_UTILS         = 8  /**< Safety limit for looping. */
};

/**
 * @def UTILS_IS_DIRECTORY
 */
#define UTILS_IS_DIRECTORY true

/**
 * @def UTILS_IS_FILE
 */
#define UTILS_IS_FILE      false
/**@}*/

#endif
