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
 * @file brain.cpp
 * @brief All non-template member functions of the Brain class.
 *
 * Abstraction layer between player characters (Account->SocketClient) and non-player characters (AI).
 */
#include "h/includes.h"
#include "h/brain.h"

/* Core */
/**
 * @brief Unload a brain from memory that was previously loaded via Brain::New().
 * @retval void
 */
const void Brain::Delete()
{
    delete this;

    return;
}

/**
 * @brief Create a new brain.
 * @retval false Returned if a new Brain was successfully created.
 * @retval true Returned if a new Brain was unable to be created.
 */
const bool Brain::New()
{
    return true;
}

/* Query */
/**
 * @brief Returns the Account associated with this Brain, if any.
 * @retval Account* A pointer to the associated account, or NULL if none.
 */
Account* Brain::gAccount() const
{
    return m_account;
}

/**
 * @brief Returns the Thing associated with this Brain, if any.
 * @retval Thing* A pointer to the associated thing, or NULL if none.
 */
Thing* Brain::gThing() const
{
    return m_thing;
}

/* Manipulate */
/**
 * @brief Sets the account associated with this brain.
 * @param[in] account A pointer to the Account to be associated with this brain.
 * @retval false Returned if unable to associate the account with this brain.
 * @retval true Returned if the account was successfully associated.
 */
const bool Brain::sAccount( Account* account )
{
    UFLAGS_DE( flags );

    if ( m_account != NULL && account != NULL )
    {
        LOGSTR( flags, "Brain::sAccount()-> called while m_account is not NULL" );
        return false;
    }

    m_account = account;

    return true;
}

/**
 * @brief Sets the thing associated with this brain.
 * @param[in] thing A pointer to the Thing to be associated with this brain.
 * @retval false Returned if unable to associate the thing with this brain.
 * @retval true Returned if the thing was successfully associated.
 */
const bool Brain::sThing( Thing* thing )
{
    UFLAGS_DE( flags );

    if ( m_thing != NULL && thing != NULL )
    {
        LOGSTR( flags, "Brain::sThing()-> called while m_thing is not NULL" );
        return false;
    }

    m_thing = thing;

    return true;
}

/* Internal */
/**
 * @brief Constructor for the Brain class.
 */
Brain::Brain()
{
    m_account = NULL;
    m_thing = NULL;

    return;
}

/**
 * @brief Destructor for the Brain class.
 */
Brain::~Brain()
{
    return;
}
