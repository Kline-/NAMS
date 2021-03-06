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
 * @file thing.h
 * @brief The Thing class.
 *
 *  This file contains the Thing class and template functions.
 */
#ifndef DEC_THING_H
#define DEC_THING_H

using namespace std;

/**
 * @brief A generic "thing": creature, character, room, object, etc.
 */
class Thing
{
    public:
        /** @name Core */ /**@{*/
        const bool AddThing( Thing* thing );
        virtual const void Delete() = 0;
        virtual const void Interpret( const uint_t& security, const string& cmd, const string& args ) = 0;
        const bool Move( Thing* source, Thing* destination, Exit* exit = NULL );
        const bool RemoveThing( Thing* thing );
        virtual const void Send( const string& msg, Thing* speaker = NULL, Thing* target = NULL ) const;
        virtual const bool Serialize() const = 0;
        virtual const bool Unserialize() = 0;
        /**@}*/

        /** @name Query */ /**@{*/
        Brain* gBrain() const;
        Thing* gContainer() const;
        const vector<Thing*> gContents() const;
        const string gDescription( const uint_t& type ) const;
        const string gId() const;
        const string gLocation() const;
        const string gName() const;
        const uint_t gType() const;
        const string gZone() const;
        /**@}*/

        /** @name Manipulate */ /**@{*/
        const bool sBrain( Brain* brain );
        const bool sDescription( const string& description, const uint_t& type );
        const bool sId( const string& id );
        const bool sLocation( const string& location );
        const bool sName( const string& name, const bool& system = false );
        const bool sType( const uint_t& type );
        const bool sZone( const string& zone );
        /**@}*/

        /** @name Internal */ /**@{*/
        const void NewId( const uint_t& seed );
        Thing();
        virtual ~Thing();
        /**@}*/

    private:
        Brain* m_brain; /**< The associated Brain. */
        Thing* m_container; /**< The Thing that this Thing is stored within. */
        vector<Thing*> m_contents; /**< Other Things that are contained within this Thing. */
        string m_description[MAX_THING_DESCRIPTION]; /**< What is displayed to other Things. */
        string m_id; /**< An identifier to denote ownership. For characters, id = account.name */
        string m_location; /**< The location id of where this Thing is located. */
        string m_name; /**< The name of the thing. */
        uint_t m_type; /**< The inherited sub-type of Thing. */
        string m_zone; /**< Part of a larger zone / group of locations? Name, if so. */
};

#endif
