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
 * @file ODEBallConnector.cpp
 *
 * Implementation for the ODEBallConnector class.
 */
#include "../types.hpp"
#include "ODEBallConnector.hpp"
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




/**
 * The implementation for this is missing from ODE (upto ode-0.9, at
 * least).
 *
 * The implementation below is copied and extended from:
 *   http://www.ode.org/old_list_archives/2006-November/020976.html
 */
lifespace::real odeAMotorAngleRateReader( const dAMotorJoint & joint,
                                          int anum )
{
  dReal rate = 0;
  dBodyID b0 = joint.getBody(0);
  dBodyID b1 = joint.getBody(1);
  dVector3 axis;
  
  if( anum != 1 )
    joint.getAxis( anum, axis );
  else {
    // anum == 1: this is also missing from ODE
    
    dVector3 axis0;
    dVector3 axis2;
    joint.getAxis( 0, axis0 );
    joint.getAxis( 2, axis2 );
    
    // cross product: axis1 = axis2 x axis0
    axis[0] = axis2[1] * axis0[2] - axis2[2] * axis0[1];
    axis[1] = -( axis2[0] * axis0[2] - axis2[2] * axis0[0] );
    axis[2] = axis2[0] * axis0[1] - axis2[1] * axis0[0];
  }
  
  if (b0 != 0) {
    const dReal *avel0 = dBodyGetAngularVel(b0);
    rate += axis[0]*avel0[0] + axis[1]*avel0[1] + axis[2]*avel0[2];
  }
  if (b1 != 0) {
    const dReal *avel1 = dBodyGetAngularVel(b1);
    rate -= axis[0]*avel1[0] + axis[1]*avel1[1] + axis[2]*avel1[2];
  }
  
  /*
  // ???
  if (b0 == 0 && b1 != 0)
    rate = -rate;
  */
  
  return rate;
}


lifespace::real ODEBallConnector::AngleDataReader::operator()()
{
  if( !active ) return 0.0;
  
  if( order == 0 ) return host->getAngle( axis );
  else if( order == 1 ) return odeAMotorAngleRateReader( *host, axis );
  else { assert( false ); return 0.0; }
}




void ODEBallConnector::init()
{
  controls.resize( ControlCount, Control( Control::ContinuousMode ) );
  sensors.resize( SensorCount );
  
  sensors[SENS_ANGLE_X]              = &angleDataReaders[0][0];
  sensors[SENS_ANGLE_Y]              = &angleDataReaders[0][1];
  sensors[SENS_ANGLE_Z]              = &angleDataReaders[0][2];
  sensors[SENS_ANGLERATE_X]          = &angleDataReaders[1][0];
  sensors[SENS_ANGLERATE_Y]          = &angleDataReaders[1][1];
  sensors[SENS_ANGLERATE_Z]          = &angleDataReaders[1][2];
}




ODEBallConnector::ODEBallConnector( const Connector & connector ) :
  Connector( connector ),
  dBallJoint(),
  dAMotorJoint(),
  isMotored( false )
{
  // leave axis params and sensory readers to their defaults (sensors will be
  // inactive by default)
  
  // init the controls and sensors, although they do not have any effect
  init();
}


ODEBallConnector::ODEBallConnector( const Connector & connector,
                                    const ODEMotorAxisParams & motorAxisParamsX,
                                    const ODEMotorAxisParams & motorAxisParamsY,
                                    const ODEMotorAxisParams & motorAxisParamsZ ) :
  Connector( connector ),
  dBallJoint(),
  dAMotorJoint(),
  isMotored( true )
{
  
  memcpy( motorAxes + 0, &motorAxisParamsX, sizeof(ODEMotorAxisParams) );
  memcpy( motorAxes + 1, &motorAxisParamsY, sizeof(ODEMotorAxisParams) );
  memcpy( motorAxes + 2, &motorAxisParamsZ, sizeof(ODEMotorAxisParams) );
  
  for( int order = 0 ; order < 2 ; order++ )
    for( int axis = 0 ; axis < 3 ; axis++ )
      angleDataReaders[order][axis] =
        AngleDataReader( this, order, axis );
  
  init();
}




