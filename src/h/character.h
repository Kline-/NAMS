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

#include "thing.h"

using namespace std;

/**
 * @brief An actor within the game world.
 */
class Character : public Thing
{
    public:
        /** @name Core */ /**@{*/
        const void Delete();
        const void Interpret( const uint_t& security, const string& cmd, const string& args );
        const bool New( const string& file, const bool& exists );
        const void Send( const string& msg );
        const bool Serialize() const;
        const bool Unserialize();
        /**@}*/

        /** @name Query */ /**@{*/
        Account* gAccount() const;
        const bool gCreation( const uint_t& pos );
        const string gPrompt() const;
        const uint_t gSex() const;
        /**@}*/

        /** @name Manipulate */ /**@{*/
        const bool sAccount( Account* account );
        const bool sCreation( const uint_t& pos, const bool& val );
        const bool sSex( const uint_t& sex );
        /**@}*/

        /** @name Internal */ /**@{*/
        Character();
        ~Character();
        /**@}*/

    private:
        Account* m_account; /**< The associated Account, if any. */
        bool m_creation[MAX_CHR_CREATION]; /**< Track if all creation options have been set. */
        string m_file; /**< Path to the file on disk. */
        uint_t m_sex; /**< The sex of the character. */
};

#endif
