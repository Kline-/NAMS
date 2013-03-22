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

/** @name Account */ /**@{*/
/**
 * @enum ACT_LOGIN
 */
enum ACT_LOGIN {
    ACT_LOGIN_FAILURE = 0, /**< Failed login attempts. */
    ACT_LOGIN_SUCCESS = 1, /**< Successful login attempts. */
    MAX_ACT_LOGIN     = 2  /**< Safety limit for looping. */
};

/**
 * @enum ACT_MENU_CHARACTER_CREATE
 */
enum ACT_MENU_CHARACTER_CREATE {
    ACT_MENU_CHARACTER_CREATE_INVALID = 0,  /**< Must be 0 for stringstream extraction. */
    ACT_MENU_CHARACTER_CREATE_NAME    = 1,  /**< Prompt to enter name. */
    ACT_MENU_CHARACTER_CREATE_SEX     = 2,  /**< Prompt to select sex. */
    ACT_MENU_CHARACTER_CREATE_BACK    = 99, /**< Back out to the previous menu level. */
    MAX_ACT_MENU_CHARACTER_CREATE     = 100 /**< Safety limit for looping. */
};

/**
 * @enum ACT_MENU_MAIN
 */
enum ACT_MENU_MAIN {
    ACT_MENU_MAIN_INVALID          = 0,  /**< Must be 0 for stringstream extraction. */
    ACT_MENU_MAIN_CHARACTER_CREATE = 1,  /**< Create a new character associated to the account. */
    ACT_MENU_MAIN_QUIT             = 99, /**< Disconnect from the server. */
    MAX_ACT_MENU_MAIN              = 100 /**< Safety limit for looping. */
};

/**
 * @enum ACT_SECURITY
 */
enum ACT_SECURITY {
    ACT_SECURITY_NONE      = 0, /**< An Account that has not logged in. Login screen commands. */
    ACT_SECURITY_AUTH_USER = 1, /**< An Account that has logged in as a normal user account. */
    ACT_SECURITY_ADMIN     = 2, /**< An Account that has logged in as an admin account. */
    MAX_ACT_SECURITY       = 3  /**< Safety limit for looping. */
};
/**@}*/

/** @name Character */ /**@{*/
/**
 * @enum CHR_CREATION
 */
enum CHR_CREATION {
    CHR_CREATION_NAME = 0, /**< Name has been selected. */
    CHR_CREATION_SEX  = 1, /**< Sex has been selected. */
    MAX_CHR_CREATION  = 2  /**< Safety limit for looping. */
};

/**
 * @enum CHR_SEX
 */
enum CHR_SEX {
    CHR_SEX_NONE   = 0, /**< No sex specified / neutral. */
    CHR_SEX_FEMALE = 1, /**< A female character. */
    CHR_SEX_MALE   = 2, /**< A male character. */
    MAX_CHR_SEX    = 3  /**< Safety limit for looping. */
};
/**@}*/

/** @name Event */ /**@{*/
/**
 * @enum EVENT_TYPE
 */
enum EVENT_TYPE {
    EVENT_TYPE_COMMAND = 0, /**< An Event to execute a game command. */
    EVENT_TYPE_RELOAD  = 1, /**< An Event to reload a game command. */
    MAX_EVENT_TYPE     = 2  /**< Safety limit for looping. */
};
/**@}*/

/** @name Plugin */ /**@{*/
/**
 * @enum PLG_TYPE
 */
enum PLG_TYPE {
    PLG_TYPE_COMMAND = 0, /**< A Plugin that specifies a game command. */
    MAX_PLG_TYPE     = 1  /**< Safety limit for looping. */
};

/**
 * @enum PLG_TYPE_COMMAND_BOOL
 */
enum PLG_TYPE_COMMAND_BOOL {
    PLG_TYPE_COMMAND_BOOL_PREEMPT = 0, /**< Ties to Command->m_preempt. */
    MAX_PLG_TYPE_COMMAND_BOOL     = 1  /**< Safety limit for looping. */
};

/**
 * @enum PLG_TYPE_COMMAND_UINT
 */
enum PLG_TYPE_COMMAND_UINT {
    PLG_TYPE_COMMAND_UINT_SECURITY = 0, /**< Ties to Command->m_security. */
    MAX_PLG_TYPE_COMMAND_UINT      = 1  /**< Safety limit for looping. */
};
/**@}*/

/** @name Server::Config */ /**@{*/
/**
 * @enum SVR_CFG_PROHIBITED_NAMES
 */
enum SVR_CFG_PROHIBITED_NAMES {
    SVR_CFG_PROHIBITED_NAMES_ACCOUNT   = 0, /**< Illegal to use as account name. */
    SVR_CFG_PROHIBITED_NAMES_CHARACTER = 1, /**< Illegal to use as character name. */
    MAX_SVR_CFG_PROHIBITED_NAMES       = 2  /**< Safety limit for looping. */
};
/**@}*/

/** @name Socket */ /**@{*/
/**
 * @enum SOC_STATE
 */
enum SOC_STATE {
    SOC_STATE_DISCONNECTED          = 0,   /**< A Socket in a disconnected state. */
    SOC_STATE_LOGIN_SCREEN          = 1,   /**< A Socket waiting at the login screen. */
    SOC_STATE_GET_OLD_PASSWORD      = 2,   /**< A Socket using an existing account entering the password. */
    SOC_STATE_GET_NEW_ACCOUNT       = 3,   /**< A Socket selecting a new account name. */
    SOC_STATE_GET_NEW_PASSWORD      = 4,   /**< A Socket selecting a new account password. */
    SOC_STATE_CREATE_ACCOUNT        = 5,   /**< An internal state where a new account is created after confirming a name and password. */
    SOC_STATE_LOAD_ACCOUNT          = 6,   /**< An internal state where an existing account is loaded after confirming a name and password. */
    SOC_STATE_ACCOUNT_MENU          = 7,   /**< A Socket at the main account menu. */
    SOC_STATE_CHARACTER_CREATE_MENU = 8,   /**< A Socket within the character creation menu. */
    SOC_STATE_CHARACTER_CREATE_NAME = 9,   /**< A Socket selecting a new Character name. */
    SOC_STATE_CHARACTER_CREATE_SEX  = 10,  /**< A Socket selecting a new Character sex. */
    SOC_STATE_PLAYING               = 100, /**< A Socket fully within the game world and actively playing. */
    MAX_SOC_STATE                   = 101  /**< Safety limit for looping. */
};
/**@}*/

/** @name SocketClient */ /**@{*/
/**
 * @enum SOC_LOGIN
 */
enum SOC_LOGIN {
    SOC_LOGIN_NAME     = 0, /**< Name received from the socket during the login process. */
    SOC_LOGIN_PASSWORD = 1, /**< Password received from the socket during the login process. */
    MAX_SOC_LOGIN      = 2  /**< Safety limit for looping. */
};

/**
 * @enum SOC_TELOPT
 */
enum SOC_TELOPT {
    SOC_TELOPT_ECHO = 0, /**< Telopt ECHO (1) */
    MAX_SOC_TELOPT  = 1  /**< Safety limit for looping. */
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
