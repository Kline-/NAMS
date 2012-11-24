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
 * @file server.cpp
 * @brief All non-trivial member functions of the Server class.
 */
#include "h/includes.h"
#include "h/class.h"

#include "h/server.h"
#include "h/command.h"

/** @name Core */ /**@{*/
/**
 * @brief Compile a plugin file.
 * @param[in] file The file to be compiled. The file extension should end in #CFG_PLG_BUILD_EXT_IN.
 * @param[in] force Force a rebuild even if the output file already exists.
 * @retval false Returned if a fault is experienced trying to build the plugin.
 * @retval true Returned if the plugin builds successfully.
 */
const bool Server::BuildPlugin( const string& file, const bool& force )
{
    UFLAGS_DE( flags );
    FILE* popen_fil = NULL;
    string build_cmd, build_res;
    char buf[CFG_STR_MAX_BUFLEN] = {'\0'};

    if ( Utils::iReadable( Utils::BuildPath( CFG_DAT_DIR_OBJ, file, CFG_PLG_BUILD_EXT_OUT ) ) && !force )
        return true;

    build_cmd = CFG_PLG_BUILD_CMD " -o ";
    build_cmd += Utils::BuildPath( CFG_DAT_DIR_OBJ, file, CFG_PLG_BUILD_EXT_OUT );
    build_cmd += " ";
    build_cmd += Utils::BuildPath( CFG_DAT_DIR_COMMAND, file );
    build_cmd += " " CFG_PLG_BUILD_OPT;

    if ( ( popen_fil = popen( CSTR( build_cmd ), "r" ) ) != NULL )
    {
        while( fgets( buf, CFG_STR_MAX_BUFLEN, popen_fil ) != NULL )
            build_res += buf;

        pclose( popen_fil );
    }

    // Something went wrong
    if ( !build_res.empty() )
    {
        LOGFMT( flags, "Server::BuildPlugin()->returned error: %s", CSTR( build_res ) );
        return false;
    }
    else
        LOGFMT( 0, "Plugin built successfully: %s", CSTR( file ) );

    return true;
}

/**
 * @brief Search all subfolders of #CFG_DAT_DIR_COMMAND and call Command::New() to load each file found to memory.
 * @retval false Returned if a fault is experienced trying to obtain a directory listing to process.
 * @retval true Returned if 0 or more Command objects are loaded from disk.
 */
const bool Server::LoadCommands()
{
    UFLAGS_DE( flags );
    timeval start, finish;
    Command* cmd = NULL;
    multimap<bool,string> files;
    MITER( multimap, bool,string, mi );

    start = Utils::CurrentTime();
    LOGSTR( 0, "Loading commands..." );

    // Populate the multimap with a recursive listing of the commands folder
    Utils::ListDirectory( CFG_DAT_DIR_COMMAND, true, files, m_dir_close, m_dir_open );

    if ( files.empty() )
    {
        LOGSTR( flags, "Server::LoadCommands()->Utils::ListDirectory()-> CFG_DAT_DIR_COMMAND returned NULL" );
        return false;
    }

    for ( mi = files.begin(); mi != files.end(); mi++ )
    {
        if ( mi->first == UTILS_IS_FILE && ( mi->second.substr( mi->second.find_last_of( "." ) + 1 ).compare( CFG_PLG_BUILD_EXT_IN ) == 0 ) )
        {
            if ( !BuildPlugin( mi->second ) )
            {
                LOGFMT( flags, "Server::LoadCommand()->Server::BuildPlugin()-> file %s returned false", CSTR( mi->second ) );
                continue;
            }

            cmd = new Command();
            if ( !cmd->New( mi->second ) )
            {
                LOGFMT( flags, "Server::LoadCommands()->Command::New()-> command %s returned false", CSTR( mi->second ) );
                delete cmd;
            }
        }
    }

    finish = Utils::CurrentTime();
    LOGFMT( 0, "Loaded %lu commands in %lums.", command_list.size(), Utils::DiffTime( start, finish, UTILS_TIME_MS ) );

    return true;
}

