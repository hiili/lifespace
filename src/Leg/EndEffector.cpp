/**
 * @file EndEffector.cpp
 *
 * Implementations for the EndEffector class.
 */
#include "EndEffector.hpp"
#include "Leg.hpp"
#include "../resources.hpp"

#include <lifespace/lifespace.hpp>
using namespace lifespace;

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;




/* joint params */
const real EndEffector::MAX_MOTOR_FORCE_X     = 1.0;
const real EndEffector::LO_STOP_ANGLE_X       = -0.3 * M_PI;
const real EndEffector::HI_STOP_ANGLE_X       = 0.3 * M_PI;
const real EndEffector::STOP_BOUNCINESS_X     = 0.2;
const real EndEffector::MAX_MOTOR_FORCE_Y     = 1.0;
const real EndEffector::LO_STOP_ANGLE_Y       = -0.1 * M_PI;
const real EndEffector::HI_STOP_ANGLE_Y       = 0.25 * M_PI;
const real EndEffector::STOP_BOUNCINESS_Y     = 0.2;
const real EndEffector::MAX_MOTOR_FORCE_Z     = 1.0;
const real EndEffector::LO_STOP_ANGLE_Z       = -0.25 * M_PI;
const real EndEffector::HI_STOP_ANGLE_Z       = 0.25 * M_PI;
const real EndEffector::STOP_BOUNCINESS_Z     = 0.2;




EndEffector::EndEffector()
{
  
  // object properties
  
  // name
  setName( "EndEffector" );
  
  // shapes
  shared_ptr<Shape> shapeMass
    ( shapes::Cube::create( makeVector3d( 0.2, 0.05, 0.3 ) ));
  shared_ptr<Shape> shape
    ( shapes::Union::create
      ( shapeMass,
        shapes::Located::create
        ( BasicLocator( makeVector3d( 0.0, 0.08, 0.05 ) ),
          shapes::Sphere::create( 0.08 ) )));
  
  // locator
  shared_ptr<ODELocator> locator
    ( new ODELocator( makeVector3d(),
                      BasisMatrix(),
                      0.0, 0.0,                  // mass, moment of inertia
                      0.001, 0.002, 0.0001,      // normal drag
                      0.001, 0.002, 0.001 ) );  // rotational drag
  locator->setInertiaShape( shapeMass );
  locator->setDensity( 10.0 );
  
  // set object properties
  setLocator( locator );
  setVisual( sptr( new BasicVisual( shape, Leg::MainVisMaterial.get() ) ));
  setGeometry( sptr( new BasicGeometry( shape, Leg::MainPhysMaterial ) ));
  
  
  // connectors
  
  connectors[CONN_BASE] =
    sptr( new ODEBallConnector
          ( Connector( *this, Connector::Master,
                       BasicLocator( makeVector3d( 0.0, 0.08, 0.05 ),
                                     BasisMatrix
                                     ( makeVector3d( 0.0, -1.0, 0.0 ),
                                       makeVector3d( 0.0, 0.0, -1.0 ) ))),
            ODEMotorAxisParams
            ( MAX_MOTOR_FORCE_X, LO_STOP_ANGLE_X, HI_STOP_ANGLE_X, STOP_BOUNCINESS_X ),
            ODEMotorAxisParams
            ( MAX_MOTOR_FORCE_Y, LO_STOP_ANGLE_Y, HI_STOP_ANGLE_Y, STOP_BOUNCINESS_Y ),
            ODEMotorAxisParams
            ( MAX_MOTOR_FORCE_Z, LO_STOP_ANGLE_Z, HI_STOP_ANGLE_Z, STOP_BOUNCINESS_Z ) ));
  
}
