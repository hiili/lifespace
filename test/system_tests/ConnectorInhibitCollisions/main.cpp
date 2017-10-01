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
 * @file main.cpp
 *
 * A test case for checking the Object's inhibitCollisions flag.
 *
 * Expected results:
 *   - The pill should rotate freely when the master does inhibit collisions.
 *   - The slave's inhibit mode should not affect the result.
 * (when the base is master, the connection can be controlled with f and r key,
 * otherwise the pill can be rotated by pushing it with the camera)
 */

#include <lifespace/lifespace.hpp>
using namespace lifespace;

#include <lifespace/plugins/glow.hpp>
using namespace lifespace::plugins::pglow;

#include <ode/ode.h>
#include <ode/odecpp.h>

#include <string>
#include <iostream>
using std::string;
using std::cout;
using std::endl;

#include <GL/gl.h>

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;
using boost::dynamic_pointer_cast;


/* Check that the current ODE is erp-patched and the patch version is ok. */
#ifndef ODE_VERSION_ERPPATCH
#error This program requires an erp-patched version of ODE!
#else
#if ODE_VERSION_ERPPATCH < 1
#error The ODEs erp-patch version must be >= 1!
#endif
#endif




/* Change these to test different combinations. */
#define BASE_INHIBIT_COLLISIONS true
#define PILL_INHIBIT_COLLISIONS true








static const GLfloat none[4]        = { 0.0, 0.0, 0.0, 0.0 };
static const GLfloat white[4]       = { 1.0, 1.0, 1.0, 1.0 };
static const GLfloat gray6[4]       = { 0.6, 0.6, 0.6, 1.0 };
static const GLfloat gray3[4]       = { 0.3, 0.3, 0.3, 1.0 };
static const GLfloat polished[1]    = { 40.0 };
static const Material whiteMat( gray3, gray6, white,
                                none, polished, GL_FRONT );
static const Material brightWhiteMat( white, white, white,
                                      none, polished, GL_FRONT );


static void fillCameraKeymap( Controller::ControlMap & keymap )
{
  // define the keyboard controls
  keymap['a'] = Controller::MapTarget(FloatingActor::CTRL_FORCEREL_X_NEG, 1.0);
  keymap['d'] = Controller::MapTarget(FloatingActor::CTRL_FORCEREL_X_POS, 1.0);
  keymap['s'] = Controller::MapTarget(FloatingActor::CTRL_FORCEREL_Z_POS, 1.0);
  keymap['w'] = Controller::MapTarget(FloatingActor::CTRL_FORCEREL_Z_NEG, 1.0);
  keymap[' '] = Controller::MapTarget(FloatingActor::CTRL_FORCEREL_Y_POS, 1.0);
  keymap['<'] = Controller::MapTarget(FloatingActor::CTRL_FORCEREL_Y_NEG, 1.0);
  
  // define the mouse controls. the last argument controls the sensitivity.
  keymap[GLOWViewport::MOUSE_DX] =
    Controller::MapTarget( FloatingActor::CTRL_TORQUEREL_Y_NEG, 0.02 );
  keymap[GLOWViewport::MOUSE_DY] =
    Controller::MapTarget( FloatingActor::CTRL_TORQUEREL_X_NEG, 0.02 );
}


static void fillAxleKeymap( Controller::ControlMap & keymap )
{
  keymap['r'] = Controller::MapTarget( ODEAxleConnector::CTRL_TORQUE, 1.0 );
  keymap['f'] = Controller::MapTarget( ODEAxleConnector::CTRL_TORQUE, -1.0 );
}








/**
 */
class Base :
  public Object
{
  shared_ptr<Shape> shape;
  
  
public:
  
  enum Connectors {
    CONN_TOP
  };
  
  /**
   */
  Base( Locator * locator, bool inhibit ) :
    Object( Object::Params( locator ) )
  {
    shape = shapes::Cube::create( makeVector3d( 1.0, 2.0, 1.0 ) );
    setVisual( sptr( new BasicVisual( shape, &whiteMat )));
    setGeometry( sptr( new BasicGeometry( shape, sptr( new CollisionMaterial
                                                       ( 1.0, 0.5, 0.0 )))));
    
    connectors[CONN_TOP] = shared_ptr<Connector>
      ( new ODEAxleConnector
        ( Connector( *this, Connector::Any,
                     BasicLocator
                     ( makeVector3d( 0.0, 1.1, 0.0 ),
                       BasisMatrix
                       ( makeVector3d( sin(-M_PI/8.0), cos(-M_PI/8.0), 0.0 ),
                         makeVector3d( 0.0, 0.0, 1.0 ))),
                     inhibit ),
          1.0, -INFINITY, INFINITY, 0.5 ));
  }
};




/**
 */
class Pill :
  public Object
{
  shared_ptr<Shape> shape;
  
  
public:
  
  enum Connectors {
    CONN_MIDDLE
  };
  
  /**
   * Constructs a new test object with a connector at the tip of the x axis
   */
  Pill( Locator * locator, bool inhibit ) :
    Object( Object::Params( locator ) )
  {
    real length = 2.0, radius = 0.5;
    
    shape = shapes::CappedCylinder::create( length, radius );
    setVisual( sptr( new BasicVisual( shape, &whiteMat )));
    setGeometry( sptr( new BasicGeometry( shape, sptr( new CollisionMaterial
                                                       ( 1.0, 0.5, 0.0 )))));
    
    connectors[CONN_MIDDLE] = shared_ptr<Connector>
      ( new ODEAxleConnector
        ( Connector( *this, Connector::Any,
                     BasicLocator
                     ( makeVector3d( 0.0, -radius, 0.0 ),
                       BasisMatrix
                       ( makeVector3d( 0.0, -1.0, 0.0 ),
                         makeVector3d( 0.0, 0.0, 1.0 ))),
                     inhibit ),
          1.0, -INFINITY, INFINITY, 0.5 ));
  }
};








