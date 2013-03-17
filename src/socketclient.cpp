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
/**
 * @file socketclient.cpp
 * @brief All non-template member functions of the SocketClient class.
 *
 * SocketClient implements client-side / remote-host functions on top of the Socket
 * class. A new SocketClient is created for every connection to the listening
 * SocketServer.
 */
#include "h/includes.h"
#include "h/class.h"

#include "h/socketclient.h"
#include "h/command.h"
#include "h/list.h"

/* Core */
/**
 * @brief Unload a client's socket from memory that was previously loaded via SocketClient::New().
 * @retval void
 */
const void SocketClient::Delete()
{
    UFLAGS_DE( flags );

    if ( !Valid() )
        return;

    // Force anything out of the buffer
    Send();

    if ( !m_server->sSocketClose( m_server->gSocketClose() + 1 ) )
        LOGFMT( flags, "SocketClient::Disconnect()->Server::sSocketClose()-> value %lu returned false", m_server->gSocketClose() + 1 );

    gServer()->sSocketClientNext( socket_client_list.erase( find( socket_client_list.begin(), socket_client_list.end(), this ) ) );
    delete this;

    return;
}

/**
 * @brief Build a SocketClient for a new client connection and set all attributes.
 * @param[in] reboot Mark if the server is undergoing a reboot or not.
 * @retval false Returned if there is an error in connecting the client.
 * @retval true Returned if the client is successfully connected.
 */
const bool SocketClient::New( const bool& reboot )
{
    UFLAGS_DE( flags );
    sint_t error = 0;
    sockaddr_storage sin;
    socklen_t size = static_cast<socklen_t>( sizeof( sin ) );
    char hostname[CFG_STR_MAX_BUFLEN], service[CFG_STR_MAX_BUFLEN];

    if ( !reboot )
    {
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

        Handler::Interpret( this );
    }

    return true;
}

/**
 * @brief Tests if there are any commands in the queue to be processed.
 * @retval false Returned if the command queue is empty.
 * @retval true Returned if the command queue is not empty.
 */
const bool SocketClient::PendingCommand() const
{
    return !m_command_queue.empty();
}

/**
 * @brief Tests if there is any output to be processed.
 * @retval false Returned if the output buffer is empty.
 * @retval true Returned if the output buffer is not empty.
 */
const bool SocketClient::PendingOutput() const
{
    return !m_output.empty();
}

/**
 * @brief Interpret the command at the front of the queue.
 * @retval false Returned if the socket is invalid or if a critical fault occurs during interpreting.
 * @retval true Returned if the queue is empty or there were no errors interpreting the command.
 */
const bool SocketClient::ProcessCommand()
{
    UFLAGS_DE( flags );
    pair<string,string> cmd;
    Command* command = NULL;
    uint_t security = ACT_SECURITY_NONE;

    if ( !Valid() )
    {
        LOGFMT( flags, "SocketClient::ProcessCommand()->SocketClient::Valid()-> descriptor %ld returned false", gDescriptor() );
        return false;
    }

    // Nothing new to process; move along
    if ( m_command_queue.empty() )
        return true;

    if ( m_account )
        security = m_account->gSecurity();

    while ( !m_command_queue.empty() )
    {
        cmd = m_command_queue.front();
        m_command_queue.pop_front();

        // Redirect if not fully logged in yet
        if ( ( command = gServer()->FindCommand( cmd.first ) ) != NULL )
        {
            if ( command->Authorized( security ) )
                command->Run( this, cmd.first, cmd.second );
            else if ( m_state < SOC_STATE_PLAYING )
                Handler::Interpret( this, cmd.first, cmd.second );
            else
                Send( CFG_STR_CMD_INVALID );
        }
        else if ( m_state < SOC_STATE_PLAYING )
            Handler::Interpret( this, cmd.first, cmd.second );
        else
            Send( CFG_STR_CMD_INVALID );
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
    string args, cmd;
    Command* search = NULL;

    if ( !Valid() )
    {
        LOGFMT( flags, "SocketClient::QueueCommand()->SocketClient::Valid()-> descriptor %ld returned false", gDescriptor() );
        return false;
    }

    args = command;
    cmd = Utils::Argument( args );

    // Check to see if the command can preempt and go to the front of the queue
    if ( ( search = gServer()->FindCommand( cmd ) ) != NULL )
    {
        if ( search->gPreempt() )
            m_command_queue.push_front( pair<string,string>( cmd, args ) );
        else
            m_command_queue.push_back( pair<string,string>( cmd, args ) );
    }
    else
        m_command_queue.push_back( pair<string,string>( cmd, args ) );

    return true;
}

/**
 * @brief Flags a client as quitting to break out of nested loops.
 * @retval void
 */
const void SocketClient::Quit()
{
    Send( CFG_STR_QUIT );
    LOGFMT( 0, "SocketClient::Quit()-> %s:%lu (%lu)", CSTR( gHostname() ), gPort(), gDescriptor() );

    m_quitting = true;

    return;
}

/**
 * @brief Checks if a client is marked as quitting.
 * @retval bool The value of m_quitting.
 */
const bool SocketClient::Quitting() const
{
    return m_quitting;
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
        LOGFMT( flags, "SocketClient::Recv()->Server::gSocket()->Server::aBytesRecvd()-> value %lu returned false", m_server->gSocket()->gBytesRecvd() + amount );
        return false;
    }

    if ( !aBytesRecvd( amount ) )
    {
        LOGFMT( flags, "SocketClient::Recv()->SocketClient::aBytesRecvd()-> value %lu returned false", gBytesRecvd() + amount );
        return false;
    }

    if ( !sIdle( 0 ) )
    {
        LOGSTR( flags, "SocketClient::Recv()->SocketClient::sIdle()-> value 0 returned false" );
        return false;
    }

    m_input.append( buf );

    return true;
}

