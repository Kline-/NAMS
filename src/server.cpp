#include "h/globals.h"
#include "h/server.h"

// Core
bool Server::InitSocket( Socket* socket )
{
    UFLAGS_DE( flags );
    uint_t enable = 1;

    if ( socket->sDescriptor( ::socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
        LOGFMT( flags, "Server::InitSocket()->socket()-> returned errno %d: %s", errno, strerror( errno ) );
        return false;
    }

    if ( ::setsockopt( socket->gDescriptor(), SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>( &enable ), sizeof( enable ) ) < 0 )
    {
        LOGFMT( flags, "Server::InitSocket()->setsockopt()-> returned errno %d: %s", errno, strerror( errno ) );
        return false;
    }

    m_socket = socket;

    return true;
}

bool Server::LoadCommands() const
{
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
        LOGFMT( flags, "Server::NewConnection()->getsockname()-> returned errno %d: %s", errno, strerror( errno ) );
        return;
    }

    if ( ( descriptor = ::accept( gSocket()->gDescriptor(), reinterpret_cast<sockaddr*>( &sin ), &size ) ) < 0 )
    {
        LOGFMT( flags, "Server::NewConnection()->accept()-> returned errno %d: %s", errno, strerror( errno ) );
        return;
    }

    if ( ::fcntl( descriptor, F_SETFL, O_NDELAY ) < 0 )
    {
        LOGFMT( flags, "Server::NewConnection()->fcntl()-> returned errno %d: %s", errno, strerror( errno ) );
        return;
    }

    socket = new Socket();
    socket->sDescriptor( descriptor );
    socket_list.push_back( socket );

    if ( ::getpeername( socket->gDescriptor(), reinterpret_cast<sockaddr*>( &sin ), &size ) < 0 )
    {
        LOGFMT( flags, "Server::NewConnection()->getpeername()-> returned errno %d: %s", errno, strerror( errno ) );
        socket->sHost( "(unknown)" );
    }
    else
    {
        socket->sHost( inet_ntoa( sin.sin_addr ) );
        socket->sPort( ntohs( sin.sin_port ) );
        LOGFMT( 0, "Server::NewConnection()-> %s:%lu (%lu)", CSTR( socket->gHost() ), socket->gPort(), socket->gDescriptor() );
        socket->ResolveHostname();
    }

    // negotiate telopts, send login message
    socket->Send( CFG_STR_LOGIN );
    socket->sState( SOC_STATE_LOGIN_SCREEN );

    return;
}

bool Server::PollSockets()
{
    UFLAGS_DE( flags );
    static struct timespec static_time;
    fd_set exc_set;
    fd_set in_set;
    fd_set out_set;
    ITER( list, Socket*, si );
    Socket* socket;
    sint_t max_desc = 0;

    FD_ZERO( &exc_set );
    FD_ZERO( &in_set );
    FD_ZERO( &out_set );

    FD_SET( gSocket()->gDescriptor(), &in_set );
    max_desc = gSocket()->gDescriptor();

    // Build three file descriptor lists to be polled
    for ( si = socket_list.begin(); si != socket_list.end(); si = m_socket_next )
    {
        socket = *si;
        m_socket_next = ++si;
        max_desc = max( gSocket()->gDescriptor(), socket->gDescriptor() );

        // Skip (this) -- only need to poll clients
        if ( socket == m_socket )
            continue;

        // Populate lists of: exceptions, pending input, pending output
        FD_SET( socket->gDescriptor(), &exc_set );
        FD_SET( socket->gDescriptor(), &in_set );
        FD_SET( socket->gDescriptor(), &out_set );
    }

    // Ensure the file descriptor lists can be watched for updates
    if ( ::pselect( max_desc + 1, &in_set, &out_set, &exc_set, &static_time, 0 ) < 0 )
    {
        LOGFMT( flags, "Server::PollSockets()->pselect()-> returned errno %d: %s", errno, strerror( errno ) );
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

        // Skip (this) -- only need to poll clients
        if ( socket == m_socket )
            continue;

        // Found a faulted socket in the exceptions list
        if ( FD_ISSET( socket->gDescriptor(), &exc_set ) )
        {
            // Remove from the other lists and disconnect
            // Continue since we invalidated the socket
            FD_CLR( socket->gDescriptor(), &in_set );
            FD_CLR( socket->gDescriptor(), &out_set );
            socket->Disconnect();
            continue;
        }
    }

    // Process input from active connections
    for ( si = socket_list.begin(); si != socket_list.end(); si = m_socket_next )
    {
        socket = *si;
        m_socket_next = ++si;

        // Skip (this) -- only need to poll clients
        if ( socket == m_socket )
            continue;

        // Found an active socket in the input list
        if ( FD_ISSET( socket->gDescriptor(), &in_set ) )
        {
            // Pending input; clear the idle timeout
            socket->sIdle( 0 );

            // Read input, save game character and disconnect socket if unable to
            if ( !socket->Recv() )
            {
                // Remove from the final list
                // Continue since we invalidated the socket
                FD_CLR( socket->gDescriptor(), &out_set );
                // todo: save character
                socket->Disconnect();
                continue;
            }
        }
        else
            socket->sIdle( socket->gIdle() + 1 );
    }

    // Process any pending output
    for ( si = socket_list.begin(); si != socket_list.end(); si = m_socket_next )
    {
        socket = *si;
        m_socket_next = ++si;

        // Skip (this) -- only need to poll clients
        if ( socket == m_socket )
            continue;

        // Disconnect sockets that have been idle for too long
        if ( socket->gIdle() >= CFG_SOC_MAX_IDLE )
        {
            socket->Disconnect( CFG_STR_IDLE );
            // todo: save character
            continue;
        }

        // Found an active socket in the output list
        if ( FD_ISSET( socket->gDescriptor(), &out_set ) && socket->PendingOutput() )
        {
            // Send output, save game character and disconnect socket if unable to
            if ( !socket->Send() )
            {
                // Continue since we invalidated the socket
                socket->Disconnect();
                // todo: save character
                continue;
            }
        }
    }

    return true;
}

