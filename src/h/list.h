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
 * @file list.h
 * @brief All globally referenced list / map / vector types.
 *
 *  All lists-type objects that are global in scope are referenced via this file.
 *  These are usually, but not always, containers that reference pointer objects
 *  to ensure proper tracking and memory cleanup later.
 */
#ifndef DEC_LIST_H
#define DEC_LIST_H

using namespace std;

/**
 * @var command_list
 * @brief All commands loaded into memory.
 * @param "const char" The first letter of the value contained within Command->m_name.
 * @param Command* A pointer to a Command object in memory.
 */
extern multimap<const char,Command*> command_list;

/**
 * @var event_list
 * @brief All events pending execution.
 * @param Event* A pointer to an Event object in memory.
 */
extern forward_list<Event*> event_list;

/**
 * @var socket_client_list
 * @brief All SocketClient objects connected to the server.
 * @param SocketClient* A pointer to a SocketClient object in memory.
 */
extern list<SocketClient*> socket_client_list;

/**
 * @var socket_server_list
 * @brief All SocketServer objects listening for new client connections.
 * @param SocketServer* A pointer to a SocketServer object in memory.
 */
extern list<SocketServer*> socket_server_list;

#endif
