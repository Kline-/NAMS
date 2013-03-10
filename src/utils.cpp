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
 * @file utils.cpp
 * @brief All non-template member functions of the Utils namespace.
 *
 * The Utils namespace contains all general purpose, multi-use, and non-class
 * functions. Classes that are implemented to extend default operators or
 * designed to be used in a utility sense, rather than actual objects, are also
 * contained within this namespace.
 */
#include "h/includes.h"
#include "h/class.h"

#include "h/utils.h"

/* Core */
/**
* @brief Returns a string consisting of directory/file.ext.
* @param[in] directory The top level directory build the path from.
* @param[in] file The file to build the path from.
* @param[in] ext Optionally replaces the file extension with this.
* @retval string A string consisting of directory/file.ext.
*/
const string Utils::DirPath( const string& directory, const string& file, const string& ext )
{
    string path( directory );

    path.append( "/" );

    if ( !ext.empty() )
    {
        path.append( file.substr( 0, file.find_last_of( "." ) + 1 ) );
        path.append( ext );
    }
    else
        path.append( file );

    return path;
}

/**
 * @brief Returns the number of a specific character in a given string.
 * @param[in] input A string value to search.
 * @param[in] item The character to search for within input.
 * @retval uint_t The total count of item within input.
 */
const uint_t Utils::NumChar( const string& input, const string& item )
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

/**
 * @brief Returns a vector of strings split at linebreaks based on input.
 * @param[in] input A string to split on newline characters.
 * @retval vector<string> A vector of strings that were split on the linebreaks detected from input.
 */
const vector<string> Utils::StrNewlines( const string& input )
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
    {
        // Strip the newline off the end
        line.resize( line.length() - 1 );
        if ( !line.empty() )
            output.push_back( line );
    }

    return output;
}

/**
 * @brief Returns a given time as a string.
 * @param[in] now A time_t to be formatted into a string.
 * @retval string A string value containing the human readable form of the contents of now.
 */
const string Utils::StrTime( const time_t& now )
{
    UFLAGS_DE( flags );
    string output;

    if ( ( output = ::ctime( &now ) ).empty() )
    {
        LOGSTR( flags, "Utils::CurrentTime()->ctime()-> returned NULL" );
        return output;
    }

    // Strip the newline off the end
    output.resize( output.length() - 1 );

    return output;
}

/**
 * @brief Returns a vector of strings split at spaces based on input.
 * @param[in] input A string to split on space characters.
 * @param[in] quiet If true, don't output error messages on fail.
 * @retval vector<string> A vector of strings that were split on the spaces detected from input.
 */
const vector<string> Utils::StrTokens( const string& input, const bool& quiet )
{
    UFLAGS_DE( flags );

    if ( input.empty() )
    {
        if ( !quiet )
            LOGSTR( flags, "Utils::StrTokens()-> called with empty input" );
        return vector<string>();
    }

    stringstream ss( input );
    istream_iterator<string> si( ss );
    istream_iterator<string> end;
    vector<string> output( si, end );

    return output;
}

/* Query */
/**
 * @brief Determines if a file path is a directory or file on disk.
 * @param[in] dir A string containing the file path to be checked.
 * @retval false Returned if the file path received in dir is not of type directory or an error occurs during stat.
 * @retval true Returned if the file path received in dir is of type directory.
 */
const bool Utils::iDirectory( const string& dir )
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

/**
 * @brief Determines if a file path is a file or directory on disk.
 * @param[in] file A string containing the file path to be checked.
 * @retval false Returned if the file path received in file is not of type file or an error occurs during stat.
 * @retval true Returned if the file path receievd in file is of type file.
 */
const bool Utils::iFile( const string& file )
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

/**
 * @brief Determines if a string is only a string of numerical values.
 * @param[in] input A string to check for numerical values.
 * @retval false Returned if input is empty or the input contains non-numerical values.
 * @retval true Returned if input contains only numerical values.
 */
const bool Utils::iNumber( const string& input )
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

/**
 * @brief Determines if a file is readable.
 * @param[in] file A string containing the file path to be checked.
 * @retval false Returned if file does not exist or is not readable.
 * @retval true Returned if file exists and is readable.
 */
