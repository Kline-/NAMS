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
 * @file exit.cpp
 * @brief All non-template member functions of the Exit class.
 *
 * An Exit is a link between two locations within the game world.
 */
#include "h/includes.h"
#include "h/exit.h"

/* Core */
/**
 * @brief Unload an exit from memory that was previously loaded via Exit::New().
 * @retval void
 */
const void Exit::Delete()
{
    delete this;

    return;
}

/**
 * @brief Create a new exit.
 * @retval false Returned if a new Location was successfully created or loaded.
 * @retval true Returned if a new Location was unable to be created.
 */
const bool Exit::New()
{
    return true;
}

/* Query */

/* Manipulate */

/* Internal */
/**
 * @brief Constructor for the Exit class.
 */
Exit::Exit()
{
    return;
}

/**
 * @brief Destructor for the Exit class.
 */
Exit::~Exit()
{
    return;
}
