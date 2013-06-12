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
 * @brief All non-template member functions of the Server namespace.
 *
 * The Server namespace handles all interfacing with the host OS and is the
 * central core of NAMS.
 *
 * During its boot sequence, the Server will spawn a SocketServer to listen for
 * incoming SocketClient connections, compile and load any Plugin objects,
 * and then indefinitely loop and process game logic until it receives a shutdown
 * notification.
 */
#include "h/includes.h"
#include "h/server.h"

#include "h/account.h"
#include "h/character.h"
#include "h/command.h"
#include "h/event.h"
#include "h/exit.h"
#include "h/list.h"
#include "h/location.h"
#include "h/object.h"
#include "h/socketclient.h"
#include "h/socketserver.h"

/* Core */
/**
 * @brief Unload runtime configuration from memory.
 * @retval void
 */
const void Server::Config::Delete()
{
    delete this;

    return;
}

/**
 * @brief Unload global variables from memory.
 * @retval void
 */
const void Server::Global::Delete()
{
    delete this;

    return;
}

/**
 * @brief Unload runtime statistics from memory.
 * @retval void
 */
const void Server::Stats::Delete()
{
    delete this;

    return;
}

/**
 * @brief Sends a message to all clients connected to the Server.
 * @param[in] msg The message to be sent.
 * @retval void
 */
const void Server::Broadcast( const string& msg )
{
    SocketClient *client = NULL;
    ITER( list, SocketClient*, si );

    for ( si = socket_client_list.begin(); si != socket_client_list.end(); si = g_global->m_next_socket_client )
    {
        client = *si;
        g_global->m_next_socket_client = ++si;

        client->Send( msg );
        client->Send();
    }

    return;
}

/**
 * @brief Compile a Plugin file.
 * @param[in] file The file to be compiled. The file extension should end in #CFG_PLG_BUILD_EXT_IN.
 * @param[in] force Force a rebuild even if the output file already exists.
 * @retval false Returned if a fault is experienced trying to build the plugin.
 * @retval true Returned if the Plugin builds successfully.
 */
