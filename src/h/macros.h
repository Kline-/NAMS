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
 * @file macros.h
 * @brief All pre-processor macros within NAMS and select reference values.
 *
 *  This file contains all macros and certain reference values that "should not"
 *  be changed and have hence been omitted from config.h for inclusion here instead.
 */
#ifndef DEC_MACROS_H
#define DEC_MACROS_H

using namespace std;

/**
 * @def PP_NARG
 * @author Laurent Deniau @ http://goo.gl/3jkAm
 * @brief Determine of the number of arguments passed to a function from a __VA_ARGS__ list.
 * @param[in] ... A variable length argument list to determine the count of.
 */
#define PP_NARG(...) \
         PP_NARG_(__VA_ARGS__,PP_RSEQ_N())

/**
 * @def PP_NARG_
 * @author Laurent Deniau @ http://goo.gl/3jkAm
 * @brief Determine of the number of arguments passed to a function from a __VA_ARGS__ list.
 */
#define PP_NARG_(...) \
         PP_ARG_N(__VA_ARGS__)

/**
 * @def PP_ARG_N
 * @author Laurent Deniau @ http://goo.gl/3jkAm
 * @brief Determine of the number of arguments passed to a function from a __VA_ARGS__ list.
 */
#define PP_ARG_N( \
         _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
         _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
         _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
         _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
         _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
         _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
         _61,_62,_63,N,...) N

/**
 * @def PP_RSEQ_N
 * @author Laurent Deniau @ http://goo.gl/3jkAm
 * @brief Determine of the number of arguments passed to a function from a __VA_ARGS__ list.
 */
#define PP_RSEQ_N() \
         63,62,61,60,                   \
         59,58,57,56,55,54,53,52,51,50, \
         49,48,47,46,45,44,43,42,41,40, \
         39,38,37,36,35,34,33,32,31,30, \
         29,28,27,26,25,24,23,22,21,20, \
         19,18,17,16,15,14,13,12,11,10, \
         9,8,7,6,5,4,3,2,1,0

/**
 * @def STR
 * @brief Stringify the calling function's file and line number for debugging.
 * @param[in] x An item to be stringified.
 */
#define STR(x) #x

/**
 * @def SX
 * @brief Stringify the calling function's file and line number for debugging.
 * @param[in] x An item to be stringified.
 */
#define SX(x) STR(x)

/**
 * @def _caller_
 * @brief Stringify the calling function's file and line number for debugging.
 */
#define _caller_ __FILE__ ":" SX(__LINE__)

/**
 * @def BSET
 * @brief Define a bitset variable (name) with values (value) already set.
 * @param[in] name  The name to use for declaring a local variable of bitset<#CFG_MEM_MAX_BITSET>.
 * @param[in] value Values from #UTILS_OPTS to be enabled on name.
 */
#define BSET( name, value ) \
        bitset<CFG_MEM_MAX_BITSET> name; \
        name.set( value )

/**
 * @def CITER
 * @brief Define a const iterator variable (name) of (type) using (container).
 * @param[in] container Type of STL container to create: list, vector, etc.
 * @param[in] type The type of object to hold within container, bool, SocketClient, etc.
 * @param[in] name The name to use for declaring a local variable of container<type>::iterator.
 */
#define CITER( container, type, name ) container<type>::const_iterator name

/**
 * @def CKPASSWORD
 * @brief Compare two the value of (check) to (old) based on (salt).
 * @param[in] salt The salt to pass to crypt().
 * @param[in] check The new value to be crypted for comparison.
 * @param[in] old The existing salted value.
 */
#define CKPASSWORD( salt, check, old ) Utils::String( crypt( CSTR( check ), CSTR( Utils::Salt( salt ) ) ) ) == old

/**
 * @def CRLF
 * @brief Output a carriage return \ line feed.
 */
#define CRLF "\r\n"

/**
 * @def CSTR
 * @brief Output std::string to const char*.
 * @param[in] str A std::string object.
 */
#define CSTR( str ) ( str ).c_str()

/**
 * @def FNAME
 * @brief Get function's name wrapped in a string.
 */
#define FNAME string( __PRETTY_FUNCTION__ )

/**
 * @def ITER
 * @brief Define an iterator variable (name) of (type) using (container).
 * @param[in] container Type of STL container to create: list, vector, etc.
 * @param[in] type The type of object to hold within container, bool, SocketClient, etc.
 * @param[in] name The name to use for declaring a local variable of container<type>::iterator.
 */
#define ITER( container, type, name ) container<type>::iterator name

/**
 * @def KEY
 * @brief Write (name) and (value) to (stream) in key:value format.
 * @param[in] stream The filestream to use for a key:value pair.
 * @param[in] name The name to use for a key:value pair.
 * @param[in] value The value to use for a key:value pair.
 */
