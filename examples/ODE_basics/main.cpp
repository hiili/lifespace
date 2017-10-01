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
 * This example demostrates how to use the Open Dynamics Engine and its joints
 * to control the world objects.
 *
 * A world with a light source, a user controllable camera and some
 * interconnected test objects will be created. First, two different test
 * object classes are defined (TestObject and TestObject2). The actual world is
 * built and started in the main() function.
 *
 * Camera movement controls: a,s,d,w,<,space
 * Camera rotation controls: with mouse after "connecting" to the viewport with
 *                           the middle mouse button or F10 (auto-roll is
 *                           always on)
 * Inverse mouse: F9
 * Exit: ESC
 */

#include <lifespace/lifespace.hpp>
using namespace lifespace;

#include <lifespace/plugins/glow.hpp>
using namespace lifespace::plugins::pglow;

#include <iostream>
using std::cout;
using std::endl;

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include <GL/gl.h>




/* Define some colors. */
static const GLfloat none[4]        = { 0.0, 0.0, 0.0, 0.0 };
static const GLfloat white[4]       = { 1.0, 1.0, 1.0, 1.0 };
static const GLfloat gray6[4]       = { 0.6, 0.6, 0.6, 1.0 };
static const GLfloat gray3[4]       = { 0.3, 0.3, 0.3, 1.0 };
static const GLfloat red[4]         = { 1.0, 0.0, 0.0, 1.0 };

/* Define some values for the OpenGL specular parameter. */
static const GLfloat polished[1]    = { 40.0 };

/* Define some basic materials. */
static const Material redMat( none, red, white,
			      gray3, polished, GL_FRONT );
static const Material whiteMat( none, gray6, white,
                                gray3, polished, GL_FRONT );
static const Material redEmissionMat( none, red, white,
			              red, polished, GL_FRONT );
static const Material emissionMat( gray6, gray6, white,
                                   gray6, polished, GL_FRONT );


/* This function fills in the keymap of the camera actor object. Modify this to
   change the keyboard controls and mouse sensitivity. */
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




/*
 * The middle object of the rotating basis vector system. This object has two
 * connectors: one at the tip of the X axis (CONN_X_AXIS_TIP) and one at the
 * tip of the Y axis (CONN_Y_AXIS_TIP). It is also a subspace and can be used
 * to test the hierarchy conversion between Lifespace and ODE (just insert the
 * other objects into this subspace, or build some hierarchies with this,
 * etc..).
 */
class TestObject :
  public Object
{
 public:
  
  enum Connectors {
    CONN_X_AXIS_TIP,
    CONN_Y_AXIS_TIP
  };
  
  /**
   * Constructs a new test object with connectors at the tip of the x and y
   * axis.
   */
  TestObject( const Object::Params & objectParams ) :
    Object( objectParams )
  {
    // create and insert the x axis connector
    connectors[CONN_X_AXIS_TIP] = shared_ptr<Connector>
      ( new ODEAxleConnector
        ( Connector( *this, Connector::Any,
                     BasicLocator
                     ( makeVector3d( 2.0, 0.0, 0.0 ),
                       BasisMatrix
                       ( makeVector3d( 1.0, 0.0, 0.0 ),
                         makeVector3d( 0.0, 1.0, 0.0 ) ))),
          1.0, -INFINITY, INFINITY, 0.5 ) );
    // create and insert the y axis connector
    connectors[CONN_Y_AXIS_TIP] = shared_ptr<Connector>
      ( new ODEAxleConnector
        ( Connector( *this, Connector::Any,
                     BasicLocator
                     ( makeVector3d( 0.0, 2.0, 0.0 ),
                       BasisMatrix
                       ( makeVector3d( 0.0, 1.0, 0.0 ),
                         makeVector3d( 0.0, 0.0, 1.0 ) ))),
          1.0, -INFINITY, INFINITY, 0.5 ) );
  }
  
  /**
   * Outputs the readings of the sensors of the x axis connector to standard
   * output.
   */
  virtual void prepare( real dt )
  {
    /*
    cout << "TestObject prepare:" << endl;
    cout << "  X_CONN angle == "
         << getConnector(CONN_X_AXIS_TIP).
      readSensor(ODEAxleConnector::SENS_ANGLE)
         << endl;
    cout << "  X_CONN anglerate == "
         << getConnector(CONN_X_AXIS_TIP).
      readSensor(ODEAxleConnector::SENS_ANGLERATE)
         << endl;
    */
    
    Object::prepare( dt );
  }
  
};


/*
 * The non-middle objects of the rotating basis vector system. This object has
 * only one connector (at the tip of the x axis) and is inherited directly from
 * Object (instead of a Subspace).
 */
