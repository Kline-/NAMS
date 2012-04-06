#include "h/globals.h"
#include "h/utils.h"

string Utils::_FormatString( const uint_t narg, const bitset<CFG_MEM_MAX_BITSET> flags, const string caller, const string fmt, ... )
{
    va_list args;
    string output;

    va_start( args, fmt );
    output = __FormatString( narg, flags, caller, fmt, args );
    va_end( args );

    return output;
}

string Utils::__FormatString( const uint_t narg, const bitset<CFG_MEM_MAX_BITSET> flags, const string caller, const string fmt, va_list val ) // Thanks go to Darien @ MudBytes.net for the start of this
{
    va_list args;
    vector<string> arguments;
    vector<string>::iterator si;
    vector<char> buf;
    string output, token;
    sint_t size = 0;

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

bool Utils::isNumber( const string input )
{
    uint_t i;

    if ( input.empty() )
        return false;

    for ( i = 0; i < input.length(); i++ )
        if ( !isdigit( input[i] ) )
            return false;

    return true;
}

void Utils::_Logger( const uint_t narg, const bitset<CFG_MEM_MAX_BITSET> flags, const string caller, const string fmt, ... )
{
    va_list args;
    string output, pre, post;
    uint_t i = 0;

    va_start( args, fmt );
    output = __FormatString( narg, flags, caller, fmt, args );
    va_end( args );

    if ( output.empty() )
        return;

    // prepend timestamp
    pre += server.gTimeCurrent(); pre += " :: ";

    for ( i = 0; i < MAX_UTILS; i++ )
    {
        if ( flags.test( i ) )
        {
            switch( i )
            {
                case UTILS_DEBUG:       post += " ["; post += caller; post += "]"; break; // output caller
                case UTILS_IGNORE_CASE: break; // noting for now
                case UTILS_RAW:         pre.clear(); post.clear(); i = MAX_UTILS;  break; //no extraneous data applied
                case UTILS_TYPE_ERROR:  pre += "ERROR: ";                          break; // so fancy!
                default: break;
            }
        }
    }

    clog << pre << output << post << endl;

//fixme    if ( !server.shutdown )
//fixme        monitor_chan( output.c_str(), MONITOR_LOG );

    return;
}

uint_t Utils::NumChar( const string input, const string item )
{
    uint_t amount = 0, i = 0;

    for ( i = 0; i < input.length(); i++ )
        if ( input[i] == item[0] )
            amount++;

    return amount;
}

vector<string> Utils::StrTokens( const string input )
{
    stringstream ss( input );
    istream_iterator<string> si( ss );
    istream_iterator<string> end;
    vector<string> output( si, end );

    return output;
}
