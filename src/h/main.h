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
 * @file main.h
 * @brief All global / server-wide variables, lists, etc.
 *
 *  This file contains all global variables, lists, and other objects that
 *  need to be referenced or accessed by the Server namespace.
 */
#ifndef DEC_MAIN_H
#define DEC_MAIN_H

list<Character*>::iterator g_character_next; /**< Used as the next iterator in all loops dealing with Character objects to prevent nested processing loop problems. */
Server::Config* g_config; /**< Runtime settings. */
forward_list<Event*>::iterator g_event_next; /**< Used as the next iterator in all loops dealing with Event objects to prevent nested processing loop problems. */
SocketServer* g_listen; /**< The listening server-side socket. */
uint_t g_port; /**< Port number to be passed to the associated SocketServer. */
bool g_shutdown; /**< Shutdown state of the game. */
list<SocketClient*>::iterator g_socket_client_next; /**< Used as the next iterator in all loops dealing with SocketClient objects to prevent nested processing loop problems. */
Server::Stats* g_stats; /**< Runtime statistics. */
chrono::high_resolution_clock::time_point g_time_boot; /**< Time the Server was first booted. */
chrono::high_resolution_clock::time_point g_time_current; /**< Current time from the host OS. */

#endif
