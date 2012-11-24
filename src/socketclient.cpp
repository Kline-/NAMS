/***************************************************************************
 * NAMS - Not Another MUD Server                                           *
 * Copyright (C) 2012 Matthew Goff (matt@goff.cc) <http://www.ackmud.net/> *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 ***************************************************************************/
#include "h/includes.h"
#include "h/class.h"

#include "h/socketclient.h"

/** @name Core */ /**@{*/
/**
 * @brief Unload a client's socket from memory that was previously loaded via SocketClient::New().
 * @retval void
 */
const void SocketClient::Delete()
{
    UFLAGS_DE( flags );

    if ( !Valid() )
        return;

    if ( !m_server->sSocketClose( m_server->gSocketClose() + 1 ) )
        LOGFMT( flags, "SocketClient::Disconnect()->Server::sSocketClose()-> value %lu returned false", m_server->gSocketClose() + 1 );

    socket_client_list.remove( this );
    delete this;

    return;
}

/**
 * @brief Build a socket for a new client connection and set all attributes.
 * @retval false Returned if there is an error in connecting the client.
 * @retval true Returned if the client is successfully connected.
 */
const bool SocketClient::New()
{
    UFLAGS_DE( flags );
    sint_t error = 0;
    sockaddr_storage sin;
    socklen_t size = static_cast<socklen_t>( sizeof( sin ) );
    char hostname[CFG_STR_MAX_BUFLEN], service[CFG_STR_MAX_BUFLEN];

    if ( ::fcntl( gDescriptor(), F_SETFL, O_NONBLOCK ) < 0 )
    {
        LOGERRNO( flags, "SocketClient::New()->fcntl()->" );
        return false;
    }

    if ( ::getpeername( gDescriptor(), reinterpret_cast<sockaddr*>( &sin ), &size ) < 0 )
    {
        LOGERRNO( flags, "SocketClient::New()->getpeername()->" );

        if ( !sHostname( "(unknown)" ) )
        {
            LOGSTR( flags, "SocketClient::New()->SocketClient::sHostname()-> hostname (unknown) returned false" );
            return false;
        }
    }
    else
    {
        if ( ( error = ::getnameinfo( reinterpret_cast<sockaddr*>( &sin ), size, hostname, sizeof( hostname ), service, sizeof( service ), NI_NUMERICSERV) ) != 0 )
        {
            LOGFMT( flags, "SocketClient::New()->getnameinfo()-> returned errno %d:%s", error, gai_strerror( error ) );
            return false;
        }

        if ( !sHostname( hostname ) )
        {
            LOGFMT( flags, "SocketClient::New()->SocketClient::sHostname()-> hostname %s returned false", hostname );
            return false;
        }

        if ( !sPort( atol( service ) ) )
        {
            LOGFMT( flags, "SocketClient::New()->SocketClient::sPort()-> port %lu returned false", atol( service ) );
            return false;
        }

        LOGFMT( 0, "SocketClient::New()-> %s:%lu (%lu)", CSTR( gHostname() ), gPort(), gDescriptor() );
    }

    // negotiate telopts, send login message
    if ( !Send( CFG_STR_LOGIN ) )
    {
        LOGSTR( flags, "SocketClient::New()->SocketClient::Send()-> msg CFG_STR_LOGIN returned false" );
        return false;
    }

    if ( !sState( SOC_STATE_LOGIN_SCREEN ) )
    {
        LOGFMT( flags, "SocketClient::New()->SocketClient::sState()-> state %lu returned false", SOC_STATE_LOGIN_SCREEN );
        return false;
    }

    return true;
}

/**
 * @brief Interpret the command at the front of the queue.
 * @retval false Returned if the socket is invalid or if a critical fault occurs during interpreting.
 * @retval true Returned if the queue is empty or there were no errors interpreting the command.
 */
