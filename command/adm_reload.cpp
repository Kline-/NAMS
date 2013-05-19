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
#include "command.h"
#include "event.h"

class AdmReload : public Plugin {
    public:
        virtual const void Run( Character* character = NULL, const string& cmd = "", const string& arg = "" ) const;
        virtual const void Run( SocketClient* client = NULL, const string& cmd = "", const string& arg = "" ) const;

        AdmReload( const string& name, const uint_t& type );
        ~AdmReload();
};

const void AdmReload::Run( Character* character, const string& cmd, const string& arg ) const
{
    UFLAGS_DE( flags );
    Command* command = NULL;
    Event* event = NULL;
    string file;
    uint_t security = ACT_SECURITY_NONE;

    if ( character )
    {
        if ( arg.empty() )
        {
            character->Send( "Reload -which- command?" CRLF );
            return;
        }

        if ( ( command = Handler::FindCommand( arg ) ) != NULL )
        {
            if ( character->gAccount() )
                security = character->gAccount()->gSecurity();

            if ( command->Authorized( security ) )
            {
                event = new Event();
                if ( !event->New( arg, EVENT_TYPE_RELOAD, 100 ) )
                {
                    LOGSTR( flags, "AdmReload::Run()->Event::New()-> returned false" );
                    delete event;
                }
                else
                    character->Send( "Command successfully reloaded." CRLF );
            }
        }
        else
            character->Send( "That command doesn't exist." CRLF );
    }

    return;
}

const void AdmReload::Run( SocketClient* client, const string& cmd, const string& arg ) const
{
    return;
}

AdmReload::AdmReload( const string& name = "::reload", const uint_t& type = PLG_TYPE_COMMAND ) : Plugin( name, type )
{
    Plugin::sBool( PLG_TYPE_COMMAND_BOOL_PREEMPT, true );
    Plugin::sUint( PLG_TYPE_COMMAND_UINT_SECURITY, ACT_SECURITY_ADMIN );

    return;
}

AdmReload::~AdmReload()
{
}

extern "C" {
    Plugin* New() { return new AdmReload(); }
    void Delete( Plugin* p ) { delete p; }
}
