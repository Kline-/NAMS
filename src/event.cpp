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
 * @file plugin.cpp
 * @brief All non-template member functions of the Plugin class.
 *
 * Plugin objects provide an API to interface with the NAMS server in
 * different ways. Each Plugin is compiled and loaded as an indepdendent shared
 * object that implements a new class.
 *
 * Server::BuildPlugin() will automatically compile any plugins at boot time
 * unless an existing shared object file is already found in #CFG_DAT_DIR_OBJ.
 */
#include "h/includes.h"
#include "h/class.h"

#include "h/event.h"

/* Core */

/* Query */

/* Manipulate */

/* Internal */
/**
 * @brief Constructor for the Event class.
 */
Event::Event()
{
    return;
}

/**
 * @brief Destructor for the Event class.
 */
Event::~Event()
{
    return;
}