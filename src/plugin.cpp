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
 * different ways. Each Plugin is compiled and loaded as an independent shared
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
 * @brief Return a bool value to tie into other object types.
 * @param[in] pos The array position to be returned.
 * @retval false Returned if the m_bool[pos] is set to false.
 * @retval true Returned if the m_bool[pos] is set to true.
 */
const bool Plugin::gBool( const uint_t& pos ) const
{
    UFLAGS_DE( flags );

    if ( pos < uintmin_t || pos >= CFG_PLG_MAX_ARR )
    {
        LOGFMT( flags, "Plugin::gBool()-> called with invalid pos: %lu", pos );
        return false;
    }

    return m_bool[pos];
}

/**
 * @brief Return a pointer to the object that initialized this Plugin.
 * @retval void* A pointer to the object that initialized this Plugin.
 */
void* Plugin::gCaller() const
{
    return m_caller;
}

/**
 * @brief Return the name of the object.
 * @retval string A string containing the name of the object.
 */
const string Plugin::gName() const
{
    return m_name;
}

/**
 * @brief Return a string value to tie into other object types.
 * @param[in] pos The array position to be returned.
 * @retval string The value of m_string[pos].
 */
const string Plugin::gString( const uint_t& pos ) const
{
    UFLAGS_DE( flags );

    if ( pos < uintmin_t || pos >= CFG_PLG_MAX_ARR )
    {
        LOGFMT( flags, "Plugin::gString()-> called with invalid pos: %lu", pos );
        return string();
    }

    return m_string[pos];
}

/**
 * @brief Return a uint_t value to tie into other object types.
 * @param[in] pos The array position to be returned.
 * @retval uint_t The value of m_uint[pos].
 */
const uint_t Plugin::gUint( const uint_t& pos ) const
{
    UFLAGS_DE( flags );

    if ( pos < uintmin_t || pos >= CFG_PLG_MAX_ARR )
    {
        LOGFMT( flags, "Plugin::gUint()-> called with invalid pos: %lu", pos );
        return uintmin_t;
    }

    return m_uint[pos];
}

/* Manipulate */
/**
 * @brief Sets a bool value to tie into other object types.
 * @param[in] pos The array position to be assigned val.
 * @param[in] val The value to assign pos.
 * @retval false Returned if pos is out of range or an error occurs.
 * @retval true Returned if pos was successfully set to val.
 */
const bool Plugin::sBool( const uint_t& pos, const bool& val )
{
    UFLAGS_DE( flags );

    if ( pos < uintmin_t || pos >= CFG_PLG_MAX_ARR )
    {
        LOGFMT( flags, "Plugin::sBool()-> called with invalid pos: %lu", pos );
        return false;
    }

    m_bool[pos] = val;

    return true;
}

/**
 * @brief Sets a pointer to the object that initialized this Plugin.
 * @param[in] caller A pointer to the object that initialized this Plugin.
 * @retval false Returned if caller is NULL.
 * @retval true Returned if caller is not NULL.
 */
const bool Plugin::sCaller( void* caller )
{
    UFLAGS_DE( flags );

    if ( !caller )
    {
        LOGSTR( flags, "Plugin::sCaller()-> called with NULL caller" );
        return false;
    }

    m_caller = caller;

    return true;
}

/**
 * @brief Sets a string value to tie into other object types.
 * @param[in] pos The array position to be assigned val.
 * @param[in] val The value to assign pos.
 * @retval false Returned if pos is out of range or an error occurs.
 * @retval true Returned if pos was successfully set to val.
 */
const bool Plugin::sString( const uint_t& pos, const string& val )
{
    UFLAGS_DE( flags );

    if ( pos < uintmin_t || pos >= CFG_PLG_MAX_ARR )
    {
        LOGFMT( flags, "Plugin::sString()-> called with invalid pos: %lu", pos );
        return false;
    }

    m_string[pos] = val;

    return true;
}

/**
 * @brief Sets a uint_t value to tie into other object types.
 * @param[in] pos The array position to be assigned val.
 * @param[in] val The value to assign pos.
 * @retval false Returned if pos is out of range or an error occurs.
 * @retval true Returned if pos was successfully set to val.
 */
const bool Plugin::sUint( const uint_t& pos, const uint_t& val )
{
    UFLAGS_DE( flags );

    if ( pos < uintmin_t || pos >= CFG_PLG_MAX_ARR )
    {
        LOGFMT( flags, "Plugin::sUint()-> called with invalid pos: %lu", pos );
        return false;
    }

    if ( val < uintmin_t || val >= uintmax_t )
    {
        LOGFMT( flags, "Plugin::sUint()-> called with invalid val: %lu", val );
        return false;
    }

    m_uint[pos] = val;

    return true;
}

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
    m_caller = NULL;
    m_name = name;
    for ( i = 0; i < CFG_PLG_MAX_ARR; i++ )
        m_string[i].clear();
    m_type = type;
    for ( i = 0; i < CFG_PLG_MAX_ARR; i++ )
        m_uint[i] = uintmin_t;

    return;
}

/**
 * @brief Destructor for the Plugin class.
 */
Plugin::~Plugin()
{
    return;
}