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
 * @file plugin.h
 * @brief The Plugin class.
 *
 *  This file contains the Plugin class, templates, and trivial member functions.
 */
#ifndef DEC_PLUGIN_H
#define DEC_PLUGIN_H

class Plugin {
    public:
        /** @name Core */ /**@{*/
        const void Register(  );
        virtual const void Run() const = 0;
        /**@}*/

        /** @name Query */ /**@{*/
        const string gName() const { return m_name; };
        /**@}*/

        /** @name Manipulate */ /**@{*/
        /**@}*/

        /** @name Internal */ /**@{*/
        Plugin( const string& name, const uint_t& type );
        ~Plugin();
        /**@}*/

    private:
        bool m_bool[CFG_PLG_MAX_ARR];
        string m_name;
        string m_string[CFG_PLG_MAX_ARR];
        uint_t m_type;
        uint_t m_uint_t[CFG_PLG_MAX_ARR];
};

typedef Plugin* NewPlugin();
typedef void DeletePlugin( Plugin* );

#endif