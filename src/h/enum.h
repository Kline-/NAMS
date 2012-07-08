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

/** @name Socket */ /**@{*/
/**
 * @enum SOC_STATE
 *
 * #uint_t
 */
enum SOC_STATE {
    SOC_STATE_DISCONNECTED = 0,     /**< A Socket in a disconnected state. */
    SOC_STATE_LOGIN_SCREEN = 1,     /**< A Socket waiting at the login screen. */
    SOC_STATE_PLAYING      = 100,   /**< A Socket fully within the game world and actively playing. */
    MAX_SOC_STATE          = 101    /**< Safety limit for looping. */
};
/**@}*/

/** @name Utils */ /**@{*/
/**
 * @enum UTILS_OPTS
 *
 * bitset<#CFG_MEM_MAX_BITSET>
 */
enum UTILS_OPTS {
    UTILS_DEBUG       = 0,  /**< Enables debugging output in Utils::_Logger(). This typically includes the calling function's file and line number. */
    UTILS_RAW         = 1,  /**< Raw data output. Skips prepending / appending anything on Utils::_Logger writes (no timestamp, etc). */
    UTILS_TYPE_ERROR  = 2,  /**< Indicates an error and prepends #CFG_STR_UTILS_ERROR to Utils::_Logger() output. */
    UTILS_TYPE_INFO   = 3,  /**< Indicates an info message and prepends #CFG_STR_UTILS_INFO to Utils::_Logger() output. */
    UTILS_TYPE_SOCKET = 4,  /**< Indicates a socket related message and prepends #CFG_STR_UTILS_SOCKET to Utils::_Logger() output. */
    UTILS_TIME_S      = 5,  /**< Will return time as @a seconds from Utils::DiffTime(). */
    UTILS_TIME_MS     = 6,  /**< Will return time as @a milliseconds from Utils::DiffTime(). */
    UTILS_TIME_US     = 7,  /**< Will return time as @a microseconds from Utils::DiffTime(). */
    MAX_UTILS         = 8   /**< Safety limit for looping. */
};

/**
 * @def UTILS_IS_DIRECTORY
 *
 * Utils::ListDirectory()
 */
#define UTILS_IS_DIRECTORY true

/**
 * @def UTILS_IS_FILE
 *
 * Utils::ListDirectory()
 */
#define UTILS_IS_FILE      false
/**@}*/

#endif
