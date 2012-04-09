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
#ifndef DEC_UTILS_H
#define DEC_UTILS_H

#include <bitset>
#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <sstream>
#include <dirent.h>
#include <sys/stat.h>

namespace Utils {
    // Core
    template <class T, class U> inline string DirPath( const T& path, const U& file )
    {
        string output = path;

        // Ensure a trailing slash is present to properly recurse
        if ( output.compare( output.length() - 1, 1, "/" ) != 0 )
            output += "/";

        // Now append the first letter of the file recieved as a subdir
        output += file[0]; output += "/";

        // Finally add the filename
        output += file;

        return output;
    }
    template <class T> inline string DelSpaces( const T& t ) { string output( t ); output.erase( remove_if( output.begin(), output.end(), ::isspace ), output.end() ); return output; }
    template <class T> inline string Lower( const T& t ) { stringstream ss; ss << nouppercase << t; return ss.str(); }
    template <class T> inline string Upper( const T& t ) { stringstream ss; ss << uppercase << t; return ss.str(); }
    template <class T> inline string String( const T& t ) { stringstream ss( t ); return ss.str(); }
    timeval CurrentTime();
    uint_t DiffTime( const timeval& prev, const timeval& current, const uint_t& granularity );
    string _FormatString( const uint_t& narg, const bitset<CFG_MEM_MAX_BITSET>& flags, const string& caller, const string& fmt, ... );
    string __FormatString( const uint_t& narg, const bitset<CFG_MEM_MAX_BITSET>& flags, const string& caller, const string& fmt, va_list& val );
    #define FormatString( flags, fmt, ... ) _FormatString( PP_NARG( __VA_ARGS__ ), flags, _caller_, fmt, ##__VA_ARGS__ )
    void _Logger( const uint_t& narg, const bitset<CFG_MEM_MAX_BITSET>& flags, const string& caller, const string& fmt, ... );
    #define Logger( flags, fmt, ... ) _Logger( PP_NARG( __VA_ARGS__ ), flags, _caller_, fmt, ##__VA_ARGS__ )
    uint_t NumChar( const string& input, const string& item );
    vector<string> StrNewlines( const string& input );
    string StrTime( const timeval& now );
    vector<string> StrTokens( const string& input );

    // Query
    bool iDirectory( const string& dir );
    bool iFile( const string& file );
    bool iNumber( const string& input );

    // Manipulate
    class DeleteObject
    {
        public:
            template <class T> inline void operator() ( const T* ptr ) const { delete ptr; }
    };
    template <class K, class V, class I> inline bool KeyValue( K& key, V& val, const I& item )
    {
        size_t loc = 0;

        if ( ( loc = item.find( "=" ) ) == string::npos )
            return false;

        key = item.substr( 0, loc - 1 );
        val = item.substr( loc + 1, item.length() );
        loc = key.find_last_not_of( " " );
        key.erase( loc + 1 );
        loc = val.find_first_not_of( " " );
        val.erase( 0, loc );

        return true;
    }
    template <class K, class V> inline void KeySet( const bool& igncase, bool& found, const K& keyd, const V& valu, const string& item, string& loc )
    {
        string key( keyd );
        string val( valu );

        if ( igncase )
        {
            transform( key.begin(), key.end(), key.begin(), ::tolower );
            transform( val.begin(), val.end(), val.begin(), ::tolower );
        }

        if( key.compare( val ) == 0 )
        {
            loc = item;
            found = true;

            return;
        }

        return;
    }
    template <class K, class V, class I, class L> inline void KeySet( const bool& igncase, bool& found, const K& keyd, const V& valu, const I& item, L& loc )
    {
        string key( keyd );
        string val( valu );

        if ( igncase )
        {
            transform( key.begin(), key.end(), key.begin(), ::tolower );
            transform( val.begin(), val.end(), val.begin(), ::tolower );
        }

        if( key.compare( val ) == 0 )
        {
            string tf( item );

            // Allow bools be any of: true / 1 or false / 0
            transform( tf.begin(), tf.end(), tf.begin(), ::tolower );

            if ( tf.compare( "true" ) == 0 )
                loc = true;
            else if ( tf.compare( "false" ) == 0 )
                loc = false;
            else
                stringstream( item ) >> loc;
            found = true;

            return;
        }

        return;
    }
    multimap<bool,string> ListDirectory( const string& dir, const bool& recursive, multimap<bool,string>& output, uint_t& dir_close, uint_t& dir_open );
};

#endif
