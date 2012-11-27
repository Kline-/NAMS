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

#include "includes.h"
#include "class.h"
#include "plugin.h"

class Help : public Plugin {
    public:
        virtual const void Run( SocketClient* client = NULL, const string& cmd = "", const string& arg = "" ) const;

        Help( const string& name, const uint_t& type );
        ~Help();
};

const void Help::Run( SocketClient* client, const string& cmd, const string& arg ) const
{
    if ( client )
        client->Send( "Help!" CRLF );
    else
        cout << "Help!" << endl;

    return;
}

Help::Help( const string& name = "help", const uint_t& type = PLG_TYPE_COMMAND ) : Plugin( name, type )
{
    return;
}

Help::~Help()
{
}

extern "C" {
    Plugin* New() { return new Help(); }
    void Delete( Plugin* p ) { delete p; }
}