const bool Utils::iReadable( const string& file )
{
    UFLAGS_DE( flags );
    ifstream ifile;
    bool ret = false;

    ifile.open( file, ifstream::in );

    if ( ifile.fail() )
        ret = false;
    else
        ret = true;

    ifile.close();

    return ret;
}

/* Manipulate */
/**
 * @brief Returns the first space separated argument from input and then erases it from input.
 * @param[in] input A space separated string of items.
 * @retval string A string containing the first argument from input.
 */
const string Utils::Argument( string& input )
{
    UFLAGS_DE( flags );
    string output;
    uint_t pos;

    if ( input.empty() )
    {
        LOGSTR( flags, "Utils::Argument() called with empty input" );
        return output;
    }

    pos = input.find_first_of( " " );
    output = input.substr( 0, pos );
    input.erase( 0, pos + 1 );

    return output;
}

/**
 * @brief Unlinks any temp files leftover from a failed write.
 * @param[in,out] dir_close A #uint_t to store the total directory opened count.
 * @param[in,out] dir_open A #uint_t to store the total directory closed count.
 * @retval void
 */
const void Utils::CleanupTemp( uint_t& dir_close, uint_t& dir_open )
{
    UFLAGS_DE( flags );
    multimap<bool,string> files;
    MITER( multimap, bool,string, mi );

    ListDirectory( CFG_DAT_DIR_VAR, false, files, dir_close, dir_open );

    if ( files.empty() )
        return;

    for ( mi = files.begin(); mi != files.end(); mi++ )
        if ( mi->first == UTILS_IS_FILE )
            if ( ::unlink( CSTR( DirPath( CFG_DAT_DIR_VAR, mi->second ) ) ) < 0 )
                LOGERRNO( flags, "Utils::CleanupTemp()->unlink()->" );

    return;
}

/**
 * @brief Begins a write to a temporary file that is intended to replace a live file.
 * @param[in] ofs The ofstream to open for writing.
 * @param[in] file The filename to replace.
 * @retval false Returned if there is an error replacing file.
 * @retval true Returned if file is successfully replaced.
 */
const bool Utils::FileOpen( ofstream& ofs, const string& file )
{
    UFLAGS_DE( flags );

    if ( ofs.is_open() )
    {
        LOGSTR( flags, "Utils::FileOpen()-> called with open ofs" );
        return false;
    }

    ofs.open( CSTR( DirPath( CFG_DAT_DIR_VAR, file ) ) );

    if ( ofs.fail() )
    {
        LOGSTR( flags, "Utils::FileOpen()-> error opening ofs" );
        return false;
    }

    return true;
}

/**
 * @brief Begins a read from a file.
 * @param[in] ifs The ifstream to open for reading.
 * @param[in] dir The directory the file resides in.
 * @param[in] file The filename to read from.
 * @param[in] quiet If true, don't output error messages on fail.
 * @retval false Returned if there is an error reading the file.
 * @retval true Returned if file is successfully read.
 */
const bool Utils::FileOpen( ifstream& ifs, const string& dir, const string& file, const bool& quiet )
{
    UFLAGS_DE( flags );

    if ( ifs.is_open() )
    {
        if ( !quiet )
            LOGSTR( flags, "Utils::FileOpen()-> called with open ifs" );
        return false;
    }

    ifs.open( CSTR( DirPath( dir, file ) ) );

    if ( ifs.fail() )
    {
        if ( !quiet )
            LOGSTR( flags, "Utils::FileOpen()-> error opening ifs" );
        return false;
    }

    return true;
}

/**
 * @brief Completes a read from a file.
 * @param[in] ifs The ifstream to close.
 * @param[in] quiet If true, don't output error messages on fail.
 * @retval false Returned if there is an error closing the file.
 * @retval true Returned if file is successfully closed.
 */
