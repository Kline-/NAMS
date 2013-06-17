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
 * @file thing.cpp
 * @brief All non-template member functions of the Thing class.
 *
 * A generic "thing": creature, character, room, object, etc.
 */
#include "h/includes.h"
#include "h/thing.h"

/* Core */
/**
 * @brief Adds a Thing to the contents of this Thing.
 * @param[in] thing A pointer to another Thing to be added to the contents of this Thing.
 * @retval false Returned if there was an error adding thing to the contents of this Thing.
 * @retval true Returned if thing was successfully added to the contents of this Thing.
 */
const bool Thing::AddThing( Thing* thing )
{
    UFLAGS_DE( flags );

    if ( thing == NULL )
    {
        LOGSTR( flags, "Thing::AddThing()-> called with NULL thing" );
        return false;
    }

    /** @todo Logic to check container size, content limits, etc. */

    if ( thing->m_container == NULL )
        thing->m_container = this;
    else
    {
        LOGSTR( flags, "Thing::AddThing()-> thing container is not NULL" );
        return false;
    }

    m_contents.push_back( thing );

    return true;
}

/**
 * @brief Moves a Thing from within one Location to another Location.
 * @param[in] location A pointer to the destination Location that this Thing should be moved into.
 * @retval false Returned if there was an error moving this Thing.
 * @retval true Returned if this Thing was successfully moved.
 */
const bool Thing::Move( Thing* source, Thing* destination )
{
    UFLAGS_DE( flags );

    if ( source == NULL )
    {
        LOGSTR( flags, "Thing::Move()-> called with NULL source" );
        return false;
    }

    if ( destination == NULL )
    {
        LOGSTR( flags, "Thing::Move()-> called with NULL destination" );
        return false;
    }

    // If already stored inside another Thing then we need to remove ourselves first.
    // If something prevents us from being removed, we can't move where we want to go.
    if ( !source->RemoveThing( this ) )
        return false;

    // Were we able to get into the destination Thing?
    if ( !destination->AddThing( this ) )
        return false;

    return true;
}

/**
 * @brief Removes a Thing to the contents of this Thing.
 * @param[in] thing A pointer to another Thing to be removed from the contents of this Thing.
 * @retval false Returned if there was an error removing thing from the contents of this Thing.
 * @retval true Returned if thing was successfully removed from the contents of this Thing.
 */
const bool Thing::RemoveThing( Thing* thing )
{
    UFLAGS_DE( flags );

    if ( thing == NULL )
    {
        LOGSTR( flags, "Thing::RemoveThing()-> called with NULL thing" );
        return false;
    }

    /** @todo Logic to check debuffs, restrictions, etc. */

    if ( find ( m_contents.begin(), m_contents.end(), thing ) != m_contents.end() )
    {
        m_contents.erase( find( m_contents.begin(), m_contents.end(), thing ) );
        thing->m_container = NULL;
    }

    return true;
}

/**
 * @brief Send data to all Thing objects within the Thing except for the speaker.
 * @param[in] msg The data to be sent.
 * @param[in] speaker The Thing originating the message.
 * @retval void
 */
const void Thing::Send( const string& msg, Thing* speaker ) const
{
    UFLAGS_DE( flags );
    CITER( vector, Thing*, ti );
    Thing* thing = NULL;

    if ( msg.empty() )
    {
        LOGSTR( flags, "Thing::Send()-> called with empty msg" );
        return;
    }

    for ( ti = m_contents.begin(); ti != m_contents.end(); ti++ )
    {
        thing = *ti;

        if ( thing == speaker )
            continue;
        else
            thing->Send( msg );
    }

    return;
}

/* Query */
/**
 * @brief Returns the Thing that this Thing is stored within.
 * @retval Thing* A pointer to the Thing that this Thing is stored within.
 */
Thing* Thing::gContainer() const
{
    return m_container;
}

/**
 * @brief Returns the contents of this Thing.
 * @retval vector<Thing*> The contents of this Thing.
 */
const vector<Thing*> Thing::gContents() const
{
    return m_contents;
}

/**
 * @brief Returns the description of the Thing from #THING_DESCRIPTION.
 * @param[in] type The specific description to retrieve.
 * @retval string The description referred to by type.
 */
const string Thing::gDescription( const uint_t& type ) const
{
    UFLAGS_DE( flags );

    if ( type < uintmin_t || type >= MAX_THING_DESCRIPTION )
    {
        LOGFMT( flags, "Thing::gDescription()-> called with invalid type %lu", type );
        return string();
    }

    return m_description[type];
}

