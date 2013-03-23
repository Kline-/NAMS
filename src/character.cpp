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
 * @file character.cpp
 * @brief All non-template member functions of the Character class.
 *
 * A Character is either autonomous and owned by a Server object or is
 * associated with an Account traceable back to a live SocketClient
 * end-user.
 */
#include "h/includes.h"
#include "h/class.h"

#include "h/character.h"
#include "h/list.h"

/* Core */
/**
 * @brief Unload a character from memory that was previously loaded via Character::New().
 * @retval void
 */
const void Character::Delete()
{
    gServer()->sCharacterNext( character_list.erase( find( character_list.begin(), character_list.end(), this ) ) );
    delete this;

    return;
}

/**
 * @brief Create a new character; wrapper for the virtual method in Thing.
 * @param[in] server The Server the character will exist within.
 * @retval false Returned if a new Character was successfully created or loaded.
 * @retval true Returned if a new Character was unable to be created.
 */
const bool Character::New( Server* server )
{
    sServer( server );

    character_list.push_back( this );

    return true;
}

/**
 * @brief Create a new character.
 * @param[in] server The Server the character will exist within.
 * @param[in] account The associated Account, if any.
 * @retval false Returned if a new Character was successfully created or loaded.
 * @retval true Returned if a new Character was unable to be created.
 */
const bool Character::New( Server* server, Account* account )
{
    sServer( server );
    m_account = account;

    character_list.push_back( this );

    return true;
}

/**
 * @brief Serialize the character data.
 * @retval false Returned if there was an error serializing the character.
 * @retval true Returned if the character was serialized successfully.
 */
const bool Character::Serialize() const
{
    UFLAGS_DE( flags );
    ofstream ofs;
    string value;
    stringstream line;
    string file( Utils::FileExt( gId(), CFG_DAT_FILE_PLR_EXT ) );

    Utils::FileOpen( ofs, file );

    if ( !ofs.good() )
    {
        LOGFMT( flags, "Character::Serialize()-> failed to open character file: %s", CSTR( file ) );
        return false;
    }

    // First to ensure name is loaded for logging later
    KEY( ofs, "name", gName() );
    KEY( ofs, "id", gId() );
    KEY( ofs, "sex", m_sex );

    Utils::FileClose( ofs, Utils::DirPath( CFG_DAT_DIR_ACCOUNT, gAccount()->gName() ), CSTR( file ) );

    return true;
}

/**
 * @brief Unserialize the character data.
 * @retval false Returned if there was an error unserializing the character.
 * @retval true Returned if the character was unserialized successfully.
 */
const bool Character::Unserialize()
{
    UFLAGS_DE( flags );
    UFLAGS_I( finfo );
    ifstream ifs;
    string key, value, line;
    bool found = false, maxb = false;
    string file( Utils::FileExt( gId(), CFG_DAT_FILE_ACT_EXT ) );

    Utils::FileOpen( ifs, Utils::DirPath( CFG_DAT_DIR_ACCOUNT, gAccount()->gName() ), file );

    if ( !ifs.good() )
    {
        LOGFMT( flags, "Character::Unserialize()-> failed to open character file: %s", CSTR( file ) );
        return false;
    }

    while ( getline( ifs, line ) )
    {
        if ( !Utils::KeyValue( key, value, line) )
        {
            LOGFMT( flags, "Character::Unserialize()-> error reading line: %s", CSTR( line ) );
            continue;
        }

        for ( ;; )
        {
            found = false;
            maxb = false;

            // First to ensure name is loaded for logging later
            if ( key == "name" )
            {
                found = true;
                sName( value );
            }
            if ( key == "id" )
            {
                found = true;
                sId( value );
            }
            Utils::KeySet( true, found, key, "sex", value, m_sex, MAX_CHR_SEX, maxb );

            if ( !found )
                LOGFMT( flags, "Character::Unserialize()->Utils::KeySet()-> key not found: %s", CSTR( key ) );

            if ( maxb )
                LOGFMT( finfo, "Character::Unserialize()->Utils::KeySet()-> character id %s, key %s has illegal value %s", CSTR( gId() ), CSTR( key ), CSTR( value ) );

            break;
        }
    }

    Utils::FileClose( ifs );

    return true;
}

/* Query */
/**
 * @brief Returns the Account associated with this Character, if any.
 * @retval Account* A pointer to the associated account, or NULL if none.
 */
Account* Character::gAccount() const
{
    return m_account;
}

/**
 * @brief Gets the creation states of this character from #CHR_CREATION.
 * @param[in] pos The creation state to get.
 * @retval false Returned if the state is set to false.
 * @retval true Returned if the state is set to true.
 */
const bool Character::gCreation( const uint_t& pos )
{
    UFLAGS_DE( flags );

    if ( pos < uintmin_t || pos >= MAX_CHR_CREATION )
    {
        LOGFMT( flags, "Character::sCreation()-> called with invalid pos %lu", pos );
        return false;
    }

    return m_creation[pos];
}

/**
 * @brief Returns the sex of this character from #CHR_SEX.
 * @retval uint_t A uint_t associated to #CHR_SEX.
 */
const uint_t Character::gSex() const
{
    return m_sex;
}

/* Manipulate */
/**
 * @brief Sets the creation states of this character from #CHR_CREATION.
 * @param[in] pos The creation state to set.
 * @param[in] val The value to set.
 * @retval false Returned if the state was unable to be set.
 * @retval true Returned if the state was successfully set.
 */
const bool Character::sCreation( const uint_t& pos, const bool& val )
{
    UFLAGS_DE( flags );

    if ( pos < uintmin_t || pos >= MAX_CHR_CREATION )
    {
        LOGFMT( flags, "Character::sCreation()-> called with invalid pos %lu", pos );
        return false;
    }

    m_creation[pos] = val;

    return true;
}

/**
 * @brief Sets the sex of this character from #CHR_SEX.
 * @param[in] sex A value from #CHR_SEX.
 * @retval false Returned if unable to set the sex of the character.
 * @retval true Returned if the sex of the character was successfully set.
 */
const bool Character::sSex( const uint_t& sex )
{
    UFLAGS_DE( flags );

    if ( sex < uintmin_t || sex >= MAX_CHR_SEX )
    {
        LOGFMT( flags, "Character::sSex()-> called with invalid sex %lu", sex );
        return false;
    }

    m_sex = sex;

    return true;
}

/* Internal */
/**
 * @brief Constructor for the Character class.
 */
Character::Character()
{
    uint_t i = uintmin_t;

    m_account = NULL;
    for ( i = 0; i < MAX_CHR_CREATION; i++ )
        m_creation[i] = false;
    m_sex = 0;

    return;
}

/**
 * @brief Destructor for the Character class.
 */
Character::~Character()
{
    return;
}
