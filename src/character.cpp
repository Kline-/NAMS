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
 * A Character is either autonomous or is associated with an Account traceable
 * back to a live SocketClient end-user.
 */
#include "h/includes.h"
#include "h/character.h"

#include "h/account.h"
#include "h/command.h"
#include "h/list.h"
#include "h/location.h"
#include "h/socketclient.h"
#include "h/exit.h"

/* Core */
/**
 * @brief Unload a character from memory that was previously loaded via Character::New().
 * @retval void
 */
const void Character::Delete()
{
    if ( find( character_list.begin(), character_list.end(), this ) != character_list.end() )
        g_global->m_next_character = character_list.erase( find( character_list.begin(), character_list.end(), this ) );

    if ( m_account )
        m_account->sCharacter( NULL );

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
const void Character::Interpret( const uint_t& security, const string& cmd, const string& args )
{
    Command* command = NULL;
    Exit* exit = NULL;

    if ( ( command = Handler::FindCommand( cmd ) ) != NULL )
    {
        if ( command->Authorized( security ) )
            command->Run( this, cmd, args );
        else
            Send( CFG_STR_CMD_INVALID );
    }
    else if ( ( exit = Handler::FindExit( gLocation(), cmd ) ) != NULL ) // Search for an exit
    {
        if ( !Move( exit->gDestination() ) )
            Send( CFG_STR_CMD_INVALID );
        else if ( ( command = Handler::FindCommand( "look" ) ) != NULL ) /** @todo Make this configurable per-account/character */
            command->Run( this );
    }
    else // Give up
        Send( CFG_STR_CMD_INVALID );

    return;
}

/**
 * @brief Create a new character.
 * @param[in] file The filename to load without any path prepended to it.
 * @param[in] exists True if the character should be loaded, false if a new one need be created.
 * @retval false Returned if a new Character was successfully created or loaded.
 * @retval true Returned if a new Character was unable to be created.
 */
const bool Character::New( const string& file, const bool& exists )
{
    UFLAGS_DE( flags );

    m_file = file;

    if ( exists )
    {
        if ( !Unserialize() )
        {
            LOGFMT( flags, "Character::New()->Character::Unserialize()-> returned false for file %s", CSTR( file ) );
            return false;
        }
    }

    character_list.push_back( this );

    return true;
}

/**
 * @brief Send data to the associated SocketClient, if any.
 * @param[in] msg The data to be sent.
 * @param[in] speaker The Thing originating the message.
 * @retval void
 */
const void Character::Send( const string& msg, Thing* speaker ) const
{
    if ( m_account )
        if ( m_account->gClient() )
            m_account->gClient()->Send( msg );

    return;
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
    uint_t i = uintmin_t;
    string file( Utils::FileExt( gId(), CFG_DAT_FILE_PLR_EXT ) );

    Utils::FileOpen( ofs, file );

    if ( !ofs.good() )
    {
        LOGFMT( flags, "Character::Serialize()-> failed to open character file: %s", CSTR( file ) );
        return false;
    }

    // First to ensure proper handling in the future
    KEY( ofs, "revision", CFG_CHR_REVISION );
    // Second to ensure id is loaded for logging later
    KEY( ofs, "id", gId() );
    KEYLISTLOOP( ofs, "description", i ); /** @todo Need to find a nicer way to do this */
    {
        for ( i = 0; i < MAX_THING_DESCRIPTION; i++ )
            ofs << "description[" << i << "]" << " = " << Utils::WriteString( gDescription( i ) ) << endl;
    }
    if ( gLocation() != NULL ) // This will be NULL during creation
        KEY( ofs, "location", gLocation()->gId() );
    KEY( ofs, "name", gName() );
    KEY( ofs, "sex", m_sex );

    Utils::FileClose( ofs, Utils::DirPath( CFG_DAT_DIR_ACCOUNT, gAccount()->gId() ), CSTR( file ) );

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
    stringstream loop, mline;
    bool found = false, maxb = false;
    uint_t revision = uintmin_t;

    Utils::FileOpen( ifs, Utils::DirPath( Utils::DirPath( CFG_DAT_DIR_ACCOUNT, gAccount()->gId() ), m_file ) );

    if ( !ifs.good() )
    {
        LOGFMT( flags, "Character::Unserialize()-> failed to open character file: %s", CSTR( m_file ) );
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
            else if ( key == "location" )
            {
                found = true;
                gAccount()->gClient()->sLogin( SOC_LOGIN_LOCATION, value );
            }
            else if ( key == "name" )
            {
                found = true;
                sName( value );
            }

            Utils::KeySet( true, found, key, "revision", value, revision, CFG_CHR_REVISION, maxb );
            Utils::KeySet( true, found, key, "sex", value, m_sex, MAX_CHR_SEX, maxb );

            if ( !found )
                LOGFMT( flags, "Character::Unserialize()-> key not found: %s", CSTR( key ) );

            if ( maxb )
                LOGFMT( finfo, "Character::Unserialize()-> character id %s, key %s has illegal value %s", CSTR( gId() ), CSTR( key ), CSTR( value ) );

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
 * @brief Generates a status-bar prompt based on tokens.
 * @retval string A string containing the status information.
 */
const string Character::gPrompt() const
{
    stringstream output;

    output << CRLF << gId() << "> ";

    return output.str();
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
 * @brief Sets the account of this character.
 * @param[in] account A pointer to the Account to be associated with this character.
 * @retval false Returned if unable to associate the account with this character.
 * @retval true Returned if the account was successfully associated.
 */
const bool Character::sAccount( Account* account )
{
    UFLAGS_DE( flags );

    if ( m_account != NULL && account != NULL )
    {
        LOGSTR( flags, "Character::sAccount()-> called while m_account is not NULL" );
        return false;
    }

    m_account = account;

    return true;
}

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

    /** Initialize attributes from parent Thing class */
    sType( THING_TYPE_CHARACTER );
    /** Initialize attributes specific to Characters */
    m_account = NULL;
    for ( i = 0; i < MAX_CHR_CREATION; i++ )
        m_creation[i] = false;
    m_file.clear();
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
