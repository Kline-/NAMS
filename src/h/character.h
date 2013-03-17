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
 * @file character.h
 * @brief The Character class.
 *
 *  This file contains the Character class and template functions.
 */
#ifndef DEC_CHARACTER_H
#define DEC_CHARACTER_H

#include "account.h"
#include "thing.h"

using namespace std;

/**
 * @brief An actor within the game world.
 */
class Character : public Thing {
    public:
        /** @name Core */ /**@{*/
        const void Delete();
        const bool New( Server* server, Account* account = NULL );
        /**@}*/

        /** @name Query */ /**@{*/
        Account* gAccount() const;
        /**@}*/

        /** @name Manipulate */ /**@{*/
        /**@}*/

        /** @name Internal */ /**@{*/
        Character();
        ~Character();
        /**@}*/

    private:
        Account* m_account; /**< The associated Account, if any. */
};

#endif
