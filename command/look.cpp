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
    vector<Exit*> exits;
    CITER( vector, Exit*, ei );
    Exit* exit = NULL;
    vector<Thing*> contents;
    CITER( vector, Thing*, ci );
    Thing* content = NULL;
    vector<Thing*> targets;
    CITER( vector, Thing*, ti );
    //Thing* target = NULL;
    string args, sobj, star;

    if ( character )
    {
        args = arg;
        sobj = Utils::Argument( args );
        star = Utils::Argument( args );
        location = dynamic_cast<Location*>( character->gContainer() );

        if ( arg.empty() && location != NULL ) // no args, display the room
        {
            exits = location->gExits();

            character->Send( "[Exits:");

            if ( exits.empty() )
                character->Send( " none]" CRLF );
            else
            {
                for ( ei = exits.begin(); ei != exits.end(); ei++ )
                {
                    exit = *ei;
                    character->Send( " " + exit->gName() );
                }

                character->Send( "]" CRLF );
            }

            character->Send( location->gName() + CRLF );
            character->Send( location->gDescription( THING_DESCRIPTION_LONG ) + CRLF );

            //Contents
            contents = location->gContents();

            if ( !contents.empty() )
            {
                // We are not alone
                if ( contents.size() > 1 )
                    character->Send( CRLF );

                for ( ci = contents.begin(); ci != contents.end(); ci++ )
                {
                    content = *ci;

                    if ( content == character )
                        continue;
                    else
                    {
                        switch ( content->gType() )
                        {
                            case THING_TYPE_CHARACTER:
                                character->Send( content->gName() + " is standing here." + CRLF );
                            break;

                            case THING_TYPE_LOCATION:
                            break;

                            case THING_TYPE_OBJECT:
                                character->Send( content->gDescription( THING_DESCRIPTION_LONG ) + CRLF );
                            break;

                            case THING_TYPE_THING:
                            break;
                        }
                    }
                }
            }
        }
        else if ( sobj == "i" || sobj == "in" ) // check for 'in'
        {
            if ( star.empty() )
            {
                character->Send( "Look in what?" CRLF );
                return;
            }

            //objects
        }


        // check for characters in location
        // check for objects in location
        // check for exits in location
        // check inventory
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
    Plugin::sUint( PLG_TYPE_COMMAND_UINT_SECURITY, ACT_SECURITY_AUTH_USER );

    return;
}

Look::~Look()
{
}

extern "C" {
    Plugin* New() { return new Look(); }
    void Delete( Plugin* p ) { delete p; }
}