#define KEY( stream, name, value ) stream << name " = " << value << endl

/**
 * @def KEYLIST
 * @brief Begins writing (name) to (stream) in key:value format.
 * @param[in] stream The filestream to use for a key:value pair.
 * @param[in] name The name to use for a key:value pair.
 */
#define KEYLIST( stream, name ) stream << name " = "

/**
 * @def KEYLISTLOOP
 * @brief Begins writing (name)[iter] to (stream) in key:value format.
 * @param[in] stream The filestream to use for a key:value pair.
 * @param[in] name The name to use for a key:value pair.
 * @param[in] iter The iterator to track through the loop.
 */
#define KEYLISTLOOP( stream, name, iter )

/**
 * @def LOGSTR
 * @brief Wrap Utils::Logger() for brevity and ease of future maintenance.
 * @param[in] flags A local variable name of type bitset<#CFG_MEM_MAX_BITSET> with #UTILS_OPTS enabled as appropriate. 0 may be used if no options are needed.
 * @param[in] message Any string, char, const char, numeric value, etc. This message will be written to log.
 */
#define LOGSTR( flags, message ) Utils::Logger( flags, message )

/**
 * @def LOGFMT
 * @brief Wrap Utils::FormatString() within Utils::Logger() for brevity and ease of future maintenance.
 * @param[in] flags A local variable name of type bitset<#CFG_MEM_MAX_BITSET> with #UTILS_OPTS enabled as appropriate. 0 may be used if no options are needed.
 * @param[in] message Any string that contains printf style format variables.
 * @param[in] ... The list of arguments to format into message.
 */
#define LOGFMT( flags, message, ... ) LOGSTR( flags, Utils::FormatString( flags, message, __VA_ARGS__ ) )

/**
 * @def LOGERRNO
 * @brief Wrap Utils::Logger() based on a locally generated errno value from system functions.
 * @param[in] flags A local variable name of type bitset<#CFG_MEM_MAX_BITSET> with #UTILS_OPTS enabled as appropriate. 0 may be used if no options are needed.
 * @param[in] message Any string that contains printf style format variables.
 */
#define LOGERRNO( flags, message ) LOGFMT( flags, message " returned errno %d: %s", errno, strerror( errno ) )

/**
 * @def MITER
 * @brief Define an iterator variable (name) of (type1,type2) using (container).
 * @param[in] container Type of STL container to create: map, multimap, etc.
 * @param[in] type1 The first object type in pair (type1,type2) to hold within the container.
 * @param[in] type2 The second object type in pair (type1,type2) to hold within the container.
 * @param[in] name The name to use for declaring a local variable of container<type1,type2>::iterator.
 */
#define MITER( container, type1, type2, name ) container<type1,type2>::iterator name

/**
 * @def UFLAGS_DE
 * @brief Define a bitset variable (name) with #UTILS_DEBUG and #UTILS_TYPE_ERROR already enabled.
 * @param[in] name The name to use for declaring a local variable of bitset<#CFG_MEM_MAX_BITSET>.
 */
#define UFLAGS_DE( name ) \
    BSET( name, UTILS_DEBUG ); \
    name.set( UTILS_TYPE_ERROR )

/**
 * @def UFLAGS_E
 * @brief Define a bitset variable (name) with #UTILS_TYPE_ERROR already enabled.
 * @param[in] name The name to use for declaring a local variable of bitset<#CFG_MEM_MAX_BITSET>.
 */
#define UFLAGS_E( name ) BSET( name, UTILS_TYPE_ERROR )

/**
 * @def UFLAGS_I
 * @brief Define a bitset variable (name) with #UTILS_TYPE_INFO already enabled.
 * @param[in] name The name to use for declaring a local variable of bitset<#CFG_MEM_MAX_BITSET>.
 */
#define UFLAGS_I( name ) BSET( name, UTILS_TYPE_INFO )

/**
 * @def UFLAGS_S
 * @brief Define a bitset variable (name) with #UTILS_TYPE_SOCKET already enabled.
 * @param[in] name The name to use for declaring a local variable of bitset<#CFG_MEM_MAX_BITSET>.
 */
#define UFLAGS_S( name ) BSET( name, UTILS_TYPE_SOCKET )

/**
 * @def USLEEP_MAX
 * @brief This is the maximum value usleep will take per man (3) usleep -- 1 second.
 *
 * Some platforms limit usleep to 999999. This should not be a problem as long as
 * #CFG_GAM_PULSE_RATE is greater than 1. Changing this value will impact the speed
 * at which every action in the game runs. If that is desired then #CFG_GAM_PULSE_RATE
 * should be modified instead.
 */
#define USLEEP_MAX 1000000

#endif
