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
 * @file brain.h
 * @brief The Brain class.
 *
 *  This file contains the Brain class and template functions.
 */
#ifndef DEC_BRAIN_H
#define DEC_BRAIN_H

using namespace std;

/**
 * @brief Abstraction layer between player characters (Account->SocketClient) and non-player characters (AI).
 * @todo Develop a single interface for external classes, i.e., nothing needs to access gAccount() directly to determine if PC/NPC, a generic gSecurity(), etc
 */
class Brain
{
    public:
        /** @name Core */ /**@{*/
        const void Delete();
        const bool New();
        /**@}*/

        /** @name Query */ /**@{*/
        Account* gAccount() const;
        Thing* gThing() const;
        /**@}*/

        /** @name Manipulate */ /**@{*/
        const bool sAccount( Account* account );
        const bool sThing( Thing* thing );
        /**@}*/

        /** @name Internal */ /**@{*/
        Brain();
        ~Brain();
        /**@}*/

    private:
        Account* m_account; /**< The associated Account, if any. */
        Thing* m_thing; /**< The associated Thing. */
};

#endif
