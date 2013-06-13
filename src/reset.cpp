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
 * @file reset.cpp
 * @brief All non-template member functions of the Reset class.
 *
 * A Reset controls how many Objects and NPCs "spawn" at a given Location
 * within the game world.
 */
#include "h/includes.h"
#include "h/reset.h"

/* Core */
/**
 * @brief Unload a reset from memory that was previously loaded via Reset::New().
 * @retval void
 */
const void Reset::Delete()
{
    delete this;

    return;
}

/**
 * @brief Create a new reset.
 * @param[in] location The owning location associated to the Reset.
 * @retval false Returned if a new Reset was successfully created or loaded.
 * @retval true Returned if a new Reset was unable to be created.
 */
const bool Reset::New( Location* location )
{
    UFLAGS_DE( flags );

    if ( location == NULL )
    {
        LOGSTR( flags, "Reset::New()-> called with NULL location" );
        return false;
    }

    m_location = location;

    return true;
}

/**
 * @brief Serialize the reset data.
 * @retval string A string containing the serialized data.
 */
const string Reset::Serialize() const
{
    stringstream output;
/*
    output << Utils::MakePair( "dest_id", m_dest_id );
    output << Utils::MakePair( "name", m_name );
*/
    return output.str();
}

/**
 * @brief Unserialize the reset data.
 * @param[in] input The serialized data to use.
 * @retval false Returned if there was an error unserializing the data.
 * @retval true Returned if the data was successfully unserialized.
 */
const bool Reset::Unserialize( const string& input )
{/*
    UFLAGS_DE( flags );
    string arg, value;
    pair<string,string> item;
    bool found = false;

    value = input;

    while ( !value.empty() )
    {
        found = false;
        arg = Utils::Argument( value, "} " );
        item = Utils::ReadPair( arg );

        Utils::KeySet( true, found, item.first, "dest_id", item.second, m_dest_id );
        Utils::KeySet( true, found, item.first, "name", item.second, m_name );

        if ( !found )
            LOGFMT( flags, "Reset::Unserialize()-> key not found: %s", CSTR( item.first ) );
    }
*/
    return true;
}

/* Query */
/**
 * @brief Returns the Location that owns the Reset.
 * @retval Location* A pointer to the Location which owns the reset.
 */
Location* Reset::gLocation() const
{
    return m_location;
}

/* Manipulate */

/* Internal */
/**
 * @brief Constructor for the Reset class.
 */
Reset::Reset()
{
    m_location = NULL;
    m_name.clear();

    return;
}

/**
 * @brief Destructor for the Reset class.
 */
Reset::~Reset()
{
    return;
}
