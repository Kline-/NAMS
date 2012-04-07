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
#ifndef DEC_MACROS_H
#define DEC_MACROS_H

// Thanks to Laurent Deniau @ https://groups.google.com/d/msg/comp.std.c/d-6Mj5Lko_s/5R6bMWTEbzQJ
#define PP_NARG(...) \
         PP_NARG_(__VA_ARGS__,PP_RSEQ_N())
#define PP_NARG_(...) \
         PP_ARG_N(__VA_ARGS__)
#define PP_ARG_N( \
         _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
         _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
         _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
         _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
         _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
         _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
         _61,_62,_63,N,...) N
#define PP_RSEQ_N() \
         63,62,61,60,                   \
         59,58,57,56,55,54,53,52,51,50, \
         49,48,47,46,45,44,43,42,41,40, \
         39,38,37,36,35,34,33,32,31,30, \
         29,28,27,26,25,24,23,22,21,20, \
         19,18,17,16,15,14,13,12,11,10, \
         9,8,7,6,5,4,3,2,1,0

// Stringify the calling function's file and line number for debugging
#define STR(x) #x
#define SX(x) STR(x)
#define _caller_ __FILE__ ":" SX(__LINE__)

// Output a carriage return \ line feed
#define CRLF "\r\n"

// Output std::string to const char*
#define CSTR( func ) ( func ).c_str()

// Define a bitset variable (name) with values (value) already set
#define BSET( name, value ) \
        bitset<CFG_MEM_MAX_BITSET> name; \
        name.set( value )

// Define an iterator variable (name) of class (type) using (container)
#define ITER( container, type, name ) container<type>::iterator name

// Define a bitset variable (name) with UTILS_DEBUG and UTILS_TYPE_ERROR already set
#define UFLAGS_DE( name ) BSET( name, UTILS_DEBUG | UTILS_TYPE_ERROR )

// Wrap Utils::Logger()
#define LOGSTR( flags, message ) Logger( flags, message )

// Wrap Utils::FormatString() within Utils::Logger()
#define LOGFMT( flags, message, ... ) LOGSTR( flags, FormatString( flags, message, __VA_ARGS__ ) )

// This is the maximum value usleep will take per man (3) usleep -- 1 second.
// Changing this will affect game speed; reference CFG_GAM_PULSE_RATE
#define USLEEP_MAX 1000000

// Total number of characters in the alphabet; used to split queues
// Be warned; changing this will impact the entire directory structure and break many things
#define ALPHA_MAX 26

/*
#define UTIL_DE( name ) \
        bitset<CFG_MEM_MAX_BITSET> name; \
        name.set( UTILS_DEBUG ); \
        name.set( UTILS_TYPE_ERROR );
*/
#endif