const bool Server::PollSockets()
{
    UFLAGS_DE( flags );
    static timespec static_time;
    fd_set exc_set;
    fd_set in_set;
    fd_set out_set;
    ITER( list, SocketClient*, si );
    SocketClient* socket_client;
    sint_t client_desc = 0, max_desc = 0, server_desc = 0;

    FD_ZERO( &exc_set );
    FD_ZERO( &in_set );
    FD_ZERO( &out_set );

    if ( ( server_desc = m_socket->gDescriptor() ) < 1 )
    {
        LOGFMT( flags, "Server::PollSockets()->SocketServer::gDescriptor()-> returned invalid descriptor: %ld", server_desc );
        return false;
    }

    FD_SET( server_desc, &in_set );
    max_desc = server_desc;

    // Build three file descriptor lists to be polled
    for ( si = socket_client_list.begin(); si != socket_client_list.end(); si = m_socket_client_next )
    {
        socket_client = *si;

        if ( ( client_desc = socket_client->gDescriptor() ) < 1 )
        {
            LOGFMT( flags, "Server::PollSockets()->SocketClient::gDescriptor()-> returned invalid descriptor: %ld", client_desc );
            socket_client->Delete();
            continue;
        }

        m_socket_client_next = ++si;
        max_desc = max( server_desc, client_desc );

        // Populate lists of: exceptions, pending input, pending output
        FD_SET( client_desc, &exc_set );
        FD_SET( client_desc, &in_set );
        FD_SET( client_desc, &out_set );
    }

    // Ensure the file descriptor lists can be watched for updates
    if ( ::pselect( max_desc + 1, &in_set, &out_set, &exc_set, &static_time, 0 ) < 0 )
    {
        LOGERRNO( flags, "Server::PollSockets()->pselect()->" );
        return false;
    }

    // Process new connections
    if ( FD_ISSET( server_desc, &in_set ) )
        m_socket->Accept();

    // Process faulted connections
    for ( si = socket_client_list.begin(); si != socket_client_list.end(); si = m_socket_client_next )
    {
        socket_client = *si;
        m_socket_client_next = ++si;

        if ( ( client_desc = socket_client->gDescriptor() ) < 1 )
        {
            LOGFMT( flags, "Server::PollSockets()->SocketClient::gDescriptor()-> returned invalid descriptor: %ld", client_desc );
            socket_client->Delete();
            continue;
        }

        // Found a faulted socket in the exceptions list
        if ( FD_ISSET( client_desc, &exc_set ) )
        {
            // Don't try to save characters on faulty clients, just boot them
            LOGFMT( flags, "Server::PollSockets()-> disconnecting faulted descriptor: %ld", client_desc );
            socket_client->Delete();
            continue;
        }
    }

    // Process input from active connections
    for ( si = socket_client_list.begin(); si != socket_client_list.end(); si = m_socket_client_next )
    {
        socket_client = *si;
        m_socket_client_next = ++si;

        if ( ( client_desc = socket_client->gDescriptor() ) < 1 )
        {
            LOGFMT( flags, "Server::PollSockets()->SocketClient::gDescriptor()-> returned invalid descriptor: %ld", client_desc );
            // todo: save character
            socket_client->Delete();
            continue;
        }

        // Found an active socket in the input list
        if ( FD_ISSET( client_desc, &in_set ) )
        {
            // Pending input; clear the idle timeout
            if ( !socket_client->sIdle( 0 ) )
            {
                LOGFMT( flags, "Server::PollSockets()->SocketClient::sIdle()-> descriptor %ld returned false setting idle: 0", client_desc );
                // todo: save character
                socket_client->Delete();
                continue;
            }

            // Read input, save game character and disconnect socket if unable to
            if ( !socket_client->Recv() )
            {
                LOGFMT( flags, "Server::PollSockets()->SocketClient::Recv()-> descriptor %ld returned false", client_desc );
                // todo: save character
                socket_client->Delete();
                continue;
            }
        }
        else
        {
            if ( !socket_client->sIdle( socket_client->gIdle() + 1 ) )
            {
                LOGFMT( flags, "Server::PollSockets()->SocketClient::sIdle()-> descriptor %ld returned false setting idle: %lu", client_desc, socket_client->gIdle() + 1 );
                // todo: save character
                socket_client->Delete();
                continue;
            }
        }
    }

    // Process any pending output
    for ( si = socket_client_list.begin(); si != socket_client_list.end(); si = m_socket_client_next )
    {
        socket_client = *si;
        m_socket_client_next = ++si;

        if ( ( client_desc = socket_client->gDescriptor() ) < 1 )
        {
            LOGFMT( flags, "Server::PollSockets()->SocketClient::gDescriptor()-> returned invalid descriptor: %ld", client_desc );
            // todo: save character
            socket_client->Delete();
            continue;
        }

        // Disconnect sockets that have been idle for too long
        if ( socket_client->gIdle() >= CFG_SOC_MAX_IDLE )
        {
            LOGFMT( flags, "Server::PollSockets()->SocketClient::gIdle()-> disconnecting idle descriptor: %ld", client_desc );
            socket_client->Send( CFG_STR_IDLE );
            socket_client->Delete();
            // todo: save character
            continue;
        }

        // Found an active socket in the output list
        if ( FD_ISSET( client_desc, &out_set ) && socket_client->PendingOutput() )
        {
            // Send output, save game character and disconnect socket if unable to
            if ( !socket_client->Send() )
            {
                LOGFMT( flags, "Server::PollSockets()->SocketClient::PendingOutput()->SocketClient::Send()-> descriptor %ld returned false", client_desc );
                socket_client->Delete();
                // todo: save character
                continue;
            }
        }
    }

    return true;
}

