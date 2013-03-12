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
 * @file account.cpp
 * @brief All non-template member functions of the Account class.
 *
 * Security structure to maintain characters, roles, and permissions.
 */
#include "h/includes.h"
#include "h/class.h"

#include "h/account.h"
#include "h/socketclient.h"

/* Core */
/**
 * @brief Unload an account from memory that was previously loaded via Account::New().
 * @retval void
 */
const void Account::Delete()
{
    return;
}

/**
 * @brief Create a new account.
 * @param[in] client The SocketClient requesting an account.
 * @param[in] name The name of the account to be created.
 * @retval false Returned if a new Account was successfully created or loaded.
 * @retval true Returned if a new Account was unable to be created.
 */
const bool Account::New( SocketClient* client, const string& name )
{
    UFLAGS_DE( flags );

    if ( client == NULL )
    {
        LOGSTR( flags, "Account::New()-> called with NULL client" );
        return false;
    }

    if ( name.empty() )
    {
        LOGSTR( flags, "Account::New()-> called with empty name" );
        return false;
    }

    return true;
}

/* Query */
/**
 * @brief Returns the name of the account.
 * @retval string A string with the name of the account.
 */
const string Account::gName() const
{
    return m_name;
}

/* Manipulate */

/* Internal */
/**
 * @brief Constructor for the Account class.
 */
Account::Account()
{
    m_name.clear();

    return;
}

/**
 * @brief Destructor for the Account class.
 */
Account::~Account()
{
    return;
}