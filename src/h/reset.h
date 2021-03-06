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
 * @file reset.h
 * @brief The Reset class.
 *
 *  This file contains the Reset class and template functions.
 */
#ifndef DEC_RESET_H
#define DEC_RESET_H

using namespace std;

/**
 * @brief A "spawn" of an Object or NPC.
 */
class Reset
{
    public:
        /** @name Core */ /**@{*/
        const void Delete();
        const bool New( Location* location );
        const string Serialize() const;
        const bool Unserialize( const string& input );
        /**@}*/

        /** @name Query */ /**@{*/
        Location* gLocation() const;
        const string gName() const;
        /**@}*/

        /** @name Manipulate */ /**@{*/
        /**@}*/

        /** @name Internal */ /**@{*/
        Reset();
        ~Reset();
        /**@}*/

    private:
        Location* m_location; /**< The owning Location. */
        string m_name; /**< The name of the reset. */
};

#endif
