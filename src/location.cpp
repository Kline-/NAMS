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

#include "h/exit.h"
#include "h/list.h"

/* Core */
/**
 * @brief Unload a location from memory that was previously loaded via Location::New().
 * @retval void
 */
const void Location::Delete()
{
    Exit* exit = NULL;
    ITER( list, Exit*, ei );

    for ( ei = m_exits.begin(); ei != m_exits.end(); )
    {
        exit = *ei;

        ei = m_exits.erase( ei );
        exit->Delete();
    }

    location_list.erase( find( location_list.begin(), location_list.end(), this ) );
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
const void Location::Interpret( const uint_t& security, const string& cmd, const string& args )
{
    return;
}

/**
 * @brief Create a new location.
 * @param[in] file The filename to load including the full path prepended to it.
 * @retval false Returned if a new Location was successfully created or loaded.
 * @retval true Returned if a new Location was unable to be created.
 */
const bool Location::New( const string& file )
{
    UFLAGS_DE( flags );

    m_file = file;

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
    uint_t i = uintmin_t;
    CITER( list, Exit*, ei );
    Exit* exit = NULL;
    string file( Utils::FileExt( gId(), CFG_DAT_FILE_LOC_EXT ) );

    Utils::FileOpen( ofs, file );

    if ( !ofs.good() )
    {
        LOGFMT( flags, "Location::Serialize()-> failed to open location file: %s", CSTR( file ) );
        return false;
    }

    // First to ensure id is loaded for logging later
    KEY( ofs, "id", gId() );
    KEYLISTLOOP( ofs, "description", i ); /** @todo Need to find a nicer way to do this */
    {
        for ( i = 0; i < MAX_THING_DESCRIPTION; i++ )
        {
            ofs << "description[" << i << "]" << " = ";

            if ( !gDescription( i ).empty() )
                ofs << Utils::WriteString( gDescription( i ) ) << endl;
            else
                ofs << endl;
        }
    }
    KEYLISTLOOP( ofs, "exit", i ); /** @todo Need to find a nicer way to do this */
    {
        for ( ei = m_exits.begin(); ei != m_exits.end(); ei++ )
        {
            exit = *ei;
            ofs << "exit = " << exit->Serialize() << endl;
        }
    }
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
    stringstream loop, mline;
    bool found = false, maxb = false;

    Utils::FileOpen( ifs, m_file );

    if ( !ifs.good() )
    {
        LOGFMT( flags, "Location::Unserialize()-> failed to open location file: %s", CSTR( m_file ) );
        return false;
    }

    while ( getline( ifs, line ) )
    {
        if ( line.empty() )
            getline( ifs, line );

        if ( !Utils::KeyValue( key, value, line) )
        {
            LOGFMT( flags, "Location::Unserialize()-> error reading line: %s", CSTR( line ) );
            continue;
        }

        for ( ;; )
        {
            found = false;
            maxb = false;

            if ( key == "id" )
            {
                found = true;
                sId( value );
            }
            if ( Utils::StrPrefix( "description", key ) )
            {
                found = true;
                sDescription( Utils::ReadString( ifs ), Utils::ReadIndex( key ) );
            }
            if ( key == "exit" )
            {
                Exit* exit = NULL;

                found = true;
                exit = new Exit();

                if ( !exit->New( this ) )
                {
                    LOGSTR( flags, "Location::Unserialize()->Exit::New()-> returned false" );
                    delete exit;
                }
                else
                {
                    if ( !exit->Unserialize( value ) )
                    {
                        LOGSTR( flags, "Location::Unserialize()->Exit::Unserialize()-> returned false" );
                        delete exit;
                    }
                    else
                        m_exits.push_back( exit );
                }
            }
            if ( key == "name" )
            {
                found = true;
                sName( value );
            }
            Utils::KeySet( true, found, key, "zone", value, m_zone );

            if ( !found )
                LOGFMT( flags, "Location::Unserialize()-> key not found: %s", CSTR( key ) );

            if ( maxb )
                LOGFMT( finfo, "Location::Unserialize()-> location id %s, key %s has illegal value %s", CSTR( gId() ), CSTR( key ), CSTR( value ) );

            break;
        }
    }

    Utils::FileClose( ifs );

    return true;
}

/* Query */
/**
 * @brief Returns the list of Exits associated to this Location.
 * @retval list<Exit*> A list of pointers to Exit objects associated with this Location.
 */
list<Exit*> Location::gExits() const
{
    return m_exits;
}

/* Manipulate */

/* Internal */
/**
 * @brief Constructor for the Location class.
 */
Location::Location()
{
    m_exits.clear();
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
