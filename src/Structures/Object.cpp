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
 * @file Object.cpp
 *
 * Implementations for the Object class.
 */
#include "../types.hpp"
#include "Object.hpp"
#include "Connector.hpp"
#include "Locator.hpp"
#include "BasicLocator.hpp"
#include "Subspace.hpp"
#include "ODEWorld.hpp"
#include "../Utility/Event.hpp"
#include "../Graphics/Visual.hpp"
#include "../Utility/Geometry.hpp"
using namespace lifespace;

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include <string>
using std::string;

#include <cassert>
#include <GL/gl.h>




void Object::setHostSpace( Subspace * newHostSpace )
{
  // prevent inserting an object to multiple subspaces simultaneously,
  // and disallow disconnecting an object multiple times.
  assert( hostSpace == 0 ^ newHostSpace == 0 );
  
  // disallow disconnecting if locked to host space with lockToHostSpace()
  assert( !isLockedToHostSpace() );
  
  // send an event
  ObjectEvent event = { OE_HOSTSPACE_CHANGING, { this, {0} } };
  event.data.changingTarget.hostSpace = newHostSpace;
  events.sendEvent( &event );
  
  // set the hostspace
  hostSpace = newHostSpace;
}




Object::Params::Params( Locator * locator_,
                        Visual * visual_,
                        Geometry * geometry_ ) :
  locator( shared_ptr<Locator>(locator_) ),
  visual( shared_ptr<Visual>(visual_) ),
  geometry( shared_ptr<Geometry>(geometry_) )
{}


Object::Params::Params( shared_ptr<Locator> locator_,
                        shared_ptr<Visual> visual_,
                        shared_ptr<Geometry> geometry_ ) :
  locator( locator_ ),
  visual( visual_ ),
  geometry( geometry_ )
{}


Object::Params::Params() :
  locator( shared_ptr<Locator>() ),
  visual( shared_ptr<Visual>() ),
  geometry( shared_ptr<Geometry>() )
{}


Object::Object( const Object::Params & params ) :
  locator( params.locator ),
  visual( params.visual ),
  geometry( params.geometry ),
  hostSpace( 0 ), lockedToHostSpace( false ),
  name( "(unnamed)" )
{
  if( locator ) {
    assert( !locator->getHostObject() );
    locator->setHostObject( this );
  }
  if( geometry ) {
    assert( !geometry->getHostObject() );
    geometry->setHostObject( this );
  }
}


/**
 * - Joints will be destroyed automatically in their destructors.
 * - ODELocators will be destroyed automatically in their destructors
 *   \em with \em limitations! Check ODELocator::~ODELocator() for
 *   details.
 */
Object::~Object()
{
  // send an OE_OBJECT_DYING event thru the EventHost
  ObjectEvent event = { OE_OBJECT_DYING, { this, {0} } };
  events.sendEvent( &event );
  
  // detach from host subspace and deactivate if active. Note that this is
  // currently checked from the status of isLockedToHostSpace()!
  if( hostSpace ) {
    if( isLockedToHostSpace() ) {
      ODEWorld::Activate( this, 0 );
    }
    hostSpace->removeObject( shared_from_this() );
  }
}




const World * Object::getHostWorld() const
{
  if( hostSpace ) return hostSpace->getHostWorld(); else return 0;
}

World * Object::getHostWorld()
{
  if( hostSpace ) return hostSpace->getHostWorld(); else return 0;
}


shared_ptr<const Locator>
Object::getSubspaceLocator( const Subspace * subspace ) const
{
  if( !locator || !hostSpace ) return shared_ptr<Locator>();
  
  // compute directly from world locators if they are available, except if the
  // target subspace is directly above (in which case nothing needs to be done:
  // the recursion will end immediately)
  if( hostSpace != subspace &&
      locator->getDirectWorldLocator() &&
      subspace->getLocator() &&
      subspace->getLocator()->getDirectWorldLocator() ) {
    // compute from world locators
    shared_ptr<Locator> result
      ( new BasicLocator( *locator->getDirectWorldLocator() ) );
    subspace->getLocator()->
      getDirectWorldLocator()->transform( *result, Reverse );
    return result;
  } else {
    // recurse
    shared_ptr<Locator> result( new BasicLocator( *locator ) );
    return
      hostSpace->transformToSubspaceCoordinates( subspace, *result ) ?
      result : shared_ptr<Locator>();
  }
}


