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

// Core
string Utils::CurrentTime()
{
    UFLAGS_DE( flags );
    timeval now;
    string output;

    if ( !CurrentTime( now ) )
    {
        LOGSTR( flags, "Utils::CurrentTime()->Utils::CurrentTime()-> returned false" );
        return output;
    }

    if ( ( output = ::ctime( &now.tv_sec ) ).empty() )
    {
        LOGSTR( flags, "Utils::CurrentTime()->ctime()-> returned NULL" );
        return output;
    }

    // Strip the newline off the end
    output.resize( output.length() - 1 );

    return output;
}

bool Utils::CurrentTime( timeval& now )
{
    UFLAGS_DE( flags );

    if ( ::gettimeofday( &now, NULL ) < 0 )
    {
        LOGERRNO( flags, "Utils::CurrentTime()->" );
        return false;
    }

    return true;
}

uint_t Utils::DiffTime( const timeval& prev, const timeval& current, const uint_t& granularity )
{
    switch ( granularity )
    {
        case  UTILS_TIME_S: return ( prev.tv_sec - current.tv_sec );
        case UTILS_TIME_MS: return ( prev.tv_usec - current.tv_usec ) / 1000;
        case UTILS_TIME_US:
                   default: return ( prev.tv_usec - current.tv_usec );
    }
}

string Utils::_FormatString( const uint_t& narg, const bitset<CFG_MEM_MAX_BITSET>& flags, const string& caller, const string& fmt, ... )
{
    UFLAGS_DE( uflags );
    va_list args;
    string output;

    if ( fmt.empty() )
    {
        LOGSTR( uflags, "Utils::_FormatString()-> called with empty fmt" );
        return output;
    }

    va_start( args, fmt );
    output = __FormatString( narg, flags, caller, fmt, args );
    va_end( args );

    return output;
}

string Utils::__FormatString( const uint_t& narg, const bitset<CFG_MEM_MAX_BITSET>& flags, const string& caller, const string& fmt, va_list& val ) // Thanks go to Darien @ MudBytes.net for the start of this
{
    UFLAGS_DE( uflags );
    va_list args;
    vector<string> arguments;
    vector<string>::iterator si;
    vector<char> buf;
    string output, token;
    sint_t size = 0;

    if ( fmt.empty() )
    {
        LOGSTR( uflags, "Utils::__FormatString()-> called with empty fmt" );
        return output;
    }

    arguments = StrTokens( fmt );
    for ( si = arguments.begin(); si != arguments.end(); si++ )
    {
        token = *si;
        if ( token.find( "%" ) != string::npos ) // not foolproof, but it should catch some worst cases by attempting
            size++;                              // to ensure a matching narg : format specifier count
    }

    if ( narg != 1 && narg != static_cast<uint_t>( size ) && narg != NumChar( fmt, "%" ) ) // if narg == 1 invocation was func( flags, string )
    {
        bitset<CFG_MEM_MAX_BITSET> eflags;

        eflags.set( UTILS_TYPE_ERROR );
        Logger( eflags, "Number of arguments (%lu) did not match number of format specifiers (%lu) at: %s", narg, size, CSTR( caller ) );
        return output = "";
    }

    va_copy( args, val );
    size = vsnprintf( NULL, 0, CSTR( fmt ), args );
    va_end( args );

    va_copy( args, val );
    buf.resize( size + 1 );
    vsnprintf( &buf[0], ( size + 1 ), CSTR( fmt ), args );
    va_end( args );

    return output = &buf[0];
}

void Utils::_Logger( const uint_t& narg, const bitset<CFG_MEM_MAX_BITSET>& flags, const string& caller, const string& fmt, ... )
{
    UFLAGS_DE( uflags );
    va_list args;
    string pre, post, output;
    uint_t i = 0;

    if ( fmt.empty() )
    {
        LOGSTR( uflags, "Utils::_Logger()-> called with empty fmt" );
        return;
    }

    va_start( args, fmt );
    output = __FormatString( narg, flags, caller, fmt, args );
    va_end( args );

    if ( output.empty() )
        return;

    // prepend timestamp
    pre = CurrentTime(); pre += " :: ";

    for ( i = 0; i < MAX_UTILS; i++ )
    {
        if ( flags.test( i ) )
        {
            switch( i )
            {
                case UTILS_DEBUG:       post += " ["; post += caller; post += "]"; break; // output caller
                case UTILS_IGNORE_CASE: break; // noting for now
                case UTILS_RAW:         pre.clear(); post.clear(); i = MAX_UTILS;  break; //no extraneous data applied
                case UTILS_TYPE_ERROR:  pre += CFG_STR_UTILS_ERROR;                break; // so fancy!
                case UTILS_TYPE_INFO:   pre += CFG_STR_UTILS_INFO;                 break;
                case UTILS_TYPE_SOCKET: pre += CFG_STR_UTILS_SOCKET;               break;
                default: break;
            }
        }
    }

    clog << pre << output << post << endl;

//fixme    if ( !server.shutdown )
//fixme        monitor_chan( output.c_str(), MONITOR_LOG );

    return;
}

