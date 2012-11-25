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

