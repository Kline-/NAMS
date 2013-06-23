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

class Give : public Plugin {
    public:
        virtual const void Run( Character* character = NULL, const string& cmd = "", const string& arg = "" ) const;
        virtual const void Run( SocketClient* client = NULL, const string& cmd = "", const string& arg = "" ) const;

        Give( const string& name, const uint_t& type );
        ~Give();
};

const void Give::Run( Character* character, const string& cmd, const string& arg ) const
{
    vector<Thing*> objects;
    vector<Thing*> targets;
    Thing* object = NULL;
    Thing* target = NULL;
    CITER( vector, Thing*, oi );
    CITER( vector, Thing*, ti );
    string args, sobj, star;

    if ( character )
    {
        args = arg;
        sobj = Utils::Argument( args );
        star = Utils::Argument( args );

        if ( sobj.empty() || star.empty() )
        {
            character->Send( "Give what to whom?" CRLF );
            return;
        }

        object = Handler::FindThing( sobj, THING_TYPE_OBJECT, HANDLER_SCOPE_INVENTORY, character );

        if ( !object )
        {
            character->Send( "You don't have that item." CRLF );
            return;
        }

        target = Handler::FindThing( star, THING_TYPE_CHARACTER, HANDLER_SCOPE_LOCATION, character );

        if ( !target )
        {
            character->Send( "They aren't here." CRLF );
            return;
        }

        character->Send( "You give " + object->gDescription( THING_DESCRIPTION_SHORT ) + " to " + target->gName() + "." CRLF );
        target->Send( character->gName() + " gives you " + object->gDescription( THING_DESCRIPTION_SHORT ) + "." CRLF );
        character->gContainer()->Send(  character->gName() + " gives " + object->gDescription( THING_DESCRIPTION_SHORT ) + " to " + target->gName() + "." CRLF, character, target );
        object->Move( character, target );
    }

    return;
}

const void Give::Run( SocketClient* client, const string& cmd, const string& arg ) const
{
    return;
}

Give::Give( const string& name = "give", const uint_t& type = PLG_TYPE_COMMAND ) : Plugin( name, type )
{
    Plugin::sBool( PLG_TYPE_COMMAND_BOOL_PREEMPT, false );
    Plugin::sUint( PLG_TYPE_COMMAND_UINT_SECURITY, ACT_SECURITY_AUTH_USER );

    return;
}

Give::~Give()
{
}

extern "C" {
    Plugin* New() { return new Give(); }
    void Delete( Plugin* p ) { delete p; }
}
