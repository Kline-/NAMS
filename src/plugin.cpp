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
 * @file plugin.cpp
 * @brief All non-template member functions of the Plugin class.
 *
 * Plugin objects provide an API to interface with the NAMS server in
 * different ways. Each Plugin is compiled and loaded as an indepdendent shared
 * object that implements a new class.
 *
 * Server::BuildPlugin() will automatically compile any plugins at boot time
 * unless an existing shared object file is already found in #CFG_DAT_DIR_OBJ.
 */
#include "h/includes.h"
#include "h/class.h"

#include "h/plugin.h"

/* Core */

/* Query */
/**
 * @brief Return the name of the object.
 * @retval string A string containing the name of the object.
 */
const string Plugin::gName() const
{
    return m_name;
}

/* Manipulate */

/* Internal */
/**
 * @brief Constructor for the Plugin class.
 * @param[in] name The name of the class implemented by the Plugin.
 * @param[in] type The #PLG_TYPE of the Plugin to assist in the API implementation for specific object types.
 */
Plugin::Plugin( const string& name, const uint_t& type )
{
    uint_t i = 0;

    for ( i = 0; i < CFG_PLG_MAX_ARR; i++ )
        m_bool[i] = false;
    m_name = name;
    for ( i = 0; i < CFG_PLG_MAX_ARR; i++ )
        m_string[i].clear();
    m_type = type;
    for ( i = 0; i < CFG_PLG_MAX_ARR; i++ )
        m_uint_t[i] = uintmin_t;

    return;
}

/**
 * @brief Destructor for the Plugin class.
 */
Plugin::~Plugin()
{
    return;
}