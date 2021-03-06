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
/**
 * @file main.cpp
 * @brief Initial server entry point.
 *
 * This file contains only the initial program entry point. This may change in
 * the future if the sever design changes to support running multiple isolated
 * instances under a single process.
 *
 * @mainpage
 * @section sec_about About
 *  NAMS is a C++ MUD server with the primary goals of a safe, modular design and a\n
 *  server core that is substantially decouppled from game logic. NAMS has been designed\n
 *  as a generic, robust framework on which to build a rich and immersive game world.\n
 *
 * @section sec_support Support
 * Support for this code is provided both via GitHub and the project website.\n\n
 * Documentation   - http://kline-.github.com/NAMS/\n
 * GitHub          - https://github.com/Kline-/NAMS\n
 * Project Website - http://www.ackmud.net\n
 */
#include "h/includes.h"
#include "h/main.h"

/* Core */
/**
 * @brief The default function required to execute the server.
 * @param[in] argc The numbers of arguments the server was started with, including 0 as itself.
 * @param[in] argv[] An array of char pointers containing the arguments the server was started with.
 * @retval EXIT_FAILURE Returned if any critical fault is encountered during operation.
 * @retval EXIT_SUCCESS Returned if NAMS is properly shutdown without fault.
 */
int main( const int argc, const char* argv[] )
{
    BSET( flags, UTILS_RAW );
    sint_t desc = 0;
    uint_t port = 0;

    // Initialize global variables, runtime configuration, and statistics
    // Ensure globals are first as other items depend on them
    g_global = new Server::Global();
    g_config = new Server::Config();
    g_stats = new Server::Stats();

    if ( argc > 1 )
    {
        if ( !Utils::iNumber( argv[1] ) )
        {
            LOGFMT( flags, "Usage: %s [port #]", argv[0] );
            Server::Shutdown( EXIT_FAILURE );
        }

        port = atoi( argv[1] );

        if ( port <= CFG_SOC_MIN_PORTNUM || port >= CFG_SOC_MAX_PORTNUM )
        {
            LOGFMT( flags, "Port number must be between %d and %d.", CFG_SOC_MIN_PORTNUM, CFG_SOC_MAX_PORTNUM );
            Server::Shutdown( EXIT_FAILURE );
        }
        else
            g_global->m_port = port;

        // Must be rebooting
        if ( argc > 2 )
            desc = atoi( argv[2] );
    }
    else
        g_global->m_port = CFG_SOC_PORTNUM;


    Server::Startup( desc );
    while( !g_global->m_shutdown )
        Server::Update();

    Server::Shutdown( EXIT_SUCCESS );
}

/* Query */

/* Manipulate */

/* Internal */

