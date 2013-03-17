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
 * @file character.cpp
 * @brief All non-template member functions of the Character class.
 *
 * A Character is either autonomous and owned by a Server object or is
 * associated with an Account traceable back to a live SocketClient
 * end-user.
 */
#include "h/includes.h"
#include "h/class.h"

#include "h/character.h"

/* Core */
/**
 * @brief Unload a character from memory that was previously loaded via Character::New().
 * @retval void
 */
const void Character::Delete()
{
    delete this;

    return;
}

/**
 * @brief Create a new character.
 * @param[in] server The Server the character will exist within.
 * @param[in] account The associated Account, if any.
 * @retval false Returned if a new Character was successfully created or loaded.
 * @retval true Returned if a new Character was unable to be created.
 */
const bool Character::New( Server* server, Account* account )
{
    return true;
}

/* Query */
/**
 * @brief Returns the Account associated with this Character, if any.
 * @retval Account* A pointer to the associated account, or NULL if none.
 */
Account* Character::gAccount() const
{
    return m_account;
}

/* Manipulate */

/* Internal */
/**
 * @brief Constructor for the Character class.
 */
Character::Character()
{
    m_account = NULL;

    return;
}

/**
 * @brief Destructor for the Character class.
 */
Character::~Character()
{
    return;
}
