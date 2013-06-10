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

class AdmDisable : public Plugin {
    public:
        virtual const void Run( Character* character = NULL, const string& cmd = "", const string& arg = "" ) const;
        virtual const void Run( SocketClient* client = NULL, const string& cmd = "", const string& arg = "" ) const;

        AdmDisable( const string& name, const uint_t& type );
        ~AdmDisable();
};

const void AdmDisable::Run( Character* character, const string& cmd, const string& arg ) const
{
    vector<string> disabled = g_config->gDisabledCommands();
    CITER( vector, string, vi );

    if ( character )
    {
        sort( disabled.begin(), disabled.end() );

        if ( arg.empty() )
        {
            character->Send( "Disabled Commands" CRLF );

            if ( disabled.empty() )
                character->Send( "    None" CRLF );
            else
            {
                for ( vi = disabled.begin(); vi != disabled.end(); vi++ )
                {
                    character->Send( "    " );
                    character->Send( *vi + CRLF );
                }
            }

            return;
        }

        if ( arg == gName() )
        {
            character->Send( "You can't disable the disable command." CRLF );
            return;
        }

        if ( find( disabled.begin(), disabled.end(), arg ) == disabled.end() )
        {
            if ( g_config->ToggleDisable( arg ) )
                character->Send( "Command disabled." CRLF );
            else
                character->Send( "There was an error disabling that command or it doesn't exist." CRLF );
        }
        else
        {
            if ( g_config->ToggleDisable( arg ) )
                character->Send( "Command enabled." CRLF );
            else
                character->Send( "There was an error enabling that command or it doesn't exist." CRLF );
        }
    }

    return;
}

const void AdmDisable::Run( SocketClient* client, const string& cmd, const string& arg ) const
{
    return;
}

AdmDisable::AdmDisable( const string& name = "::disable", const uint_t& type = PLG_TYPE_COMMAND ) : Plugin( name, type )
{
    Plugin::sBool( PLG_TYPE_COMMAND_BOOL_PREEMPT, true );
    Plugin::sUint( PLG_TYPE_COMMAND_UINT_SECURITY, ACT_SECURITY_ADMIN );

    return;
}

AdmDisable::~AdmDisable()
{
}

extern "C" {
    Plugin* New() { return new AdmDisable(); }
    void Delete( Plugin* p ) { delete p; }
}
