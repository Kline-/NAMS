#include "h/globals.h"
#include "h/server.h"

// Core
bool Server::PollSockets() const
{
    bitset<CFG_MEM_MAX_BITSET> flags;
    static struct timeval null_time;
    fd_set in_set;
    fd_set out_set;
    fd_set exc_set;
    ITER( Socket*, si );
    Socket* socket;
    sint_t max_desc = 0;

    flags.set( UTILS_DEBUG );
    flags.set( UTILS_TYPE_ERROR );

    FD_ZERO( &in_set );
    FD_ZERO( &out_set );
    FD_ZERO( &exc_set );

    FD_SET( server.gSocket()->gDescriptor(), &in_set );

    for ( si = socket_list.begin(); si != socket_list.end(); si++ )
    {
        socket = *si;

        max_desc = max( server.gSocket()->gDescriptor(), socket->gDescriptor() );
        FD_SET( socket->gDescriptor(), &in_set );
        FD_SET( socket->gDescriptor(), &out_set );
        FD_SET( socket->gDescriptor(), &exc_set );
    }

    if ( ::select( max_desc + 1, &in_set, &out_set, &exc_set, &null_time ) < 0 )
    {
        Utils::Logger( flags, Utils::FormatString( flags, "select() returned errno %d: %s", errno, strerror( errno ) ) );
        return false;
    }

    return true;
}

const void Server::Shutdown( const sint_t status )
{
    for_each( socket_list.begin(), socket_list.end(), Utils::DeleteObject() );

    m_shutdown = true;
}

const void Server::Update() const
{
    bitset<CFG_MEM_MAX_BITSET> flags;

    flags.set( UTILS_DEBUG );
    flags.set( UTILS_TYPE_ERROR );

    if ( !PollSockets() )
    {
        Utils::Logger( flags, Utils::FormatString( flags, "Error while calling PollSockets()" ) );
        server.Shutdown( EXIT_FAILURE );
        return;
    }

    return;
}

// Query
string Server::gTimeBoot() const
{
    string output;

    output = ctime( &m_time_boot );
    output.resize( output.length() - 1 );

    return output;
}

string Server::gTimeCurrent() const
{
    string output;

    output = ctime( &m_time_current );
    output.resize( output.length() - 1 );

    return output;
}

// Manipulate
bool Server::sPort( const uint_t port )
{
    if ( port <= CFG_SOC_MIN_PORTNUM || port >= CFG_SOC_MAX_PORTNUM )
        return false;

    m_port = port;

    return true;
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

    m_socket = socket;

    return true;
}

const void Server::sTimeBoot()
{
    struct timeval now;

    gettimeofday( &now, NULL );
    m_time_boot = now.tv_sec;

    return;
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
    return;
}
