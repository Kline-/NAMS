#include "h/globals.h"
#include "h/socket.h"

// Core
const void Socket::Send( const string msg )
{
    if ( m_output.empty() )
    {
        m_output += "\r\n";
        m_output += msg;
    }
    else
        m_output += msg;

    return;
}

bool Socket::Send()
{
    UFLAGS_DE( flags );

    if ( ::send( m_descriptor, CSTR( m_output ), m_output.length(), 0 ) < 0 )
    {
        LOGFMT( flags, "Socket::Send()->send()-> returned errno %d: %s", errno, strerror( errno ) );
        return false;
    }

    m_output.clear();

    return true;
}

// Query
bool Socket::Listen() const
{
    UFLAGS_DE( flags );

    if ( !isValid() )
    {
        LOGSTR( flags, "Socket::Listen()-> called with invalid socket" );
        return false;
    }

    if ( ::listen( m_descriptor, CFG_SOC_MAX_PENDING ) < 0 )
    {
        LOGFMT( flags, "Socket::Listen()->listen()-> returned errno %d: %s", errno, strerror( errno ) );
        return false;
    }

    return true;
}

// Manipulate
bool Socket::Bind( const uint_t port, const string addr )
{
    UFLAGS_DE( flags );
    static struct sockaddr_in sa_zero;
    struct sockaddr_in sa = sa_zero;

    if ( !isValid() )
    {
        LOGSTR( flags, "Socket::Bind()-> called with invalid socket" );
        return false;
    }

    sa.sin_family = AF_INET;
    sa.sin_port = htons( port );

    if ( !addr.empty() )
        sa.sin_addr.s_addr = inet_addr( CSTR( addr ) );

    if ( ::bind( m_descriptor, reinterpret_cast<sockaddr*>( &sa ), sizeof( sa ) ) < 0 )
    {
        LOGFMT( flags, "Socket::Bind()->bind()-> returned errno %d: %s", errno, strerror( errno ) );
        return false;
    }

    return true;
}

bool Socket::sDescriptor( const sint_t descriptor )
{
    UFLAGS_DE( flags );

    if ( descriptor < 0 || descriptor >= sintmax_t )
    {
        LOGFMT( flags, "Socket::sDescriptor()-> called with invalid descriptor: %ld", descriptor );
        return false;
    }

    m_descriptor = descriptor;

    return true;
}

bool Socket::sHost( const string host )
{
    UFLAGS_DE( flags );

    if ( host.empty() )
    {
        LOGSTR( flags, "Socket::sHost()-> called with empty host" );
        return false;
    }

    m_host = host;

    return true;
}

bool Socket::sPort( const uint_t port )
{
    UFLAGS_DE( flags );

    if ( port <= uintmin_t || port >= uintmax_t )
    {
        LOGFMT( flags, "Socket::sPort()-> called with invalid input: %lu", port );
        return false;
    }

    m_port = port;

    return true;
}

const void Socket::ResolveHostname()
{
    pthread_t res_thread;
    pthread_attr_t res_attr;

    pthread_attr_init( &res_attr );
    pthread_attr_setdetachstate( &res_attr, PTHREAD_CREATE_DETACHED );
    pthread_create( &res_thread, &res_attr, &Socket::tResolveHostname, this );

    return;
}

void* Socket::tResolveHostname( void* data )
{
    UFLAGS_DE( flags );
    Socket* socket = reinterpret_cast<Socket*>( data );
    static struct sockaddr_in sa_zero;
    struct sockaddr_in sa = sa_zero;
    sint_t error = 0;
    char host[CFG_STR_MAX_BUFLEN];

    sa.sin_family = AF_INET;

    if ( ( error = inet_pton( AF_INET, CSTR( socket->gHost() ), &sa.sin_addr ) ) != 1 )
    {
        LOGFMT( flags, "Socket::tResolveHostname()->inet_pton()-> returned errno %ld: %s", error, gai_strerror( error ) );
        pthread_exit( reinterpret_cast<void*>( EXIT_FAILURE ) );
    }

    if ( ( error = getnameinfo( reinterpret_cast<struct sockaddr*>( &sa ), sizeof( sa ), host, sizeof( host ), NULL, 0, 0 ) ) != 0 )
    {
        LOGFMT( flags, "Socket::tResolveHostname()->getnameinfo()-> returned errno %ld: %s", error, gai_strerror( error ) );
        pthread_exit( reinterpret_cast<void*>( EXIT_FAILURE ) );
    }

    socket->sHost( host );
    LOGFMT( 0, "Socket::ResolveHostname()-> %s", CSTR( socket->gHost() ) );

    pthread_exit( reinterpret_cast<void*>( EXIT_SUCCESS ) );
}

Socket::Socket()
{
    m_descriptor = 0;
    m_host.clear();
    m_output.clear();
    m_port = 0;

    return;
}

Socket::~Socket()
{
    if ( isValid() )
        ::close( m_descriptor );
}
