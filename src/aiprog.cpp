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
 * @file aiprog.cpp
 * @brief All non-template member functions of the AIProg class.
 *
 * An AIProg is analogous to an Account. It provides intelligence behind certain NPCs, locations, and objects.
 */
#include "h/includes.h"
#include "h/aiprog.h"

/* Core */
/**
 * @brief Unload an aiprog from memory that was previously loaded via AIProg::New().
 * @retval void
 */
const void AIProg::Delete()
{
    delete this;

    return;
}

/**
 * @brief Create a new aiprog.
 * @retval false Returned if a new AIProg was successfully created.
 * @retval true Returned if a new AIProg was unable to be created.
 */
const bool AIProg::New()
{
    return true;
}

/* Query */
/**
 * @brief Returns the Thing associated with this AIProg.
 * @retval Thing* A pointer to the associated thing.
 */
Thing* AIProg::gThing() const
{
    return m_thing;
}

/* Manipulate */

/**
 * @brief Sets the thing associated with this aiprog.
 * @param[in] thing A pointer to the Thing to be associated with this aiprog.
 * @retval false Returned if unable to associate the thing with this aiprog.
 * @retval true Returned if the thing was successfully associated.
 */
const bool AIProg::sThing( Thing* thing )
{
    UFLAGS_DE( flags );

    if ( m_thing != NULL && thing != NULL )
    {
        LOGSTR( flags, "AIProg::sThing()-> called while m_thing is not NULL" );
        return false;
    }

    m_thing = thing;

    return true;
}

/* Internal */
/**
 * @brief Constructor for the AIProg class.
 */
AIProg::AIProg()
{
    m_thing = NULL;

    return;
}

/**
 * @brief Destructor for the AIProg class.
 */
AIProg::~AIProg()
{
    return;
}
