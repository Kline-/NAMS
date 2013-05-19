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
 * @file exit.h
 * @brief The Exit class.
 *
 *  This file contains the Exit class and template functions.
 */
#ifndef DEC_EXIT_H
#define DEC_EXIT_H

using namespace std;

/**
 * @brief A link between two locations within the game world.
 */
class Exit
{
    public:
        /** @name Core */ /**@{*/
        const void Delete();
        const bool New();
        /**@}*/

        /** @name Query */ /**@{*/
        /**@}*/

        /** @name Manipulate */ /**@{*/
        /**@}*/

        /** @name Internal */ /**@{*/
        Exit();
        ~Exit();
        /**@}*/

    private:
};

#endif
