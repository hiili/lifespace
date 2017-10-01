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
 * @file ODEUniversalConnector.cpp
 *
 * Implementation for the ODEUniversalConnector class.
 */
#include "../types.hpp"
#include "ODEUniversalConnector.hpp"
#include "Connector.hpp"
#include "ODEMotorAxisParams.hpp"
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




lifespace::real ODEUniversalConnector::AngleDataReader::operator()()
{
  if( !active ) return 0.0;
  assert( (order == 0 || order == 1) && (axis == 0 || axis == 1) );
  
  if( order == 0 ) return ( axis == 0 ? host->getAngle1() : host->getAngle2() );
  else return ( axis == 0 ? host->getAngle1Rate() : host->getAngle2Rate() );
}




void ODEUniversalConnector::init()
{
  controls.resize( ControlCount, Control( Control::ContinuousMode ) );
  sensors.resize( SensorCount );
  
  sensors[SENS_ANGLE_X]              = &angleDataReaders[0][0];
  sensors[SENS_ANGLE_Y]              = &angleDataReaders[0][1];
  sensors[SENS_ANGLERATE_X]          = &angleDataReaders[1][0];
  sensors[SENS_ANGLERATE_Y]          = &angleDataReaders[1][1];
}




ODEUniversalConnector::
ODEUniversalConnector( const Connector & connector,
                       const ODEMotorAxisParams & motorAxisParamsX,
                       const ODEMotorAxisParams & motorAxisParamsY ) :
  Connector( connector ),
  dUniversalJoint()
{
  
  memcpy( motorAxes + 0, &motorAxisParamsX, sizeof(ODEMotorAxisParams) );
  memcpy( motorAxes + 1, &motorAxisParamsY, sizeof(ODEMotorAxisParams) );
  
  for( int order = 0 ; order < 2 ; order++ )
    for( int axis = 0 ; axis < 2 ; axis++ )
      angleDataReaders[order][axis] =
        AngleDataReader( this, order, axis );
  
  init();
}




void ODEUniversalConnector::connect( shared_ptr<Connector> target,
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
  
  // create the universal joint
  assert_internal( !dUniversalJoint::id() && !dUniversalJoint::id() );
  dUniversalJoint::create( odeWorldId );
  
  // connect the universal joint to the objects
  dUniversalJoint::attach
    ( thisODELocator ? thisODELocator->getODEBodyId() : 0,
      targetODELocator ? targetODELocator->getODEBodyId() : 0 );
  
  // activate the sensors
  for( int order = 0 ; order < 2 ; order++ )
    for( int axis = 0 ; axis < 2 ; axis++ )
      angleDataReaders[order][axis].activate( true );
  
  // set the joint params
  assert_internal( thisODELocator || targetODELocator );
  BasicLocator absConnector( thisODELocator ? *this : *target );
  (thisODELocator ? thisODELocator : targetODELocator)->
    getDirectWorldLocator()->transform( absConnector );
  dUniversalJoint::setAnchor( absConnector.getLoc()[0],
                              absConnector.getLoc()[1],
                              absConnector.getLoc()[2] );
  dUniversalJoint::setAxis1( absConnector.getBasis().getBasisVec(DIM_X)[0],
                             absConnector.getBasis().getBasisVec(DIM_X)[1],
                             absConnector.getBasis().getBasisVec(DIM_X)[2] );
  dUniversalJoint::setAxis2( absConnector.getBasis().getBasisVec(DIM_Y)[0],
                             absConnector.getBasis().getBasisVec(DIM_Y)[1],
                             absConnector.getBasis().getBasisVec(DIM_Y)[2] );
  for( int i = 0; i < 2 ; i++ ) {   // setting loStop twice avoids an ODE bug
    dUniversalJoint::setParam( dParamLoStop + dParamGroup * i, motorAxes[i].loStop );
    dUniversalJoint::setParam( dParamHiStop + dParamGroup * i, motorAxes[i].hiStop );
    dUniversalJoint::setParam( dParamLoStop + dParamGroup * i, motorAxes[i].loStop );
    dUniversalJoint::setParam( dParamBounce + dParamGroup * i,
                               motorAxes[i].stopBounciness );
  }

  // handle the DontAlign case (restore the temporarily aligned locator)
  if( locationSnapshotExists ) {
    restoreLocation( *this, *target, aligning );
    locationSnapshotExists = false;
  }

}


void ODEUniversalConnector::disconnect()
{
  assert( isConnectedAndMaster() );
  
  // deactivate the sensors
  for( int order = 0 ; order < 2 ; order++ )
    for( int axis = 0 ; axis < 2 ; axis++ )
      angleDataReaders[order][axis].activate( false );
  
  // disconnect the underlying ODE bodies (inactivate the ODE joint)
  dUniversalJoint::attach( 0, 0 );
  
  // destroy the ODE joint
  dJointDestroy( dUniversalJoint::_id ); dUniversalJoint::_id = 0;
  
  // disconnect the connectors
  Connector::disconnect();
}




void ODEUniversalConnector::prepare( real dt )
{
  if( isConnectedAndMaster() ) {
    dUniversalJoint::setParam( dParamVel,
                               readControl( CTRL_MOTOR_TARGETVEL_X ) );
    dUniversalJoint::setParam( dParamFMax,
                               readControl( CTRL_MOTOR_FORCEFACTOR_X )
                               * motorAxes[0].maxMotorForce );
    dUniversalJoint::setParam( dParamVel2,
                               readControl( CTRL_MOTOR_TARGETVEL_Y ) );
    dUniversalJoint::setParam( dParamFMax2,
                               readControl( CTRL_MOTOR_FORCEFACTOR_Y )
                               * motorAxes[1].maxMotorForce );
    dUniversalJoint::addTorques( readControl( CTRL_TORQUE_X ),
                                 readControl( CTRL_TORQUE_Y ) );
  }
  
  Connector::prepare( dt );
}
