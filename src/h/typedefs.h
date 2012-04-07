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
#ifndef DEC_TYPEDEFS_H
#define DEC_TYPEDEFS_H

#if __WORDSIZE == 64
 typedef signed long int sint_t;
 #define sintmax_t numeric_limits<signed long int>::max()
 #define sintmin_t numeric_limits<signed long int>::min()

 typedef unsigned long int uint_t;
 #define uintmax_t numeric_limits<unsigned long int>::max()
 #define uintmin_t numeric_limits<unsigned long int>::min()
#else
 typedef signed int sint_t;
 #define sintmax_t numeric_limits<signed int>::max()
 #define sintmin_t numeric_limits<signed int>::min()

 typedef unsigned int uint_t;
 #define uintmax_t numeric_limits<unsigned int>::max()
 #define uintmin_t numeric_limits<unsigned int>::min()
#endif

class Command;
class Directory;
class Server;
class Socket;
class SocketClient;
class SocketServer;
class Utils;

#endif