const bool Server::BuildPlugin( const string& file, const bool& force )
{
    UFLAGS_DE( flags );
    FILE* popen_fil = NULL;
    string build_cmd, build_res;
    char buf[CFG_STR_MAX_BUFLEN] = {'\0'};

    // No need to run if the output file already exists and a rebuild wasn't forced
    if ( Utils::iReadable( Utils::DirPath( CFG_DAT_DIR_OBJ, file, CFG_PLG_BUILD_EXT_OUT ) ) && !force )
        return true;

    build_cmd = CFG_PLG_BUILD_CMD " -o ";
    build_cmd.append( Utils::DirPath( CFG_DAT_DIR_OBJ, file, CFG_PLG_BUILD_EXT_OUT ) );
    build_cmd.append( " " );
    build_cmd.append( Utils::DirPath( CFG_DAT_DIR_COMMAND, file ) );
    build_cmd.append( " " CFG_PLG_BUILD_OPT );

    // Pipe the build_cmd to the host for processing
    if ( ( popen_fil = popen( CSTR( build_cmd ), "r" ) ) != NULL )
    {
        while( fgets( buf, CFG_STR_MAX_BUFLEN, popen_fil ) != NULL )
            build_res.append( buf );

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
 * @brief Links Exit pointers together after all locations are loaded.
 * @retval void
 */
const void Server::LinkExits()
{
    UFLAGS_E( flags );
    chrono::high_resolution_clock::time_point start, finish;
    double duration = uintmin_t;
    ITER( list, Location*, li );
    ITER( list, Exit*, ei );
    list<Exit*> loc_exits;
    Location* destination = NULL;
    Location* location = NULL;
    Exit* exit = NULL;
    uint_t count = 0;

    start = chrono::high_resolution_clock::now();
    LOGSTR( 0, CFG_STR_FILE_EXIT_READ );

    for ( li = location_list.begin(); li != location_list.end(); li++ )
    {
        location = *li;
        loc_exits = location->gExits();

        for ( ei = loc_exits.begin(); ei != loc_exits.end(); ei++ )
        {
            exit = *ei;

            if ( ( destination = Handler::FindLocation( exit->gDestId(), HANDLER_FIND_ID ) ) == NULL )
            {
                LOGFMT( flags, "Server::LinkExits()-> location %s has invalid exit to %s", CSTR( location->gId() ), CSTR( exit->gDestId() ) );
                location->RemoveExit( exit );
            }
            else
            {
                count++;
                exit->sDestination( destination );
            }
        }
    }

    finish = chrono::high_resolution_clock::now();
    if ( ( duration = chrono::duration_cast<chrono::milliseconds>( finish - start ).count() ) > 1000 )
        LOGFMT( 0, "Linked %lu exits in %1.2fs.", count, ( duration / 1000 ) );
    else
        LOGFMT( 0, "Linked %lu exits in %1.0fms.", count, duration );

    return;
}

/**
 * @brief Search all subfolders of #CFG_DAT_DIR_COMMAND and call Command::New() to load each file found to memory.
 * @retval false Returned if a fault is experienced trying to obtain a directory listing to process.
 * @retval true Returned if 0 or more Command objects are loaded from disk.
 */
const bool Server::LoadCommands()
{
    UFLAGS_DE( flags );
    chrono::high_resolution_clock::time_point start, finish;
    double duration = uintmin_t;
    Command* cmd = NULL;
    multimap<bool,string> files;
    MITER( multimap, bool,string, mi );

    start = chrono::high_resolution_clock::now();
    LOGSTR( 0, CFG_STR_FILE_COMMAND_READ );

    // Populate the multimap with a recursive listing of the commands folder
    Utils::ListDirectory( CFG_DAT_DIR_COMMAND, true, false, files, g_stats->m_dir_close, g_stats->m_dir_open );

    if ( files.empty() )
    {
        LOGSTR( flags, "Server::LoadCommands()->Utils::ListDirectory()-> CFG_DAT_DIR_COMMAND returned NULL" );
        return false;
    }

    for ( mi = files.begin(); mi != files.end(); mi++ )
    {
        if ( mi->first == UTILS_IS_FILE && ( mi->second.substr( mi->second.find_last_of( "." ) + 1 ) == CFG_PLG_BUILD_EXT_IN ) )
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
                cmd->Delete();
            }
        }
    }

    finish = chrono::high_resolution_clock::now();
    if ( ( duration = chrono::duration_cast<chrono::milliseconds>( finish - start ).count() ) > 1000 )
        LOGFMT( 0, "Loaded %lu commands in %1.2fs.", command_list.size(), ( duration / 1000 ) );
    else
        LOGFMT( 0, "Loaded %lu commands in %1.0fms.", command_list.size(), duration );

    return true;
}

/**
 * @brief Search all subfolders of #CFG_DAT_DIR_WORLD and call Location::New() to load each file found to memory.
 * @retval false Returned if a fault is experienced trying to obtain a directory listing to process.
 * @retval true Returned if 0 or more Location objects are loaded from disk.
 */
const bool Server::LoadLocations()
{
    UFLAGS_DE( flags );
    chrono::high_resolution_clock::time_point start, finish;
    double duration = uintmin_t;
    Location* loc = NULL;
    multimap<bool,string> files;
    MITER( multimap, bool,string, mi );

    start = chrono::high_resolution_clock::now();
    LOGSTR( 0, CFG_STR_FILE_LOCATION_READ );

    // Populate the multimap with a recursive listing of the locations folder
    Utils::ListDirectory( CFG_DAT_DIR_WORLD, true, true, files, g_stats->m_dir_close, g_stats->m_dir_open );

    if ( files.empty() )
    {
        LOGSTR( flags, "Server::LoadLocations()->Utils::ListDirectory()-> CFG_DAT_DIR_WORLD returned NULL" );
        return false;
    }

    for ( mi = files.begin(); mi != files.end(); mi++ )
    {
        if ( mi->first == UTILS_IS_FILE && ( mi->second.substr( mi->second.find_last_of( "." ) + 1 ) == CFG_DAT_FILE_LOC_EXT ) )
        {
            loc = new Location();
            if ( !loc->New( mi->second ) )
            {
                LOGFMT( flags, "Server::LoadLocations()->Location::New()-> location %s returned false", CSTR( mi->second ) );
                loc->Delete();
            }
        }
    }

    finish = chrono::high_resolution_clock::now();
    if ( ( duration = chrono::duration_cast<chrono::milliseconds>( finish - start ).count() ) > 1000 )
        LOGFMT( 0, "Loaded %lu locations in %1.2fs.", location_list.size(), ( duration / 1000 ) );
    else
        LOGFMT( 0, "Loaded %lu locations in %1.0fms.", location_list.size(), duration );

    return true;
}

