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
 * @file ODESliderConnector.cpp
 *
 * Implementation for the ODESliderConnector class.
 */
#include "../types.hpp"
#include "ODESliderConnector.hpp"
#include "Connector.hpp"
#include "Vector.hpp"
#include "BasisMatrix.hpp"
#include "Object.hpp"
#include "Locator.hpp"
#include "ODELocator.hpp"
#include "../Control/Actor.hpp"
#include <ode/ode.h>
#include <ode/odecpp.h>
#include <functional>
#include <boost/shared_ptr.hpp>
#include <cassert>
using namespace lifespace;
using namespace std;
using boost::shared_ptr;




void ODESliderConnector::init()
{
  controls.resize( ControlCount, Control( Control::ContinuousMode ) );
  sensors.resize( SensorCount );
  
  sensors[SENS_LENGTH]             = &lengthReader;
  sensors[SENS_VELOCITY]           = &velocityReader;
}




ODESliderConnector::ODESliderConnector( const Connector & connector,
                                        real maxMotorForce_,
                                        real loStop_, real hiStop_,
                                        real stopBounciness_ ) :
  Connector( connector ),
  dSliderJoint(),
  maxMotorForce( maxMotorForce_ ),
  loStop( !finite(loStop_) ?
          (loStop_<0.0 ? -dInfinity : dInfinity) : loStop_ ),
  hiStop( !finite(hiStop_) ?
          (hiStop_<0.0 ? -dInfinity : dInfinity) : hiStop_ ),
  stopBounciness( stopBounciness_ ),
  lengthReader( &dSliderJoint::getPosition, (dSliderJoint *)this,
                0.0, false ),
  velocityReader( &dSliderJoint::getPositionRate, (dSliderJoint *)this,
                  0.0, false )
{ init(); }




void ODESliderConnector::connect( shared_ptr<Connector> target,
                                  Aligning aligning )
{
  
  // handle the DontAlign case (snapshot either location and temporarily align)
  bool locationSnapshotExists = false;
  if( aligning == DontAlign ) {
    aligning = snapshotEitherLocation( *this, *target );
    locationSnapshotExists = true;
  }
  
  
  // align and connect the connectors
  Connector::connect( target, aligning );
  
  
  // connect the underlying ODE body/bodies with the ODE joint
  
  // resolve the ODELocators
  boost::shared_ptr<ODELocator> thisODELocator
    ( boost::dynamic_pointer_cast<ODELocator>
      ( getHostObject().getLocator() ) );
  boost::shared_ptr<ODELocator> targetODELocator
    ( boost::dynamic_pointer_cast<ODELocator>
      ( target->getHostObject().getLocator() ) );
  
  // make sure that at least one of them is an ODELocator
  assert_user( thisODELocator || targetODELocator,
               "At least one of the Objects being connected "
               "must have an ODELocator!" );
  
  // make sure that all involved ODELocators are active
  assert_user( (!thisODELocator || thisODELocator->isActive()) &&
               (!targetODELocator || targetODELocator->isActive()),
               "ODELocators must be activated before connecting them!" );
  
  // resolve the host ODEWorld's internal ODE id
  dWorldID odeWorldId = thisODELocator ?
    thisODELocator->getHostODEWorld().id() :
    targetODELocator->getHostODEWorld().id();
  
  // if both Objects have ODELocators, then make sure they are in the same
  // ODEWorld
  assert_user( !(thisODELocator && targetODELocator) ||
               thisODELocator->getHostODEWorld().id() == 
               targetODELocator->getHostODEWorld().id(),
               "Both ODELocators must be in the same ODEWorld!" );
  
  // create the joint
  assert_internal( !dSliderJoint::id() );
  dSliderJoint::create( odeWorldId );
  
  // connect the joint to the objects
  dSliderJoint::attach
    ( thisODELocator ? thisODELocator->getODEBodyId() : 0,
      targetODELocator ? targetODELocator->getODEBodyId() : 0 );
  
  // activate the sensors
  lengthReader.activate( true );
  velocityReader.activate( true );
  
  // set the joint params
  assert_internal( thisODELocator || targetODELocator );
  BasicLocator absConnector( thisODELocator ? *this : *target );
  (thisODELocator ? thisODELocator : targetODELocator)->
    getDirectWorldLocator()->transform( absConnector );
  dSliderJoint::setAxis( absConnector.getBasis().getBasisVec(DIM_Z)[0],
                         absConnector.getBasis().getBasisVec(DIM_Z)[1],
                         absConnector.getBasis().getBasisVec(DIM_Z)[2] );
  dSliderJoint::setParam( dParamLoStop, loStop );
  dSliderJoint::setParam( dParamHiStop, hiStop );
  dSliderJoint::setParam( dParamLoStop, loStop );   // avoids an ODE bug
  dSliderJoint::setParam( dParamBounce, stopBounciness );
  
  
  // handle the DontAlign case (restore the temporarily aligned locator)
  if( locationSnapshotExists ) {
    restoreLocation( *this, *target, aligning );
    locationSnapshotExists = false;
  }
  
}


void ODESliderConnector::disconnect()
{
  assert( isConnectedAndMaster() );
  
  // deactivate the sensors
  lengthReader.activate( false );
  velocityReader.activate( false );
  
  // disconnect the underlying ODE bodies (inactivate the ODE joint)
  dSliderJoint::attach( 0, 0 );
  
  // destroy the ODE joint
  dJointDestroy( dSliderJoint::_id ); dSliderJoint::_id = 0;
  
  // disconnect the connectors
  Connector::disconnect();
}




void ODESliderConnector::prepare( real dt )
{
  if( isConnectedAndMaster() ) {
    dSliderJoint::setParam( dParamVel,
                            readControl( CTRL_MOTOR_TARGETVEL ) );
    dSliderJoint::setParam( dParamFMax,
                            readControl( CTRL_MOTOR_FORCEFACTOR )
                            * maxMotorForce );
    dSliderJoint::addForce( readControl( CTRL_FORCE ) );
  }
  
  Connector::prepare( dt );
}
