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
 * @file globals.h
 * @brief Externs to reference global scope variables.
 *
 *  This file contains extern references to all global scope variables.
 */
#ifndef DEC_GLOBAL_H
#define DEC_GLOBAL_H

extern list<Character*>::iterator g_character_next; /**< Used as the next iterator in all loops dealing with Character objects to prevent nested processing loop problems. */
extern Server::Config* g_config; /**< Runtime settings. */
extern forward_list<Event*>::iterator g_event_next; /**< Used as the next iterator in all loops dealing with Event objects to prevent nested processing loop problems. */
extern SocketServer* g_listen; /**< The listening server-side socket. */
extern uint_t g_port; /**< Port number to be passed to the associated SocketServer. */
extern bool g_shutdown; /**< Shutdown state of the game. */
extern list<SocketClient*>::iterator g_socket_client_next; /**< Used as the next iterator in all loops dealing with SocketClient objects to prevent nested processing loop problems. */
extern Server::Stats* g_stats; /**< Runtime statistics. */
extern chrono::high_resolution_clock::time_point g_time_boot; /**< Time the Server was first booted. */
extern chrono::high_resolution_clock::time_point g_time_current; /**< Current time from the host OS. */

#endif