/**
 * @brief Search all subfolders of #CFG_DAT_DIR_WORLD and call Object::New() to load each file found to memory.
 * @retval false Returned if a fault is experienced trying to obtain a directory listing to process.
 * @retval true Returned if 0 or more Object objects are loaded from disk.
 */
const bool Server::LoadObjects()
{
    UFLAGS_DE( flags );
    chrono::high_resolution_clock::time_point start, finish;
    double duration = uintmin_t;
    Object* obj = NULL;
    multimap<bool,string> files;
    MITER( multimap, bool,string, mi );

    start = chrono::high_resolution_clock::now();
    LOGSTR( 0, CFG_STR_FILE_OBJECT_READ );

    // Populate the multimap with a recursive listing of the objectss folder
    Utils::ListDirectory( CFG_DAT_DIR_WORLD, true, true, files, g_stats->m_dir_close, g_stats->m_dir_open );

    if ( files.empty() )
    {
        LOGSTR( flags, "Server::LoadObjects()->Utils::ListDirectory()-> CFG_DAT_DIR_WORLD returned NULL" );
        return false;
    }

    for ( mi = files.begin(); mi != files.end(); mi++ )
    {
        if ( mi->first == UTILS_IS_FILE && ( mi->second.substr( mi->second.find_last_of( "." ) + 1 ) == CFG_DAT_FILE_OBJ_EXT ) )
        {
            obj = new Object();
            if ( !obj->New( mi->second ) )
            {
                LOGFMT( flags, "Server::LoadObjects()->Object::New()-> object %s returned false", CSTR( mi->second ) );
                obj->Delete();
            }
        }
    }

    finish = chrono::high_resolution_clock::now();
    if ( ( duration = chrono::duration_cast<chrono::milliseconds>( finish - start ).count() ) > 1000 )
        LOGFMT( 0, "Loaded %lu objects in %1.2fs.", object_list.size(), ( duration / 1000 ) );
    else
        LOGFMT( 0, "Loaded %lu objects in %1.0fms.", object_list.size(), duration );

    return true;
}

