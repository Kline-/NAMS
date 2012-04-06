#include "h/globals.h"
#include "h/server.h"

// Core
bool Server::InitSocket( Socket* socket )
{
    UFLAGS_DE( flags );
    uint_t enable = 1;

    if ( socket->sDescriptor( ::socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
        Utils::Logger( flags, Utils::FormatString( flags, "Server::InitSocket()->socket()-> returned errno %d: %s", errno, strerror( errno ) ) );
        return false;
    }

    if ( ::setsockopt( socket->gDescriptor(), SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>( &enable ), sizeof( enable ) ) < 0 )
    {
        Utils::Logger( flags, Utils::FormatString( flags, "Server::InitSocket()->setsockopt()-> returned errno %d: %s", errno, strerror( errno ) ) );
        return false;
    }

    m_socket = socket;

    return true;
}

const void Server::NewConnection() const
{
    UFLAGS_DE( flags );
    struct sockaddr_in sin;
    uint_t descriptor;
    socklen_t size = static_cast<socklen_t>( sizeof( sin ) );
    Socket* socket;

    if ( ::getsockname( gSocket()->gDescriptor(), reinterpret_cast<sockaddr*>( &sin ), &size ) < 0 )
    {
        Utils::Logger( flags, Utils::FormatString( flags, "Server::NewConnection()->getsockname()-> returned errno %d: %s", errno, strerror( errno ) ) );
        return;
    }

    if ( ( descriptor = ::accept( gSocket()->gDescriptor(), reinterpret_cast<sockaddr*>( &sin ), &size ) ) < 0 )
    {
        Utils::Logger( flags, Utils::FormatString( flags, "Server::NewConnection()->accept()-> returned errno %d: %s", errno, strerror( errno ) ) );
        return;
    }

    if ( ::fcntl( descriptor, F_SETFL, O_NDELAY ) < 0 )
    {
        Utils::Logger( flags, Utils::FormatString( flags, "Server::NewConnection()->fcntl()-> returned errno %d: %s", errno, strerror( errno ) ) );
        return;
    }

    socket = new Socket();
    socket->sDescriptor( descriptor );

    if ( ::getpeername( socket->gDescriptor(), reinterpret_cast<sockaddr*>( &sin ), &size ) < 0 )
    {
        Utils::Logger( flags, Utils::FormatString( flags, "Server::NewConnection()->getpeername()-> returned errno %d: %s", errno, strerror( errno ) ) );
        socket->sHost( "(unknown)" );
    }
    else
    {
        socket->sHost( inet_ntoa( sin.sin_addr ) );
        socket->sPort( ntohs( sin.sin_port ) );
        Utils::Logger( 0, Utils::FormatString( 0, "Server::NewConnection()-> %s:%lu", CSTR( socket->gHost() ), socket->gPort() ) );
        socket->ResolveHostname();
    }

    // negotiate telopts, send login message

    return;
}

bool Server::PollSockets()
{
    UFLAGS_DE( flags );
    static struct timespec static_time;
    fd_set in_set;
    fd_set out_set;
    fd_set exc_set;
    ITER( list, Socket*, si );
    Socket* socket;
    sint_t max_desc = 0;

    FD_ZERO( &in_set );
    FD_ZERO( &out_set );
    FD_ZERO( &exc_set );

    FD_SET( gSocket()->gDescriptor(), &in_set );

    // Build three file descriptor lists to be polled
    for ( si = socket_list.begin(); si != socket_list.end(); si = m_socket_next )
    {
        socket = *si;
        m_socket_next = ++si;

        max_desc = max( gSocket()->gDescriptor(), socket->gDescriptor() );
        FD_SET( socket->gDescriptor(), &in_set );
        FD_SET( socket->gDescriptor(), &out_set );
        FD_SET( socket->gDescriptor(), &exc_set );
    }

    // Ensure the file descriptor lists can be watched for updates
    if ( ::pselect( max_desc + 1, &in_set, &out_set, &exc_set, &static_time, 0 ) < 0 )
    {
        Utils::Logger( flags, Utils::FormatString( flags, "Server::PollSockets()->pselect()-> returned errno %d: %s", errno, strerror( errno ) ) );
        return false;
    }

    // Process new connections
    if ( FD_ISSET( gSocket()->gDescriptor(), &in_set ) )
        NewConnection();

    // Process faulted connections
    for ( si = socket_list.begin(); si != socket_list.end(); si = m_socket_next )
    {
        socket = *si;
        m_socket_next = ++si;

        if ( FD_ISSET( socket->gDescriptor(), &exc_set ) )
        {
            FD_CLR( socket->gDescriptor(), &in_set );
            FD_CLR( socket->gDescriptor(), &out_set );
            socket_list.remove( socket );
            delete socket;
        }
    }

    // Process input from active connections
    for ( si = socket_list.begin(); si != socket_list.end(); si = m_socket_next )
    {
        socket = *si;
        m_socket_next = ++si;

        if ( FD_ISSET( socket->gDescriptor(), &in_set ) )
        {
            if ( 0 )
            {
                // reset game character idle timer
            }

            if ( 0 )
            {
                // read input, save game character and disconnect socket if unable to
                // continue since we invalidated the socket
                continue;
            }
        }
    }

    // Process any pending output
    for ( si = socket_list.begin(); si != socket_list.end(); si = m_socket_next )
    {
        socket = *si;
        m_socket_next = ++si;

        if ( /* check output buffer */ FD_ISSET( socket->gDescriptor(), &out_set ) )
        {
            if ( 0 )
            {
                // send output, save game character and disconnect socket if unable to
                // continue since we invalidated the socket
                continue;
            }
        }
    }

    // Sleep to control game pacing
    usleep( USLEEP_MAX / m_pulse_rate );

    return true;
}

const void Server::Shutdown( const sint_t status )
{
    m_shutdown = true;

    for_each( socket_list.begin(), socket_list.end(), Utils::DeleteObject() );

    if ( status == EXIT_SUCCESS )
        Utils::Logger( 0, "Normal termination of server." );
    else
        Utils::Logger( 0, "Server terminated." );

    exit( status );
}

const void Server::Update()
{
    UFLAGS_DE( flags );

    sTimeCurrent();

    if ( !PollSockets() )
    {
        Utils::Logger( flags, "Server::Update()->Server::PollSockets()-> returned false" );
        Shutdown( EXIT_FAILURE );
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
    UFLAGS_DE( flags );

    if ( port <= CFG_SOC_MIN_PORTNUM || port >= CFG_SOC_MAX_PORTNUM )
    {
        Utils::Logger( flags, Utils::FormatString( flags, "Server::sPort()-> called with invalid port: %lu", port ) );
        return false;
    }

    m_port = port;

    return true;
}

bool Server::sPulseRate( const uint_t rate )
{
    UFLAGS_DE( flags );

    if ( rate < 1 || rate > USLEEP_MAX )
    {
        Utils::Logger( flags, Utils::FormatString( flags, "Server::sPulseRate()-> called with invalid rate: %lu", rate ) );
        return false;
    }

    return true;
}

const void Server::sRunning()
{
    m_shutdown = false;
    Utils::Logger( 0, Utils::FormatString( 0, "%s is ready on port %lu.", CFG_STR_VERSION, server.gPort() ) );
    Utils::Logger( 0, "Last compiled on " __DATE__ " at " __TIME__ "." );

    return;
}

bool Server::sSocket( Socket* socket )
{
    UFLAGS_DE( flags );

    if ( !socket )
    {
        Utils::Logger( flags, "Server::sSocket()-> called with NULL socket" );
        return false;
    }

    if ( !socket->isValid() )
    {
        Utils::Logger( flags, "Server::sSocket()-> called with invalid socket" );
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
    m_pulse_rate = CFG_GAM_PULSE_RATE;
    m_shutdown = true;
    m_socket = 0;
    m_time_boot = 0;
    m_time_current = 0;

    return;
}

Server::~Server()
{
    return;
}
