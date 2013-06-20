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
    vector<Thing*> objects;
    Thing* object = NULL;
    CITER( vector, Thing*, vi );
    bool found = false;

    if ( character )
    {
        if ( arg.empty() )
        {
            character->Send( "Get what?" CRLF );
            return;
        }

        objects = character->gContainer()->gContents();
        for ( vi = objects.begin(); vi != objects.end(); vi++ )
        {
            object = *vi;

            // Don't want characters to pick up characters...yet
            if ( object->gType() != THING_TYPE_OBJECT )
                continue;

            if ( Utils::iName( arg, object->gName() ) )
            {
                found = true;

                character->Send( "You get " + object->gDescription( THING_DESCRIPTION_SHORT ) + "." + CRLF );
                character->gContainer()->Send( character->gName() + " gets " + object->gDescription( THING_DESCRIPTION_SHORT ) + "." + CRLF, character );
                object->Move( character->gContainer(), character );
                break;
            }
        }

        if ( !found )
            character->Send( "There is no " + arg + " here." CRLF );
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
