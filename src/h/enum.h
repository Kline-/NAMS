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
#ifndef DEC_ENUM_H
#define DEC_ENUM_H

// uint_t    Socket
enum {
    SOC_STATE_DISCONNECTED = 0,
    SOC_STATE_LOGIN_SCREEN = 1,
    SOC_STATE_PLAYING      = 100,
    MAX_SOC_STATE          = 101
};

// bitset<CFG_MEM_MAX_BITSET>    Utils
enum {
    UTILS_DEBUG       = 0,
    UTILS_IGNORE_CASE = 1,
    UTILS_RAW         = 2,
    UTILS_TYPE_ERROR  = 3,
    UTILS_TYPE_INFO   = 4,
    UTILS_TYPE_SOCKET = 5,
    MAX_UTILS         = 6
};

// bool    Utils::ListDirectory()
#define UTILS_IS_DIRECTORY true
#define UTILS_IS_FILE      false

#endif