const bool SocketClient::ProcessCommand()
{
    UFLAGS_DE( flags );
    ITER( vector, string, vi );
    ITER( vector, string, vi_next );
    string cmd;

    if ( !Valid() )
    {
        LOGFMT( flags, "SocketClient::ProcessCommand()->SocketClient::Valid()-> descriptor %ld returned false", gDescriptor() );
        return false;
    }

    // Nothing new to process; move along
    if ( m_command_queue.empty() )
        return true;

    for ( vi = m_command_queue.begin(); vi != m_command_queue.end(); vi = vi_next )
    {
        cmd = *vi;
        vi_next = m_command_queue.erase( vi );

        if ( cmd.compare( "shutdown" ) == 0 )
            gServer()->Shutdown( EXIT_SUCCESS );
        else
        {
            Send( cmd );
            Send( CRLF );
        }
    }

    return true;
}

/**
 * @brief Process data from the client's receive buffer and split it into commands to queue as necessary.
 * @retval false Returned if the socket is invalid or if a critical fault occurs when adding the command to the queue.
 * @retval true Returned if there is no input to process or input was processed successfully.
 */
const bool SocketClient::ProcessInput()
{
    UFLAGS_DE( flags );
    vector<string> commands;
    string command;
    ITER( vector, string, vi );

    if ( !Valid() )
    {
        LOGFMT( flags, "SocketClient::ProcessInput()->SocketClient::Valid()-> descriptor %ld returned false", gDescriptor() );
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
            LOGFMT( flags, "SocketClient::ProcessInput()->SocketClient::QueueCommand()-> command %s returned false", CSTR( command ) );
            return false;
        }
    }

    m_input.clear();

    return true;
}

/**
 * @brief Append a command to the back of the queue.
 * @retval false Returned if the socket is invalid.
 * @retval true Returned if a command was successfully added.
 */
const bool SocketClient::QueueCommand( const string& command )
{
    UFLAGS_DE( flags );

    if ( !Valid() )
    {
        LOGFMT( flags, "SocketClient::QueueCommand()->SocketClient::Valid()-> descriptor %ld returned false", gDescriptor() );
        return false;
    }

    m_command_queue.push_back( command );

    return true;
}

/**
 * @brief Receive data from the client and append it to a receive buffer.
 * @retval false Returned if there was an error while receiving data.
 * @retval true Returned if there is no data to receive or data was received successfully.
 */
const bool SocketClient::Recv()
{
    UFLAGS_DE( flags );
    ssize_t amount = 0;
    char buf[CFG_STR_MAX_BUFLEN] = {'\0'};

    if ( !Valid() )
    {
        LOGFMT( flags, "SocketClient::Recv()->SocketClient::Valid()-> descriptor %ld returned false", gDescriptor() );
        return false;
    }

    if ( ( m_input.length() + CFG_STR_MAX_BUFLEN ) >= m_input.max_size() )
    {
        LOGFMT( flags, "SocketClient::Recv()-> called with m_input overflow: %lu + %lu", m_input.length(), CFG_STR_MAX_BUFLEN );
        return false;
    }

    if ( ( amount = ::recv( gDescriptor(), buf, CFG_STR_MAX_BUFLEN - 1, 0 ) ) < 1 )
    {
        if ( amount == 0 )
        {
            LOGFMT( flags, "SocketClient::Recv()->recv()-> broken pipe encountered on recv from: %s", CSTR( gHostname() ) );
            return false;
        }
        else if ( errno != EAGAIN && errno != EWOULDBLOCK )
        {
            LOGERRNO( flags, "SocketClient::Recv()->recv()->" );
            return false;
        }
    }

    if ( !m_server->gSocket()->aBytesRecvd( amount ) )
    {
        LOGFMT( flags, "SocketClient::Recv()->Server::gSocket()->Server::aBytesRecvd()-> value %ld returned false", m_server->gSocket()->gBytesRecvd() + amount );
        return false;
    }

    if ( !aBytesRecvd( amount ) )
    {
        LOGFMT( flags, "SocketClient::Recv()->SocketClient::aBytesRecvd()-> value %ld returned false", gBytesRecvd() + amount );
        return false;
    }

    if ( !sIdle( 0 ) )
    {
        LOGSTR( flags, "SocketClient::Recv()->SocketClient::sIdle()-> value 0 returned false" );
        return false;
    }

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
        LOGFMT( flags, "SocketClient::ResolveHostname()->SocketClient::Valid()-> descriptor %ld returned false", gDescriptor() );
        return;
    }

    if ( ::pthread_attr_init( &res_attr ) != 0 )
    {
        LOGERRNO( flags, "SocketClient::ResolveHostname()->pthread_attr_init()->" );
        return;
    }

    if ( ::pthread_attr_setdetachstate( &res_attr, PTHREAD_CREATE_DETACHED ) != 0 )
    {
        LOGERRNO( flags, "SocketClient::ResolveHostname()->pthread_attr_setdetachstate()->" );
        return;
    }

    if ( ::pthread_create( &res_thread, &res_attr, &SocketClient::tResolveHostname, this ) != 0 )
    {
        LOGERRNO( flags, "SocketClient::ResolveHostname()->pthread_create()->" );
        return;
    }

    if ( ::pthread_attr_destroy( &res_attr ) != 0 )
    {
        LOGERRNO( flags, "SocketClient::ResolveHostname()->pthread_attr_destroy()->" );
        return;
    }

    return;
}

