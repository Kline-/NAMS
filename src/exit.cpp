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
 * @param[in] location The owning location associated to the Exit.
 * @retval false Returned if a new Location was successfully created or loaded.
 * @retval true Returned if a new Location was unable to be created.
 */
const bool Exit::New( Location* location )
{
    UFLAGS_DE( flags );

    if ( location == NULL )
    {
        LOGSTR( flags, "Exit::New()-> called with NULL location" );
        return false;
    }

    m_location = location;

    return true;
}

/**
 * @brief Serialize the exit data.
 * @retval string A string containing the serialized data.
 */
const string Exit::Serialize() const
{
    stringstream output;

    output << Utils::MakePair( "dest_id", m_dest_id );
    output << Utils::MakePair( "name", m_name );

    return output.str();
}

/**
 * @brief Unserialize the exit data.
 * @param[in] input The serialized data to use.
 * @retval false Returned if there was an error unserializing the data.
 * @retval true Returned if the data was successfully unserialized.
 */
const bool Exit::Unserialize( const string& input )
{
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
            LOGFMT( flags, "Exit::Unserialize()-> key not found: %s", CSTR( item.first ) );
    }

    return true;
}

/* Query */
/**
 * @brief Returns the destination id of the Exit.
 * @retval string A string containing the destination id of the Exit.
 */
const string Exit::gDestId() const
{
    return m_dest_id;
}

/**
 * @brief Returns the destination that this Exit leads to.
 * @retval Location* A pointer to the Location that this Exit leads to.
 */
Location* Exit::gDestination() const
{
    return m_destination;
}

/**
 * @brief Returns the Location that owns the Exit.
 * @retval Location* A pointer to the Location which owns the exit.
 */
Location* Exit::gLocation() const
{
    return m_location;
}

/**
 * @brief Returns the name of the Exit.
 * @retval string A string containing the name of the Exit.
 */
const string Exit::gName() const
{
    return m_name;
}

/* Manipulate */
/**
 * @brief Sets the target destination of this Exit.
 * @param[in] location The target location this Exit should lead to.
 * @retval false Returned if there was an error setting the destination.
 * @retval true Returned if the destination was successfully set.
 */
const bool Exit::sDestination( Location* location )
{
    UFLAGS_DE( flags );

    if ( location == NULL )
    {
        LOGSTR( flags, "Exit::sDestination()-> called with NULL location" );
        return false;
    }

    m_destination = location;

    return true;
}

/* Internal */
/**
 * @brief Constructor for the Exit class.
 */
Exit::Exit()
{
    m_destination = NULL;
    m_dest_id.clear();
    m_location = NULL;
    m_name.clear();

    return;
}

/**
 * @brief Destructor for the Exit class.
 */
Exit::~Exit()
{
    return;
}
