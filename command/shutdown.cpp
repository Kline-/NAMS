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

class Shutdown : public Plugin {
    public:
        virtual const void Run( SocketClient* client = NULL ) const;

        Shutdown( const string& name, const uint_t& type );
        ~Shutdown();
};

const void Shutdown::Run( SocketClient* client ) const
{
    if ( client )
        client->gServer()->Shutdown( EXIT_SUCCESS );

    return;
}

Shutdown::Shutdown( const string& name = "Shutdown", const uint_t& type = PLG_TYPE_COMMAND ) : Plugin( name, type )
{
    return;
}

Shutdown::~Shutdown()
{
}

extern "C" {
    Plugin* New() { return new Shutdown(); }
    void Delete( Plugin* p ) { delete p; }
}