/**
 * @brief Append data to an output buffer in preparation of being sent to the client.
 * @param[in] msg The data to be buffered for sending.
 * @retval false Returned if the socket is invalid.
 * @retval true Returned if data was successfully buffered for sending.
 */
const bool SocketClient::Send( const string& msg )
{
    UFLAGS_DE( flags );

    if ( !Valid() )
    {
        LOGFMT( flags, "SocketClient::Send()->SocketClient::Valid()-> descriptor %ld returned false", gDescriptor() );
        return false;
    }

    // Prepend a CRLF to ensure output lands on a newline.
    // Make this client configurable in the future.
    if ( m_output.empty() && m_state > SOC_STATE_LOGIN_SCREEN )
    {
        m_output += CRLF;
        m_output += msg;
    }
    else
        m_output += msg;

    return true;
}

/**
 * @brief Process data from the client's send buffer and transmit it via the socket.
 * @retval false Returned if the socket is invalid or there is an error while sending.
 * @retval true Returned if there is no data to send or data was sent successfully.
 */
const bool SocketClient::Send()
{
    UFLAGS_DE( flags );
    ssize_t amount = 0;

    if ( !Valid() )
    {
        LOGFMT( flags, "SocketClient::Send()->SocketClient::Valid()-> descriptor %ld returned false", gDescriptor() );
        return false;
    }

    // Nothing new to process; move along
    if ( m_output.empty() )
        return true;

    if ( ( amount = ::send( gDescriptor(), CSTR( m_output ), m_output.length(), 0 ) ) < 1 )
    {
        if ( amount == 0 )
        {
            LOGFMT( flags, "SocketClient::Send()->send()-> broken pipe encountered on send to: %s", CSTR( gHostname() ) );
            return false;
        }
        else if ( errno != EAGAIN && errno != EWOULDBLOCK )
        {
            LOGERRNO( flags, "SocketClient::Send()->send()->" );
            return false;
        }
    }

    if ( !m_server->gSocket()->aBytesSent( amount ) )
    {
        LOGFMT( flags, "SocketClient::Send()->Server::gSocket()->Server::aBytesSent()-> value %lu returned false", m_server->gSocket()->gBytesSent() + amount );
        return false;
    }

    if ( !aBytesSent( amount ) )
    {
        LOGFMT( flags, "SocketClient::Send()->SocketClient::aBytesSent()-> value %lu returned false", gBytesSent() + amount );
        return false;
    }

    m_output.clear();

    return true;
}
/**@}*/

/** @name Query */ /**@{*/
/**@}*/