const bool Server::ProcessInput()
{
    UFLAGS_DE( flags );
    ITER( list, SocketClient*, si );
    SocketClient* socket_client;
    sint_t client_desc = 0;

    for ( si = socket_client_list.begin(); si != socket_client_list.end(); si = m_socket_client_next )
    {
        socket_client = *si;
        m_socket_client_next = ++si;

        if ( ( client_desc = socket_client->gDescriptor() ) < 1 )
        {
            LOGFMT( flags, "Server::ProcessInput()->SocketClient::gDescriptor()-> returned invalid descriptor: %ld", client_desc );
            // todo: save character
            socket_client->Delete();
            continue;
        }

        if ( !socket_client->ProcessInput() )
        {
            LOGFMT( flags, "Server::ProcessInput()->SocketClient::ProcessInput()-> descriptor %ld returned false", client_desc );
            // todo: save character
            socket_client->Delete();
            continue;
        }

        if ( socket_client->PendingCommand() )
        {
            if ( !socket_client->ProcessCommand() )
            {
                LOGFMT( flags, "Server::ProcessInput()->SocketClient::PendingCommand()->SocketClient::ProcessCommand()-> descriptor %ld returned false", client_desc );
                // todo: save character
                socket_client->Delete();
                continue;
            }
        }
    }

    return true;
}

/**
 * @brief Perform a clean shutdown of the NAMS server providing a chance to complete disk writes and free all memory to verify there are no leaks.
 * @param[in] status The shutdown code to pass to exit() when NAMS exits. Either EXIT_FAILURE or EXIT_SUCCESS.
 * @retval void
 */
