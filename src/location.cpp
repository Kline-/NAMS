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
 * @file location.cpp
 * @brief All non-template member functions of the Location class.
 *
 * A Location is a representation of a physical location within the game world.
 */
#include "h/includes.h"
#include "h/class.h"

#include "h/location.h"
#include "h/list.h"

/* Core */
/**
 * @brief Unload a location from memory that was previously loaded via Location::New().
 * @retval void
 */
const void Location::Delete()
{
    location_list.erase( find( location_list.begin(), location_list.end(), this ) );
    delete this;

    return;
}

/**
 * @brief Create a new location.
 * @param[in] server The Server the location will exist within.
 * @retval false Returned if a new Location was successfully created or loaded.
 * @retval true Returned if a new Location was unable to be created.
 */
const bool Location::New( Server* server )
{
    location_list.push_back( this );

    return true;
}

/**
 * @brief Serialize the location data.
 * @retval false Returned if there was an error serializing the location.
 * @retval true Returned if the location was serialized successfully.
 */
const bool Location::Serialize() const
{
    return true;
}

/**
 * @brief Unserialize the location data.
 * @retval false Returned if there was an error unserializing the location.
 * @retval true Returned if the location was unserialized successfully.
 */
const bool Location::Unserialize()
{
    return true;
}

/* Query */

/* Manipulate */

/* Internal */
/**
 * @brief Constructor for the Location class.
 */
Location::Location()
{
    return;
}

/**
 * @brief Destructor for the Location class.
 */
Location::~Location()
{
    return;
}
