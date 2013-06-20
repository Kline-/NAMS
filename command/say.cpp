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

class Say : public Plugin {
    public:
        virtual const void Run( Character* character = NULL, const string& cmd = "", const string& arg = "" ) const;
        virtual const void Run( SocketClient* client = NULL, const string& cmd = "", const string& arg = "" ) const;

        Say( const string& name, const uint_t& type );
        ~Say();
};

const void Say::Run( Character* character, const string& cmd, const string& arg ) const
{
    if ( character )
    {
        if ( arg.empty() )
        {
            character->Send( "Say what?" CRLF );
            return;
        }

        if ( character->gContainer() != NULL )
        {
            character->Send( "You say '" + arg + "'." );
            character->gContainer()->Send( character->gName() + " says '" + arg + "'.", character );
        }
    }

    return;
}

const void Say::Run( SocketClient* client, const string& cmd, const string& arg ) const
{
    return;
}

Say::Say( const string& name = "say", const uint_t& type = PLG_TYPE_COMMAND ) : Plugin( name, type )
{
    Plugin::sBool( PLG_TYPE_COMMAND_BOOL_PREEMPT, true );
    Plugin::sUint( PLG_TYPE_COMMAND_UINT_SECURITY, ACT_SECURITY_AUTH_USER );

    return;
}

Say::~Say()
{
}

extern "C" {
    Plugin* New() { return new Say(); }
    void Delete( Plugin* p ) { delete p; }
}
