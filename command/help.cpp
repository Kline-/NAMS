#include "includes.h"
#include "class.h"
#include "plugin.h"

class Help : public Plugin {
    public:
        virtual const void Run( SocketClient* client = NULL ) const;

        Help( const string& name, const uint_t& type );
        ~Help();
};

const void Help::Run( SocketClient* client ) const
{
    if ( client )
        client->Send( "Help!" CRLF );
    else
        cout << "Help!" << endl;

    return;
}

Help::Help( const string& name = "Help", const uint_t& type = PLG_TYPE_COMMAND ) : Plugin( name, type )
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

