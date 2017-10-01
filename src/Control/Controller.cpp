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
 * @file Controller.cpp
 *
 * Implementation for the Controller base class.
 */
#include "../types.hpp"
#include "Controller.hpp"
#include <map>
#include <list>
#include <algorithm>
#include <cassert>
using namespace lifespace;
using namespace std;

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;




void Controller::useControl( unsigned int id, real force )
{
  // loop though all connected actors
  for( list<ActorEntry>::iterator i = actors.begin() ;
       i != actors.end() ; i++ ) {
    
    // does this actor have a control map associated?
    if( i->controlMap ) {
      
      // find the mapping for this control id
      ControlMap::const_iterator j = i->controlMap->find( id );
      
      // is this control id mapped?
      if( j != i->controlMap->end() ) {
        // map and send
        i->actor->useControl( j->second.controlId,
                              j->second.sensitivity * force );
      } else {
        // unmapped, suppress the signal
      }
      
    } else {
      
      // send directly
      i->actor->useControl( id, force );
      
    }
    
  }
}




void Controller::addActor( shared_ptr<Actor> actor, ControlMap * controlMap )
{
  // add it
  actors.push_front( ActorEntry( actor, controlMap ) );
}


/**
 * Removes an actor. It is an error to remove a non-existing actor.
 */
void Controller::removeActor( shared_ptr<Actor> actor )
{
  // find the actor (last instance if multiple instances exist)
  list<ActorEntry>::iterator i =
    find( actors.begin(), actors.end(), actor );
  
  // make sure that the actor did exist here
  assert( i != actors.end() );
  
  // remove it
  actors.erase( i );
}
