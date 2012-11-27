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
#include "command.h"

class AdmReload : public Plugin {
    public:
        virtual const void Run( SocketClient* client = NULL, const string& cmd = "", const string& arg = "" ) const;

        AdmReload( const string& name, const uint_t& type );
        ~AdmReload();
};

const void AdmReload::Run( SocketClient* client, const string& cmd, const string& arg ) const
{
    pair<multimap<const char,Command*>::iterator,multimap<const char,Command*>::iterator> cmd_list;
    MITER( multimap, const char,Command*, mi );
    bool found = false;
    UFLAGS_DE( flags );

    if ( client )
    {
        if ( CFG_GAM_CMD_IGNORE_CASE )
            cmd_list = command_list.equal_range( Utils::Lower( arg )[0] );
        else
            cmd_list = command_list.equal_range( arg[0] );

        if ( cmd_list.first == cmd_list.second )
            client->Send( "That command doesn't exist." CRLF );
        else
        {
            for ( mi = cmd_list.first; mi != cmd_list.second; mi++ )
            {
                found = false;

                if ( CFG_GAM_CMD_IGNORE_CASE )
                {
                    if ( Utils::Lower( mi->second->gName() ).find( Utils::Lower( arg ) ) == 0 )
                        found = true;
                }
                else
                {
                    if ( mi->second->gName().find( arg ) == 0 )
                        found = true;
                }

                if ( found )
                    break;
            }

            mi->second->m_plg_delete( mi->second->m_plg );
            ::dlerror();

            if ( ::dlclose( mi->second->m_plg_handle ) )
                LOGFMT( flags, "Command::Delete()->dlclose() returned error: %s", ::dlerror() );

            if ( ( mi->second->m_plg_handle = ::dlopen( "obj/help.so", RTLD_LAZY | RTLD_GLOBAL ) ) == NULL )
            {
                LOGFMT( flags, "Command::New()->dlopen returned error: %s", ::dlerror() );
                return;
            }
            else
            {
                mi->second->m_plg_delete = (PluginDelete*) ::dlsym( mi->second->m_plg_handle, "Delete" );
                mi->second->m_plg_new = (PluginNew*) ::dlsym( mi->second->m_plg_handle, "New" );
                mi->second->m_plg = mi->second->m_plg_new();
            }
        }
    }

    return;
}

AdmReload::AdmReload( const string& name = "::reload", const uint_t& type = PLG_TYPE_COMMAND ) : Plugin( name, type )
{
    sBool( PLG_TYPE_COMMAND_BOOL_PREEMPT, true );
    sUint( PLG_TYPE_COMMAND_UINT_SECURITY, SOC_SECURITY_ADMIN );

    return;
}

AdmReload::~AdmReload()
{
}

extern "C" {
    Plugin* New() { return new AdmReload(); }
    void Delete( Plugin* p ) { delete p; }
}
