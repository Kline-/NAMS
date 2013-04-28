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
 *  This file contains the Plugin class and template functions.
 */
#ifndef DEC_PLUGIN_H
#define DEC_PLUGIN_H

using namespace std;

/**
 * @brief Functions that are indepdently built and loaded to extend the server core via an API.
 */
class Plugin {
    public:
        /** @name Core */ /**@{*/
        /**
         * @brief Execute the primary function of the implemented class.
         * @param[in] client If called from a SocketClient, the caller is passed through to the Plugin for reference.
         * @param[in] cmd If called from a SocketClient, the command from the client is passed through.
         * @param[in] arg If called from a SocketClient, the arguments from the client are passed through.
         * @retval void
         */
        virtual const void Run( SocketClient* client = NULL, const string& cmd = "", const string& arg = "" ) const = 0;
        /**@}*/

        /** @name Query */ /**@{*/
        const bool gBool( const uint_t& pos ) const;
        void* gCaller() const;
        const string gName() const;
        const string gString( const uint_t& pos ) const;
        const uint_t gUint( const uint_t& pos ) const;
        /**@}*/

        /** @name Manipulate */ /**@{*/
        const bool sBool( const uint_t& pos, const bool& val );
        const bool sCaller( void* caller );
        const bool sString( const uint_t& pos, const string& val );
        const bool sUint( const uint_t& pos, const uint_t& val );
        /**@}*/

        /** @name Internal */ /**@{*/
        Plugin( const string& name, const uint_t& type );
        virtual ~Plugin();
        /**@}*/

    private:
        bool m_bool[CFG_PLG_MAX_ARR]; /**< Any bool that needs to be tied back to the parent object member variables. */
        void* m_caller; /**< Pointer back to the object that initially loaded the plugin. */
        string m_name; /**< The name of the class the Plugin implements. */
        string m_string[CFG_PLG_MAX_ARR]; /**< Any string that needs to be tied back to the parent object member variables. */
        uint_t m_type; /**< The #PLG_TYPE of the class the Plugin implements. */
        uint_t m_uint[CFG_PLG_MAX_ARR]; /**< Any #uint_t that needs to be tied back to the parent object member variables. */
};

#endif
