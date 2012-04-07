#include "h/globals.h"
#include "h/socket.h"

// Core

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

Socket::Socket()
{
    m_bytes_recvd = 0;
    m_bytes_sent = 0;
    m_descriptor = 0;
    m_host.clear();
    m_port = 0;

    return;
}

Socket::~Socket()
{
    if ( Valid() )
        ::close( m_descriptor );

    return;
}

// Client
// Core
const void SocketClient::Disconnect( const string msg )
{
    if ( !msg.empty() )
    {
        Send( msg );
        Send();
    }

    socket_client_list.remove( this );
    delete this;

    return;
}

bool SocketClient::ProcessCommand()
{
    UFLAGS_DE( flags );

    if ( !Valid() )
    {
        LOGSTR( flags, "SocketClient::ProcessCommand()-> called with invalid socket" );
        return false;
    }

    if ( m_command_queue.empty() )
    {
        LOGSTR( flags, "SocketClient::ProcessCommand()-> called with empty command queue" );
        return false;
    }

    return true;
}

bool SocketClient::ProcessInput()
{
    UFLAGS_DE( flags );

    vector<string> commands;
    string command;
    ITER( vector, string, vi );

    if ( !Valid() )
    {
        LOGSTR( flags, "SocketClient::ProcessInput()-> called with invalid socket" );
        return false;
    }

    // Nothing new to process; move along
    if ( m_input.empty() )
        return true;

    commands = StrNewlines( m_input );
    for ( vi = commands.begin(); vi != commands.end(); vi++ )
    {
        command = *vi;
        if ( !QueueCommand( command ) )
        {
            LOGSTR( flags, "SocketClient::ProcessInput()->SocketClient::QueueCommand()-> returned false" );
            return false;
        }
    }

    m_input.clear();

    return true;
}

bool SocketClient::QueueCommand( const string command )
{
    UFLAGS_DE( flags );

    if ( !Valid() )
    {
        LOGSTR( flags, "SocketClient::QueueCommand()-> called with invalid socket" );
        return false;
    }

    if ( command.empty() )
    {
        LOGSTR( flags, "SocketClient::QueueCommand()-> called with empty command" );
        return false;
    }

    m_command_queue.push_back( command );

    return true;
}

bool SocketClient::Recv()
{
    UFLAGS_DE( flags );
    ssize_t amount = 0;
    char buf[CFG_STR_MAX_BUFLEN] = {'\0'};

    if ( !Valid() )
    {
        LOGSTR( flags, "SocketClient::Recv()-> called with invalid socket" );
        return false;
    }

    if ( ( m_input.length() + CFG_STR_MAX_BUFLEN ) >= m_input.max_size() )
    {
        LOGSTR( flags, "SocketClient::Recv()->recv()-> called with m_input overflow" );
        return false;
    }

    if ( ( amount = ::recv( m_descriptor, buf, CFG_STR_MAX_BUFLEN - 1, 0 ) ) < 1 )
    {
        if ( amount == 0 )
        {
            LOGFMT( flags, "SocketClient::Recv()->recv()-> broken pipe encountered on recv from: %s", CSTR( m_host ) );
            return false;
        }
        else if ( errno != EAGAIN && errno != EWOULDBLOCK )
        {
            LOGFMT( flags, "SocketClient::Recv()->recv()-> returned errno %d: %s", errno, strerror( errno ) );
            return false;
        }
    }

    m_bytes_recvd += amount;
    m_input += buf;

    return true;
}

const void SocketClient::ResolveHostname()
{
    UFLAGS_DE( flags );
    pthread_t res_thread;
    pthread_attr_t res_attr;

    if ( !Valid() )
    {
        LOGSTR( flags, "SocketClient::ResolveHostname()-> called with invalid socket" );
        return;
    }

    if ( pthread_attr_init( &res_attr ) != 0 )
    {
        LOGFMT( flags, "SocketClient::ResolveHostname()->pthread_attr_init()-> returned errno %d: %s", errno, strerror( errno ) );
        return;
    }

    if ( pthread_attr_setdetachstate( &res_attr, PTHREAD_CREATE_DETACHED ) != 0 )
    {
        LOGFMT( flags, "SocketClient::ResolveHostname()->pthread_attr_setdetachstate()-> returned errno %d: %s", errno, strerror( errno ) );
        return;
    }

    if ( pthread_create( &res_thread, &res_attr, &SocketClient::tResolveHostname, this ) != 0 )
    {
        LOGFMT( flags, "SocketClient::ResolveHostname()->pthread_create()-> returned errno %d: %s", errno, strerror( errno ) );
        return;
    }
 
    if ( pthread_attr_destroy( &res_attr ) != 0 )
    {
        LOGFMT( flags, "SocketClient::ResolveHostname()->pthread_attr_destroy()-> returned errno %d: %s", errno, strerror( errno ) );
        return;
    }

    return;
}