/**
 * @brief A wrapper to SocketClient::tResolveHostname(). Necessary for threading a member function.
 * @retval void
 */
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
    if ( m_output.empty() && m_state == SOC_STATE_PLAYING )
    {
        m_output.append( CRLF );
        m_output.append( msg );
    }
    else
        m_output.append( msg );

    return true;
}

/**
 * @brief Serializes the object for writing to disk to be later reloaded.
 * @retval string A serialized string of all object data.
 * @todo Write this function.
 */
const string SocketClient::Serialize() const
{
    return string();
}

/* Query */
/**
 * @brief Returns the Account object associated to this SocketClient.
 * @retval Account Pointer to the associated Account object.
 */
Account* SocketClient::gAccount() const
{
    return m_account;
}

/**
 * @brief Returns the amount of time the client has been idle.
 * @retval uint_t The number of seconds the client has been idle.
 */
const uint_t SocketClient::gIdle() const
{
    return m_idle;
}

/**
 * @brief Returns the value of a #SOC_LOGIN key.
 * @param[in] key The key position to retrieve.
 * @retval string The string stored within the key position, or an empty string if key is outside the proper range.
 */
const string SocketClient::gLogin( const uint_t& key ) const
{
    UFLAGS_DE( flags );

    if ( key < uintmin_t || key >= MAX_SOC_LOGIN )
    {
        LOGFMT( flags, "SocketClient::gLogin()-> called with invalid key: %lu", key );
        return string();
    }

    return m_login[key];
}

/**
 * @brief Returns the Server object associated to this SocketClient.
 * @retval Server Pointer to the associated Server object.
 */
Server* SocketClient::gServer() const
{
    return m_server;
}

/**
 * @brief Returns the connection state of the client from #SOC_STATE.
 * @retval uint_t The current connection state of the client.
 */
const uint_t SocketClient::gState() const
{
    return m_state;
}

/* Manipulate */
/**
 * @brief Associates an Account object to the SocketClient.
 * @param[in] account A pointer to the Account object to be associated.
 * @retval false Returned if the account pointer is NULL.
 * @retval true Returned if the account is successfully associated.
 */
const bool SocketClient::sAccount( Account* account )
{
    UFLAGS_DE( flags );

    if ( account == NULL )
    {
        LOGSTR( flags, "SocketClient::sAccount()-> called with NULL account" );
        return false;
    }

    m_account = account;

    return true;
}

/**
 * @brief Set the idle timer value of the socket.
 * @param[in] idle A #uint_t value ranging from 0 to #CFG_SOC_MAX_IDLE.
 * @retval false Returned if the idle value is outside the proper range.
 * @retval true Returned if the idle value was successfully set.
 */
const bool SocketClient::sIdle( const uint_t& idle )
{
    UFLAGS_DE( flags );

    if ( idle < uintmin_t || idle > CFG_SOC_MAX_IDLE )
    {
        LOGFMT( flags, "SocketClient::sIdle()-> called with invalid idle: %lu", idle );
        return false;
    }

    m_idle = idle;

    return true;
}

/**
 * @brief Set the login command or arguments to be passed between handler functions.
 * @param[in] key A value from #SOC_LOGIN.
 * @param[in] val The value to set key to.
 * @retval false Returned if the key value is outside the proper range.
 * @retval true Returned if the key value was successfully set.
 */
const bool SocketClient::sLogin( const uint_t& key, const string& val )
{
    UFLAGS_DE( flags );

    if ( key < uintmin_t || key >= MAX_SOC_LOGIN )
    {
        LOGFMT( flags, "SocketClient::sLogin()-> called with invalid key: %lu", key );
        return false;
    }

    m_login[key] = val;

    return true;
}

/**
 * @brief Resolve the hostname of the client. Threaded to avoid locking on long resolutions.
 * @param[in] data A self-reference passed via this to use for callback.
 * @retval void
 */
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

/* Internal */
/**
 * @brief Constructor for the SocketClient class.
 * @param[in] server A pointer to an instance of a Server object.
 * @param[in] descriptor A #sint_t value of the file descriptor that has been opened for the socket.
 */
SocketClient::SocketClient( Server* server, const sint_t& descriptor ) : Socket( server, descriptor )
{
    uint_t i = uintmin_t;

    m_account = NULL;
    m_command_queue.clear();
    m_idle = 0;
    m_input.clear();
    for ( i = 0; i < MAX_SOC_LOGIN; i++ )
        m_login[i].clear();
    m_output.clear();
    m_quitting = false;
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
    if ( m_account != NULL )
        m_account->Delete();

    return;
}
