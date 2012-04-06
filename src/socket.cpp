#include "h/globals.h"
#include "h/socket.h"

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

Socket::Socket()
{
    bitset<CFG_MEM_MAX_BITSET> flags;
    sint_t descriptor = 0;
    uint_t enable = 1;

    flags.set( UTILS_DEBUG );
    flags.set( UTILS_TYPE_ERROR );

    if ( ( descriptor = ::socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
        Utils::Logger( flags, Utils::FormatString( flags, "socket() returned errno %d: %s", errno, strerror( errno ) ) );
        return;
    }

    if ( ::setsockopt( descriptor, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>( &enable ), sizeof( enable ) ) < 0 )
    {
        Utils::Logger( flags, Utils::FormatString( flags, "setsockopt() returned errno %d: %s", errno, strerror( errno ) ) );
        return;
    }

    m_descriptor = descriptor;
    socket_list.push_back( this );

    return;
}

Socket::~Socket()
{
    if ( isValid() )
        ::close( m_descriptor );
}
