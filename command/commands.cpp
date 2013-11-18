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

#include "pincludes.h"

#include "account.h"
#include "command.h"
#include "list.h"

class Commands : public Plugin {
    public:
        virtual const void Run( Character* character = NULL, const string& cmd = "", const string& arg = "" ) const;
        virtual const void Run( SocketClient* client = NULL, const string& cmd = "", const string& arg = "" ) const;

        Commands( const string& name, const uint_t& type );
        ~Commands();
};

const void Commands::Run( Character* character, const string& cmd, const string& arg ) const
{
    ITER( vector, Command*, vi );
    Command* command = NULL;
    vector<string> output;
    string name;
    CITER( vector, string, oi );
    uint_t count = 0;
    uint_t security = ACT_SECURITY_NONE;

    if ( character )
    {
        if ( character->gBrain()->gAccount() )
            security = character->gBrain()->gAccount()->gSecurity();

        character->Send( "Available commands:" CRLF "    " );

        for ( vi = command_list.begin(); vi != command_list.end(); vi++ )
        {
            command = *vi;

            if ( command->Authorized( security ) )
                output.push_back( command->gName() );
        }

        sort( output.begin(), output.end() );

        for( oi = output.begin(); oi != output.end(); oi++ )
        {
            name = *oi;

            if ( ++count % 6 == 0 )
                character->Send( CRLF "    " + Utils::FormatString( 0, "%-12s ", CSTR( name ) ) );
            else
                character->Send( Utils::FormatString( 0, "%-12s ", CSTR( name ) ) );
        }

        character->Send( CRLF );
    }

    return;
}

const void Commands::Run( SocketClient* client, const string& cmd, const string& arg ) const
{
    return;
}

Commands::Commands( const string& name = "commands", const uint_t& type = PLG_TYPE_COMMAND ) : Plugin( name, type )
{
    Plugin::sBool( PLG_TYPE_COMMAND_BOOL_PREEMPT, true );
    Plugin::sUint( PLG_TYPE_COMMAND_UINT_SECURITY, ACT_SECURITY_AUTH_USER );

    return;
}

Commands::~Commands()
{
}

extern "C" {
    Plugin* New() { return new Commands(); }
    void Delete( Plugin* p ) { delete p; }
}
