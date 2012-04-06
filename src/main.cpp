#include "h/globals.h"
#include "h/main.h"

Server server;

int main( const int argc, const char* argv[] )
{
    BSET( flags, UTILS_RAW );
    Socket* socket;

    server.sTimeBoot();
    server.sTimeCurrent();

    if ( argc > 1 )
    {
        if ( !Utils::isNumber( argv[1] ) )
        {
            LOGFMT( flags, "Usage: %s [port #]", argv[0] );
            server.Shutdown( EXIT_FAILURE );
        }
        else if ( !server.sPort( atoi( argv[1] ) ) )
        {
            LOGFMT( flags, "Port number must be between %d and %d.", CFG_SOC_MIN_PORTNUM, CFG_SOC_MAX_PORTNUM );
            server.Shutdown( EXIT_FAILURE );
        }
    }
    else
        server.sPort( CFG_SOC_PORTNUM );

    socket = new Socket();
    socket_list.push_back( socket );

    if ( !server.InitSocket( socket ) )
        server.Shutdown( EXIT_FAILURE );
    if ( !socket->Bind( server.gPort() ) )
        server.Shutdown( EXIT_FAILURE );
    if ( !socket->Listen() )
        server.Shutdown( EXIT_FAILURE );

    server.sRunning();
    while( server.isRunning() )
        server.Update();

    server.Shutdown( EXIT_SUCCESS );
}
