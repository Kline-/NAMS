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
 * @file object.cpp
 * @brief All non-template member functions of the Object class.
 *
 * An Object is a representation of a physical location within the game world.
 */
#include "h/includes.h"
#include "h/object.h"

#include "h/list.h"

/* Core */
/**
 * @brief Clones an Object from the object_template_list into the object_list.
 * @param[in] name The name of the template object to clone.
 * @param[in] type The field to search against, from #HANDLER_FIND.
 * @retval false Returned if there was an error cloning the Object.
 * @retval true Returned if the Object was successfully cloned.
 */
const bool Object::Clone( const string& name, const uint_t& type )
{
    UFLAGS_DE( flags );
    Object* obj = NULL;
    uint_t search = type;

    if ( name.empty() )
    {
        LOGSTR( flags, "Object::Clone()-> called with empty name" );
        return false;
    }

    if ( search < uintmin_t || search >= MAX_HANDLER_FIND )
    {
        LOGFMT( flags, "Object::Clone()-> called with invalid type: %lu", search );
        LOGSTR( flags, "Object::Clone()-> defaulting to HANDLER_FIND_ID" );
        search = HANDLER_FIND_ID;
    }

    if ( ( obj = Handler::FindObject( name, type, object_template_list ) ) == NULL )
        return false;

    /** Copy elements from Thing parent class */
    for ( search = 0; search < MAX_THING_DESCRIPTION; search++ )
        sDescription( obj->gDescription( search ), search );
    sName( obj->gName() );
    /** Copy elements internal to Object class */
    m_file = obj->m_file;
    m_zone = obj->m_zone;
    /** Generate a unique id based on obj_list.size() and current time */
    NewId( object_list.size() );

    object_list.push_back( this );

    return true;
}

/**
 * @brief Unload an object from memory that was previously loaded via Object::New() or Object::Clone().
 * @retval void
 */
const void Object::Delete()
{
    if ( find( object_list.begin(), object_list.end(), this ) != object_list.end() )
        object_list.erase( find( object_list.begin(), object_list.end(), this ) );
    else if ( find( object_template_list.begin(), object_template_list.end(), this ) != object_template_list.end() )
            object_template_list.erase( find( object_template_list.begin(), object_template_list.end(), this ) );

    delete this;

    return;
}

/**
 * @brief Interprets cmd with args at level security.
 * @param[in] security Security level which is inherited from the Account, if any. Values from #ACT_SECURITY.
 * @param[in] cmd The command to search for and attempt to run.
 * @param[in] args Arguments to be passed to the command.
 * @retval void
 */
const void Object::Interpret( const uint_t& security, const string& cmd, const string& args )
{
    return;
}

/**
 * @brief Create a new object.
 * @param[in] file The filename to load including the full path prepended to it.
 * @retval false Returned if a new Object was successfully created or loaded.
 * @retval true Returned if a new Object was unable to be created.
 */
const bool Object::New( const string& file )
{
    UFLAGS_DE( flags );
    Object* object = NULL;

    m_file = file;

    if ( !Unserialize() )
    {
        LOGFMT( flags, "Object::New()->Object::Unserialize()-> returned false for file %s", CSTR( file ) );
        return false;
    }

    // Check for duplicate Object ids
    if ( ( object = Handler::FindObject( gId(), HANDLER_FIND_ID, object_template_list ) ) != NULL )
    {
        LOGFMT( flags, "Object::New()->Handler::FindObject()-> didn't return NULL, object %s has duplicate id of %s", CSTR( m_file ), CSTR( gId() ) );
        return false;
    }

    object_template_list.push_back( this );

    return true;
}

/**
 * @brief Serialize the object data.
 * @retval false Returned if there was an error serializing the object.
 * @retval true Returned if the object was serialized successfully.
 */
const bool Object::Serialize() const
{
    UFLAGS_DE( flags );
    ofstream ofs;
    string value;
    stringstream line;
    uint_t i = uintmin_t;
    string file( Utils::FileExt( gId(), CFG_DAT_FILE_OBJ_EXT ) );

    Utils::FileOpen( ofs, file );

    if ( !ofs.good() )
    {
        LOGFMT( flags, "Object::Serialize()-> failed to open object file: %s", CSTR( file ) );
        return false;
    }

    // First to ensure proper handling in the future
    KEY( ofs, "revision", CFG_OBJ_REVISION );
    // Second to ensure id is loaded for logging later
    KEY( ofs, "id", gId() );
    KEYLISTLOOP( ofs, "description", i ); /** @todo Need to find a nicer way to do this */
    {
        for ( i = 0; i < MAX_THING_DESCRIPTION; i++ )
            ofs << "description[" << i << "]" << " = " << Utils::WriteString( gDescription( i ) ) << endl;
    }
    KEY( ofs, "name", gName() );
    KEY( ofs, "zone", m_zone );

    Utils::FileClose( ofs, Utils::DirPath( CFG_DAT_DIR_WORLD, m_zone ), CSTR( file ) );

    return true;
}

/**
 * @brief Unserialize the object data.
 * @retval false Returned if there was an error unserializing the object.
 * @retval true Returned if the object was unserialized successfully.
 */
const bool Object::Unserialize()
{
    UFLAGS_DE( flags );
    UFLAGS_I( finfo );
    ifstream ifs;
    string key, value, line;
    stringstream loop, mline;
    bool found = false, maxb = false;
    uint_t revision = uintmin_t;

    Utils::FileOpen( ifs, m_file );

    if ( !ifs.good() )
    {
        LOGFMT( flags, "Object::Unserialize()-> failed to open object file: %s", CSTR( m_file ) );
        return false;
    }

    while ( getline( ifs, line ) )
    {
        if ( line.empty() )
            getline( ifs, line );

        if ( !Utils::KeyValue( key, value, line) )
        {
            LOGFMT( flags, "Object::Unserialize()-> error reading line: %s", CSTR( line ) );
            continue;
        }

        for ( ;; )
        {
            found = false;
            maxb = false;

            if ( Utils::StrPrefix( "description", key ) )
            {
                found = true;
                sDescription( Utils::ReadString( ifs ), Utils::ReadIndex( key ) );
            }
            else if ( key == "id" )
            {
                found = true;
                sId( value );
            }
            else if ( key == "name" )
            {
                found = true;
                sName( value );
            }
            Utils::KeySet( true, found, key, "revision", value, revision, CFG_OBJ_REVISION, maxb );
            Utils::KeySet( true, found, key, "zone", value, m_zone );

            if ( !found )
                LOGFMT( flags, "Object::Unserialize()-> key not found: %s", CSTR( key ) );

            if ( maxb )
                LOGFMT( finfo, "Object::Unserialize()-> object id %s, key %s has illegal value %s", CSTR( gId() ), CSTR( key ), CSTR( value ) );

            break;
        }
    }

    Utils::FileClose( ifs );

    return true;
}

/* Query */

/* Manipulate */

/* Internal */
/**
 * @brief Constructor for the Object class.
 */
Object::Object()
{
    m_file.clear();
    m_zone.clear();

    return;
}

/**
 * @brief Destructor for the Object class.
 */
Object::~Object()
{
    return;
}
