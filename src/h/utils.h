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

namespace Utils {
    class DeleteObject
    {
        public:
            template <class T> inline void operator() (const T* ptr) const { delete ptr; };
    };
    template <class T> inline string toLower( const T& t )  { stringstream ss; ss << nouppercase << t; return ss.str(); }
    template <class T> inline string toString( const T& t ) { stringstream ss; ss << t; return ss.str(); }
    template <class T> inline string toUpper( const T& t )  { stringstream ss; ss << uppercase << t; return ss.str(); }

    string CurrentTime();
    string _FormatString( const uint_t narg, const bitset<CFG_MEM_MAX_BITSET> flags, const string caller, const string fmt, ... );
    string __FormatString( const uint_t narg, const bitset<CFG_MEM_MAX_BITSET> flags, const string caller, const string fmt, va_list val );
    #define FormatString( flags, fmt, ... ) _FormatString( PP_NARG( __VA_ARGS__ ), flags, _caller_, fmt, ##__VA_ARGS__ )
    void _Logger( const uint_t narg, const bitset<CFG_MEM_MAX_BITSET> flags, const string caller, const string fmt, ... );
    #define Logger( flags, fmt, ... ) _Logger( PP_NARG( __VA_ARGS__ ), flags, _caller_, fmt, ##__VA_ARGS__ )
    bool iNumber( const string input );
    uint_t NumChar( const string input, const string item );
    vector<string> StrNewlines( const string input );
    vector<string> StrTokens( const string input );
};

#endif
