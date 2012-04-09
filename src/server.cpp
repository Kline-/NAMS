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

#include "h/server.h"
#include "h/command.h"

// Core
bool Server::InitSocket( SocketServer* socket_server )
{
    UFLAGS_DE( flags );
    uint_t enable = 1;

    if ( socket_server->sDescriptor( ::socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
        LOGERRNO( flags, "Server::InitSocket()->socket()->" );
        return false;
    }

    if ( ::setsockopt( socket_server->gDescriptor(), SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>( &enable ), sizeof( enable ) ) < 0 )
    {
        LOGERRNO( flags, "Server::InitSocket()->setsockopt()->" );
        return false;
    }

    m_socket = socket_server;

    return true;
}

bool Server::LoadCommands()
{
    UFLAGS_DE( flags );
    timeval start, finish;
    Command* cmd = NULL;
    multimap<bool,string> files;
    MITER( multimap, bool,string, mi );

    start = Utils::CurrentTime();
    LOGSTR( 0, "Loading commands..." );

    // Populate the multimape with a recursive listing of the commands folder
    Utils::ListDirectory( CFG_DAT_DIR_COMMAND, true, files, m_dir_close, m_dir_open );

    if ( files.empty() )
    {
        LOGFMT( flags, "Server::LoadCommands()->Utils::ListDirectory()-> returned NULL for: %s", CFG_DAT_DIR_COMMAND );
        return false;
    }

    for ( mi = files.begin(); mi != files.end(); mi++ )
    {
        if ( mi->first == UTILS_IS_FILE )
        {
            cmd = new Command();
            if ( !cmd->Load( mi->second ) )
            {
                LOGFMT( flags, "Server::LoadCommands()->Command::Load()-> returned false for command: %s", CSTR( mi->second ) );
                delete cmd;
            }
        }
    }

    finish = Utils::CurrentTime();
    LOGFMT( 0, "Loaded %lu commands in %lums.", command_list.size(), Utils::DiffTime( start, finish, UTILS_TIME_MS ) );

    return true;
}

const void Server::NewConnection()
{
    UFLAGS_DE( flags );
    sockaddr_in sin;
    sint_t descriptor;
    socklen_t size = static_cast<socklen_t>( sizeof( sin ) );
    SocketClient* socket_client;

    if ( ::getsockname( m_socket->gDescriptor(), reinterpret_cast<sockaddr*>( &sin ), &size ) < 0 )
    {
        LOGERRNO( flags, "Server::NewConnection()->getsockname()->" );
        return;
    }

    if ( ( descriptor = ::accept( m_socket->gDescriptor(), reinterpret_cast<sockaddr*>( &sin ), &size ) ) < 0 )
    {
        LOGERRNO( flags, "Server::NewConnection()->accept()->" );
        return;
    }

    if ( ::fcntl( descriptor, F_SETFL, O_NDELAY ) < 0 )
    {
        LOGERRNO( flags, "Server::NewConnection()->fcntl()->" );
        return;
    }

    socket_client = new SocketClient( this, descriptor );

    if ( ::getpeername( socket_client->gDescriptor(), reinterpret_cast<sockaddr*>( &sin ), &size ) < 0 )
    {
        LOGERRNO( flags, "Server::NewConnection()->getpeername()->" );

        if ( !socket_client->sHostname( "(unknown)" ) )
        {
            LOGSTR( flags, "Server::NewConnection()->Socket()->sHostname()-> returned false setting hostname: (unknown)" );
            socket_client->Disconnect();
            return;
        }
    }
    else
    {
        if ( !socket_client->sHostname( inet_ntoa( sin.sin_addr ) ) )
        {
            LOGFMT( flags, "Server::NewConnection()->Socket()->sHostname()-> returned false setting hostname: %s", inet_ntoa( sin.sin_addr ) );
            socket_client->Disconnect();
            return;
        }

        if ( !socket_client->sPort( ntohs( sin.sin_port ) ) )
        {
            LOGFMT( flags, "Server::NewConnection()->Socket()->sPort()-> returned false setting port: %lu", ntohs( sin.sin_port ) );
            socket_client->Disconnect();
            return;
        }

        LOGFMT( 0, "Server::NewConnection()-> %s:%lu (%lu)", CSTR( socket_client->gHostname() ), socket_client->gPort(), socket_client->gDescriptor() );
        socket_client->ResolveHostname();
    }

    // negotiate telopts, send login message
    if ( !socket_client->Send( CFG_STR_LOGIN ) )
    {
        LOGFMT( flags, "Server::NewConnection()->SocketClient()->Send()-> returned false sending: %s", CFG_STR_LOGIN );
        socket_client->Disconnect();
        return;
    }

    if ( !socket_client->sState( SOC_STATE_LOGIN_SCREEN ) )
    {
        LOGFMT( flags, "Server::NewConnection()->SocketClient()->sState()-> returned false setting state: %lu", SOC_STATE_LOGIN_SCREEN );
        socket_client->Disconnect();
        return;
    }

    return;
}

bool Server::PollSockets()
{
    UFLAGS_DE( flags );
    static timespec static_time;
    fd_set exc_set;
    fd_set in_set;
    fd_set out_set;
    ITER( list, SocketClient*, si );
    SocketClient* socket_client;
    sint_t max_desc = 0;

    FD_ZERO( &exc_set );
    FD_ZERO( &in_set );
    FD_ZERO( &out_set );

    FD_SET( m_socket->gDescriptor(), &in_set );
    max_desc = m_socket->gDescriptor();

    // Build three file descriptor lists to be polled
    for ( si = socket_client_list.begin(); si != socket_client_list.end(); si = m_socket_client_next )
    {
        socket_client = *si;
        m_socket_client_next = ++si;
        max_desc = max( m_socket->gDescriptor(), socket_client->gDescriptor() );

        // Populate lists of: exceptions, pending input, pending output
        FD_SET( socket_client->gDescriptor(), &exc_set );
        FD_SET( socket_client->gDescriptor(), &in_set );
        FD_SET( socket_client->gDescriptor(), &out_set );
    }

    // Ensure the file descriptor lists can be watched for updates
    if ( ::pselect( max_desc + 1, &in_set, &out_set, &exc_set, &static_time, 0 ) < 0 )
    {
        LOGERRNO( flags, "Server::PollSockets()->pselect()->" );
        return false;
    }

    // Process new connections
    if ( FD_ISSET( m_socket->gDescriptor(), &in_set ) )
        NewConnection();

    // Process faulted connections
    for ( si = socket_client_list.begin(); si != socket_client_list.end(); si = m_socket_client_next )
    {
        socket_client = *si;
        m_socket_client_next = ++si;

        // Found a faulted socket in the exceptions list
        if ( FD_ISSET( socket_client->gDescriptor(), &exc_set ) )
        {
            // Remove from the other lists and disconnect
            // Continue since we invalidated the socket
            FD_CLR( socket_client->gDescriptor(), &in_set );
            FD_CLR( socket_client->gDescriptor(), &out_set );
            // Don't try to save characters on faulty clients, just boot them
            socket_client->Disconnect();
            continue;
        }
    }

    // Process input from active connections
    for ( si = socket_client_list.begin(); si != socket_client_list.end(); si = m_socket_client_next )
    {
        socket_client = *si;
        m_socket_client_next = ++si;

        // Found an active socket in the input list
        if ( FD_ISSET( socket_client->gDescriptor(), &in_set ) )
        {
            // Pending input; clear the idle timeout
            if ( !socket_client->sIdle( 0 ) )
            {
                LOGSTR( flags, "Server::PollSockets()->SocketClient()->sIdle()-> returned false setting idle: 0" );
                FD_CLR( socket_client->gDescriptor(), &out_set );
                // todo: save character
                socket_client->Disconnect();
                continue;
            }

            // Read input, save game character and disconnect socket if unable to
            if ( !socket_client->Recv() )
            {
                // Remove from the final list
                // Continue since we invalidated the socket
                FD_CLR( socket_client->gDescriptor(), &out_set );
                // todo: save character
                socket_client->Disconnect();
                continue;
            }
        }
        else
        {
            if ( !socket_client->sIdle( socket_client->gIdle() + 1 ) )
            {
                LOGFMT( flags, "Server::PollSockets()->SocketClient()->sIdle()-> returned false setting idle: %lu", socket_client->gIdle() + 1 );
                FD_CLR( socket_client->gDescriptor(), &out_set );
                // todo: save character
                socket_client->Disconnect();
                continue;
            }
        }
    }

    // Process any pending output
    for ( si = socket_client_list.begin(); si != socket_client_list.end(); si = m_socket_client_next )
    {
        socket_client = *si;
        m_socket_client_next = ++si;

        // Disconnect sockets that have been idle for too long
        if ( socket_client->gIdle() >= CFG_SOC_MAX_IDLE )
        {
            socket_client->Disconnect( CFG_STR_IDLE );
            // todo: save character
            continue;
        }

        // Found an active socket in the output list
        if ( FD_ISSET( socket_client->gDescriptor(), &out_set ) && socket_client->PendingOutput() )
        {
            // Send output, save game character and disconnect socket if unable to
            if ( !socket_client->Send() )
            {
                LOGSTR( flags, "Server::PollSockets()->SocketClient()->Send()-> returned false" );
                // Continue since we invalidated the socket
                socket_client->Disconnect();
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
    ITER( list, SocketClient*, si );
    SocketClient* socket_client;

    for ( si = socket_client_list.begin(); si != socket_client_list.end(); si = m_socket_client_next )
    {
        socket_client = *si;
        m_socket_client_next = ++si;

        if ( !socket_client->ProcessInput() )
        {
            LOGSTR( flags, "Server::ProcessInput()->SocketClient()->ProcessInput()-> returned false" );
            socket_client->Disconnect();
            continue;
        }

        if ( socket_client->PendingCommand() )
        {
            if ( !socket_client->ProcessCommand() )
            {
                LOGSTR( flags, "Server::ProcessInput()->SocketClient()->ProcessCommand()-> returned false" );
                socket_client->Disconnect();
                continue;
            }
        }
    }

    return true;
}

const void Server::Shutdown( const sint_t& status )
{
    bool was_running = !m_shutdown;
    MITER( multimap, const char,Command*, mi );
    MITER( multimap, const char,Command*, mi_next );

    m_shutdown = true;

    // Special handling for multimaps
    for ( mi = command_list.begin(); mi != command_list.end(); )
    {
        mi_next = mi++;
        mi_next->second->Unload();
        command_list.erase( mi_next );
    }
    for_each( socket_client_list.begin(), socket_client_list.end(), Utils::DeleteObject() );
    for_each( socket_server_list.begin(), socket_server_list.end(), Utils::DeleteObject() );

    // Only output if the server actually booted; otherwise it probably faulted while getting a port from main()
    if ( was_running )
    {
        if ( status == EXIT_SUCCESS )
            LOGSTR( 0, CFG_STR_EXIT_SUCCESS );
        else
            LOGSTR( 0, CFG_STR_EXIT_FAILURE );
    }

    ::exit( status );
}

const void Server::Startup()
{
    UFLAGS_DE( flags );
    SocketServer* socket_server = new SocketServer();

    LOGFMT( 0, "%s started.", CFG_STR_VERSION );
    m_time_boot = Utils::CurrentTime();

    if ( !InitSocket( socket_server ) )
    {
        LOGSTR( flags, "Server::Startup()->Server::InitSocket()-> returned false" );
        Shutdown( EXIT_FAILURE );
    }

    if ( !socket_server->Bind( m_port, CFG_SOC_BIND_ADDR ) )
    {
        LOGSTR( flags, "Server::Startup()->SocketServer::Bind()-> returned false" );
        Shutdown( EXIT_FAILURE );
    }

    if ( !socket_server->Listen() )
    {
        LOGSTR( flags, "Server::Startup()->SocketServer::Listen()-> returned false" );
        Shutdown( EXIT_FAILURE );
    }

    if ( !socket_server->sHostname( gHostname() ) )
    {
        LOGFMT( flags, "Server::Startup()->Socket()->sHostname()-> returned false setting hostname: %s", CSTR( gHostname() ) );
        Shutdown( EXIT_FAILURE );
    }

    // Ordinarily this is passed via a Socket() constructor, but SocketServer() has no Server* pointer
    // back to its owner (why should it?); so we do it here after we know the socket is valid
    if ( !sSocketOpen( m_socket_open + 1 ) )
    {
        LOGFMT( flags, "Server::Startup()->Server::sSocketOpen()-> returned false setting value: %lu", m_socket_open + 1 );
        Shutdown( EXIT_FAILURE );
    }

    // Bump ourselves to the root folder for file paths
    if ( ::chdir( ".." ) < 0 )
    {
        LOGERRNO( flags, "Server::Startup()->chdir()->" );
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

    m_time_current = Utils::CurrentTime();

    if ( !PollSockets() )
    {
        LOGSTR( flags, "Server::Update()->Server::PollSockets()-> returned false" );
        Shutdown( EXIT_FAILURE );
    }

    if ( !ProcessInput() )
    {
        LOGSTR( flags, "Server::Update()->Server::ProcessInput()-> returned false" );
        Shutdown( EXIT_FAILURE );
    }

    // Sleep to control game pacing
    ::usleep( USLEEP_MAX / m_pulse_rate );

    return;
}

// Query
string Server::gHostname() const
{
    UFLAGS_DE( flags );

    string output;
    char hostname[CFG_STR_MAX_BUFLEN] = {'\0'};

    if ( gethostname( hostname, CFG_STR_MAX_BUFLEN - 1 ) < 0 )
    {
        LOGERRNO( flags, "Server::gHostname()->gethostname()->" );
        output = "(unknown)";

        return output;
    }

    output = hostname;

    return output;
}

string Server::gStatus() const
{
    string output;

    return output;
}

// Manipulate
bool Server::sPort( const uint_t& port )
{
    UFLAGS_DE( flags );

    // No logger output; this should only be called pre-boot
    if ( port <= CFG_SOC_MIN_PORTNUM || port >= CFG_SOC_MAX_PORTNUM )
        return false;

    m_port = port;

    return true;
}

bool Server::sPulseRate( const uint_t& rate )
{
    UFLAGS_DE( flags );

    if ( rate < 1 || rate > USLEEP_MAX )
    {
        LOGFMT( flags, "Server::sPulseRate()-> called with invalid rate: %lu", rate );
        return false;
    }

    return true;
}

bool Server::sSocketClose( const uint_t& amount )
{
    UFLAGS_DE( flags );

    if ( amount < 1 || ( ( m_socket_close + amount ) >= uintmax_t ) )
    {
        LOGFMT( flags, "Server::sSocketClose()-> called with m_socket_close overflow: %lu + %lu", m_socket_close, amount );
        return false;
    }

    m_socket_close += amount;

    return true;
}

bool Server::sSocketOpen( const uint_t& amount )
{
    UFLAGS_DE( flags );

    if ( amount < 1 || ( ( m_socket_open + amount ) >= uintmax_t ) )
    {
        LOGFMT( flags, "Server::sSocketOpen()-> called with m_socket_open overflow: %lu + %lu", m_socket_open, amount );
        return false;
    }

    m_socket_open += amount;

    return true;
}

Server::Server()
{
    m_dir_close = 0;
    m_dir_open = 0;
    m_port = 0;
    m_pulse_rate = CFG_GAM_PULSE_RATE;
    m_shutdown = true;
    m_socket = 0;
    m_socket_client_next = socket_client_list.begin();
    m_socket_close = 0;
    m_socket_open = 0;
    m_time_boot = timeval();
    m_time_current = timeval();

    return;
}

Server::~Server()
{
    return;
}