bool Server::ProcessInput()
{
    UFLAGS_DE( flags );
    ITER( list, Socket*, si );
    Socket* socket;

    for ( si = socket_list.begin(); si != socket_list.end(); si = m_socket_next )
    {
        socket = *si;
        m_socket_next = ++si;

        // Skip (this) -- only need to process clients
        if ( socket == m_socket )
            continue;

        if ( !socket->ProcessInput() )
        {
            socket->Disconnect();
            continue;
        }

        if ( socket->PendingCommand() )
        {
            socket->ProcessCommand();
        }
    }

    return true;
}

const void Server::Shutdown( const sint_t status )
{
    bool was_running = !m_shutdown;

    m_shutdown = true;

    for_each( socket_list.begin(), socket_list.end(), Utils::DeleteObject() );

    // Only output if the server actually booted; otherwise it probably faulted while getting a port from main()
    if ( was_running )
    {
        if ( status == EXIT_SUCCESS )
            LOGSTR( 0, "Normal termination of server." );
        else
            LOGSTR( 0, "Server terminated." );
    }

    exit( status );
}

const void Server::Startup()
{
    UFLAGS_DE( flags );
    Socket* socket;

    sTimeBoot();
    sTimeCurrent();

    socket = new Socket();
    socket_list.push_back( socket );

    if ( !InitSocket( socket ) )
        Shutdown( EXIT_FAILURE );
    if ( !socket->Bind( m_port, CFG_SOC_BIND_ADDR ) )
        Shutdown( EXIT_FAILURE );
    if ( !socket->Listen() )
        Shutdown( EXIT_FAILURE );

    // Bump ourselves to the root folder for file paths
    if ( chdir( ".." ) < 0 )
    {
        LOGFMT( flags, "Server::Startup()->chdir()-> returned errno %d: %s", errno, strerror( errno ) );
        Shutdown( EXIT_FAILURE );
    }

    if ( !LoadCommands() )
    {
        LOGSTR( flags, "Server::Startup()->LoadCommands()-> returned false" );
        Shutdown( EXIT_FAILURE );
    }

    m_shutdown = false;
    LOGFMT( 0, "%s is ready on port %lu.", CFG_STR_VERSION, m_port );
    LOGSTR( 0, "Last compiled on " __DATE__ " at " __TIME__ "." );

    return;
}

const void Server::Update()
{
    UFLAGS_DE( flags );

    sTimeCurrent();

    if ( !PollSockets() )
    {
        LOGSTR( flags, "Server::Update()->Server::PollSockets()-> returned false" );
        Shutdown( EXIT_FAILURE );
        return;
    }

    if ( !ProcessInput() )
    {
        LOGSTR( flags, "Server::Update()->Server::ProcessInput()-> returned false" );
        Shutdown( EXIT_FAILURE );
        return;
    }

    // Sleep to control game pacing
    usleep( USLEEP_MAX / m_pulse_rate );

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

    // No logger output; this should only be called pre-boot
    if ( port <= CFG_SOC_MIN_PORTNUM || port >= CFG_SOC_MAX_PORTNUM )
        return false;

    m_port = port;

    return true;
}

bool Server::sPulseRate( const uint_t rate )
{
    UFLAGS_DE( flags );

    if ( rate < 1 || rate > USLEEP_MAX )
    {
        LOGFMT( flags, "Server::sPulseRate()-> called with invalid rate: %lu", rate );
        return false;
    }

    return true;
}

bool Server::sSocket( Socket* socket )
{
    UFLAGS_DE( flags );

    if ( !socket )
    {
        LOGSTR( flags, "Server::sSocket()-> called with NULL socket" );
        return false;
    }

    if ( !socket->isValid() )
    {
        LOGSTR( flags, "Server::sSocket()-> called with invalid socket" );
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
