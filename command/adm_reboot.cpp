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

#include "list.h"
#include "server.h"
#include "socketserver.h"

class AdmReboot : public Plugin {
    public:
        virtual const void Run( Character* character = NULL, const string& cmd = "", const string& arg = "" ) const;
        virtual const void Run( SocketClient* client = NULL, const string& cmd = "", const string& arg = "" ) const;

        AdmReboot( const string& name, const uint_t& type );
        ~AdmReboot();
};

const void AdmReboot::Run( Character* character, const string& cmd, const string& arg ) const
{
    SocketClient* socket_client = NULL;
    ITER( list, SocketClient*, si );
    string desc, port;
    ofstream ofs;

    Utils::FileOpen( ofs, CFG_DAT_FILE_REBOOT );
    for ( si = socket_client_list.begin(); si != socket_client_list.end(); si = g_global->m_next_socket_client )
    {
        socket_client = *si;
        g_global->m_next_socket_client = ++si;

        ofs << "desc = " << socket_client->gDescriptor() << endl;
        ofs << "port = " << socket_client->gPort() << endl;
        ofs << "host = " << socket_client->gHostname() << endl;
        ofs << "recv = " << socket_client->gBytesRecvd() << endl;
        ofs << "sent = " << socket_client->gBytesSent() << endl;
        ofs << "idle = " << socket_client->gIdle() << endl;
        ofs << "stat = " << socket_client->gState() << endl;
    }
    Utils::FileClose( ofs );

    port = Utils::String( g_global->m_listen->gPort() );
    desc = Utils::String( g_global->m_listen->gDescriptor() );

    execl( "src/nams", "nams", CSTR( port ), CSTR( desc ), (char*)NULL );
    LOGERRNO( 0, "" );

    return;
}

const void AdmReboot::Run( SocketClient* client, const string& cmd, const string& arg ) const
{
    return;
}

AdmReboot::AdmReboot( const string& name = "::reboot", const uint_t& type = PLG_TYPE_COMMAND ) : Plugin( name, type )
{
    Plugin::sBool( PLG_TYPE_COMMAND_BOOL_PREEMPT, true );
    Plugin::sUint( PLG_TYPE_COMMAND_UINT_SECURITY, ACT_SECURITY_ADMIN );

    return;
}

AdmReboot::~AdmReboot()
{
}

extern "C" {
    Plugin* New() { return new AdmReboot(); }
    void Delete( Plugin* p ) { delete p; }
}