/**
 * @brief Poll all SocketClient objects that are communicating with the associated SocketServer object.
 * @retval false Returned if the file descriptor of the SocketServer is invalid and no further processing can take place.
 * @retval true Returned if the file descriptor of the SocketServer is valid and all SocketClient objects were polled successfully.
 */
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

    if ( ( server_desc = g_global->m_listen->gDescriptor() ) < 1 )
    {
        LOGFMT( flags, "Server::PollSockets()->SocketServer::gDescriptor()-> returned invalid descriptor: %ld", server_desc );
        return false;
    }

    FD_SET( server_desc, &in_set );
    max_desc = server_desc;

    // Build three file descriptor lists to be polled
    for ( si = socket_client_list.begin(); si != socket_client_list.end(); si = g_global->m_next_socket_client )
    {
        socket_client = *si;
        g_global->m_next_socket_client = ++si;

        if ( ( client_desc = socket_client->gDescriptor() ) < 1 )
        {
            LOGFMT( flags, "Server::PollSockets()->SocketClient::gDescriptor()-> returned invalid descriptor: %ld", client_desc );
            socket_client->Delete();
            continue;
        }

        if ( socket_client->Quitting() )
        {
            socket_client->Send();
            socket_client->Delete();
            continue;
        }

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
        g_global->m_listen->Accept();

    // Process faulted connections
    for ( si = socket_client_list.begin(); si != socket_client_list.end(); si = g_global->m_next_socket_client )
    {
        socket_client = *si;
        g_global->m_next_socket_client = ++si;

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
    for ( si = socket_client_list.begin(); si != socket_client_list.end(); si = g_global->m_next_socket_client )
    {
        socket_client = *si;
        g_global->m_next_socket_client = ++si;

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

                // Detach the Character as linkdead
                if ( socket_client->gState() >= SOC_STATE_PLAYING )
                {
                    if ( socket_client->gAccount() != NULL )
                    {
                        if ( socket_client->gAccount()->gCharacter() != NULL )
                        {
                            Event* event = new Event();

                            socket_client->sState( SOC_STATE_DISC_LINKDEAD );
                            socket_client->gAccount()->gCharacter()->sAccount( NULL );
                            event->New( "quit", "quit", socket_client->gAccount()->gCharacter(), Handler::FindCommand( "quit" ), EVENT_TYPE_CMD_CHARACTER, CFG_CHR_MAX_IDLE );
                            socket_client->gAccount()->sCharacter( NULL );
                        }
                    }
                }

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
    for ( si = socket_client_list.begin(); si != socket_client_list.end(); si = g_global->m_next_socket_client )
    {
        socket_client = *si;
        g_global->m_next_socket_client = ++si;

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

/**
 * @brief Process any scheduled events tied to the Server.
 * @retval void
 */
const void Server::ProcessEvents()
{
    ITER( forward_list, Event*, ei );
    Event* event;

    for ( ei = event_list.begin(); ei != event_list.end(); ei = g_global->m_next_event )
    {
        event = *ei;
        g_global->m_next_event = ++ei;

        if ( !event->Update() )
            event->Run();
    }

    return;
}

/**
 * @brief Processes input from all active SocketClient objects.
 * @retval void
 */
const void Server::ProcessInput()
{
    UFLAGS_DE( flags );
    ITER( list, SocketClient*, si );
    SocketClient* socket_client;
    sint_t client_desc = 0;

    for ( si = socket_client_list.begin(); si != socket_client_list.end(); si = g_global->m_next_socket_client )
    {
        socket_client = *si;
        g_global->m_next_socket_client = ++si;

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

    return;
}

/**
 * @brief Recovers the server state and re-connects client sockets after a reboot.
 * @param[in] reboot True if the server was started via a reboot. Must be true for this to run.
 * @retval void
 * @todo Clean this up.
 */
const void Server::RebootRecovery( const bool& reboot )
{
    ifstream recovery;
    string key, value, line, silent;
    SocketClient *client = NULL;
    Account *account = NULL;
    Character *character = NULL;

    if ( reboot )
    {
        recovery.open( CFG_DAT_DIR_VAR "/" CFG_DAT_FILE_REBOOT, ifstream::in );

        while ( recovery.is_open() && recovery.good() && getline( recovery, line ) )
        {
            if ( !Utils::KeyValue( key, value, line) )
            {
                cout << "Error reading line " << line << endl;
                continue;
            }

            if ( key == "slnt" )
                silent = value;
            else if ( key == "desc" )
            {
                client = new SocketClient();
                client->New( atoi( CSTR( value ) ), true );
            }
            else if ( key == "port" )
                client->sPort( atoi( CSTR( value ) ) );
            else if ( key == "host" )
                client->sHostname( value );
            else if ( key == "recv" )
                client->aBytesRecvd( atoi( CSTR( value ) ) );
            else if ( key == "sent" )
                client->aBytesSent( atoi( CSTR( value ) ) );
            else if ( key == "idle" )
                client->sIdle( atoi( CSTR( value ) ) );
            else if ( key == "stat" )
                client->sState( atoi( CSTR( value ) ) );
            else if ( key == "acct" )
                client->sLogin( SOC_LOGIN_NAME, value );
            else if ( key == "pasw" )
            {
                account = new Account();
                client->sLogin( SOC_LOGIN_PASSWORD, value );
                account->New( client, true );
            }
            else if ( key == "char" )
            {
                character = new Character();
                client->gAccount()->sCharacter( character );
                character->sAccount( client->gAccount() );
                character->New( client->gAccount()->gId() + "." + value + "." + CFG_DAT_FILE_PLR_EXT, true );
                Handler::EnterGame( client, "reboot", silent );
            }
        }

        recovery.close();
        ::unlink( CFG_DAT_DIR_VAR "/" CFG_DAT_FILE_REBOOT );
    }

    return;
}

/**
 * @brief Reloads a Command Plugin.
 * @param[in] name The name of the Command to be reloaded.
 * @retval false Returned if the command is unable to be reloaded.
 * @retval true Returned if the command was successfully reloaded.
 */
const bool Server::ReloadCommand( const string& name )
{
    UFLAGS_DE( flags );
    Command* command = NULL;
    string file;

    command = Handler::FindCommand( name );

    if ( command == NULL )
    {
        LOGFMT( flags, "Server::ReloadCommand()->Server::FindCommand()-> command %s returned NULL", CSTR( name ) );
        return false;
    }

    file = command->gFile();
    command->Delete();
    command = new Command();

    if ( !command->New( file ) )
    {
        LOGFMT( flags, "Server::ReloadCommand()->Command::New()-> command %s returned false", CSTR( file ) );
        command->Delete();
        return false;
    }

    return true;
}

/**
 * @brief Perform a clean shutdown of the NAMS Server providing a chance to complete disk writes and free all memory to verify there are no leaks.
 * @param[in] status The shutdown code to pass to exit() when NAMS exits. Either EXIT_FAILURE or EXIT_SUCCESS.
 * @retval void
 */
const void Server::Shutdown( const sint_t& status )
{
    bool was_running = !g_global->m_shutdown;

    Broadcast( CFG_STR_SHUTDOWN );
    g_global->m_shutdown = true;

    // Write runtime settings
    g_config->Serialize();

    // Cleanup commands
    while ( !command_list.empty() )
        command_list.begin()->second->Delete();
    // Cleanup events
    while ( !event_list.empty() )
        event_list.front()->Delete();
    // Cleanup locations
    while ( !location_list.empty() )
        location_list.front()->Delete();
    // Cleanup objects
    while ( !object_list.empty() )
        object_list.front()->Delete();
    // Cleanup socket clients
    while ( !socket_client_list.empty() )
        socket_client_list.front()->Delete();

    //Cleanup globals
    g_config->Delete();
    g_stats->Delete();
    g_global->Delete();

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
 * @brief Start the NAMS Server. Responsible for calling all critical boot-time functions such as SocketServer initialization, Command loading, etc.
 * @retval void
 */
const void Server::Startup( const sint_t& desc )
{
    UFLAGS_DE( flags );
    SocketServer* socket_server = NULL;
    sint_t descriptor = 0;
    bool reboot = false;
    g_global->m_shutdown = false;

    LOGFMT( 0, "%s started.", CFG_STR_VERSION );

    // Fresh boot, otherwise it would already be assigned during a reboot
    if ( desc == 0 )
    {
        if ( ( descriptor = ::socket( AF_INET6, SOCK_STREAM, 0 ) ) < 0 )
        {
            LOGERRNO( flags, "Server::Startup()->socket()->" );
            Shutdown( EXIT_FAILURE );
        }
    }
    else
    {
        descriptor = desc;
        reboot = true;
    }

    socket_server = new SocketServer();
    g_global->m_listen = socket_server;

    if ( !socket_server->New( descriptor, reboot ) )
    {
        LOGSTR( flags, "Server::Startup()->SocketServer::New()-> returned false" );
        Shutdown( EXIT_FAILURE );
    }

    // Bump ourselves to the root folder for file paths
    if ( !reboot && ::chdir( ".." ) < 0 )
    {
        LOGERRNO( flags, "Server::Startup()->chdir()->" );
        Shutdown( EXIT_FAILURE );
    }

    if ( !g_config->Unserialize() )
    {
        LOGSTR( flags, "Server::Config::Unserialize()-> returned false" );
        Shutdown( EXIT_FAILURE );
    }

    if ( !LoadCommands() )
    {
        LOGSTR( flags, "Server::Startup()->Server::LoadCommands()-> returned false" );
        Shutdown( EXIT_FAILURE );
    }
    if ( !LoadLocations() )
    {
        LOGSTR( flags, "Server::Startup()->Server::LoadLocations()-> returned false" );
        Shutdown( EXIT_FAILURE );
    }
    if ( !LoadObjects() )
    {
        LOGSTR( flags, "Server::Startup()->Server::LoadObjects()-> returned false" );
        Shutdown( EXIT_FAILURE );
    }

    LinkExits();
    RebootRecovery( reboot );

    // Cleanup any leftovers from a hard crash mid-write
    Utils::CleanupTemp( g_stats->m_dir_close, g_stats->m_dir_open );

    LOGFMT( 0, "%s is ready on port %lu.", CFG_STR_VERSION, g_global->m_port );
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

    g_global->m_time_current = chrono::high_resolution_clock::now();

    // Poll all sockets for changes
    if ( !PollSockets() )
    {
        LOGSTR( flags, "Server::Update()->Server::PollSockets()-> returned false" );
        Shutdown( EXIT_FAILURE );
    }

    // Process any input received
    ProcessInput();

    // Process any scheduled events
    ProcessEvents();

    // Sleep to control game pacing
    ::usleep( USLEEP_MAX / CFG_GAM_PULSE_RATE );

    return;
}

/* Query */
/**
 * @brief Returns a copy of the disabled commands list.
 * @retval vector<string> A copy of the disabled commands list.
 */
vector<string> Server::Config::gDisabledCommands() const
{
    return m_disabled_commands;
}

/**
 * @brief Returns a a copy of the prohibited names list using type from #SVR_CFG_PROHIBITED_NAMES.
 * @param[in] type The specific prohibited names list to retrieve.
 * @retval forward_list<string> A copy of the prohibited names list referenced by type.
 */
forward_list<string> Server::Config::gProhibitedNames( const uint_t& type ) const
{
    UFLAGS_DE( flags );

    if ( type < uintmin_t || type >= MAX_SVR_CFG_PROHIBITED_NAMES )
    {
        LOGFMT( flags, "Server::Config::gProhibitedNames()-> called with invalid type %lu", type );
        return forward_list<string>();
    }

    return m_prohibited_names[type];
}

/**
 * @brief Returns the combined number of SocketClient and SocketServer objects that have been destroyed.
 * @retval uint_t The total number of closed sockets that were tied to this object.
 */
const uint_t Server::Stats::gSocketClose() const
{
    return m_socket_close;
}

/**
 * @brief Returns the combined number of SocketClient and SocketServer objects that have been created.
 * @retval uint_t The total number of opened sockets that are tied to this object.
 */
const uint_t Server::Stats::gSocketOpen() const
{
    return m_socket_open;
}

/**
 * @brief Gets the hostname of the machine that NAMS is running on.
 * @retval string A string is returned containing either "(unknown)" or the machine hostname.
 * @todo Move this to Server::sHostname() and redo this func as a simple getter. Don't need to query the host server multiple times.
 */
const string Server::gHostname()
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
 * @brief Display miscellaneous data about the NAMS Server, such as total data transfered, objects in memory, etc.
 * @retval string A string is returned containing a pre-formatted data display of all Server information.
 * @todo Write this entire function.
 */
const string Server::gStatus()
{
    string output;

    return output;
}

/* Manipulate */
/**
 * @brief Serialize runtime configuration settings and write them to #CFG_DAT_FILE_SETTINGS.
 * @retval false Returned if there was an error serializing settings.
 * @retval true Returned if settings were serialized successfully.
 */
const bool Server::Config::Serialize()
{
    UFLAGS_DE( flags );
    ofstream ofs;
    string value;
    stringstream line;
    CITER( forward_list, string, li );
    CITER( vector, string, vi );
    uint_t i = uintmin_t;

    LOGSTR( 0, CFG_STR_FILE_SETTINGS_WRITE );
    Utils::FileOpen( ofs, CFG_DAT_FILE_SETTINGS );

    if ( !ofs.good() )
    {
        LOGFMT( flags, "Server::Config::Serialize()-> failed to open settings file: %s", CFG_DAT_FILE_SETTINGS );
        return false;
    }

    KEYLIST( ofs, "disabled_commands" );
    {
        line.str( "" );

        if ( !m_disabled_commands.empty() )
        {
            for ( vi = m_disabled_commands.begin(); vi != m_disabled_commands.end(); vi++ )
                line << *vi << " ";

            value = line.str();
            value.erase( value.end() - 1 );
            ofs << value << endl;
        }
        else
            ofs << endl;
    }
    KEYLISTLOOP( ofs, "prohibited_names", i ); /** @todo Need to find a nicer way to do this */
    {
        for ( i = 0; i < MAX_SVR_CFG_PROHIBITED_NAMES; i++ )
        {
            ofs << "prohibited_names[" << i << "]" << " = ";
            line.str( "" );

            if ( !m_prohibited_names[i].empty() )
            {
                for ( li = m_prohibited_names[i].begin(); li != m_prohibited_names[i].end(); li++ )
                    line << *li << " ";

                value = line.str();
                value.erase( value.end() - 1 );
                ofs << value << endl;
            }
            else
                ofs << endl;
        }
    }

    Utils::FileClose( ofs, CFG_DAT_DIR_ETC, CFG_DAT_FILE_SETTINGS );
    LOGSTR( 0, CFG_STR_FILE_DONE );

    return true;
}

/**
 * @brief Toggles if a command is disabled or not.
 * @param[in] command The name of the command to disable or enable.
 * @retval false Returned if the command was successfully toggled.
 * @retval true Returned if there was an error toggling the command or it doesn't exist.
 */
const bool Server::Config::ToggleDisable( const string& command )
{
    UFLAGS_DE( flags );
    Command* cmd = NULL;
    ITER( vector, string, vi );

    if ( command.empty() )
    {
        LOGSTR( flags, "Server::Config::TogleDisable()-> called with empty command" );
        return false;
    }

    if ( ( cmd = Handler::FindCommand( command ) ) == NULL )
        return false;

    cmd->ToggleDisable();

    if ( ( vi = find( m_disabled_commands.begin(), m_disabled_commands.end(), Utils::Lower( cmd->gName() ) ) ) == m_disabled_commands.end() )
        m_disabled_commands.push_back( Utils::Lower( cmd->gName() ) );
    else
        m_disabled_commands.erase( vi );

    Serialize();

    return true;
}

/**
 * @brief Unserialize runtime configuration settings from #CFG_DAT_FILE_SETTINGS.
 * @retval false Returned if there was an error unserializing settings.
 * @retval true Returned if settings were unserialized successfully.
 */
const bool Server::Config::Unserialize()
{
    UFLAGS_DE( flags );
    ifstream ifs;
    string key, value, line;
    bool found = false;
    vector<string> token;
    ITER( vector, string, ti );
    uint_t i = uintmin_t;
    stringstream loop;

    LOGSTR( 0, CFG_STR_FILE_SETTINGS_READ );
    Utils::FileOpen( ifs, Utils::DirPath( CFG_DAT_DIR_ETC, CFG_DAT_FILE_SETTINGS ) );

    if ( !ifs.good() )
    {
        LOGFMT( flags, "Server::Config::Unserialize()-> failed to open settings file: %s", CFG_DAT_FILE_SETTINGS );
        return false;
    }

    while ( getline( ifs, line ) )
    {
        if ( !Utils::KeyValue( key, value, line) )
        {
            LOGFMT( flags, "Server::Config::Unserialize()-> error reading line: %s", CSTR( line ) );
            continue;
        }

        for ( ;; )
        {
            found = false;

            if ( key == "disabled_commands" )
            {
                found = true;
                token = Utils::StrTokens( value, true );
                for ( ti = token.begin(); ti != token.end(); ti++ )
                    m_disabled_commands.push_back( *ti );
                sort( m_disabled_commands.begin(), m_disabled_commands.end() );
            }
            else if ( Utils::StrPrefix( "prohibited_names", key ) ) /** @todo Need to find a nicer way to do this */
            {
                for ( ; i < MAX_SVR_CFG_PROHIBITED_NAMES; i++ )
                {
                    loop.str( "" );
                    loop << "prohibited_names[" << i << "]";

                    if ( key == loop.str() )
                    {
                        found = true;
                        token = Utils::StrTokens( value, true );
                        for ( ti = token.begin(); ti != token.end(); ti++ )
                            m_prohibited_names[i].push_front( *ti );
                        m_prohibited_names[i].reverse();
                        break;
                    }
                }
            }

            if ( !found )
                LOGFMT( flags, "Server::Config::Unserialize()->Utils::KeySet()-> key not found: %s", CSTR( key ) );

            break;
        }
    }

    Utils::FileClose( ifs );
    LOGSTR( 0, CFG_STR_FILE_DONE );

    return true;
}

/**
 * @brief Set the amount of subordinate SocketClient and SocketServer objects that have been closed on a NAMS Server object.
 * @param[in] amount The amount that Server::m_socket_close should be set to.
 * @retval false Returned if amount is outside the boundaries of a uint_t variable.
 * @retval true Returned if amount is within the boundaries of a uint_t variable.
 */
const bool Server::Stats::sSocketClose( const uint_t& amount )
{
    UFLAGS_DE( flags );

    if ( amount < uintmin_t || amount >= uintmax_t )
    {
        LOGFMT( flags, "Server::Stats::sSocketClose()-> called with m_socket_close overflow: %lu + %lu", m_socket_close, amount );
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
const bool Server::Stats::sSocketOpen( const uint_t& amount )
{
    UFLAGS_DE( flags );

    if ( amount < uintmin_t || amount >= uintmax_t )
    {
        LOGFMT( flags, "Server::Stats::sSocketOpen()-> called with m_socket_open overflow: %lu + %lu", m_socket_open, amount );
        return false;
    }

    m_socket_open = amount;

    return true;
}

/* Internal */
/**
 * @brief Constructor for the Server::Config class.
 */
Server::Config::Config()
{
    uint_t i = uintmin_t;

    m_disabled_commands.clear();
    for ( i = 0; i < MAX_SVR_CFG_PROHIBITED_NAMES; i++ )
        m_prohibited_names[i].clear();

    return;
}

/**
 * @brief Destructor for the Server::Config class.
 */
Server::Config::~Config()
{
    return;
}

/**
 * @brief Constructor for the Server::Global class.
 */
Server::Global::Global()
{
    m_listen = NULL;
    m_next_character = character_list.begin();
    m_next_event = event_list.begin();
    m_next_socket_client = socket_client_list.begin();
    m_port = 0;
    m_shutdown = true;
    m_time_boot = chrono::high_resolution_clock::now();
    m_time_current = chrono::high_resolution_clock::now();

    return;
}

/**
 * @brief Destructor for the Server::Global class.
 */
Server::Global::~Global()
{
    m_listen->Delete();

    return;
}

/**
 * @brief Constructor for the Server::Stats class.
 */
Server::Stats::Stats()
{
    m_dir_close = 0;
    m_dir_open = 0;
    m_socket_close = 0;
    m_socket_open = 0;

    return;
}

/**
 * @brief Destructor for the Server::Stats class.
 */
Server::Stats::~Stats()
{
    return;
}
