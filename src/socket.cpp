#include "h/globals.h"
#include "h/socket.h"

// Core

// Query
bool Socket::Listen() const
{
    bitset<CFG_MEM_MAX_BITSET> flags;

    flags.set( UTILS_DEBUG );
    flags.set( UTILS_TYPE_ERROR );

    if ( !isValid() )
    {
        Utils::Logger( flags, "listen() attempted with invalid socket" );
        return false;
    }

    if ( ::listen( m_descriptor, CFG_SOC_MAX_PENDING ) < 0 )
    {
        Utils::Logger( flags, Utils::FormatString( flags, "listen() returned errno %d: %s", errno, strerror( errno ) ) );
        return false;
    }

    return true;
}

// Manipulate
bool Socket::Bind( const uint_t port, const string addr )
{
    bitset<CFG_MEM_MAX_BITSET> flags;
    static struct sockaddr_in sa_zero;
    struct sockaddr_in sa = sa_zero;

    flags.set( UTILS_DEBUG );
    flags.set( UTILS_TYPE_ERROR );

    if ( !isValid() )
    {
        Utils::Logger( flags, "bind() attempted with invalid socket" );
        return false;
    }

    sa.sin_family = AF_INET;
    sa.sin_port = htons( port );

    if ( !addr.empty() )
        sa.sin_addr.s_addr = inet_addr( CSTR( addr ) );

    if ( ::bind( m_descriptor, reinterpret_cast<sockaddr*>( &sa ), sizeof( sa ) ) < 0 )
    {
        Utils::Logger( flags, Utils::FormatString( flags, "bind() returned errno %d: %s", errno, strerror( errno ) ) );
        return false;
    }

    return true;
}

bool Socket::sDescriptor( const sint_t descriptor )
{
    bitset<CFG_MEM_MAX_BITSET> flags;

    flags.set( UTILS_DEBUG );
    flags.set( UTILS_TYPE_ERROR );

    if ( descriptor < 0 || descriptor >= sintmax_t )
    {
        Utils::Logger( flags, "sDescriptor() called with invalid input" );
        return false;
    }

    m_descriptor = descriptor;

    return true;
}

bool Socket::sHost( const string host )
{
    bitset<CFG_MEM_MAX_BITSET> flags;

    flags.set( UTILS_DEBUG );
    flags.set( UTILS_TYPE_ERROR );

    if ( host.empty() )
    {
        Utils::Logger( flags, "sHost() called with invalid input" );
        return false;
    }

    m_host = host;

    return true;
}

bool Socket::sPort( const uint_t port )
{
    bitset<CFG_MEM_MAX_BITSET> flags;

    flags.set( UTILS_DEBUG );
    flags.set( UTILS_TYPE_ERROR );

    if ( port <= uintmin_t || port >= uintmax_t )
    {
        Utils::Logger( flags, "sPort() called with invalid input" );
        return false;
    }

    m_port = port;

    return true;
}

Socket::Socket()
{
    m_descriptor = 0;
    m_host.clear();
    m_port = 0;

    return;
}

Socket::~Socket()
{
    if ( isValid() )
        ::close( m_descriptor );
}
