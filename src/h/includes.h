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
 * @file includes.h
 * @brief All non-system includes that need to be within each .cpp file.
 *
 *  This file is a "headers of headers" in that it only references other
 *  headers within NAMS that should be included to each .cpp file. This
 *  header file should be the first file included within every .cpp file.
 */
#ifndef DEC_INCLUDES_H
#define DEC_INCLUDES_H

#include "sysincludes.h"
#include "limits.h"
#include "macros.h"
#include "enum.h"
#include "config.h"
#include "class.h"
#include "namespace.h"
#include "globals.h"

/**
 * @brief Used as a pointer to the New() function within an implemented class.
 */
typedef Plugin* PluginNew();

/**
 * @brief Used as a pointer to the Delete() function within an implemented class.
 */
typedef void PluginDelete( Plugin* );

#endif
