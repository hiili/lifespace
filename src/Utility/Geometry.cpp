/*
 * Copyright (C) 2004-2005 Paul J. Wagner
 * This file is part of the Lifespace Simulator.
 * 
 * Lifespace Simulator is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * Lifespace Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with the Lifespace Simulator; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 * For more information about the program:
 *   http://www.cis.hut.fi/pwagner/lifespace/
 */

/**
 * @file Geometry.cpp
 */
#include "../types.hpp"
#include "Geometry.hpp"
#include "Event.hpp"
using namespace lifespace;

#include <utility>
using std::make_pair;




Geometry::Geometry() :
  hostObject( 0 ),
  contacts(),
  events( *this )
{}


Geometry::~Geometry()
{
  events.sendEvent( DestructorEvent() );
}




void Geometry::addContact( const Geometry * other,
                           Contact * contact )
{
  events.sendEvent( AddContactEvent( other, contact ) );
  
  // make sure that no contact between these Geometries exist already
  assert( contacts.find( other ) == contacts.end() );
  
  contacts.insert( make_pair(other, contact) );
}


void Geometry::removeContact( const Geometry * other )
{
  events.sendEvent( RemoveContactEvent( other ) );
  
  contacts_t::iterator i = contacts.find( other );
  
  // assert that the contact existed
  assert( i != contacts.end() );
  
  contacts.erase( i );
}
