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
 * @file pincludes.h
 * @brief All non-system includes that need to be within each .cpp plugin.
 *
 *  This file is a "headers of headers" in that it only references other
 *  headers within NAMS that should be included to each .cpp plugin. This
 *  header file should be the first file included within every .cpp plugin.
 */
#ifndef DEC_PINCLUDES_H
#define DEC_PINCLUDES_H

#include "includes.h"
#include "plugin.h"
#include "socketclient.h"

#endif