class TestObject2 :
  public Object
{
 public:
  
  enum Connectors {
    CONN_X_AXIS_TIP
  };
  
  /**
   * Constructs a new test object with a connector at the tip of the x axis
   */
  TestObject2( const Object::Params & objectParams ) :
    Object( objectParams )
  {
    // create and insert the x axis connector
    connectors[CONN_X_AXIS_TIP] = shared_ptr<Connector>
      ( new ODEAxleConnector
        ( Connector( *this, Connector::Any,
                     BasicLocator
                     ( makeVector3d( 2.0, 0.0, 0.0 ),
                       BasisMatrix
                       ( makeVector3d( 1.0, 0.0, 0.0 ),
                         makeVector3d( 0.0, 1.0, 0.0 ) ))),
          1.0, -INFINITY, INFINITY, 0.5 ) );
  }
};




/*
 * The actual world will be created and started here.
 */
int main( int argc, char * argv[] )
{
  
  /* Create and init the user interface. */
  GLOWDevice::Init( argc, argv );
  GLOWDevice window;
  GLOWViewport viewport( window );
  
  
  /* Create the world. Use ODEWorld instead of World to allow ODELocators to be
     used. */
  ODEWorld world;
  
  
  /* Create and connect the camera. */
  
  // create an actor object which the user can move
  FloatingActor camObject;
  world.addObject( &camObject );
  
  // connect the input feed (keyboard, mouse, ...) from the viewport to the
  // object
  Controller::ControlMap camKeymap; fillCameraKeymap( camKeymap );
  viewport.addActor( &camObject, &camKeymap );
  
  // create the actual camera, make it follow the camObject and forward its
  // video feed into the viewport.
  Camera camera;
  camera.setTargetObject( &camObject );
  viewport.setCamera( &camera );
  
  
  /* Create a light source into the world. */

  // create an object which defines the location and visual of the light source
  Object lightObject
    ( Object::Params( new InertiaLocator( makeVector3d( 1.5, 2.0, -2.0 )),
                      new BasicVisual( shapes::Sphere::create(),
                                       &redEmissionMat )));
  world.addObject( &lightObject );
  
  // create the actual light and make it follow the lightObject
  GLfloat lightatt[3] = { 1.0, 0.0, 0.02 };
  Light light( &whiteMat, &lightObject, lightatt );
  
  // activate the light into the global environment
  world.getEnvironment()->addLight( &light );
  
  
  /*
   * Now create some objects into the world. Three basis vector objects will be
   * created and connected together with ODEAxleConnectors (an abstraction for
   * the ODE hinge joint).
   */
  
  // create the middle part
  TestObject middlePart
    ( Object::Params
      ( new ODELocator( makeVector3d( -2.0, -2.0, -6.0 ) ),
        new BasicVisual
        ( shapes::Scaled::create( makeVector3d( 2.0, 2.0, 2.0 ),
                                  shapes::Basis::create() ),
          &redMat )));
  world.addObject( &middlePart );
  
  // create the first endpart (Initial position is displaced so that the
  // connectors don't match. When connecting, the objects are moved so that the
  // connectors get aligned.)
  TestObject2 endPart1
    ( Object::Params
      ( new ODELocator( makeVector3d( 0.0, 1.0, -5.0 ) ),
        new BasicVisual
        ( shapes::Scaled::create( makeVector3d( 2.0, 2.0, 2.0 ),
                                  shapes::Basis::create() ),
          &whiteMat )));
  world.addObject( &endPart1 );

  // create the second endpart (initial position is also displaced)
  TestObject2 endPart2
    ( Object::Params
      ( new ODELocator( makeVector3d( 0.0, 1.0, -10.0 ) ),
        new BasicVisual
        ( shapes::Scaled::create( makeVector3d( 2.0, 2.0, 2.0 ),
                                  shapes::Basis::create() ),
          &whiteMat )));
  world.addObject( &endPart2 );
  
  // activate the world (ODEWorld requires this before use and currently
  // _after_ construction, but this limitation is going to change)
  world.activate( true );
  
  
  /* Now connect some connectors (the latter will be moved to align the
     connectors. */
  middlePart.getConnector(TestObject::CONN_X_AXIS_TIP).connect
    ( endPart1.getConnector(TestObject2::CONN_X_AXIS_TIP) );
  middlePart.getConnector(TestObject::CONN_Y_AXIS_TIP).connect
    ( endPart2.getConnector(TestObject2::CONN_X_AXIS_TIP) );
  
  /* Apply a strong torque impulse to the middlePart. */
  middlePart.getLocator()->addTorqueAbs( makeVector3d( -500.0, 0.0, 0.0 ) );
  
  
  /* Finally, set the timestep length and connect the world to the window's
     ticker to receive a step command on every screen refresh. */
  world.setDefaultDt( 0.05 );
  window.events.addListener( &world );
  
  /* Start the system. */
  GLOWDevice::MainLoop();
  
  
  return 0;
}
