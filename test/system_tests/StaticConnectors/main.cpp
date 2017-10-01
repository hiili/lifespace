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
 * A test setup for testing connections to non-ODE locators (which represent
 * the "static background" for ODE). Two bar objects are created apart from
 * each other, and two plates are dropped between them. Both bars have a fixed
 * connector at one end. They are connected with an aligning mode which can be
 * selected from the command line (start with no arguments to get the
 * usage). The left bar has a BasicLocator instead of an ODELocator.
 *
 * Some values which produce some meaningful(?) results (compiled with single
 * precision):
 * @code
 *   ./lifespace DontAlign master=left 0.3 0.2
 *   ./lifespace DontAlign master=left 0.4 0.2
 *   ./lifespace DontAlign master=left 0.3 0.25
 *   ./lifespace DontAlign master=left 0.9 0.5
 *   ./lifespace AlignSlave master=left 0.3 0.3
 *   ./lifespace AlignSlave master=left 0.9 1.0
 * @endcode
 */

#include <lifespace/lifespace.hpp>
using namespace lifespace;

#include <lifespace/plugins/glow.hpp>
using namespace lifespace::plugins::pglow;

#include <ode/ode.h>
#include <ode/odecpp.h>

#include <iostream>
using std::cout;
using std::endl;

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;


/* Check that the current ODE is erp-patched and the patch version is ok. */
#ifndef ODE_VERSION_ERPPATCH
#error This program requires an erp-patched version of ODE!
#else
#if ODE_VERSION_ERPPATCH < 1
#error The ODEs erp-patch version must be >= 1!
#endif
#endif




static const GLfloat none[4]        = { 0.0, 0.0, 0.0, 0.0 };
static const GLfloat white[4]       = { 1.0, 1.0, 1.0, 1.0 };
static const GLfloat gray6[4]       = { 0.6, 0.6, 0.6, 1.0 };
static const GLfloat gray3[4]       = { 0.3, 0.3, 0.3, 1.0 };
static const GLfloat red3[4]        = { 0.3, 0.0, 0.0, 1.0 };
static const GLfloat red8[4]        = { 0.8, 0.0, 0.0, 1.0 };
static const GLfloat polished[1]    = { 40.0 };
static const Material whiteMat( gray3, gray6, white,
                                none, polished, GL_FRONT );
