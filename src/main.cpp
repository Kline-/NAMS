#include "h/globals.h"
#include "h/main.h"

Server _server;

int main( const int argc, const char* argv[] )
{
    BSET( flags, UTILS_RAW );

    if ( argc > 1 )
    {
        if ( !Utils::isNumber( argv[1] ) )
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

    _server.Startup();
    while( _server.iRunning() )
        _server.Update();

    _server.Shutdown( EXIT_SUCCESS );
}
