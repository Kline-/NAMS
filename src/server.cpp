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
#include "h/directory.h"

// Core
bool Server::InitSocket( SocketServer* socket_server )
{
    UFLAGS_DE( flags );
    uint_t enable = 1;

    if ( socket_server->sDescriptor( ::socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
        LOGFMT( flags, "Server::InitSocket()->socket()-> returned errno %d: %s", errno, strerror( errno ) );
        return false;
    }

    if ( ::setsockopt( socket_server->gDescriptor(), SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>( &enable ), sizeof( enable ) ) < 0 )
    {
        LOGFMT( flags, "Server::InitSocket()->setsockopt()-> returned errno %d: %s", errno, strerror( errno ) );
        return false;
    }

    m_socket = socket_server;

    return true;
}

bool Server::LoadCommands()
{
    UFLAGS_DE( flags );
    Directory dir;
    vector<string> contents;
    string content;
    ITER( vector, string, vi );

    if ( !dir.Open( CFG_DAT_DIR_COMMAND ) )
    {
        LOGSTR( flags, "Server::LoadCommands()->Directory::Open()-> returned false" );
        return false;
    }

    contents = dir.List();
    dir.Close();
    for ( vi = contents.begin(); vi != contents.end(); vi++ )
    {
        content = *vi;
        LOGSTR( flags, CSTR( content ) );
    }

    if ( !dir.Open( CFG_DAT_DIR_COMMAND ) )
    {
        LOGSTR( flags, "Server::LoadCommands()->Directory::Open()-> returned false" );
        return false;
    }

    return true;
}

const void Server::NewConnection()
{
    UFLAGS_DE( flags );
    struct sockaddr_in sin;
    uint_t descriptor;
    socklen_t size = static_cast<socklen_t>( sizeof( sin ) );
    SocketClient* socket_client;

    if ( ::getsockname( m_socket->gDescriptor(), reinterpret_cast<sockaddr*>( &sin ), &size ) < 0 )
    {
        LOGFMT( flags, "Server::NewConnection()->getsockname()-> returned errno %d: %s", errno, strerror( errno ) );
        return;
    }

    if ( ( descriptor = ::accept( m_socket->gDescriptor(), reinterpret_cast<sockaddr*>( &sin ), &size ) ) < 0 )
    {
        LOGFMT( flags, "Server::NewConnection()->accept()-> returned errno %d: %s", errno, strerror( errno ) );
        return;
    }

    if ( ::fcntl( descriptor, F_SETFL, O_NDELAY ) < 0 )
    {
        LOGFMT( flags, "Server::NewConnection()->fcntl()-> returned errno %d: %s", errno, strerror( errno ) );
        return;
    }

    socket_client = new SocketClient();
    socket_client->sDescriptor( descriptor );
    socket_client->sOwner( this );
    // Usually this is in the constructor, but we have to be certain the socket is fully
    // Configured to avoid any chance of a processing loop accessing it in an invalid state
    socket_client_list.push_back( socket_client );

    if ( ::getpeername( socket_client->gDescriptor(), reinterpret_cast<sockaddr*>( &sin ), &size ) < 0 )
    {
        LOGFMT( flags, "Server::NewConnection()->getpeername()-> returned errno %d: %s", errno, strerror( errno ) );
        socket_client->sHost( "(unknown)" );
    }
    else
    {
        socket_client->sHost( inet_ntoa( sin.sin_addr ) );
        socket_client->sPort( ntohs( sin.sin_port ) );
        LOGFMT( 0, "Server::NewConnection()-> %s:%lu (%lu)", CSTR( socket_client->gHost() ), socket_client->gPort(), socket_client->gDescriptor() );
        socket_client->ResolveHostname();
    }

    // negotiate telopts, send login message
    socket_client->Send( CFG_STR_LOGIN );
    socket_client->sState( SOC_STATE_LOGIN_SCREEN );

    return;
}

bool Server::PollSockets()
{
    UFLAGS_DE( flags );
    static struct timespec static_time;
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
        LOGFMT( flags, "Server::PollSockets()->pselect()-> returned errno %d: %s", errno, strerror( errno ) );
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
            socket_client->sIdle( 0 );

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
            socket_client->sIdle( socket_client->gIdle() + 1 );
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
            socket_client->Disconnect();
            continue;
        }

        if ( socket_client->PendingCommand() )
        {
            socket_client->ProcessCommand();
        }
    }

    return true;
}

const void Server::Shutdown( const sint_t status )
{
    bool was_running = !m_shutdown;

    m_shutdown = true;

    for_each( socket_client_list.begin(), socket_client_list.end(), DeleteObject() );
    for_each( socket_server_list.begin(), socket_server_list.end(), DeleteObject() );

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
    SocketServer* socket_server;

    sTimeBoot();

    socket_server = new SocketServer();

    if ( !InitSocket( socket_server ) )
        Shutdown( EXIT_FAILURE );
    if ( !socket_server->Bind( m_port, CFG_SOC_BIND_ADDR ) )
        Shutdown( EXIT_FAILURE );
    if ( !socket_server->Listen() )
        Shutdown( EXIT_FAILURE );
    socket_server->sHost( gHost() );

    // Bump ourselves to the root folder for file paths
    if ( ::chdir( ".." ) < 0 )
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
string Server::gHost()
{
    UFLAGS_DE( flags );

    string output;
    char buf[CFG_STR_MAX_BUFLEN] = {'\0'};

    if ( gethostname( buf, CFG_STR_MAX_BUFLEN - 1 ) < 0 )
    {
        LOGFMT( flags, "Server::gHost()->gethostname()-> returned errno %d: %s", errno, strerror( errno ) );
        output = "";

        return output;
    }

    output = buf;

    return output;
}

string Server::gTimeBoot() const
{
    string output;

    output = ctime( &m_time_boot );
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

const void Server::sTimeBoot()
{
    struct timeval now;

    gettimeofday( &now, NULL );
    m_time_boot = now.tv_sec;

    return;
}

Server::Server()
{
    m_dir_close = 0;
    m_dir_open = 0;
    m_port = 0;
    m_pulse_rate = CFG_GAM_PULSE_RATE;
    m_shutdown = true;
    m_socket = 0;
    m_time_boot = 0;

    return;
}

Server::~Server()
{
    return;
}
