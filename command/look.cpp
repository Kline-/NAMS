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
#include "exit.h"
#include "location.h"

class Look : public Plugin {
    public:
        virtual const void Run( Character* character = NULL, const string& cmd = "", const string& arg = "" ) const;
        virtual const void Run( SocketClient* client = NULL, const string& cmd = "", const string& arg = "" ) const;

        Look( const string& name, const uint_t& type );
        ~Look();
};

const void Look::Run( Character* character, const string& cmd, const string& arg ) const
{
    Location* location = NULL;
    list<Exit*> loc_exits;
    CITER( list, Exit*, ei );
    Exit* exit = NULL;
    vector<Thing*> loc_contents;
    CITER( vector, Thing*, ti );
    Thing* thing = NULL;

    if ( character )
    {
        location = character->gLocation();

        if ( arg.empty() && location != NULL )
        {
            loc_exits = location->gExits();

            character->Send( "[Exits:");

            if ( loc_exits.empty() )
                character->Send( " none]" CRLF );
            else
            {
                for ( ei = loc_exits.begin(); ei != loc_exits.end(); ei++ )
                {
                    exit = *ei;
                    character->Send( " " + exit->gName() );
                }

                character->Send( "]" CRLF );
            }

            character->Send( location->gName() + CRLF );
            character->Send( location->gDescription( THING_DESCRIPTION_LONG ) );

            //Contents
            loc_contents = location->gContents();

            if ( !loc_contents.empty() )
            {
                // We are not alone
                if ( loc_contents.size() > 1 )
                    character->Send( CRLF );

                for ( ti = loc_contents.begin(); ti != loc_contents.end(); ti++ )
                {
                    thing = *ti;

                    if ( thing == character )
                        continue;
                    else
                        character->Send( thing->gName() + " is standing here." + CRLF );
                }
            }
        }
    }

    return;
}

const void Look::Run( SocketClient* client, const string& cmd, const string& arg ) const
{
    return;
}

Look::Look( const string& name = "look", const uint_t& type = PLG_TYPE_COMMAND ) : Plugin( name, type )
{
    Plugin::sBool( PLG_TYPE_COMMAND_BOOL_PREEMPT, true );
    Plugin::sUint( PLG_TYPE_COMMAND_UINT_SECURITY, ACT_SECURITY_NONE );

    return;
}

Look::~Look()
{
}

extern "C" {
    Plugin* New() { return new Look(); }
    void Delete( Plugin* p ) { delete p; }
}
