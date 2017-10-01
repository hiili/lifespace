/**
 * @file LowerPart.cpp
 *
 * Implementations for the LowerPart class.
 */
#include "LowerPart.hpp"
#include "Leg.hpp"
#include "../resources.hpp"

#include <lifespace/lifespace.hpp>
using namespace lifespace;

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;




/* joint params */
const real LowerPart::MAX_MOTOR_FORCE     = 1.0;
const real LowerPart::LO_STOP_ANGLE       = -0.90 * M_PI;
const real LowerPart::HI_STOP_ANGLE       = -0.05 * M_PI;
const real LowerPart::STOP_BOUNCINESS     = 0.2;




LowerPart::LowerPart()
{
  
  // object properties
  
  // name
  setName( "LowerPart" );
  
  // shapes
  shared_ptr<Shape> shapeMass
    ( shapes::Cube::create( makeVector3d( 0.15, 0.5, 0.05 ) ));
  shared_ptr<Shape> shape
    ( shapes::Union::create
      ( shapeMass,
        shapes::Located::create
        ( BasicLocator( makeVector3d( 0.0, 0.25, 0.0 ),
                        BasisMatrix( makeVector3d( 1.0, 0.0, 0.0 ),
                                     makeVector3d( 0.0, 1.0, 0.0 ) )),
          shapes::CappedCylinder::create( 0.2, 0.05 ) )));
  
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
    sptr( new ODEAxleConnector
          ( Connector( *this, Connector::Master,
                       BasicLocator( makeVector3d( 0.0, 0.25, 0.0 ),
                                     BasisMatrix
                                     ( makeVector3d( -1.0, 0.0, 0.0 ),
                                       makeVector3d( 0.0, 1.0, 0.0 ) ))),
            MAX_MOTOR_FORCE, LO_STOP_ANGLE, HI_STOP_ANGLE, STOP_BOUNCINESS ));
  
  connectors[CONN_BOTTOM] =
    sptr( new Connector( *this, Connector::Slave,
                         BasicLocator( makeVector3d( 0.0, -0.25, 0.0 ),
                                       BasisMatrix
                                       ( makeVector3d( 0.0, 1.0, 0.0 ),
                                         makeVector3d( 0.0, 0.0, -1.0 ) ))));
  
}