static const Material redMat( red3, red8, white,
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




/**
 * A test object with a connector at one end.
 */
class TestObject :
  public Object
{
  shared_ptr<Shape> shape;
  
 public:
  
  enum Connectors {
    CONN_X_AXIS_TIP
  };
  
  TestObject( Locator * locator ) :
    shape( shapes::Cube::create( makeVector3d( 2.0, 0.2, 0.5 )))
  {
    // finish the Object base class
    setLocator( sptr( locator ) );
    setVisual( sptr( new BasicVisual( shape, &whiteMat ) ) );
    setGeometry( sptr( new BasicGeometry( shape, sptr( new CollisionMaterial
                                                       ( 1.0, 0.5, 0.0 ) ))));
    
    // create and insert the x axis connector
    connectors[CONN_X_AXIS_TIP] = shared_ptr<Connector>
      ( new ODEFixedConnector
        ( Connector( *this, Connector::Any,
                     BasicLocator
                     ( makeVector3d( 1.0, 0.0, 0.0 ),
                       BasisMatrix
                       ( makeVector3d( 1.0, 0.0, 0.0 ),
                         makeVector3d( 0.0, 1.0, 0.0 ) )))));
  }
};




int main( int argc, char * argv[] )
{
  if( argc != 5 ) {
    cout << "Usage: " << argv[0] << " AlignSlave|AlignMaster|DontAlign"
         << " master=left|right <connector erp> <connector cfm>" << endl;
    return 1;
  }
  
  Connector::Aligning mode;
  if( 0 == strcmp( argv[1], "AlignSlave" ) ) {
    mode = Connector::AlignSlave;
  } else if( 0 == strcmp( argv[1], "AlignMaster" ) ) {
    mode = Connector::AlignMaster;
  } else if( 0 == strcmp( argv[1], "DontAlign" ) ) {
    mode = Connector::DontAlign;
  } else {
    cout << "Unknown mode: " << argv[1] << "!" << endl;
    exit(1);
  }

  bool leftIsMaster;
  if( 0 == strcmp( argv[2], "master=left" ) ) {
    leftIsMaster = true;
  } else if( 0 == strcmp( argv[2], "master=right" ) ) {
    leftIsMaster = false;
  } else {
    cout << "Unable to parse master setup: " << argv[2] << "!" << endl;
    exit(1);
  }
  
  
  // graphics and user interface
  GLOWDevice::Init( argc, argv );
  GLOWDevice window;
  GLOWViewport viewport( window );
  
  // world
  ODEWorld world;
  world.setGravityVector( makeVector3d( 0.0, -1.5, 0.0 ) );
  
  // collision detection
  ODECollisionRenderer collisionRenderer( &world );
  
  
  
  
  // create the objects
  TestObject * leftObj =
    new TestObject( new BasicLocator
                    ( makeVector3d( -2.0, -0.5, -3.0 ),
                      BasisMatrix( makeVector3d( 0.0, 0.0, -1.0 ),
                                   makeVector3d( 0.0, 1.0, 0.0 ))));
  world.addObject( leftObj );
  TestObject * rightObj =
    new TestObject( new ODELocator
                    ( makeVector3d( 2.0, -0.5, -3.0 ),
                      BasisMatrix( makeVector3d( 1.0, 0.0, 0.0 ),
                                   makeVector3d( 0.0, 1.0, 0.0 ))));
  world.addObject( rightObj );
  Object * wall1 =
    new Object( Object::Params
                ( new ODELocator( makeVector3d( 0.0, 0.0, -3.0 )),
                  new BasicVisual( shapes::Cube::create
                                   ( makeVector3d( 0.2, 2.0, 2.0 )),
                                   &whiteMat ),
                  new BasicGeometry
                  ( shapes::Cube::create
                    ( makeVector3d( 0.2, 2.0, 2.0 )),
                    sptr( new CollisionMaterial( 0.3, 0.5, 0.0 )))));
  world.addObject( wall1 );
  Object * wall2 =
    new Object( Object::Params
                ( new ODELocator( makeVector3d( -0.05, 10.0, -3.0 ),
                                  BasisMatrix( makeVector3d( -0.3, 0.0, -1.0 ),
                                               makeVector3d( 0.0, 1.0, 0.0 )),
                                  3.0, 1.0 ),
                  new BasicVisual( shapes::Cube::create
                                   ( makeVector3d( 0.4, 2.0, 2.0 )),
                                   &redMat ),
                  new BasicGeometry
                  ( shapes::Cube::create
                    ( makeVector3d( 0.4, 2.0, 2.0 )),
                    sptr( new CollisionMaterial( 0.6, 1.0, 0.0 )))));
  world.addObject( wall2 );
  
  
  
  
  // activate the world and collision detection
  world.activate( true );
  collisionRenderer.connect();
  
  
  // connect the connectors
  Object & master = leftIsMaster ? *leftObj : *rightObj;
  Object & slave = !leftIsMaster ? *leftObj : *rightObj;
  master.getConnector(TestObject::CONN_X_AXIS_TIP).connect
    ( slave.getConnector(TestObject::CONN_X_AXIS_TIP), mode );
  ((ODEFixedConnector &)(master.getConnector(TestObject::CONN_X_AXIS_TIP))).
    setParam( dParamERP, atof(argv[3]) );
  ((ODEFixedConnector &)(master.getConnector(TestObject::CONN_X_AXIS_TIP))).
    setParam( dParamCFM, atof(argv[4]) );
  /*
  world.setERP( atof(argv[3]) );
  world.setCFM( atof(argv[4]) );
  */
  
  
  
  
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
  FloatingActor * cameraObject = new FloatingActor;
  world.addObject( cameraObject );
  Controller::ControlMap cameraKeymap; fillCameraKeymap( cameraKeymap );
  viewport.addActor( cameraObject, &cameraKeymap );
  Camera * camera = new Camera;
  camera->setTargetObject( cameraObject );
  viewport.setCamera( camera );
  
  
  // start the system
  world.setDefaultDt( 0.05 );
  window.events.addListener( &collisionRenderer );   // order is important!
  window.events.addListener( &world );               // order is important!
  GLOWDevice::MainLoop();
  
  // this is never reached
  return 0;
}