const bool Utils::FileClose( ifstream& ifs, const bool& quiet )
{
    UFLAGS_DE( flags );

    if ( !ifs.is_open() )
    {
        if ( !quiet )
            LOGSTR( flags, "Utils::FileClose()-> called with closed ifs" );
        return false;
    }

    ifs.clear();
    ifs.close();

    if ( ifs.fail() )
    {
        if ( !quiet )
            LOGSTR( flags, "Utils::FileClose()-> error closing ifs" );
        return false;
    }

    return true;
}

/**
 * @brief Completes a write from a file.
 * @param[in] ofs The ofstream to close.
 * @retval false Returned if there is an error closing the file.
 * @retval true Returned if file is successfully closed.
 */
const bool Utils::FileClose( ofstream& ofs )
{
    UFLAGS_DE( flags );

    if ( !ofs.is_open() )
    {
        LOGSTR( flags, "Utils::FileClose()-> called with closed ofs" );
        return false;
    }

    ofs.clear();
    ofs.close();

    if ( ofs.fail() )
    {
        LOGSTR( flags, "Utils::FileClose()-> error closing ofs" );
        return false;
    }

    return true;
}

/**
 * @brief Unlinks file and replaces it with the temp copy after a successful write.
 * @param[in] ofs The ofstream to close upon completion.
 * @param[in] dir The directory the file resides in.
 * @param[in] file The filename to replace.
 * @retval false Returned if there is an error replacing file.
 * @retval true Returned if file is successfully replaced.
 */
const bool Utils::FileClose( ofstream& ofs, const string& dir, const string& file )
{
    UFLAGS_DE( flags );
    string oldfi, newfi;

    if ( !ofs.is_open() )
    {
        LOGSTR( flags, "Utils::FileClose()-> called with closed ofs" );
        return false;
    }

    ofs.clear();
    ofs.close();

    if ( ofs.fail() )
    {
        LOGSTR( flags, "Utils::FileClose()-> error closing ofs" );
        return false;
    }

    if ( dir.empty() )
    {
        LOGSTR( flags, "Utils::FileClose()-> called with empty dir" );
        return false;
    }

    if ( file.empty() )
    {
        LOGSTR( flags, "Utils::FileClose()-> called with empty file" );
        return false;
    }

    // Ensure the copy to move from temp exists and we have permissions before unlinking the live file
    newfi = DirPath( CFG_DAT_DIR_VAR, file );
    if ( ::access( CSTR( newfi ), R_OK | W_OK ) < 0 )
    {
        LOGERRNO( flags, "Utils::FileClose()->access()->" );
        return false;
    }

    // Remove the live copy
    oldfi = DirPath( dir, file );
    if ( ::unlink( CSTR( oldfi ) ) < 0 )
    {
        LOGERRNO( flags, "Utils::FileClose()->unlink()->" );
        return false;
    }

    // Move the new copy over
    if ( ::rename( CSTR( newfi ), CSTR( oldfi ) ) < 0 )
    {
        LOGERRNO( flags, "Utils::FileClose()->rename()->" );
        return false;
    }

    return true;
}

/**
 * @brief Return a multimap of a specified directory tree on disk.
 * @param[in] dir The filesystem path to search.
 * @param[in] recursive If true, the function will continue to recursively list folders multi-layers deep rather than top level only.
 * @param[in,out] output A multimap<bool,string> consisting of a boolean value denoting either #UTILS_IS_DIRECTORY or #UTILS_IS_FILE. If recursive, this will be updated on each pass.
 * @param[in,out] dir_close A #uint_t to store the total directory opened count.
 * @param[in,out] dir_open A #uint_t to store the total directory closed count.
 * @retval multimap<bool,string> A multimap<bool,string> consisting of a boolean value denoting either #UTILS_IS_DIRECTORY or #UTILS_IS_FILE. If recursive, this will be updated on each pass.
 */
