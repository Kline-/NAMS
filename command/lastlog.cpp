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

class LastLog : public Plugin {
    public:
        virtual const void Run( Character* character = NULL, const string& cmd = "", const string& arg = "" ) const;
        virtual const void Run( SocketClient* client = NULL, const string& cmd = "", const string& arg = "" ) const;

        LastLog( const string& name, const uint_t& type );
        ~LastLog();
};

const void LastLog::Run( Character* character, const string& cmd, const string& arg ) const
{
    list<pair<string,string>> logins;
    list<pair<string,string>>::const_iterator li;

    if ( character )
    {
        if ( character->gAccount() )
        {
            logins = character->gAccount()->gLogins( ACT_LOGIN_FAILURE );
            character->Send( Utils::FormatString( 0, "Last %lu failed logins:" CRLF, CFG_ACT_LOGIN_MAX ) );
            for ( li = logins.begin(); li != logins.end(); li++ )
                character->Send( Utils::FormatString( 0, "    [%s] %s" CRLF, CSTR( li->first ), CSTR( li->second ) ) );

            logins = character->gAccount()->gLogins( ACT_LOGIN_SUCCESS );
            character->Send( Utils::FormatString( 0, "Last %lu successful logins:" CRLF, CFG_ACT_LOGIN_MAX ) );
            for ( li = logins.begin(); li != logins.end(); li++ )
                character->Send( Utils::FormatString( 0, "    [%s] %s" CRLF, CSTR( li->first ), CSTR( li->second ) ) );
        }
    }

    return;
}

const void LastLog::Run( SocketClient* client, const string& cmd, const string& arg ) const
{
    return;
}

LastLog::LastLog( const string& name = ".lastlog", const uint_t& type = PLG_TYPE_COMMAND ) : Plugin( name, type )
{
    Plugin::sBool( PLG_TYPE_COMMAND_BOOL_PREEMPT, true );
    Plugin::sUint( PLG_TYPE_COMMAND_UINT_SECURITY, ACT_SECURITY_NONE );

    return;
}

LastLog::~LastLog()
{
}

extern "C" {
    Plugin* New() { return new LastLog(); }
    void Delete( Plugin* p ) { delete p; }
}
