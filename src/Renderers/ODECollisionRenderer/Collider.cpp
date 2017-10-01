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
 * @file Collider.cpp
 */


#include "../../types.hpp"
#include "Collider.hpp"
#include "ObjectNode.hpp"
#include "ODECollisionRenderer.hpp"
#include "../../Structures/ODEWorld.hpp"
#include "../../Structures/ODELocator.hpp"
#include "../../Structures/Connector.hpp"
#include "../../Utility/Geometry.hpp"
#include "../../Utility/CollisionMaterial.hpp"
#include "../../Utility/Contact.hpp"
using namespace lifespace;

#include <ode/ode.h>
#include <ode/odecpp.h>
#include <ode/odecpp_collision.h>

#include <boost/scoped_array.hpp>
#include <boost/shared_ptr.hpp>
using boost::scoped_array;
using boost::shared_ptr;
using boost::dynamic_pointer_cast;

#include <iostream>
using std::cout;
using std::endl;

#include <algorithm>
using std::for_each;




/**
 * Checks if collisions between these Objects are inhibited. Currently the only
 * thing that inhibits collisions is a connected connector with the
 * inhibitCollisions flag set.
 */
static bool AreCollisionsInhibited( const Object & lhs, const Object & rhs )
{
  const Object::connectors_t & lhsConnectors = lhs.getConnectors();
  
  // for_each( lhsConnectors )
  for( Object::connectors_t::const_iterator i = lhsConnectors.begin() ;
       i != lhsConnectors.end() ; ++i ) {
    // do: if this connector is connected and is connected to rhs and the
    // master of the connection inhibits collisions, then return true
    if( i->second->isConnected() &&
        &(i->second->getTargetConnector()->getHostObject()) == &rhs &&
        ( i->second->isConnectedAndMaster() ?
          i->second->doesInhibitCollisions() :
          i->second->getTargetConnector()->doesInhibitCollisions() ) ) {
      return true;
    }
  }
  
  return false;
}




Collider::Collider( ODEWorld & world_ ) :
  world( world_ ),
  collisionSpace( 0 ),
  jointGroup(),
  contactBuf( new dContact[CONTACTBUF_SIZE] ),
  objectNodes(),
  allContacts()
{
  collisionSpace.setCleanup( 0 );   // objects have their own collision spaces
                                    // managed through the ODE C++ wrapper
                                    // interface!
  initGeoms( collisionSpace, world );
}


Collider::~Collider()
{
  // wipe all Contact objects
  for_each( allContacts.begin(), allContacts.end(), deleter<Contact>() );
  allContacts.clear();
  
  // wipe all ObjectNode objects
  for_each( objectNodes.begin(), objectNodes.end(), deleter<ObjectNode>() );
  objectNodes.clear();
}




/**
 * @todo
 * Optimize away all Geometry data re-fetching while traversing primitive geoms
 * within the same Geometry objects.
 *
 * @todo
 * Optimize the AreCollisionsInhibited() function (cache the inhibited pairs in
 * Objects).
 */
