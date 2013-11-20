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
 * @file aiprog.h
 * @brief The AIProg class.
 *
 *  This file contains the AIProg class and template functions.
 */
#ifndef DEC_AIPROG_H
#define DEC_AIPROG_H

using namespace std;

/**
 * @brief An AIProg is analogous to an Account. It provides intelligence behind certain NPCs, locations, and objects.
*/
class AIProg
{
    public:
        /** @name Core */ /**@{*/
        const void Delete();
        const bool New();
        /**@}*/

        /** @name Query */ /**@{*/
        Brain* gBrain() const;
        /**@}*/

        /** @name Manipulate */ /**@{*/
        const bool sBrain( Brain* brain );
        /**@}*/

        /** @name Internal */ /**@{*/
        AIProg();
        ~AIProg();
        /**@}*/

    private:
        Brain* m_brain; /**< The associated Brain. */
};

#endif