/**
 * @brief Returns the id associated with this Thing.
 * @retval string A string containing the id associated with this Thing.
 */
const string Thing::gId() const
{
    return m_id;
}

/**
 * @brief Returns the name associated with this Thing.
 * @retval string A string containing the name associated with this Thing.
 */
const string Thing::gName() const
{
    return m_name;
}

/**
 * @brief Returns the inherited sub-type of this Thing.
 * @retval uint_t A value from #THING_TYPE.
 */
const uint_t Thing::gType() const
{
    return m_type;
}

/* Manipulate */
/**
 * @brief Sets the description of the Thing from #THING_DESCRIPTION.
 * @param[in] description The description.
 * @param[in] type The specific description to set.
 * @retval false Returned if there was an error setting the description.
 * @retval true Returned if the description was successfully set.
 */
const bool Thing::sDescription( const string& description, const uint_t& type )
{
    UFLAGS_DE( flags );

    if ( type < uintmin_t || type >= MAX_THING_DESCRIPTION )
    {
        LOGFMT( flags, "Thing::sDescription()-> called with invalid type %lu", type );
        return false;
    }

    m_description[type] = description;

    return true;
}

/**
 * @brief Sets the id of this Thing.
 * @param[in] id A string containing the id this Thing should be set to.
 * @retval false Returned if there was an error setting the id.
 * @retval true Returned if the id was set successfully.
 */
const bool Thing::sId( const string& id )
{
    UFLAGS_DE( flags );

    if ( id.length() < CFG_THG_ID_MIN_LEN || id.length() > CFG_THG_ID_MAX_LEN )
    {
        LOGFMT( flags, "Thing::sId()-> called with invalid id length %lu", id.length() );
        return false;
    }

    m_id = id;

    return true;
}

/**
 * @brief Sets the name of this Thing.
 * @param[in] name A string containing the name this Thing should be set to.
 * @param[in] system If true, denotes a system-level call (NPCs, etc) and will bypass length limits.
 * @retval false Returned if there was an error setting the name.
 * @retval true Returned if the name was set successfully.
 */
const bool Thing::sName( const string& name, const bool& system )
{
    UFLAGS_DE( flags );

    if ( ( name.length() < CFG_THG_NAME_MIN_LEN || name.length() > CFG_THG_NAME_MAX_LEN ) && !system )
    {
        LOGFMT( flags, "Thing::sName()-> called with invalid name length %lu", name.length() );
        return false;
    }

    m_name = name;

    return true;
}

/**
 * @brief Sets the inherited sub-type of this Thing.
 * @param[in] type A value from #THING_TYPE.
 * @retval false Returned if there was an error setting the type.
 * @retval true Returned if the type was set successfully.
 */
const bool Thing::sType( const uint_t& type )
{
    UFLAGS_DE( flags );

    if ( type < uintmin_t || type >= MAX_THING_TYPE )
    {
        LOGFMT( flags, "Thing::sType()-> called with invalid type %lu", type );
        return false;
    }

    m_type = type;

    return true;
}

/* Internal */
/**
 * @brief Generates a new unique id for this Thing.
 * @param[in] seed Typically the size of the owning list, such as object_list.
 * @retval void
 */
const void Thing::NewId( const uint_t& seed )
{
    string input;

    input += seed;
    input += m_name;
    input += chrono::duration_cast<chrono::milliseconds>( g_global->m_time_current.time_since_epoch() ).count();

    m_id = ::crypt( CSTR( input ), CSTR( Utils::Salt( Utils::String( chrono::duration_cast<chrono::milliseconds>( g_global->m_time_current.time_since_epoch() ).count() ) ) ) );

    return;
}

/**
 * @brief Constructor for the Thing class.
 */
Thing::Thing()
{
    uint_t i = uintmin_t;

    m_container = NULL;
    m_contents.clear();
    for ( i = 0; i < MAX_THING_DESCRIPTION; i++ )
        m_description[i].clear();
    m_id.clear();
    m_name.clear();
    m_type = THING_TYPE_THING;

    return;
}

/**
 * @brief Destructor for the Thing class.
 */
Thing::~Thing()
{
    if ( !g_global->m_shutdown )
    {
        // To properly handle quit
        if ( m_container != NULL )
           m_container->RemoveThing( this );
    }

    return;
}
