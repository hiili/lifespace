/**
 * @file UpperPart.cpp
 *
 * Implementations for the UpperPart class.
 */
#include "UpperPart.hpp"
#include "Leg.hpp"
#include "../resources.hpp"

#include <lifespace/lifespace.hpp>
using namespace lifespace;

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;




/* joint params */
const real UpperPart::MAX_MOTOR_FORCE_X     = 1.0;
const real UpperPart::LO_STOP_ANGLE_X       = -0.5 * M_PI;
const real UpperPart::HI_STOP_ANGLE_X       = 0.45 * M_PI;
const real UpperPart::STOP_BOUNCINESS_X     = 0.2;
const real UpperPart::MAX_MOTOR_FORCE_Y     = 1.0;
const real UpperPart::LO_STOP_ANGLE_Y       = -0.1 * M_PI;
const real UpperPart::HI_STOP_ANGLE_Y       = 0.45 * M_PI;
const real UpperPart::STOP_BOUNCINESS_Y     = 0.2;




UpperPart::UpperPart()
{
  
  // object properties
  
  // name
  setName( "UpperPart" );
  
  // shapes
  shared_ptr<Shape> shapeMass
    ( shapes::Cube::create( makeVector3d( 0.2, 0.5, 0.05 ) ));
  shared_ptr<Shape> shape
    ( shapes::Union::create
      ( shapeMass,
        shapes::Located::create
        ( BasicLocator( makeVector3d( 0.0, 0.25, 0.0 ) ),
          shapes::Sphere::create( 0.11 ) )));
  
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
    sptr( new ODEUniversalConnector
          ( Connector( *this, Connector::Master,
                       BasicLocator( makeVector3d( 0.0, 0.25, 0.0 ),
                                     BasisMatrix
                                     ( makeVector3d( 0.0, 1.0, 0.0 ),
                                       makeVector3d( 0.0, 0.0, 1.0 ) ))),
            ODEMotorAxisParams
            ( MAX_MOTOR_FORCE_X, LO_STOP_ANGLE_X, HI_STOP_ANGLE_X, STOP_BOUNCINESS_X ),
            ODEMotorAxisParams
            ( MAX_MOTOR_FORCE_Y, LO_STOP_ANGLE_Y, HI_STOP_ANGLE_Y, STOP_BOUNCINESS_Y ) ));
  
  connectors[CONN_BOTTOM] =
    sptr( new Connector( *this, Connector::Slave,
                         BasicLocator( makeVector3d( 0.0, -0.25, 0.0 ),
                                       BasisMatrix
                                       ( makeVector3d( 1.0, 0.0, 0.0 ),
                                         makeVector3d( 0.0, 1.0, 0.0 ) ))));
  
}
