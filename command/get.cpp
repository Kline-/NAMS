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

class Get : public Plugin {
    public:
        virtual const void Run( Character* character = NULL, const string& cmd = "", const string& arg = "" ) const;
        virtual const void Run( SocketClient* client = NULL, const string& cmd = "", const string& arg = "" ) const;

        Get( const string& name, const uint_t& type );
        ~Get();
};

const void Get::Run( Character* character, const string& cmd, const string& arg ) const
{
    Thing* object = NULL;
    Thing* target = NULL;
    string args, sobj, star;

    if ( character )
    {
        args = arg;
        sobj = Utils::Argument( args );
        star = Utils::Argument( args );

        if ( sobj.empty() )
        {
            character->Send( "Get what?" CRLF );
            return;
        }

        // No 'from' target specified, so search the room
        if ( star.empty() )
        {
            object = Handler::FindThing( sobj, THING_TYPE_OBJECT, HANDLER_SCOPE_LOCATION, character );

            if ( object )
            {
                character->Send( "You get " + object->gDescription( THING_DESCRIPTION_SHORT ) + "." + CRLF );
                character->gContainer()->Send( character->gName() + " gets " + object->gDescription( THING_DESCRIPTION_SHORT ) + "." + CRLF, character );
                object->Move( character->gContainer(), character );
            }
            else
            {
                character->Send( "There is no " + arg + " here." CRLF );
                return;
            }
        }
        else
        {
            target = Handler::FindThing( star, THING_TYPE_OBJECT, HANDLER_SCOPE_LOC_INV, character );

            if ( !target )
            {
                character->Send( "There is no " + star + " here." CRLF );
                return;
            }

            object = Handler::FindThing( sobj, THING_TYPE_OBJECT, HANDLER_SCOPE_INVENTORY, target );

            if ( object )
            {
                character->Send( "You get " + object->gDescription( THING_DESCRIPTION_SHORT ) + " from " + target->gDescription( THING_DESCRIPTION_SHORT ) + "." CRLF );
                character->gContainer()->Send(  character->gName() + " gets " + object->gDescription( THING_DESCRIPTION_SHORT ) + " from " + target->gDescription( THING_DESCRIPTION_SHORT )+ "." CRLF, character );
                object->Move( target, character );
            }
            else
            {
                character->Send( "There is no " + sobj + " inside the " + target->gDescription( THING_DESCRIPTION_SHORT ) + "." CRLF );
                return;
            }
        }
    }

    return;
}

const void Get::Run( SocketClient* client, const string& cmd, const string& arg ) const
{
    return;
}

Get::Get( const string& name = "get", const uint_t& type = PLG_TYPE_COMMAND ) : Plugin( name, type )
{
    Plugin::sBool( PLG_TYPE_COMMAND_BOOL_PREEMPT, false );
    Plugin::sUint( PLG_TYPE_COMMAND_UINT_SECURITY, ACT_SECURITY_AUTH_USER );

    return;
}

Get::~Get()
{
}

extern "C" {
    Plugin* New() { return new Get(); }
    void Delete( Plugin* p ) { delete p; }
}
