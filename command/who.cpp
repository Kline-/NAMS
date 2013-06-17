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
#include "list.h"

class Who : public Plugin {
    public:
        virtual const void Run( Character* character = NULL, const string& cmd = "", const string& arg = "" ) const;
        virtual const void Run( SocketClient* client = NULL, const string& cmd = "", const string& arg = "" ) const;

        Who( const string& name, const uint_t& type );
        ~Who();
};

const void Who::Run( Character* character, const string& cmd, const string& arg ) const
{
    CITER( vector, SocketClient*, si );
    SocketClient* target = NULL;

    if ( character )
    {
        character->Send( CFG_STR_VERSION CRLF );
        for ( si = socket_client_list.begin(); si != socket_client_list.end(); si++ )
        {
            target = *si;

            if ( target->gAccount() == NULL )
                continue;
            if ( target->gAccount()->gCharacter() == NULL )
                continue;
            character->Send( "    " + target->gAccount()->gCharacter()->gName() + CRLF );
        }
    }

    return;
}

const void Who::Run( SocketClient* client, const string& cmd, const string& arg ) const
{
    return;
}

Who::Who( const string& name = "who", const uint_t& type = PLG_TYPE_COMMAND ) : Plugin( name, type )
{
    Plugin::sBool( PLG_TYPE_COMMAND_BOOL_PREEMPT, true );
    Plugin::sUint( PLG_TYPE_COMMAND_UINT_SECURITY, ACT_SECURITY_NONE );

    return;
}

Who::~Who()
{
}

extern "C" {
    Plugin* New() { return new Who(); }
    void Delete( Plugin* p ) { delete p; }
}
