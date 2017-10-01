/**
 * @file Leg.cpp
 *
 * Implementations for the Leg class.
 */
#include "Leg.hpp"
#include "UpperPart.hpp"
#include "LowerPart.hpp"
#include "EndEffector.hpp"
#include "../resources.hpp"

#include <lifespace/lifespace.hpp>
using namespace lifespace;

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;




const shared_ptr<Material> Leg::MainVisMaterial
( new Material( gray1, gray5, white,
                none, polished, GL_FRONT ) );
//( new Material( gray4, gray9, white,
//                none, polished, GL_FRONT ) );
const shared_ptr<CollisionMaterial> Leg::MainPhysMaterial
( new CollisionMaterial( 0.66, 0.66, 0.001 ) );




Leg::Leg()
{
  // set a default name
  setName( "Leg" );
  
  // leg upper part
  upperPart = sptr( new UpperPart() );
  addObject( upperPart );
  
  // leg lower part
  lowerPart = sptr( new LowerPart() );
  addObject( lowerPart );
  
  // leg end effector
  endEffector = sptr( new EndEffector() );
  addObject( endEffector );
  
  
  // external connectors
  connectors[CONN_BASE] = upperPart->getConnector( UpperPart::CONN_BASE );
  
  
  
  
  // sensors
  
  // init
  sensors.resize( SensorCount );
  
  
  // ANGLE sensors
  
  // upper part
  sensors[SENS_UPPERPART_ANGLE_X] =
    upperPart->getConnector( UpperPart::CONN_BASE )->getSensor
    ( ODEUniversalConnector::SENS_ANGLE_X );
  sensors[SENS_UPPERPART_ANGLE_Y] =
    upperPart->getConnector( UpperPart::CONN_BASE )->getSensor
    ( ODEUniversalConnector::SENS_ANGLE_Y );
  
  // lower part
  sensors[SENS_LOWERPART_ANGLE] =
    lowerPart->getConnector( LowerPart::CONN_BASE )->getSensor
    ( ODEAxleConnector::SENS_ANGLE );
  
  // end effector
  sensors[SENS_ENDEFFECTOR_ANGLE_X] =
    endEffector->getConnector( EndEffector::CONN_BASE )->getSensor
    ( ODEBallConnector::SENS_ANGLE_X );
  sensors[SENS_ENDEFFECTOR_ANGLE_Y] =
    endEffector->getConnector( EndEffector::CONN_BASE )->getSensor
    ( ODEBallConnector::SENS_ANGLE_Y );
  sensors[SENS_ENDEFFECTOR_ANGLE_Z] =
    endEffector->getConnector( EndEffector::CONN_BASE )->getSensor
    ( ODEBallConnector::SENS_ANGLE_Z );
  
  
  // ANGLERATE sensors
  
  // upper part
  sensors[SENS_UPPERPART_ANGLERATE_X] =
    upperPart->getConnector( UpperPart::CONN_BASE )->getSensor
    ( ODEUniversalConnector::SENS_ANGLERATE_X );
  sensors[SENS_UPPERPART_ANGLERATE_Y] =
    upperPart->getConnector( UpperPart::CONN_BASE )->getSensor
    ( ODEUniversalConnector::SENS_ANGLERATE_Y );
  
  // lower part
  sensors[SENS_LOWERPART_ANGLERATE] =
    lowerPart->getConnector( LowerPart::CONN_BASE )->getSensor
    ( ODEAxleConnector::SENS_ANGLERATE );
  
  // end effector
  sensors[SENS_ENDEFFECTOR_ANGLERATE_X] =
    endEffector->getConnector( EndEffector::CONN_BASE )->getSensor
    ( ODEBallConnector::SENS_ANGLERATE_X );
  sensors[SENS_ENDEFFECTOR_ANGLERATE_Y] =
    endEffector->getConnector( EndEffector::CONN_BASE )->getSensor
    ( ODEBallConnector::SENS_ANGLERATE_Y );
  sensors[SENS_ENDEFFECTOR_ANGLERATE_Z] =
    endEffector->getConnector( EndEffector::CONN_BASE )->getSensor
    ( ODEBallConnector::SENS_ANGLERATE_Z );
  
  
  
  
  // controls
  
  // init
  controls.resize( ControlCount );
  
  
  // MOTOR_TARGETVEL controls
  
  // upper part
  controls[CTRL_UPPERPART_MOTOR_TARGETVEL_X] =
    upperPart->getConnector( UpperPart::CONN_BASE )->makeControlProxy
    ( ODEUniversalConnector::CTRL_MOTOR_TARGETVEL_X );
  controls[CTRL_UPPERPART_MOTOR_TARGETVEL_Y] =
    upperPart->getConnector( UpperPart::CONN_BASE )->makeControlProxy
    ( ODEUniversalConnector::CTRL_MOTOR_TARGETVEL_Y );
  
  // lower part
  controls[CTRL_LOWERPART_MOTOR_TARGETVEL] =
    lowerPart->getConnector( LowerPart::CONN_BASE )->makeControlProxy
    ( ODEAxleConnector::CTRL_MOTOR_TARGETVEL );
  
  // end effector
  controls[CTRL_ENDEFFECTOR_MOTOR_TARGETVEL_X] =
    endEffector->getConnector( EndEffector::CONN_BASE )->makeControlProxy
    ( ODEBallConnector::CTRL_MOTOR_TARGETVEL_X );
  controls[CTRL_ENDEFFECTOR_MOTOR_TARGETVEL_Y] =
    endEffector->getConnector( EndEffector::CONN_BASE )->makeControlProxy
    ( ODEBallConnector::CTRL_MOTOR_TARGETVEL_Y );
  controls[CTRL_ENDEFFECTOR_MOTOR_TARGETVEL_Z] =
    endEffector->getConnector( EndEffector::CONN_BASE )->makeControlProxy
    ( ODEBallConnector::CTRL_MOTOR_TARGETVEL_Z );
  
  
  // MOTOR_FORCEFACTOR controls
  
  // upper part
  controls[CTRL_UPPERPART_MOTOR_FORCEFACTOR_X] =
    upperPart->getConnector( UpperPart::CONN_BASE )->makeControlProxy
    ( ODEUniversalConnector::CTRL_MOTOR_FORCEFACTOR_X );
  controls[CTRL_UPPERPART_MOTOR_FORCEFACTOR_Y] =
    upperPart->getConnector( UpperPart::CONN_BASE )->makeControlProxy
    ( ODEUniversalConnector::CTRL_MOTOR_FORCEFACTOR_Y );
  
  // lower part
  controls[CTRL_LOWERPART_MOTOR_FORCEFACTOR] =
    lowerPart->getConnector( LowerPart::CONN_BASE )->makeControlProxy
    ( ODEAxleConnector::CTRL_MOTOR_FORCEFACTOR );
  
  // end effector
  controls[CTRL_ENDEFFECTOR_MOTOR_FORCEFACTOR_X] =
    endEffector->getConnector( EndEffector::CONN_BASE )->makeControlProxy
    ( ODEBallConnector::CTRL_MOTOR_FORCEFACTOR_X );
  controls[CTRL_ENDEFFECTOR_MOTOR_FORCEFACTOR_Y] =
    endEffector->getConnector( EndEffector::CONN_BASE )->makeControlProxy
    ( ODEBallConnector::CTRL_MOTOR_FORCEFACTOR_Y );
  controls[CTRL_ENDEFFECTOR_MOTOR_FORCEFACTOR_Z] =
    endEffector->getConnector( EndEffector::CONN_BASE )->makeControlProxy
    ( ODEBallConnector::CTRL_MOTOR_FORCEFACTOR_Z );
  
  
  // TORQUE controls
  
  // upper part
  controls[CTRL_UPPERPART_TORQUE_X] =
    upperPart->getConnector( UpperPart::CONN_BASE )->makeControlProxy
    ( ODEUniversalConnector::CTRL_TORQUE_X );
  controls[CTRL_UPPERPART_TORQUE_Y] =
    upperPart->getConnector( UpperPart::CONN_BASE )->makeControlProxy
    ( ODEUniversalConnector::CTRL_TORQUE_Y );
  
  // lower part
  controls[CTRL_LOWERPART_TORQUE] =
    lowerPart->getConnector( LowerPart::CONN_BASE )->makeControlProxy
    ( ODEAxleConnector::CTRL_TORQUE );
  
  // end effector
  controls[CTRL_ENDEFFECTOR_TORQUE_X] =
    endEffector->getConnector( EndEffector::CONN_BASE )->makeControlProxy
    ( ODEBallConnector::CTRL_TORQUE_X );
  controls[CTRL_ENDEFFECTOR_TORQUE_Y] =
    endEffector->getConnector( EndEffector::CONN_BASE )->makeControlProxy
    ( ODEBallConnector::CTRL_TORQUE_Y );
  controls[CTRL_ENDEFFECTOR_TORQUE_Z] =
    endEffector->getConnector( EndEffector::CONN_BASE )->makeControlProxy
    ( ODEBallConnector::CTRL_TORQUE_Z );
  
}




void Leg::connect()
{
  // lower part --> upper part
  lowerPart->getConnector( LowerPart::CONN_BASE )->connect
    ( upperPart->getConnector( UpperPart::CONN_BOTTOM ),
      Connector::AlignMaster );
  
  // end effector --> lower part
  endEffector->getConnector( EndEffector::CONN_BASE )->connect
    ( lowerPart->getConnector( LowerPart::CONN_BOTTOM ),
      Connector::AlignMaster );
}