/** @name Manipulate */ /**@{*/
/**
 * @brief Set the idle timer value of the socket.
 * @param[in] idle A #uint_t value ranging from 0 to #CFG_SOC_MAX_IDLE.
 * @retval false Returned if the idle value is outside the proper range.
 * @retval true Returned if the idle value was successfully set.
 */
const bool SocketClient::sIdle( const uint_t& idle )
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
    static sockaddr_in6 sa_zero;
    sockaddr_in6 sa = sa_zero;
    sint_t error = 0;
    char hostname[CFG_STR_MAX_BUFLEN];

    sa.sin6_family = AF_INET;

    if ( ( error = ::inet_pton( AF_INET6, CSTR( socket_client->gHostname() ), &sa.sin6_addr ) ) != 1 )
    {
        LOGFMT( flags, "SocketClient::tResolveHostname()->inet_pton()-> returned errno %d: %s", error, gai_strerror( error ) );
        ::pthread_exit( reinterpret_cast<void*>( EXIT_FAILURE ) );
    }

    if ( ( error = ::getnameinfo( reinterpret_cast<sockaddr*>( &sa ), sizeof( sa ), hostname, sizeof( hostname ), NULL, 0, 0 ) ) != 0 )
    {
        LOGFMT( flags, "SocketClient::tResolveHostname()->getnameinfo()-> returned errno %d: %s", error, gai_strerror( error ) );
        ::pthread_exit( reinterpret_cast<void*>( EXIT_FAILURE ) );
    }
clog << hostname << endl;
    if ( !socket_client->sHostname( hostname ) )
    {
        LOGFMT( flags, "SocketClient::tResolveHostname()->SocketClient::sHostname()-> hostname %s returned false", hostname );
        ::pthread_exit( reinterpret_cast<void*>( EXIT_FAILURE ) );
    }

    LOGFMT( 0, "SocketClient::ResolveHostname()-> %s", CSTR( socket_client->gHostname() ) );

    ::pthread_exit( reinterpret_cast<void*>( EXIT_SUCCESS ) );
}

/**
 * @brief Set the owning server object that the socket is actually connected to.
 * @param[in] server A pointer to an instance of a Server object. By default this is the server instance which initially accepted the client connection.
 * @retval false Returned if the server is either invalid (NULL) or shutdown.
 * @retval true Returned if owning server is successfully set.
 */
const bool SocketClient::sServer( Server* server )
{
    UFLAGS_DE( flags );

    if ( !server )
    {
        LOGSTR( flags, "SocketClient::sServer()-> called with NULL server" );
        return false;
    }

    if ( !server->Running() )
    {
        LOGSTR( flags, "SocketClient::sServer()-> called with offline server" );
        return false;
    }

    m_server = server;

    return true;
}

/**
 * @brief Set the connection state value of the socket.
 * @param[in] state A #uint_t value ranging from #SOC_STATE_DISCONNECTED to #MAX_SOC_STATE-1.
 * @retval false Returned if the socket state value is outside the proper range.
 * @retval true Returned if the socket state was successfully set.
 */
const bool SocketClient::sState( const uint_t& state )
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
/**@}*/

/** @name Internal */ /**@{*/
/**
 * @brief Constructor for the SocketClient class.
 * @param[in] server A pointer to an instance of a Server object.
 * @param[in] descriptor A #sint_t value of the file descriptor that has been opened for the socket.
 */
SocketClient::SocketClient( Server* server, const sint_t& descriptor ) : Socket( server, descriptor )
{
    m_command_queue.clear();
    m_idle = 0;
    m_input.clear();
    m_output.clear();
    sServer( server );
    m_state = SOC_STATE_DISCONNECTED;

    m_server->sSocketOpen( m_server->gSocketOpen() + 1 );
    socket_client_list.push_back( this );

    return;
}

/**
 * @brief Destructor for the SocketClient class.
 */
SocketClient::~SocketClient()
{
    return;
}
/**@}*/