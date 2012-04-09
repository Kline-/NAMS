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
#include "h/includes.h"
#include "h/class.h"

#include "h/server.h"

int main( const int argc, const char* argv[] )
{
    BSET( flags, UTILS_RAW );
    Server _server;

    if ( argc > 1 )
    {
        if ( !Utils::iNumber( argv[1] ) )
        {
            LOGFMT( flags, "Usage: %s [port #]", argv[0] );
            _server.Shutdown( EXIT_FAILURE );
        }
        else if ( !_server.sPort( atoi( argv[1] ) ) )
        {
            LOGFMT( flags, "Port number must be between %d and %d.", CFG_SOC_MIN_PORTNUM, CFG_SOC_MAX_PORTNUM );
            _server.Shutdown( EXIT_FAILURE );
        }
    }
    else
        _server.sPort( CFG_SOC_PORTNUM );
LOGSTR( flags, "Booting" );
    _server.Startup();
    while( _server.Running() )
        _server.Update();

    _server.Shutdown( EXIT_SUCCESS );
}
