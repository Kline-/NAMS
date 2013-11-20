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
#include "h/brain.h"
#include "h/command.h"
#include "h/list.h"
#include "h/location.h"
#include "h/socketclient.h"
#include "h/exit.h"

/* Core */
/**
 * @brief Clones a Character from the character_template_list into the character_list.
 * @param[in] name The name of the template character to clone.
 * @param[in] type The field to search against, from #HANDLER_FIND.
 * @retval false Returned if there was an error cloning the Character.
 * @retval true Returned if the Character was successfully cloned.
 */
const bool Character::Clone( const string& name, const uint_t& type )
{
    UFLAGS_DE( flags );
    Character* chr = NULL;
    uint_t search = type;

    if ( name.empty() )
    {
        LOGSTR( flags, "Character::Clone()-> called with empty name" );
        return false;
    }

    if ( search < uintmin_t || search >= MAX_HANDLER_FIND )
    {
        LOGFMT( flags, "Character::Clone()-> called with invalid type: %lu", search );
        LOGSTR( flags, "Character::Clone()-> defaulting to HANDLER_FIND_ID" );
        search = HANDLER_FIND_ID;
    }

    if ( ( chr = Handler::FindCharacter( name, type, character_template_list ) ) == NULL )
        return false;

    /** Copy elements from Thing parent class */
    for ( search = 0; search < MAX_THING_DESCRIPTION; search++ )
        sDescription( chr->gDescription( search ), search );
    sName( chr->gName() );
    sZone( chr->gZone() );
    /** Copy elements internal to Character class */
    for ( search = 0; search < MAX_CHR_CREATION; search++ )
        sCreation( chr->gCreation( search ), search );
    m_file = chr->m_file;
    m_sex = chr->m_sex;
    /** Generate a unique id based on obj_list.size() and current time */
    NewId( character_list.size() );

    character_list.push_back( this );

    return true;
}

/**
 * @brief Unload a character from memory that was previously loaded via Character::New().
 * @retval void
 */
const void Character::Delete()
{
    if ( find( character_list.begin(), character_list.end(), this ) != character_list.end() )
        g_global->m_next_character = character_list.erase( find( character_list.begin(), character_list.end(), this ) );
    else if ( find( character_template_list.begin(), character_template_list.end(), this ) != character_template_list.end() )
            character_template_list.erase( find( character_template_list.begin(), character_template_list.end(), this ) );

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
    else if ( ( exit = Handler::FindExit( cmd, dynamic_cast<Location*>( gContainer() ) ) ) != NULL ) // Search for an exit
    {
        if ( Move( gContainer(), exit->gDestination(), exit ) )
        {
            // Auto-look
            if ( ( command = Handler::FindCommand( "look" ) ) != NULL ) /** @todo Make this configurable per-account/character */
                command->Run( this );
        }
        else
            Send( CFG_STR_CMD_INVALID );
    }
    else // Give up
        Send( CFG_STR_CMD_INVALID );

    return;
}

/**
 * @brief Create a new character.
 * @param[in] file The filename to load without any path prepended to it.
 * @param[in] itemplate True if the character should be loaded into the character_template_list, otherwise it will be loaded into the character_list.
 * @param[in] exists True if the character should be loaded, false if a new one need be created.
 * @retval false Returned if a new Character was successfully created or loaded.
 * @retval true Returned if a new Character was unable to be created.
 */
const bool Character::New( const string& file, const bool& itemplate, const bool& exists )
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

    if ( itemplate )
        character_template_list.push_back( this );
    else
        character_list.push_back( this );

    return true;
}

/**
 * @brief Send data to the associated SocketClient, if any.
 * @param[in] msg The data to be sent.
 * @param[in] speaker The Thing originating the message.
 * @param[in] target The Thing who is the target of the message.
 * @retval void
 */
const void Character::Send( const string& msg, Thing* speaker, Thing* target ) const
{
    if ( gBrain()->gAccount() )
        if ( gBrain()->gAccount()->gClient() )
            gBrain()->gAccount()->gClient()->Send( msg );

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
    string file;

    // If the Brain is attached to an account, serialize as a player character, otherwise serialize as a NPC
    if ( gBrain()->gAccount() )
        file = Utils::FileExt( gId(), CFG_DAT_FILE_PLR_EXT );
    else
        file = Utils::FileExt( gId(), CFG_DAT_FILE_NPC_EXT );

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
    if ( gContainer() != NULL ) // This will be NULL during creation
        KEY( ofs, "location", gContainer()->gId() );
    KEY( ofs, "name", gName() );
    KEY( ofs, "sex", m_sex );
    KEY( ofs, "zone", gZone() );

    // If the Brain is attached to an account, serialize as a player character, otherwise serialize as a NPC
    if ( gBrain()->gAccount() )
        Utils::FileClose( ofs, Utils::DirPath( CFG_DAT_DIR_ACCOUNT, gBrain()->gAccount()->gId() ), CSTR( file ) );
    else
        Utils::FileClose( ofs, Utils::DirPath( CFG_DAT_DIR_WORLD, gZone() ), CSTR( file ) );

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

    // If the Brain is attached to an account, load as a player character, otherwise load as a NPC
    if ( gBrain() && gBrain()->gAccount() )
        Utils::FileOpen( ifs, Utils::DirPath( Utils::DirPath( CFG_DAT_DIR_ACCOUNT, gBrain()->gAccount()->gId() ), m_file ) );
    else
        Utils::FileOpen( ifs, m_file );

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
                sLocation( value );
            }
            else if ( key == "name" )
            {
                found = true;
                sName( value );
            }
            else if ( key == "zone" )
            {
                found = true;
                sZone( value );
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
        LOGFMT( flags, "Character::sCreation()-> called with invalid pos: %lu", pos );
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

    output << CRLF << gId() << "> " CRLF;

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
        LOGFMT( flags, "Character::sCreation()-> called with invalid pos: %lu", pos );
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
        LOGFMT( flags, "Character::sSex()-> called with invalid sex: %lu", sex );
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