void Collider::ODECollisionCallback( void * data, dGeomID lhs, dGeomID rhs )
{
  Object & lhsObject = *(Object *)dGeomGetData( lhs );
  Object & rhsObject = *(Object *)dGeomGetData( rhs );
  
  if( dGeomIsSpace(lhs) || dGeomIsSpace(rhs) ) {
    // spaces involved, recurse
    dSpaceCollide2( lhs, rhs, data, &ODECollisionCallback );
    if( dGeomIsSpace(lhs) ) dSpaceCollide( (dSpaceID)lhs, data,
                                           &ODECollisionCallback );
    if( dGeomIsSpace(rhs) ) dSpaceCollide( (dSpaceID)rhs, data,
                                           &ODECollisionCallback );
  } else {
    
    // geom-geom collision, create contacts if not connected
    
    // do not collide static objects
    if( !dGeomGetBody(lhs) && !dGeomGetBody(rhs) ) return;
    
    // do not collide connected objects
    if( AreCollisionsInhibited( lhsObject, rhsObject ) ) return;
    
    
    // collide
    Collider & collider = *(Collider *)data;
    int count = dCollide( lhs, rhs,
                          CONTACTBUF_SIZE,
                          &collider.contactBuf[0].geom,
                          sizeof(dContact) );
    
    // check result
    if( count == 0 ) return;
    if( count == CONTACTBUF_SIZE ) {
      cout << "*** WARNING: ODECollisionRenderer: contact buffer overflow!"
           << endl;
    }
    
    
    /* The current geoms are in contact. Create contact joints and update
       involved Contact objects. */
    
    // resolve colliding Geometry and BasicGeometry objects
    assert_user( lhsObject.getGeometry() && rhsObject.getGeometry(),
                 "Both colliding Objects must have a Geometry!" );
    Geometry & lhsGeometry = *lhsObject.getGeometry();
    Geometry & rhsGeometry = *rhsObject.getGeometry();
    const BasicGeometry * lhsBasicGeometry =
      dynamic_cast<const BasicGeometry *>( &lhsGeometry );
    const BasicGeometry * rhsBasicGeometry =
      dynamic_cast<const BasicGeometry *>( &rhsGeometry );
    assert_user( lhsBasicGeometry && rhsBasicGeometry,
                 "Both colliding Objects must have a Geometry and currently "
                 "it must be of type BasicGeometry!" );
    assert_user( lhsBasicGeometry->collisionMaterial &&
                 rhsBasicGeometry->collisionMaterial,
                 "CollisionMaterial field of BasicGeometry must be defined "
                 "for both colliding Objects!" );
    
    // resolve contact materials
    const CollisionMaterial & lhsMat = *lhsBasicGeometry->collisionMaterial;
    const CollisionMaterial & rhsMat = *rhsBasicGeometry->collisionMaterial;
    
    // compute contact params
    real friction = lhsMat.friction * rhsMat.friction;
    real bounciness = lhsMat.bounciness * rhsMat.bounciness;
    real bounceMinVel = lhsMat.bounceMinVel + rhsMat.bounceMinVel;
    
    // insert contacts
    for( int i = 0 ; i < count ; i++ ) {
      
      // write contact params
      collider.contactBuf[i].surface.mode =
        (bounciness > 0.0 ? dContactBounce : 0 ) |
        dContactApprox1;
      collider.contactBuf[i].surface.mu = friction;
      collider.contactBuf[i].surface.bounce = bounciness;
      collider.contactBuf[i].surface.bounce_vel = bounceMinVel;
      
      // attach the joint
      dJointAttach
        ( dJointCreateContact( collider.world.id(),
                               collider.jointGroup.id(),
                               &collider.contactBuf[i] ),
          dGeomGetBody(lhs), dGeomGetBody(rhs) );
      
    }
    
    /* update involved Contact objects */
    
    const Geometry::contacts_t & lhsContacts = lhsGeometry.getContacts();
    Geometry::contacts_t::const_iterator i = lhsContacts.find( &rhsGeometry );
    
    Contact * contact;
    if( i == lhsContacts.end() ) {
      // create a new contact and insert it into all three containers
      contact = new Contact( &lhsGeometry, &rhsGeometry );
      collider.allContacts.push_back( contact );
    } else {
      // contact exists already
      contact = i->second;
    }
    
    // update contact data
    contact->getFlipflop() = collider.currentFlipflop;
    
  }
}




void Collider::initGeom( dSpace & geomSpace, Object & object )
{
  shared_ptr<const Geometry> geometry = object.getGeometry();
  
  if( !geometry ) return;
  
  objectNodes.push_back( new ObjectNode( *this, geomSpace, object ) );
}




void Collider::initGeoms( dSpace & geomSpace, Object & object )
{
  // attempt downcasts to supported types
  if( Subspace * subspace = dynamic_cast<Subspace *>( &object ) ) {
    initGeoms( geomSpace, *subspace );

    return;
  }
  
  initGeom( geomSpace, object );
}


void Collider::initGeoms( dSpace & geomSpace, Subspace & subspace )
{
  // create an own geomspace here
  //...
  
  // recurse
  Subspace::objects_t & objects = subspace.getObjects();
  for( Subspace::objects_t::iterator i = objects.begin() ;
       i != objects.end() ; i++ ) {
    initGeoms( geomSpace, **i );
  }
  
  // add own geom
  initGeom( geomSpace, subspace );
}




void Collider::collide()
{
  currentFlipflop = !currentFlipflop;
  
  jointGroup.empty();
  collisionSpace.collide( (void *)this, &ODECollisionCallback );
  
  // wipe old Contact objects
  for( contacts_t::iterator i = allContacts.begin() ;
       i != allContacts.end() ; ) {
    if( (*i)->getFlipflop() != currentFlipflop ) {
      delete *i;
      contacts_t::iterator tmp = i++;
      allContacts.erase( tmp );
    } else {
      ++i;
    }
  }
}