const multimap<bool,string> Utils::ListDirectory( const string& dir, const bool& recursive, multimap<bool,string>& output, uint_t& dir_close, uint_t& dir_open )
{
    UFLAGS_DE( flags );
    DIR* directory = NULL;
    dirent* entry = NULL;
    string ifile, idir;

    if ( dir.empty() )
    {
        LOGSTR( flags, "Utils::OpenDirectory()-> called with empty dir" );
        return output;
    }

    if ( ( directory = ::opendir( CSTR( dir ) ) ) == NULL )
    {
        LOGFMT( flags, "Utils::OpenDirectory()->opendir()-> returned NULL for dir: %s", CSTR( dir ) );
        return output;
    }

    dir_open++;
    idir = dir;

    // Ensure a trailing slash is present to properly recurse
    if ( idir.compare( dir.length() - 1, 1, "/" ) != 0 )
        idir.append( "/" );

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

/* Internal */
/**
 * @brief This is a nested wrapper for Utils::__FormatString and should not be called directly.
 * @param[in] narg A #uint_t variable of the total number of arguments passed. Handled automatically.
 * @param[in] flags Any number of flags from #UTILS_OPTS to control output formatting and options.
 * @param[in] caller A string value containing the calling function. Handled automatically.
 * @param[in] fmt A string value containing a printf-style format string.
 * @param[in] ... A variable arguments list to populate fmt with.
 * @retval string A printf-style formatted string.
 */
const string Utils::_FormatString( const uint_t& narg, const bitset<CFG_MEM_MAX_BITSET>& flags, const string& caller, const string& fmt, ... )
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

/**
 * @brief This is the printf-style string formatter. It should not be invoked directly, but rather by using Utils::FormatString() to ensure proper argument count and caller passing.
 * @param[in] narg A #uint_t variable of the total number of arguments passed. Handled automatically.
 * @param[in] flags Any number of flags from #UTILS_OPTS to control output formatting and options.
 * @param[in] caller A string value containing the calling function. Handled automatically.
 * @param[in] fmt A string value containing a printf-style format string.
 * @param[in] val A variable arguments list to populate fmt with.
 * @retval string A printf-style formatted string.
 */
const string Utils::__FormatString( const uint_t& narg, const bitset<CFG_MEM_MAX_BITSET>& flags, const string& caller, const string& fmt, va_list& val ) // Thanks go to Darien @ MudBytes.net for the start of this
{
    UFLAGS_DE( uflags );
    va_list args;
    vector<string> arguments;
    ITER( vector, string, si );
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

/**
 * @brief This is the logging output engine. It should not be invoked directly, but rather by calling Utils::Logger() to ensure proper argument count and caller passing.
 * @param[in] narg A #uint_t variable of the total number of arguments passed. Handled automatically.
 * @param[in] flags Any number of flags from #UTILS_OPTS to control output formatting and options.
 * @param[in] caller A string value containing the calling function. Handled automatically.
 * @param[in] fmt A string value containing a printf-style format string.
 * @param[in] ... A variable arguments list to populate fmt with.
 * @retval string A printf-style formatted string.
 */
const void Utils::_Logger( const uint_t& narg, const bitset<CFG_MEM_MAX_BITSET>& flags, const string& caller, const string& fmt, ... )
{
    UFLAGS_DE( uflags );
    va_list args;
    string pre, post, output;
    uint_t i = 0;
    chrono::high_resolution_clock::time_point tt;

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
    tt = chrono::high_resolution_clock::now();
    pre = StrTime( chrono::high_resolution_clock::to_time_t( tt ) );
    pre.append( " :: " );

    for ( i = 0; i < MAX_UTILS; i++ )
    {
        if ( flags.test( i ) )
        {
            switch( i )
            {
                case UTILS_DEBUG: //output caller
                    post.append( " [" );
                    post.append( caller );
                    post.append( "]" );
                break;

                case UTILS_RAW: //no extraneous data applied
                    pre.clear();
                    post.clear();
                    i = MAX_UTILS;
                break;

                case UTILS_TYPE_ERROR: //so fancy!
                    pre.append( CFG_STR_UTILS_ERROR );
                break;

                case UTILS_TYPE_INFO: //so fancy!
                    pre.append( CFG_STR_UTILS_INFO );
                break;

                case UTILS_TYPE_SOCKET: //so fancy!
                    pre.append( CFG_STR_UTILS_SOCKET );
                break;

                default:
                break;
            }
        }
    }

    clog << pre << output << post << endl;
    /** @todo Add monitor channel support */

    return;
}
