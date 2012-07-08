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
 * @file limits.h
 * @brief Custom defined numeric types and system limits.
 *
 *  NAMS utilizes two numeric types, both defined within this file. These
 *  types, #sint_t (signed) and #uint_t (unsigned) are implemented as the
 *  numeric maximums of the host operating system. Further OS-specific
 *  limitations may be placed here in the future.
 */
#ifndef DEC_LIMITS_H
#define DEC_LIMITS_H

#include <limits>

#if __WORDSIZE == 64
    /** @name Signed */ /**@{*/
    /**
     * @typedef sint_t
     * @brief A signed integer value based upon system limits.
     */
    typedef signed long int sint_t;

    /**
     * @def sintmax_t
     * @brief The maximum value of any #sint_t variable.
     */
    #define sintmax_t numeric_limits<signed long int>::max()

    /**
     * @def sintmin_t
     * @brief The minimum value of any #sint_t variable.
     */
    #define sintmin_t numeric_limits<signed long int>::min()
    /**@}*/

    /** @name Unsigned */ /**@{*/
    /**
     * @typedef uint_t
     * @brief An unsigned integer value based upon system limits.
     */
    typedef unsigned long int uint_t;

    /**
     * @def uintmax_t
     * @brief The maximum value of any #uint_t variable.
     */
    #define uintmax_t numeric_limits<unsigned long int>::max()

    /**
     * @def uintmin_t
     * @brief The minimum value of any #uint_t variable.
     */
    #define uintmin_t numeric_limits<unsigned long int>::min()
    /**@}*/
#else
    /** @name Signed */ /**@{*/
    /**
     * @typedef sint_t
     * @brief A signed integer value based upon system limits.
     */
    typedef signed int sint_t;

    /**
     * @def sintmax_t
     * @brief The maximum value of any #sint_t variable.
     */
    #define sintmax_t numeric_limits<signed int>::max()

    /**
     * @def sintmin_t
     * @brief The minimum value of any #sint_t variable.
     */
    #define sintmin_t numeric_limits<signed int>::min()
    /**@}*/

    /** @name Unsigned */ /**@{*/
    /**
     * @typedef uint_t
     * @brief An unsigned integer value based upon system limits.
     */
    typedef unsigned int uint_t;

    /**
     * @def uintmax_t
     * @brief The maximum value of any #uint_t variable.
     */
    #define uintmax_t numeric_limits<unsigned int>::max()

    /**
     * @def uintmin_t
     * @brief The minimum value of any #uint_t variable.
     */
    #define uintmin_t numeric_limits<unsigned int>::min()
    /**@}*/
#endif

#endif