shared_ptr<const Locator> Object::getWorldLocator() const
{
  if( !locator || !hostSpace ) return shared_ptr<Locator>();
  
  shared_ptr<const Locator> directLocator( locator->getDirectWorldLocator() );
  if( directLocator ) {
    return directLocator;
  } else {
    shared_ptr<Locator> result( new BasicLocator( *locator ) );
    return
      hostSpace->transformToWorldCoordinates( *result ) ?
      result : shared_ptr<Locator>();
  }
}




void Object::setName( const string & newName )
{
  assert_user( newName.find_first_of( ".:/" ) == string::npos,
               "Object names may not contain dots, colons nor slashes!" );
  
  name = newName;
}


string Object::getFullName() const
{
  string result;
  
  // within a subspace?
  if( getHostSpace() ) {
    // return the host subspace's full name and append our own name to it
    result = getHostSpace()->getFullName() + "/" + name;
  } else {
    // not within a subspace, just return our own name
    result = name;
  }
  
  return result;
}




void Object::setLocator( shared_ptr<Locator> newLocator )
{
  // send an event
  ObjectEvent event = { OE_LOCATOR_CHANGING, { this, {0} } };
  event.data.changingTarget.locator = newLocator.get();
  events.sendEvent( &event );
  
  // check and set the hostObject pointers
  if( locator ) locator->setHostObject( 0 );
  if( newLocator ) {
    // assert that the new locator is not a part of another object
    assert( !newLocator->getHostObject() );
    newLocator->setHostObject( this );
  }
  
  // set the new locator
  locator = newLocator;
}


void Object::setVisual( boost::shared_ptr<Visual> newVisual )
{
  // send an event
  ObjectEvent event = { OE_VISUAL_CHANGING, { this, {0} } };
  event.data.changingTarget.visual = newVisual.get();
  events.sendEvent( &event );
  
  // set the new visual
  visual = newVisual;
}


void Object::setGeometry( boost::shared_ptr<Geometry> newGeometry )
{
  // send an event
  ObjectEvent event = { OE_GEOMETRY_CHANGING, { this, {0} } };
  event.data.changingTarget.geometry = newGeometry.get();
  events.sendEvent( &event );
  
  // check and set the hostObject pointers
  if( geometry ) geometry->setHostObject( 0 );
  if( newGeometry ) {
    // assert that the new geometry is not a part of another object
    assert( !newGeometry->getHostObject() );
    newGeometry->setHostObject( this );
  }
  
  // set the new geometry
  geometry = newGeometry;
}




shared_ptr<const Connector> Object::getConnector( unsigned int id ) const
{
  connectors_t::const_iterator i = connectors.find( id );
  assert_user( i != connectors.end(),
               "No connector with the specified id was found "
               "from the Object!" );
  return i->second;
}


shared_ptr<Connector> Object::getConnector( unsigned int id )
{
  connectors_t::iterator i = connectors.find( id );
  assert_user( i != connectors.end(),
               "No connector with the specified id was found "
               "from the Object!" );
  return i->second;
}




/**
 * Call point for the prepare pass. If a Locator is attached, then calls its
 * prepare(). Also propagates the call to all connectors.
 */
void Object::prepare( real dt )
{
  if( locator ) locator->prepare( dt );
  
  for( connectors_t::iterator i = connectors.begin() ;
       i != connectors.end() ; i++ ) {
    i->second->prepare( dt );
  }
}


/**
 * Call point for the step pass. If a Locator is attached, then calls its
 * step(). Also propagates the call to all connectors.
 */
void Object::step()
{
  if( locator ) locator->step();
  
  for( connectors_t::iterator i = connectors.begin() ;
       i != connectors.end() ; i++ ) {
    i->second->step();
  }
}
