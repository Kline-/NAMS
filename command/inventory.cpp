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

class Inventory : public Plugin {
    public:
        virtual const void Run( Character* character = NULL, const string& cmd = "", const string& arg = "" ) const;
        virtual const void Run( SocketClient* client = NULL, const string& cmd = "", const string& arg = "" ) const;

        Inventory( const string& name, const uint_t& type );
        ~Inventory();
};

const void Inventory::Run( Character* character, const string& cmd, const string& arg ) const
{
    ITER( vector, Thing*, vi );
    vector<Thing*> contents;
    Thing* item = NULL;

    if ( character )
    {
        contents = character->gContents();
        character->Send( "You are carrying:" CRLF );

        if ( contents.empty() )
        {
            character->Send( "    Nothing." CRLF );
            return;
        }

        for ( vi = contents.begin(); vi != contents.end(); vi++ )
        {
            item = *vi;
            character->Send( "    " + item->gDescription( THING_DESCRIPTION_SHORT ) + CRLF );
        }
    }

    return;
}

const void Inventory::Run( SocketClient* client, const string& cmd, const string& arg ) const
{
    return;
}

Inventory::Inventory( const string& name = "inventory", const uint_t& type = PLG_TYPE_COMMAND ) : Plugin( name, type )
{
    Plugin::sBool( PLG_TYPE_COMMAND_BOOL_PREEMPT, true );
    Plugin::sUint( PLG_TYPE_COMMAND_UINT_SECURITY, ACT_SECURITY_NONE );

    return;
}

Inventory::~Inventory()
{
}

extern "C" {
    Plugin* New() { return new Inventory(); }
    void Delete( Plugin* p ) { delete p; }
}
