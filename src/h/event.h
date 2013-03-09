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
 * @file event.h
 * @brief The Event class.
 *
 *  This file contains the Event class and template functions.
 */
#ifndef DEC_EVENT_H
#define DEC_EVENT_H

#include "command.h"
#include "server.h"

using namespace std;

/**
 * @brief Actions that execute after a specified delay.
 */
class Event {
    public:
        /** @name Core */ /**@{*/
        const bool New( const string& args, Server* server, const uint_t& type, const uint_t& time );
        const void Run();
        const bool Update();
        /**@}*/

        /** @name Query */ /**@{*/
        const uint_t gTime() const;
        /**@}*/

        /** @name Manipulate */ /**@{*/
        /**@}*/

        /** @name Internal */ /**@{*/
        Event();
        ~Event();
        /**@}*/

    private:
        string m_args;   /**< Any arguments to be passed to the function. */
        Server* m_server; /**< Server to execute the event on. */
        uint_t m_time;  /**< Amount of time to wait before executing. Based on #CFG_GAM_PULSE_RATE. */
        uint_t m_type; /**< The type of function to be called. */
};

#endif
