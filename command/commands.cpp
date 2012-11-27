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

#include "includes.h"
#include "class.h"
#include "plugin.h"

#include "command.h"

class Commands : public Plugin {
    public:
        virtual const void Run( SocketClient* client = NULL, const string& cmd = "", const string& arg = "" ) const;

        Commands( const string& name, const uint_t& type );
        ~Commands();
};

const void Commands::Run( SocketClient* client, const string& cmd, const string& arg ) const
{
    MITER( multimap, const char,Command*, mi );
    list<string> output;
    uint_t count = 0;

    if ( client )
    {
        client->Send( "Available commands:" CRLF "    " );

        for ( mi = command_list.begin(); mi != command_list.end(); mi++ )
        {
            if ( mi->second->Authorized( client->gSecurity() ) )
                output.push_back( mi->second->gName() );
        }

        output.sort();

        while ( !output.empty() )
        {
            if ( ++count % 6 == 0 )
            {
                client->Send( CRLF "    " );
                client->Send( Utils::FormatString( 0, "%-12s", CSTR( (*output.begin()) ) ) );
            }
            else
                client->Send( Utils::FormatString( 0, "%-12s", CSTR( (*output.begin()) ) ) );

            client->Send( " " );
            output.pop_front();
        }

        client->Send( CRLF );
    }

    return;
}

Commands::Commands( const string& name = "commands", const uint_t& type = PLG_TYPE_COMMAND ) : Plugin( name, type )
{
    sBool( PLG_TYPE_COMMAND_BOOL_PREEMPT, true );
    sUint( PLG_TYPE_COMMAND_UINT_SECURITY, SOC_SECURITY_AUTH_USER );

    return;
}

Commands::~Commands()
{
}

extern "C" {
    Plugin* New() { return new Commands(); }
    void Delete( Plugin* p ) { delete p; }
}