void ODEBallConnector::connect( shared_ptr<Connector> target, Aligning aligning )
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
  
  // create the ball joint
  assert_internal( !dBallJoint::id() && !dAMotorJoint::id() );
  dBallJoint::create( odeWorldId );
  
  // connect the ball joint to the objects
  dBallJoint::attach
    ( thisODELocator ? thisODELocator->getODEBodyId() : 0,
      targetODELocator ? targetODELocator->getODEBodyId() : 0 );
  
  // handle the amotor joint
  if( isMotored ) {
    
    // create it
    dAMotorJoint::create( odeWorldId );
    
    // attach it to the objects
    dAMotorJoint::attach
      ( thisODELocator ? thisODELocator->getODEBodyId() : 0,
        targetODELocator ? targetODELocator->getODEBodyId() : 0 );
    
    // activate the sensors
    for( int order = 0 ; order < 2 ; order++ )
      for( int axis = 0 ; axis < 3 ; axis++ )
        angleDataReaders[order][axis].activate( true );
    
  }
  
  // set the joint params
  assert_internal( thisODELocator || targetODELocator );
  BasicLocator absConnector( thisODELocator ? *this : *target );
  (thisODELocator ? thisODELocator : targetODELocator)->
    getDirectWorldLocator()->transform( absConnector );
  dBallJoint::setAnchor( absConnector.getLoc()[0],
                         absConnector.getLoc()[1],
                         absConnector.getLoc()[2] );
  if( isMotored ) {
    dAMotorJoint::setMode( dAMotorEuler );
    dAMotorJoint::setAxis( 0, 1, 
                           absConnector.getBasis().getBasisVec(DIM_X)[0],
                           absConnector.getBasis().getBasisVec(DIM_X)[1],
                           absConnector.getBasis().getBasisVec(DIM_X)[2] );
    dAMotorJoint::setAxis( 2, 2, 
                           absConnector.getBasis().getBasisVec(DIM_Z)[0],
                           absConnector.getBasis().getBasisVec(DIM_Z)[1],
                           absConnector.getBasis().getBasisVec(DIM_Z)[2] );
    for( int i = 0; i < 3 ; i++ ) {   // setting loStop twice avoids an ODE bug
      dAMotorJoint::setParam( dParamLoStop + dParamGroup * i, motorAxes[i].loStop );
      dAMotorJoint::setParam( dParamHiStop + dParamGroup * i, motorAxes[i].hiStop );
      dAMotorJoint::setParam( dParamLoStop + dParamGroup * i, motorAxes[i].loStop );
      dAMotorJoint::setParam( dParamBounce + dParamGroup * i,
                              motorAxes[i].stopBounciness );
    }
  }
  
  // handle the DontAlign case (restore the temporarily aligned locator)
  if( locationSnapshotExists ) {
    restoreLocation( *this, *target, aligning );
    locationSnapshotExists = false;
  }
  
}


void ODEBallConnector::disconnect()
{
  assert( isConnectedAndMaster() );
  
  // deactivate the sensors
  if( isMotored ) {
    for( int order = 0 ; order < 2 ; order++ )
      for( int axis = 0 ; axis < 3 ; axis++ )
        angleDataReaders[order][axis].activate( false );
  }
  
  // disconnect the underlying ODE bodies (inactivate the ODE joint)
  dBallJoint::attach( 0, 0 );
  if( isMotored ) { dAMotorJoint::attach( 0, 0 ); }
  
  // destroy the ODE joint
  dJointDestroy( dBallJoint::_id ); dBallJoint::_id = 0;
  if( isMotored ) { dJointDestroy( dAMotorJoint::_id ); dAMotorJoint::_id = 0; }
  
  // disconnect the connectors
  Connector::disconnect();
}




void ODEBallConnector::prepare( real dt )
{
  if( isConnectedAndMaster() && isMotored ) {
    dAMotorJoint::setParam( dParamVel,
                            readControl( CTRL_MOTOR_TARGETVEL_X ) );
    dAMotorJoint::setParam( dParamFMax,
                            readControl( CTRL_MOTOR_FORCEFACTOR_X )
                            * motorAxes[0].maxMotorForce );
    dAMotorJoint::setParam( dParamVel2,
                            readControl( CTRL_MOTOR_TARGETVEL_Y ) );
    dAMotorJoint::setParam( dParamFMax2,
                            readControl( CTRL_MOTOR_FORCEFACTOR_Y )
                            * motorAxes[1].maxMotorForce );
    dAMotorJoint::setParam( dParamVel3,
                            readControl( CTRL_MOTOR_TARGETVEL_Z ) );
    dAMotorJoint::setParam( dParamFMax3,
                            readControl( CTRL_MOTOR_FORCEFACTOR_Z )
                            * motorAxes[2].maxMotorForce );
    dAMotorJoint::addTorques( readControl( CTRL_TORQUE_X ),
                              readControl( CTRL_TORQUE_Y ),
                              readControl( CTRL_TORQUE_Z ) );
  }
  
  Connector::prepare( dt );
}
