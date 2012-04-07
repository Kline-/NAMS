#include "h/globals.h"
#include "h/socket.h"

// Core
bool Socket::Bind( const uint_t port, const string addr )
{
    UFLAGS_DE( flags );
    static struct sockaddr_in sa_zero;
    struct sockaddr_in sa = sa_zero;

    if ( !iValid() )
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

const void Socket::Disconnect( const string msg )
{
    if ( !msg.empty() )
    {
        Send( msg );
        Send();
    }

    socket_list.remove( this );
    delete this;

    return;
}

bool Socket::Listen() const
{
    UFLAGS_DE( flags );

    if ( !iValid() )
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

bool Socket::ProcessCommand()
{
    UFLAGS_DE( flags );

    if ( !iValid() )
    {
        LOGSTR( flags, "Socket::ProcessCommand()-> called with invalid socket" );
        return false;
    }

    if ( m_command_queue.empty() )
    {
        LOGSTR( flags, "Socket::ProcessCommand()-> called with empty command queue" );
        return false;
    }

    return true;
}

bool Socket::ProcessInput()
{
    UFLAGS_DE( flags );

    vector<string> commands;
    string command;
    ITER( vector, string, vi );

    if ( !iValid() )
    {
        LOGSTR( flags, "Socket::ProcessInput()-> called with invalid socket" );
        return false;
    }

    // Nothing new to process; move along
    if ( m_input.empty() )
        return true;

    commands = Utils::StrNewlines( m_input );
    for ( vi = commands.begin(); vi != commands.end(); vi++ )
    {
        command = *vi;
        if ( !QueueCommand( command ) )
        {
            LOGSTR( flags, "Socket::ProcessInput()->Socket::QueueCommand()-> returned false" );
            return false;
        }
    }

    m_input.clear();

    return true;
}

bool Socket::QueueCommand( const string command )
{
    UFLAGS_DE( flags );

    if ( !iValid() )
    {
        LOGSTR( flags, "Socket::QueueCommand()-> called with invalid socket" );
        return false;
    }

    if ( command.empty() )
    {
        LOGSTR( flags, "Socket::QueueCommand()-> called with empty command" );
        return false;
    }

    m_command_queue.push_back( command );

    return true;
}

bool Socket::Recv()
{
    UFLAGS_DE( flags );
    ssize_t amount = 0;
    char buf[CFG_STR_MAX_BUFLEN] = {'\0'};

    if ( !iValid() )
    {
        LOGSTR( flags, "Socket::Recv()-> called with invalid socket" );
        return false;
    }

    if ( ( m_input.length() + CFG_STR_MAX_BUFLEN ) >= m_input.max_size() )
    {
        LOGSTR( flags, "Socket::Recv()->recv()-> called with m_input overflow" );
        return false;
    }

    if ( ( amount = ::recv( m_descriptor, buf, CFG_STR_MAX_BUFLEN - 1, 0 ) ) < 1 )
    {
        if ( amount == 0 )
        {
            LOGFMT( flags, "Socket::Recv()->recv()-> broken pipe encountered on recv from: %s", CSTR( m_host ) );
            return false;
        }
        else if ( errno != EAGAIN && errno != EWOULDBLOCK )
        {
            LOGFMT( flags, "Socket::Recv()->recv()-> returned errno %d: %s", errno, strerror( errno ) );
            return false;
        }
    }

    m_bytes_recvd += amount;
    m_input += buf;

    return true;
}

const void Socket::ResolveHostname()
{
    UFLAGS_DE( flags );
    pthread_t res_thread;
    pthread_attr_t res_attr;

    if ( !iValid() )
    {
        LOGSTR( flags, "Socket::ResolveHostname()-> called with invalid socket" );
        return;
    }

    pthread_attr_init( &res_attr );
    pthread_attr_setdetachstate( &res_attr, PTHREAD_CREATE_DETACHED );
    pthread_create( &res_thread, &res_attr, &Socket::tResolveHostname, this );

    return;
}

const void Socket::Send( const string msg )
{
    UFLAGS_DE( flags );

    if ( !iValid() )
    {
        LOGSTR( flags, "Socket::Send()-> called with invalid socket" );
        return;
    }

    if ( m_output.empty() && m_state > SOC_STATE_LOGIN_SCREEN )
    {
        m_output += CRLF;
        m_output += msg;
    }
    else
        m_output += msg;

    return;
}

bool Socket::Send()
{
    UFLAGS_DE( flags );
    ssize_t amount = 0;

    if ( !iValid() )
    {
        LOGSTR( flags, "Socket::Send()-> called with invalid socket" );
        return false;
    }

    if ( m_output.empty() )
    {
        LOGSTR( flags, "Socket::Send()->send()-> called with empty output buffer" );
        return false;
    }

    if ( ( amount = ::send( m_descriptor, CSTR( m_output ), m_output.length(), 0 ) ) < 1 )
    {
        if ( amount == 0 )
        {
            LOGFMT( flags, "Socket::Send()->send()-> broken pipe encountered on send to: %s", CSTR( m_host ) );
            return false;
        }
        else if ( errno != EAGAIN && errno != EWOULDBLOCK )
        {
            LOGFMT( flags, "Socket::Send()->send()-> returned errno %d: %s", errno, strerror( errno ) );
            return false;
        }
    }

    m_bytes_sent += amount;
    m_output.clear();

    return true;
}

// Query

// Manipulate
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

bool Socket::sIdle( const uint_t idle )
{
    UFLAGS_DE( flags );

    if ( idle < 0 || idle > CFG_SOC_MAX_IDLE )
    {
        LOGFMT( flags, "Socket::sIdle()-> called with invalid idle: %ld", idle );
        return false;
    }

    m_idle = idle;

    return true;
}

bool Socket::sPort( const uint_t port )
{
    UFLAGS_DE( flags );

    if ( port <= uintmin_t || port >= uintmax_t )
    {
        LOGFMT( flags, "Socket::sPort()-> called with invalid port: %lu", port );
        return false;
    }

    m_port = port;

    return true;
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

bool Socket::sServer( Server* server )
{
    UFLAGS_DE( flags );

    if ( !server )
    {
        LOGSTR( flags, "Socket::sServer()-> called with NULL server" );
        return false;
    }

    if ( !server->iRunning() )
    {
        LOGSTR( flags, "Socket::sServer()-> called with invalid server" );
        return false;
    }

    m_server = server;

    return true;
}

bool Socket::sState( const uint_t state )
{
    UFLAGS_DE( flags );

    if ( state < SOC_STATE_DISCONNECTED || state >= MAX_SOC_STATE )
    {
        LOGFMT( flags, "Socket::sState()-> called with invalid state: %lu", state );
        return false;
    }

    m_state = state;

    return true;
}

Socket::Socket()
{
    m_bytes_recvd = 0;
    m_bytes_sent = 0;
    m_command_queue.clear();
    m_descriptor = 0;
    m_host.clear();
    m_idle = 0;
    m_input.clear();
    m_output.clear();
    m_port = 0;
    m_server = 0;
    m_state = SOC_STATE_DISCONNECTED;

    return;
}

Socket::~Socket()
{
    if ( iValid() )
        ::close( m_descriptor );

    return;
}
