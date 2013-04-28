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
 * @file location.cpp
 * @brief All non-template member functions of the Location class.
 *
 * A Location is a representation of a physical location within the game world.
 */
#include "h/includes.h"
#include "h/location.h"

#include "h/list.h"

/* Core */
/**
 * @brief Unload a location from memory that was previously loaded via Location::New().
 * @retval void
 */
const void Location::Delete()
{
    location_list.erase( find( location_list.begin(), location_list.end(), this ) );
    delete this;

    return;
}

/**
 * @brief Create a new location.
 * @param[in] server The Server the location will exist within.
 * @param[in] file The filename to load including the full path prepended to it.
 * @retval false Returned if a new Location was successfully created or loaded.
 * @retval true Returned if a new Location was unable to be created.
 */
const bool Location::New( Server* server, const string& file )
{
    UFLAGS_DE( flags );

    m_file = file;
    sServer( server );

    if ( !Unserialize() )
    {
        LOGFMT( flags, "Location::New()->Location::Unserialize()-> returned false for file %s", CSTR( file ) );
        return false;
    }

    location_list.push_back( this );

    return true;
}

/**
 * @brief Serialize the location data.
 * @retval false Returned if there was an error serializing the location.
 * @retval true Returned if the location was serialized successfully.
 */
const bool Location::Serialize() const
{
    UFLAGS_DE( flags );
    ofstream ofs;
    string value;
    stringstream line;
    string file( Utils::FileExt( gId(), CFG_DAT_FILE_LOC_EXT ) );

    Utils::FileOpen( ofs, file );

    if ( !ofs.good() )
    {
        LOGFMT( flags, "Location::Serialize()-> failed to open location file: %s", CSTR( file ) );
        return false;
    }

    // First to ensure id is loaded for logging later
    KEY( ofs, "id", gId() );
    KEY( ofs, "name", gName() );
    KEY( ofs, "zone", m_zone );

    Utils::FileClose( ofs, Utils::DirPath( CFG_DAT_DIR_WORLD, m_zone ), CSTR( file ) );

    return true;
}

/**
 * @brief Unserialize the location data.
 * @retval false Returned if there was an error unserializing the location.
 * @retval true Returned if the location was unserialized successfully.
 */
const bool Location::Unserialize()
{
    UFLAGS_DE( flags );
    UFLAGS_I( finfo );
    ifstream ifs;
    string key, value, line;
    bool found = false, maxb = false;

    Utils::FileOpen( ifs, m_file );

    if ( !ifs.good() )
    {
        LOGFMT( flags, "Location::Unserialize()-> failed to open location file: %s", CSTR( m_file ) );
        return false;
    }

    while ( getline( ifs, line ) )
    {
        if ( !Utils::KeyValue( key, value, line) )
        {
            LOGFMT( flags, "Location::Unserialize()-> error reading line: %s", CSTR( line ) );
            continue;
        }

        for ( ;; )
        {
            found = false;
            maxb = false;

            // First to ensure id is loaded for logging later
            if ( key == "id" )
            {
                found = true;
                sId( value );
            }
            if ( key == "name" )
            {
                found = true;
                sName( value );
            }
            Utils::KeySet( true, found, key, "zone", value, m_zone );

            if ( !found )
                LOGFMT( flags, "Location::Unserialize()->Utils::KeySet()-> key not found: %s", CSTR( key ) );

            if ( maxb )
                LOGFMT( finfo, "Location::Unserialize()->Utils::KeySet()-> location id %s, key %s has illegal value %s", CSTR( gId() ), CSTR( key ), CSTR( value ) );

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
 * @brief Constructor for the Location class.
 */
Location::Location()
{
    m_file.clear();
    m_zone.clear();

    return;
}

/**
 * @brief Destructor for the Location class.
 */
Location::~Location()
{
    return;
}
