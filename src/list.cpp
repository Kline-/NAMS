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
 * @file list.cpp
 * @brief All globally referenced list / map / vector types.
 *
 *  All lists-type objects that are global in scope are referenced via this file.
 *  These are usually, but not always, containers that reference pointer objects
 *  to ensure proper tracking and memory cleanup later.
 */
#include "h/includes.h"
#include "h/list.h"

/**
 * @var character_list
 * @brief All characters that exist within the server.
 * @param Character* A pointer to a Character object in memory.
 */
vector<Character*> character_list;

/**
 * @var character_template_list
 * @brief All characters that exist as templates within the server.
 * @param Character* A pointer to a Character template in memory.
 */
vector<Character*> character_template_list;

/**
 * @var command_list
 * @brief All commands loaded into memory.
 * @param Command* A pointer to a Command object in memory.
 */
vector<Command*> command_list;

/**
 * @var event_list
 * @brief All events pending execution.
 * @param Event* A pointer to an Event object in memory.
 */
vector<Event*> event_list;

/**
 * @var exit_list
 * @brief All exits that exist within the server.
 * @param Exit* A pointer to an Exit object in memory.
 */
vector<Exit*> exit_list;

/**
 * @var location_list
 * @brief All locations that exist within the server.
 * @param Location* A pointer to a Location object in memory.
 */
vector<Location*> location_list;

/**
 * @var object_list
 * @brief All objects that exist as a copy of a template within the server.
 * @param Object* A pointer to an Object copy in memory.
 */
vector<Object*> object_list;

/**
 * @var object_template_list
 * @brief All objects that exist as templates within the server.
 * @param Object* A pointer to an Object template in memory.
 */
vector<Object*> object_template_list;

/**
 * @var socket_client_list
 * @brief All SocketClient objects connected to the server.
 * @param SocketClient* A pointer to a SocketClient object in memory.
 */
vector<SocketClient*> socket_client_list;
