#include "h/globals.h"
#include "h/main.h"

ServerInfo server;

int main( const int argc, const char *argv[] )
{
    bitset<CFG_MEM_MAX_BITSET> flags;
    Socket* socket;

    flags.set( UTILS_RAW );
    server.sTimeBoot();
    server.sTimeCurrent();

    if ( argc > 1 )
    {
        if ( !Utils::isNumber( argv[1] ) )
        {
            Utils::Logger( flags, Utils::FormatString( flags, "Usage: %s [port #]", argv[0] ) );
            server.Shutdown( EXIT_FAILURE );
        }
        else if ( !server.sPort( atoi( argv[1] ) ) )
        {
            Utils::Logger( flags, Utils::FormatString( flags, "Port number must be between %d and %d.", CFG_SOC_MIN_PORTNUM, CFG_SOC_MAX_PORTNUM ) );
            server.Shutdown( EXIT_FAILURE );
        }
    }
    else
        server.sPort( CFG_SOC_PORTNUM );

    socket = new Socket();

    if ( !socket->Bind( server.gPort() ) )
        server.Shutdown( EXIT_FAILURE );
    if ( !socket->Listen() )
        server.Shutdown( EXIT_FAILURE );

    server.sSocket( socket );

    Utils::Logger( 0, Utils::FormatString( 0, "%s is ready on port %lu.", CFG_STR_VERSION, server.gPort() ) );
    Utils::Logger( 0, "Last compiled on " __DATE__ " at " __TIME__ "." );

    while( server.isRunning() )
        server.Update();

    Utils::Logger( 0, "Normal termination of server." );
    server.Shutdown( EXIT_SUCCESS );
}
