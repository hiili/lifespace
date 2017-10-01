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
 * @file ODEAxleConnector.cpp
 *
 * Implementation for the ODEAxleConnector class.
 */
#include "../types.hpp"
#include "ODEAxleConnector.hpp"
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




void ODEAxleConnector::init()
{
  controls.resize( ControlCount, Control( Control::ContinuousMode ) );
  sensors.resize( SensorCount );
  
  sensors[SENS_ANGLE]              = &angleReader;
  sensors[SENS_ANGLERATE]          = &angleRateReader;
}




ODEAxleConnector::ODEAxleConnector( const Connector & connector,
                                    real maxMotorForce_,
                                    real loStopAngle_, real hiStopAngle_,
                                    real stopBounciness_ ) :
  Connector( connector ),
  dHingeJoint(),
  maxMotorForce( maxMotorForce_ ),
  loStopAngle( !finite(loStopAngle_) ?
               (loStopAngle_<0.0 ? -dInfinity : dInfinity) : loStopAngle_ ),
  hiStopAngle( !finite(hiStopAngle_) ?
               (hiStopAngle_<0.0 ? -dInfinity : dInfinity) : hiStopAngle_ ),
  stopBounciness( stopBounciness_ ),
  angleReader( &dHingeJoint::getAngle, (dHingeJoint *)this,
               0.0, false ),
  angleRateReader( &dHingeJoint::getAngleRate, (dHingeJoint *)this,
                   0.0, false )
{ init(); }




void ODEAxleConnector::connect( shared_ptr<Connector> target,
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
  assert_internal( !dHingeJoint::id() );
  dHingeJoint::create( odeWorldId );
  
  // connect the joint to the objects
  dHingeJoint::attach
    ( thisODELocator ? thisODELocator->getODEBodyId() : 0,
      targetODELocator ? targetODELocator->getODEBodyId() : 0 );
  
  // activate the sensors
  angleReader.activate( true );
  angleRateReader.activate( true );
  
  // set the joint params
  assert_internal( thisODELocator || targetODELocator );
  BasicLocator absConnector( thisODELocator ? *this : *target );
  (thisODELocator ? thisODELocator : targetODELocator)->
    getDirectWorldLocator()->transform( absConnector );
  dHingeJoint::setAnchor( absConnector.getLoc()[0],
                          absConnector.getLoc()[1],
                          absConnector.getLoc()[2] );
  dHingeJoint::setAxis( absConnector.getBasis().getBasisVec(DIM_Z)[0],
                        absConnector.getBasis().getBasisVec(DIM_Z)[1],
                        absConnector.getBasis().getBasisVec(DIM_Z)[2] );
  dHingeJoint::setParam( dParamLoStop, loStopAngle );
  dHingeJoint::setParam( dParamHiStop, hiStopAngle );
  dHingeJoint::setParam( dParamLoStop, loStopAngle );   // avoids an ODE bug
  dHingeJoint::setParam( dParamBounce, stopBounciness );
  
  
  // handle the DontAlign case (restore the temporarily aligned locator)
  if( locationSnapshotExists ) {
    restoreLocation( *this, *target, aligning );
    locationSnapshotExists = false;
  }
  
}


void ODEAxleConnector::disconnect()
{
  assert( isConnectedAndMaster() );
  
  // deactivate the sensors
  angleReader.activate( false );
  angleRateReader.activate( false );
  
  // disconnect the underlying ODE bodies (inactivate the ODE joint)
  dHingeJoint::attach( 0, 0 );
  
  // destroy the ODE joint
  dJointDestroy( dHingeJoint::_id ); dHingeJoint::_id = 0;
  
  // disconnect the connectors
  Connector::disconnect();
}




void ODEAxleConnector::prepare( real dt )
{
  if( isConnectedAndMaster() ) {
    dHingeJoint::setParam( dParamVel,
                           readControl( CTRL_MOTOR_TARGETVEL ) );
    dHingeJoint::setParam( dParamFMax,
                           readControl( CTRL_MOTOR_FORCEFACTOR )
                           * maxMotorForce );
    dHingeJoint::addTorque( readControl( CTRL_TORQUE ) );
  }
  
  Connector::prepare( dt );
}
