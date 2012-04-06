#include "h/globals.h"
#include "h/server.h"

const void Server::Shutdown( const sint_t status )
{
    m_shutdown = true;
    for_each( socket_list.begin(), socket_list.end(), Utils::DeleteObject() );
    exit( status );
}

const void Server::Update() const
{
}

bool Server::sPort( const uint_t port )
{
    if ( port <= CFG_SOC_MIN_PORTNUM || port >= CFG_SOC_MAX_PORTNUM )
        return false;

    m_port = port;

    return true;
}

string Server::gTimeBoot() const
{
    string output;

    output = ctime( &m_time_boot );
    output.resize( output.length() - 1 );

    return output;
}

const void Server::sTimeBoot()
{
    struct timeval now;

    gettimeofday( &now, NULL );
    m_time_boot = now.tv_sec;

    return;
}

string Server::gTimeCurrent() const
{
    string output;

    output = ctime( &m_time_current );
    output.resize( output.length() - 1 );

    return output;
}

bool Server::sSocket( Socket* socket )
{
    bitset<CFG_MEM_MAX_BITSET> flags;

    flags.set( UTILS_DEBUG );
    flags.set( UTILS_TYPE_ERROR );

    if ( !socket )
    {
        Utils::Logger( flags, "sSocket() called with invalid input" );
        return false;
    }

    if ( !socket->isValid() )
    {
        Utils::Logger( flags, "sSocket() called with invalid socket" );
        return false;
    }

    return true;
}

const void Server::sTimeCurrent()
{
    struct timeval now;

    gettimeofday( &now, NULL );
    m_time_current = now.tv_sec;

    return;
}

Server::Server()
{
    m_port = 0;
    m_shutdown = false;
    m_socket = 0;
    m_time_boot = 0;
    m_time_current = 0;

    return;
}

Server::~Server()
{
    delete m_socket;

    return;
}
