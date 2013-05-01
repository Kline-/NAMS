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
 * @file command.h
 * @brief The Command class.
 *
 *  This file contains the Command class and template functions.
 */
#ifndef DEC_COMMAND_H
#define DEC_COMMAND_H

using namespace std;

/**
 * @brief Functions that are built and loaded independently as Plugin objects.
 */
class Command {
    public:
        /** @name Core */ /**@{*/
        const bool Authorized( const uint_t& sec ) const;
        const void Delete();
        const bool New( const string& file );
        const void Run( Character* character = NULL, const string& cmd = "", const string& arg = "" ) const;
        const void Run( SocketClient* client = NULL, const string& cmd = "", const string& arg = "" ) const;
        /**@}*/

        /** @name Query */ /**@{*/
        void* gCaller() const;
        const string gFile() const;
        const string gName() const;
        const bool gPreempt() const;
        /**@}*/

        /** @name Manipulate */ /**@{*/
        /**@}*/

        /** @name Internal */ /**@{*/
        Command();
        ~Command();
        /**@}*/

    private:
        Plugin* m_plg; /**< Pointer to the associated Plugin. */
        PluginDelete* m_plg_delete; /**< Pointer to the PluginDelete object within the associated Plugin. */
        string m_plg_file; /**< Filename of the Plugin object for reloading later. */
        void* m_plg_handle; /**< Pointer to the file handle of the associated Plugin. */
        PluginNew* m_plg_new; /**< Pointer to the PluginNew object within the associated Plugin. */
        bool m_preempt; /**< If true, the Command will process ahead of any other queued commands from a Socket. */
        uint_t m_security; /**< Security level required to execute the Command. */
};

#endif
