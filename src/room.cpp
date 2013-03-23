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
 * @file room.cpp
 * @brief All non-template member functions of the Room class.
 *
 * A Room is a representation of a physical location within the game world.
 */
#include "h/includes.h"
#include "h/class.h"

#include "h/room.h"

/* Core */
/**
 * @brief Unload a room from memory that was previously loaded via Room::New().
 * @retval void
 */
const void Room::Delete()
{
    delete this;

    return;
}

/**
 * @brief Create a new room.
 * @param[in] server The Server the room will exist within.
 * @retval false Returned if a new Room was successfully created or loaded.
 * @retval true Returned if a new Room was unable to be created.
 */
const bool Room::New( Server* server )
{
    return true;
}

/**
 * @brief Serialize the room data.
 * @retval false Returned if there was an error serializing the room.
 * @retval true Returned if the room was serialized successfully.
 */
const bool Room::Serialize() const
{
    return true;
}

/**
 * @brief Unserialize the room data.
 * @retval false Returned if there was an error unserializing the room.
 * @retval true Returned if the room was unserialized successfully.
 */
const bool Room::Unserialize()
{
    return true;
}

/* Query */

/* Manipulate */

/* Internal */
/**
 * @brief Constructor for the Room class.
 */
Room::Room()
{
    return;
}

/**
 * @brief Destructor for the Room class.
 */
Room::~Room()
{
    return;
}