const void Server::Shutdown( const sint_t& status )
{
    bool was_running = !m_shutdown;
    MITER( multimap, const char,Command*, mi );
    MITER( multimap, const char,Command*, mi_next );
    m_shutdown = true;

    // Cleanup commands
    for ( mi = command_list.begin(); mi != command_list.end(); )
    {
        mi_next = mi++;
        mi_next->second->Delete();
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

/**
 * @brief Start the NAMS server. Responsible for calling all critical boot-time functions such as SocketServer initialization, Command loading, etc.
 * @retval void
 */
const void Server::Startup()
{
    UFLAGS_DE( flags );
    SocketServer* socket_server;
    sint_t descriptor = 0;
    m_shutdown = false;

    LOGFMT( 0, "%s started.", CFG_STR_VERSION );
    m_time_boot = Utils::CurrentTime();

    if ( ( descriptor = ::socket( AF_INET6, SOCK_STREAM, 0 ) ) < 0 )
    {
        LOGERRNO( flags, "Server::Startup()->socket()->" );
        Shutdown( EXIT_FAILURE );
    }

    socket_server = new SocketServer( this, descriptor );
    m_socket = socket_server;

    if ( !socket_server->New() )
    {
        LOGSTR( flags, "Server::Startup()->SocketServer::New()-> returned false" );
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
        LOGSTR( flags, "Server::Startup()->Server::LoadCommands()-> returned false" );
        Shutdown( EXIT_FAILURE );
    }

    LOGFMT( 0, "%s is ready on port %lu.", CFG_STR_VERSION, m_port );
    LOGSTR( 0, "Last compiled on " __DATE__ " at " __TIME__ "." );

    return;
}

/**
 * @brief The core update loop of NAMS. This loop spawns all other subsystem update routines and then sleeps for ( #USLEEP_MAX / #CFG_GAM_PULSE_RATE ) each cycle.
 * @retval void
 */
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
    ::usleep( USLEEP_MAX / CFG_GAM_PULSE_RATE );

    return;
}
/**@}*/

/** @name Query */ /**@{*/
/**
 * @brief Gets the hostname of the machine that NAMS is running on.
 * @retval string A string is returned containing either "(unknown)" or the machine hostname.
 * @todo Move this to Server::sHostname() and redo this func as a simple getter. Don't need to query the host server multiple times.
 */
const string Server::gHostname() const
{
    UFLAGS_DE( flags );
    string output;
    char hostname[CFG_STR_MAX_BUFLEN] = {'\0'};

    if ( ::gethostname( hostname, CFG_STR_MAX_BUFLEN - 1 ) < 0 )
    {
        LOGERRNO( flags, "Server::gHostname()->gethostname()->" );
        output = "(unknown)";

        return output;
    }

    output = hostname;

    return output;
}

/**
 * @brief Display miscellaneous data about the NAMS server, such as total data transfered, objects in memory, etc.
 * @retval string A string is returned containing a pre-formatted data display of all Server information.
 * @todo Write this entire function.
 */
const string Server::gStatus() const
{
    string output;

    return output;
}
/**@}*/

/** @name Manipulate */ /**@{*/
/**
 * @brief Set the port of a NAMS Server object.
 * @param[in] port The port number to have a SocketServer instance listen for new connections on.
 * @retval false Returned if the port was <= #CFG_SOC_MIN_PORTNUM or >= #CFG_SOC_MAX_PORTNUM.
 * @retval true Returned if the port was > #CFG_SOC_MIN_PORTNUM and < #CFG_SOC_MAX_PORTNUM.
 */
const bool Server::sPort( const uint_t& port )
{
    UFLAGS_DE( flags );

    // No logger output; this should only be called pre-boot
    if ( port <= CFG_SOC_MIN_PORTNUM || port >= CFG_SOC_MAX_PORTNUM )
        return false;

    m_port = port;

    return true;
}

/**
 * @brief Set the amount of subordinate SocketClient and SocketServer objects that have been closed on a NAMS Server object.
 * @param[in] amount The amount that Server::m_socket_close should be set to.
 * @retval false Returned if amount is outside the boundaries of a uint_t variable.
 * @retval true Returned if amount is within the boundaries of a uint_t variable.
 */
const bool Server::sSocketClose( const uint_t& amount )
{
    UFLAGS_DE( flags );

    if ( amount < 0 || amount >= uintmax_t )
    {
        LOGFMT( flags, "Server::sSocketClose()-> called with m_socket_close overflow: %lu + %lu", m_socket_close, amount );
        return false;
    }

    m_socket_close = amount;

    return true;
}

/**
 * @brief Set the amount of subordinate SocketClient and SocketServer objects that have been opened on a NAMS Server object.
 * @param[in] amount The amount that Server::m_socket_open should be set to.
 * @retval false Returned if amount is outside the boundaries of a uint_t variable.
 * @retval true Returned if amount is within the boundaries of a uint_t variable.
 */
const bool Server::sSocketOpen( const uint_t& amount )
{
    UFLAGS_DE( flags );

    if ( amount < 0 || amount >= uintmax_t )
    {
        LOGFMT( flags, "Server::sSocketOpen()-> called with m_socket_open overflow: %lu + %lu", m_socket_open, amount );
        return false;
    }

    m_socket_open = amount;

    return true;
}
/**@}*/

/** @name Internal */ /**@{*/
/**
 * @brief Constructor for the Server class.
 */
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

/**
 * @brief Destructor for the Server class.
 */
Server::~Server()
{
    return;
}
/**@}*/
