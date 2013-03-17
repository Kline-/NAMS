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
 * @file thing.h
 * @brief The Thing class.
 *
 *  This file contains the Thing class and template functions.
 */
#ifndef DEC_THING_H
#define DEC_THING_H

#include "account.h"

using namespace std;

/**
 * @brief A generic "thing": creature, character, room, object, etc.
 */
class Thing {
    public:
        /** @name Core */ /**@{*/
        /**@}*/

        /** @name Query */ /**@{*/
        Account* gAccount() const;
        const string gId() const;
        const string gName() const;
        /**@}*/

        /** @name Manipulate */ /**@{*/
        const bool sId( const string& id );
        const bool sName( const string& name, const bool& system = false );
        /**@}*/

        /** @name Internal */ /**@{*/
        Thing();
        ~Thing();
        /**@}*/

    private:
        Account* m_account; /**< The associated Account, if any. */
        string m_id; /**< An identifier to denote ownership. For characters, id = account.name */
        string m_name; /**< The name of the thing. */
};

#endif