int main( int argc, char * argv[] )
{
  if( argc != 4 ) {
    cout << "Usage: " << argv[0] << " master=base|pill "
         << "base-inhibit=true|false pill-inhibit=true|false" << endl;
    return 1;
  }
  
  bool baseIsMaster;
  if( 0 == strcmp( argv[1], "master=base" ) ) {
    baseIsMaster = true;
  } else if( 0 == strcmp( argv[1], "master=pill" ) ) {
    baseIsMaster = false;
  } else {
    cout << "Unable to parse master setup: " << argv[1] << "!" << endl;
    exit(1);
  }

  bool baseInhibit;
  if( 0 == strcmp( argv[2], "base-inhibit=true" ) ) {
    baseInhibit = true;
  } else if( 0 == strcmp( argv[2], "base-inhibit=false" ) ) {
    baseInhibit = false;
  } else {
    cout << "Unable to parse base-inhibit setup: " << argv[2] << "!" << endl;
    exit(1);
  }

  bool pillInhibit;
  if( 0 == strcmp( argv[3], "pill-inhibit=true" ) ) {
    pillInhibit = true;
  } else if( 0 == strcmp( argv[3], "pill-inhibit=false" ) ) {
    pillInhibit = false;
  } else {
    cout << "Unable to parse pill-inhibit setup: " << argv[3] << "!" << endl;
    exit(1);
  }
  
  
  GLOWDevice::Init( argc, argv );
  GLOWDevice window;
  GLOWViewport viewport( window );
  
  // world
  ODEWorld world;
  world.setGravityVector( makeVector3d( 0.0, -2.5, 0.0 ) );
  
  // collision detection
  ODECollisionRenderer collisionRenderer( &world );
  
  
  
  
  shared_ptr<Shape> camBasisShape
    ( new shapes::Located
      ( BasicLocator( makeVector3d( -0.3, -0.5, -1.5 ),
                      BasisMatrix
                      ( makeVector3d( 0.0, 0.0, -1.0 ),
                        makeVector3d( 0.0, 1.0, 0.0 ))),
        sptr( new shapes::Scaled
              ( makeVector3d( 1.0, 1.0, 1.0 ),
                sptr( new shapes::Basis )))));
  shared_ptr<BasicGeometry> camGeom
    ( new BasicGeometry
      ( camBasisShape, sptr( new CollisionMaterial( 1.0, 0.2, 0.0 ))));
  shared_ptr<BasicVisual> camVis
    ( new BasicVisual
      ( camBasisShape, &whiteMat ));
  
  
  
  
  Base * base = new Base( new BasicLocator( makeVector3d( 0.0, 0.0, -10.0 )),
                          baseInhibit );
  world.addObject( base );
  Pill * pill = new Pill( new ODELocator(), pillInhibit );
  world.addObject( pill );
  
  Controller::ControlMap axleKeymap; fillAxleKeymap( axleKeymap );
  viewport.addActor( &base->getConnector( Base::CONN_TOP ), &axleKeymap );
  
  
  
  
  // lights
  Object * lightObject = new Object
    ( Object::Params( new BasicLocator( makeVector3d( 2.0, 2.0, -2.0 ) ),
                      new BasicVisual( shapes::Sphere::create( 0.5 ),
                                       &brightWhiteMat )));
  world.addObject( lightObject );
  GLfloat lightAttenuation[3] = { 1.0, 0.0, 0.02 };
  Light * light = new Light( &brightWhiteMat, lightObject, lightAttenuation );
  world.getEnvironment()->addLight( light );
  
  // camera
  FloatingActor * cameraObject = new FloatingActor
    ( Object::Params
      ( sptr( new ODELocator
              ( makeVector3d(), BasisMatrix(),
                1.0, 2.0,
                0.01, 0.2, 1.0,     // normal drag
                0.1, 0.5, 2.0 )),   // rotation drag
        camVis,
        camGeom ));
  world.addObject( cameraObject );
  Controller::ControlMap cameraKeymap; fillCameraKeymap( cameraKeymap );
  viewport.addActor( cameraObject, &cameraKeymap );
  Camera * camera = new Camera;
  camera->setTargetObject( cameraObject );
  viewport.setCamera( camera );
  
  
  
  
  // activate the world
  world.activate( true );
  collisionRenderer.connect();
  
  
  // connect the connectors
  if( baseIsMaster ) {
    base->getConnector(Base::CONN_TOP).connect
      ( pill->getConnector(Pill::CONN_MIDDLE) );
  } else {
    pill->getConnector(Pill::CONN_MIDDLE).connect
      ( base->getConnector(Base::CONN_TOP), Connector::AlignMaster );
  }
  
  
  // disable gravity from camera
  dynamic_pointer_cast<ODELocator>( cameraObject->getLocator() )->
    setGravityEnabled( false );
  
  
  
  
  // start the system
  world.setDefaultDt( 0.05 );
  window.events.addListener( &collisionRenderer );   // order is important!
  window.events.addListener( &world );               // order is important!
  GLOWDevice::MainLoop();
  
  // this is never reached
  return 0;
}