uint_t Utils::NumChar( const string& input, const string& item )
{
    UFLAGS_DE( flags );
    uint_t amount = 0, i = 0;

    if ( input.empty() )
    {
        LOGSTR( flags, "Utils::NumChar()-> called with empty input" );
        return amount;
    }

    for ( i = 0; i < input.length(); i++ )
        if ( input[i] == item[0] )
            amount++;

    return amount;
}

vector<string> Utils::StrNewlines( const string& input )
{
    UFLAGS_DE( flags );

    if ( input.empty() )
    {
        LOGSTR( flags, "Utils::StrNewlines()-> called with empty input" );
        return vector<string>();
    }

    stringstream ss( input );
    string line;
    vector<string> output;

    while ( getline( ss, line ) )
        output.push_back( line );

    return output;
}

vector<string> Utils::StrTokens( const string& input )
{
    UFLAGS_DE( flags );

    if ( input.empty() )
    {
        LOGSTR( flags, "Utils::StrTokens()-> called with empty input" );
        return vector<string>();
    }

    stringstream ss( input );
    istream_iterator<string> si( ss );
    istream_iterator<string> end;
    vector<string> output( si, end );

    return output;
}

// Query
bool Utils::iDirectory( const string& dir )
{
    UFLAGS_DE( flags );
    struct stat dir_info;

    if ( ::stat( CSTR( dir ), &dir_info ) < 0 )
    {
        LOGERRNO( flags, "Utils::iDirectory()->stat()->" );
        return false;
    }

    if ( !S_ISDIR( dir_info.st_mode ) )
        return false;

    return true;
}

bool Utils::iFile( const string& file )
{
    UFLAGS_DE( flags );
    struct stat dir_info;

    if ( ::stat( CSTR( file ), &dir_info ) < 0 )
    {
        LOGERRNO( flags, "Utils::iFile()->stat()->" );
        return false;
    }

    if ( !S_ISREG( dir_info.st_mode ) )
        return false;

    return true;
}

bool Utils::iNumber( const string& input )
{
    UFLAGS_DE( flags );
    uint_t i = 0;

    if ( input.empty() )
    {
        LOGSTR( flags, "Utils::iNumber()-> called with empty input" );
        return false;
    }

    for ( i = 0; i < input.length(); i++ )
        if ( !isdigit( input[i] ) )
            return false;

    return true;
}

// Manipulate
multimap<bool,string> Utils::ListDirectory( const string& dir, const bool& recursive, multimap<bool,string>& output, uint_t& dir_close, uint_t& dir_open )
{
    UFLAGS_DE( flags );
    DIR* directory = NULL;
    dirent* entry = NULL;
    string ifile, idir;

    if ( ( directory = ::opendir( CSTR( dir ) ) ) == NULL )
    {
        LOGFMT( flags, "Utils::OpenDirectory()->opendir()-> returned NULL for dir: %s", CSTR( dir ) );
        return output;
    }

    dir_open++;
    idir = dir;

    // Ensure a trailing slash is present to properly recurse
    if ( idir.compare( dir.length() - 1, 1, "/" ) != 0 )
        idir += "/";

    while ( ( entry = ::readdir( directory ) ) != NULL )
    {
        ifile = entry->d_name;

        // Skip over the unwanteds
        if ( ifile.compare( "." ) == 0 || ifile.compare( ".." ) == 0 )
            continue;

        if ( iDirectory( idir + ifile ) )
            output.insert( pair<bool,string>( UTILS_IS_DIRECTORY, ifile ) );
        else
            output.insert( pair<bool,string>( UTILS_IS_FILE, ifile ) );

        // Only recurse if another directory is found, otherwise a file was found, so skip it
        if ( iDirectory( idir + ifile ) && recursive )
            ListDirectory( idir + ifile, recursive, output, dir_close, dir_open );
    }

    if ( ::closedir( directory ) < 0 )
        LOGERRNO( flags, "Utils::OpenDir()->closedir()->" );
    else
        dir_close++;

    return output;
}
