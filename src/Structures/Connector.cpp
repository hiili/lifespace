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
 * @file Connector.cpp
 *
 * Implementation for the Connector class.
 */
#include "../types.hpp"
#include "../Control/Actor.hpp"
#include "Connector.hpp"
#include "Vector.hpp"
#include "BasisMatrix.hpp"
#include "Object.hpp"
#include "Locator.hpp"
#include <functional>
#include <boost/shared_ptr.hpp>
#include <cassert>
using namespace lifespace;
using namespace std;
using boost::shared_ptr;




Connector::Aligning
Connector::snapshotEitherLocation( const Connector & master,
                                   const Connector & slave )
{
  // check that no snapshots are left around
  assert_internal( !locationSnapshot );
  
  // check that both objects have locators
  assert_user( master.hostObject.getLocator() &&
               slave.hostObject.getLocator(),
               "Both Objects being connected must have locators!" );
  
  // check which one has a directworldlocator
  if( slave.hostObject.getLocator()->getDirectWorldLocator() ) {
    // snapshot slave
    locationSnapshot = new BasicLocator
      ( *slave.hostObject.getLocator()->getDirectWorldLocator() );
    return AlignSlave;
  } else if( master.hostObject.getLocator()->getDirectWorldLocator() ) {
    // snapshot master
    locationSnapshot = new BasicLocator
      ( *master.hostObject.getLocator()->getDirectWorldLocator() );
    return AlignMaster;
  } else {
    // Neither of the Objects have an ODELocator
    assert_user( false,
                 "At least one of the Objects being connected with "
                 "an ODE connector must have an ODELocator!" );
    return DontAlign;
  }
}


void Connector::restoreLocation( Connector & master, Connector & slave,
                                 Aligning aligning )
{
  // check that we have a snapshot
  assert_internal( locationSnapshot );
  
  // check that both objects still have locators
  assert_internal( master.hostObject.getLocator() &&
                   slave.hostObject.getLocator() );
  
  // check which one was snapshot
  if( aligning == AlignSlave ) {
    // check that the object still has a directworldlocator
    assert_internal( slave.hostObject.getLocator()->getDirectWorldLocator() );

    // restore slave
    slave.hostObject.getLocator()->getDirectWorldLocator()->alignWith
      ( *locationSnapshot );
  } else if( aligning == AlignMaster ) {
    // check that the object still has a directworldlocator
    assert_internal( master.hostObject.getLocator()->getDirectWorldLocator() );
    
    // restore master
    master.hostObject.getLocator()->getDirectWorldLocator()->alignWith
      ( *locationSnapshot );
  } else assert_internal(false);
  delete locationSnapshot; locationSnapshot = 0;
}




Connector::Connector( Object & hostObject_, Role role,
                      const BasicLocator & locator,
                      bool inhibitCollisions_ ) :
  BasicLocator( locator ),
  Actor( 0, 0 ),
  hostObject( hostObject_ ),
  allowedRole( role ), activeRole( _Invalid ),
  flipAxis( makeVector3d( 0.0, 1.0, 0.0 ) ),
  inhibitCollisions( inhibitCollisions_ ),
  locationSnapshot( 0 ),
  targetConnector()
{}


Connector::Connector( const Connector & other ) :
  BasicLocator( other ),
  Actor( 0, 0 ),
  hostObject( other.hostObject ),
  allowedRole( other.allowedRole ),
  activeRole( _Invalid ),
  flipAxis( other.flipAxis ),
  inhibitCollisions( other.inhibitCollisions ),
  locationSnapshot( 0 ),
  targetConnector()
{}


Connector & Connector::operator=( const Connector & other )
{
  assert( !locationSnapshot && !targetConnector );
      
  *(BasicLocator *)this = (BasicLocator)other;
  allowedRole = other.allowedRole;
  activeRole = _Invalid;
  flipAxis = other.flipAxis;
  inhibitCollisions = other.inhibitCollisions;
  
  return *this;
}


Connector::~Connector()
{
  assert_internal( !locationSnapshot );
  delete locationSnapshot; locationSnapshot = 0;
}




void Connector::connect( shared_ptr<Connector> target, Aligning aligning )
{
  
  /* initialization and some checks */
  
  // check that not already connected
  assert_user( !isConnected(), "The connector is already connected!" );
  
  // check roles
  assert_user( (allowedRole == Master || allowedRole == Any) &&
               (target->allowedRole == Slave || target->allowedRole == Any),
               "Illegal connector roles!" );
  
  // check align parameter: should be AlignMaster or AlignSlave, DontAlign is
  // not allowed here because it should be handled in the inherited connector
  // classes.
  assert( aligning == AlignMaster || aligning == AlignSlave );
  
  // resolve the the source and target objects' locators and assert that
  // they exist
  boost::shared_ptr<Locator> thisLocator = hostObject.getLocator();
  boost::shared_ptr<Locator> targetLocator = target->hostObject.getLocator();
  assert_user( thisLocator && targetLocator,
               "Both Objects being connected must have locators!" );
  

  /* select the stationary and moving connector and some locators from their
     host objects */
  
  // select the stationary and the moving connector
  const Connector & stationaryConnector =
    (aligning == AlignSlave ? *this : *target );
  Connector & movingConnector =
    (aligning == AlignSlave ? *target : *this );
  
  // resolve the stationary object's world locator and the moving object's
  // direct world locator (which must be an ODELocator to get the internal
  // direct locator!)
  boost::shared_ptr<const Locator> stationaryWorldLocator =
    ( aligning == AlignSlave ?
      hostObject.getWorldLocator() :
      target->hostObject.getWorldLocator() );
  boost::shared_ptr<Locator> movingDirectWorldLocator =
    ( aligning == AlignSlave ?
      targetLocator->getDirectWorldLocator() :
      thisLocator->getDirectWorldLocator() );
  
  // assert that the locators acquired above really exist. (these requirements
  // could be avoided if necessary..)
  assert_user( stationaryWorldLocator && movingDirectWorldLocator,
               "The object being aligned for the connection must have "
               "an ODELocator, and with DontAlign at least one of the "
               "Objects must have an ODELocator!" );
  
  
  /* align the moving connector */
  
  // start from stationary connector's relative position, make it absolute, and
  // flip it around to the stationary connector's flip axis.
  BasicLocator movingFinalAbs( stationaryConnector );
  stationaryWorldLocator->transform( movingFinalAbs );
  movingFinalAbs.rotate3dRel( stationaryConnector.flipAxis, M_PI );
  
  // back up from the moving connector's new absolute location to the moving
  // object's new absolute location (ResolveTransformation) and align the
  // moving object's internal directWorldLocator directly with it
  movingDirectWorldLocator->alignWith
    ( BasicLocator::ResolveTransformation( movingFinalAbs, movingConnector ) );
  
  
  /* finalize */
  
  // connect the Connectors
  targetConnector = target;
  activeRole = Master;
  target->targetConnector = shared_from_this();
  target->activeRole = Slave;
  
}


void Connector::disconnect()
{
  assert_user( isConnectedAndMaster(),
               "The connector being disconnected is either not connected "
               "or is not the master connector of the connection!" );
      
  targetConnector->targetConnector.reset();
  targetConnector->activeRole = _Invalid;
  targetConnector.reset();
  activeRole = _Invalid;
}