const void SocketClient::Send( const string msg )
{
    UFLAGS_DE( flags );

    if ( !Valid() )
    {
        LOGSTR( flags, "SocketClient::Send()-> called with invalid socket" );
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

bool SocketClient::Send()
{
    UFLAGS_DE( flags );
    ssize_t amount = 0;

    if ( !Valid() )
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
bool SocketClient::sIdle( const uint_t idle )
{
    UFLAGS_DE( flags );

    if ( idle < 0 || idle > CFG_SOC_MAX_IDLE )
    {
        LOGFMT( flags, "SocketClient::sIdle()-> called with invalid idle: %ld", idle );
        return false;
    }

    m_idle = idle;

    return true;
}

void* SocketClient::tResolveHostname( void* data )
{
    UFLAGS_DE( flags );
    SocketClient* socket_client = reinterpret_cast<SocketClient*>( data );
    static struct sockaddr_in sa_zero;
    struct sockaddr_in sa = sa_zero;
    sint_t error = 0;
    char host[CFG_STR_MAX_BUFLEN];

    sa.sin_family = AF_INET;

    if ( ( error = inet_pton( AF_INET, CSTR( socket_client->gHost() ), &sa.sin_addr ) ) != 1 )
    {
        socket_client->Logger( flags, socket_client->FormatString( flags, "SocketClient::tResolveHostname()->inet_pton()-> returned errno %ld: %s", error, gai_strerror( error ) ) );
        pthread_exit( reinterpret_cast<void*>( EXIT_FAILURE ) );
    }

    if ( ( error = getnameinfo( reinterpret_cast<struct sockaddr*>( &sa ), sizeof( sa ), host, sizeof( host ), NULL, 0, 0 ) ) != 0 )
    {
        socket_client->Logger( flags, socket_client->FormatString( flags, "SocketClient::tResolveHostname()->getnameinfo()-> returned errno %ld: %s", error, gai_strerror( error ) ) );
        pthread_exit( reinterpret_cast<void*>( EXIT_FAILURE ) );
    }

    socket_client->sHost( host );
    socket_client->Logger( 0, "SocketClient::ResolveHostname()-> %s", CSTR( socket_client->gHost() ) );

    pthread_exit( reinterpret_cast<void*>( EXIT_SUCCESS ) );
}

bool SocketClient::sServer( Server* server )
{
    UFLAGS_DE( flags );

    if ( !server )
    {
        LOGSTR( flags, "SocketClient::sServer()-> called with NULL server" );
        return false;
    }

    if ( !server->iRunning() )
    {
        LOGSTR( flags, "SocketClient::sServer()-> called with invalid server" );
        return false;
    }

    m_server = server;

    return true;
}

bool SocketClient::sState( const uint_t state )
{
    UFLAGS_DE( flags );

    if ( state < SOC_STATE_DISCONNECTED || state >= MAX_SOC_STATE )
    {
        LOGFMT( flags, "SocketClient::sState()-> called with invalid state: %lu", state );
        return false;
    }

    m_state = state;

    return true;
}

SocketClient::SocketClient()
{
    m_command_queue.clear();
    m_idle = 0;
    m_input.clear();
    m_output.clear();
    m_server = 0;
    m_state = SOC_STATE_DISCONNECTED;

    return;
}

SocketClient::~SocketClient()
{
    return;
}

// Server
// Core
bool SocketServer::Bind( const uint_t port, const string addr )
{
    UFLAGS_DE( flags );
    static struct sockaddr_in sa_zero;
    struct sockaddr_in sa = sa_zero;

    if ( !Valid() )
    {
        LOGSTR( flags, "SocketServer::Bind()-> called with invalid socket" );
        return false;
    }

    sa.sin_family = AF_INET;
    sa.sin_port = htons( port );

    if ( !addr.empty() )
        sa.sin_addr.s_addr = inet_addr( CSTR( addr ) );

    if ( ::bind( m_descriptor, reinterpret_cast<sockaddr*>( &sa ), sizeof( sa ) ) < 0 )
    {
        LOGFMT( flags, "SocketServer::Bind()->bind()-> returned errno %d: %s", errno, strerror( errno ) );
        return false;
    }

    return true;
}

bool SocketServer::Listen()
{
    UFLAGS_DE( flags );

    if ( !Valid() )
    {
        LOGSTR( flags, "SocketServer::Listen()-> called with invalid socket" );
        return false;
    }

    if ( ::listen( m_descriptor, CFG_SOC_MAX_PENDING ) < 0 )
    {
        LOGFMT( flags, "SocketServer::Listen()->listen()-> returned errno %d: %s", errno, strerror( errno ) );
        return false;
    }

    return true;
}

// Query

// Manipulate
SocketServer::SocketServer()
{
    return;
}

SocketServer::~SocketServer()
{
    return;
}
