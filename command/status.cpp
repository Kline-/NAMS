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

class Status : public Plugin {
    public:
        virtual const void Run( Character* character = NULL, const string& cmd = "", const string& arg = "" ) const;
        virtual const void Run( SocketClient* client = NULL, const string& cmd = "", const string& arg = "" ) const;

        Status( const string& name, const uint_t& type );
        ~Status();
};

const void Status::Run( Character* character, const string& cmd, const string& arg ) const
{
    if ( character )
    {
        character->Send( Server::gStatus() );
    }

    return;
}

const void Status::Run( SocketClient* client, const string& cmd, const string& arg ) const
{
    if ( client )
    {
        client->Send( Server::gStatus() );
    }

    return;
}

Status::Status( const string& name = "status", const uint_t& type = PLG_TYPE_COMMAND ) : Plugin( name, type )
{
    Plugin::sBool( PLG_TYPE_COMMAND_BOOL_PREEMPT, true );
    Plugin::sUint( PLG_TYPE_COMMAND_UINT_SECURITY, ACT_SECURITY_NONE );

    return;
}

Status::~Status()
{
}

extern "C" {
    Plugin* New() { return new Status(); }
    void Delete( Plugin* p ) { delete p; }
}
