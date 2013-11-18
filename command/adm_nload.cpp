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

#include "character.h"

class AdmNload : public Plugin {
    public:
        virtual const void Run( Character* character = NULL, const string& cmd = "", const string& arg = "" ) const;
        virtual const void Run( SocketClient* client = NULL, const string& cmd = "", const string& arg = "" ) const;

        AdmNload( const string& name, const uint_t& type );
        ~AdmNload();
};

const void AdmNload::Run( Character* character, const string& cmd, const string& arg ) const
{
    Character* chr = NULL;

    if ( character )
    {
        if ( arg.empty() )
        {
            character->Send( "Load -which- NPC id?" CRLF );
            return;
        }

        chr = new Character();
        if ( !chr->Clone( arg, HANDLER_FIND_ID ) )
        {
            character->Send( "Unable to find that NPC." CRLF );
            delete chr;

            return;
        }

        character->Send( "You create " );
        character->Send( chr->gDescription( THING_DESCRIPTION_SHORT ) + "." CRLF );
        character->gContainer()->AddThing( chr );
    }

    return;
}

const void AdmNload::Run( SocketClient* client, const string& cmd, const string& arg ) const
{
    return;
}

AdmNload::AdmNload( const string& name = "::nload", const uint_t& type = PLG_TYPE_COMMAND ) : Plugin( name, type )
{
    Plugin::sBool( PLG_TYPE_COMMAND_BOOL_PREEMPT, true );
    Plugin::sUint( PLG_TYPE_COMMAND_UINT_SECURITY, ACT_SECURITY_ADMIN );

    return;
}

AdmNload::~AdmNload()
{
}

extern "C" {
    Plugin* New() { return new AdmNload(); }
    void Delete( Plugin* p ) { delete p; }
}
