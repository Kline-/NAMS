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
 * @file handler.cpp
 * @brief All non-template member functions of the Handler namespace.
 *
 * The Handler namespace contains all non-class process control functions.
 * To pointedly avoid large complex functions such as "nanny()" from Diku
 * heritage, process controls should be implemented as individual functions
 * within this namespace.
 */
#include "h/includes.h"
#include "h/class.h"

#include "h/handler.h"

/* Core */
const void Handler::LoginScreen( SocketClient* client, const string& cmd, const string& args )
{
    return;
}

/* Query */

/* Manipulate */

/* Internal */
