#include "h/globals.h"
#include "h/srvinfo.h"

const void ServerInfo::Shutdown( const sint_t status ) const
{
    exit( status );
}

bool ServerInfo::sPort( const uint_t port )
{
    if ( port <= CFG_SOC_MIN_PORTNUM || port >= CFG_SOC_MAX_PORTNUM )
        return false;

    m_port = port;

    return true;
}

string ServerInfo::gTimeBoot() const
{
    string output;

    output = ctime( &m_time_boot );
    output.resize( output.length() - 1 );

    return output;
}

const void ServerInfo::sTimeBoot()
{
    struct timeval now;

    gettimeofday( &now, NULL );
    m_time_boot = now.tv_sec;

    return;
}

string ServerInfo::gTimeCurrent() const
{
    string output;

    output = ctime( &m_time_current );
    output.resize( output.length() - 1 );

    return output;
}

bool ServerInfo::sSocket( Socket* socket )
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

const void ServerInfo::sTimeCurrent()
{
    struct timeval now;

    gettimeofday( &now, NULL );
    m_time_current = now.tv_sec;

    return;
}

ServerInfo::ServerInfo()
{
    m_port = 0;
    m_socket = 0;
    m_time_boot = 0;
    m_time_current = 0;

    return;
}

ServerInfo::~ServerInfo()
{
    delete m_socket;

    return;
}
