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
 * A test setup which uses connectors with ODE objects and tests the hierarchy
 * conversion between Lifespace and ODE (the world setup contains unnecessary
 * subspaces which exist just to test that the hierarchy works).
 */

#include <lifespace/lifespace.hpp>
using namespace lifespace;

#include <lifespace/plugins/glow.hpp>
using namespace lifespace::plugins::pglow;

#include <iostream>
using std::cout;
using std::endl;

#include <GL/gl.h>

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;




static const GLfloat none[4]        = { 0.0, 0.0, 0.0, 0.0 };
static const GLfloat white[4]       = { 1.0, 1.0, 1.0, 1.0 };
static const GLfloat gray6[4]       = { 0.6, 0.6, 0.6, 1.0 };
static const GLfloat gray3[4]       = { 0.3, 0.3, 0.3, 1.0 };
static const GLfloat red[4]         = { 1.0, 0.0, 0.0, 1.0 };
static const GLfloat polished[1]    = { 40.0 };
static const Material redMat( none, red, white,
			      gray3, polished, GL_FRONT );
static const Material whiteMat( none, gray6, white,
                                gray3, polished, GL_FRONT );
static const Material redEmissionMat( none, red, white,
			              red, polished, GL_FRONT );
static const Material emissionMat( gray6, gray6, white,
                                   gray6, polished, GL_FRONT );


static void fillCameraKeymap( Actor::Keymap & keymap )
{
  keymap['a'] = Actor::Keydef( FloatingActor::CTRL_MOVE_X_NEG, 1.0 );
  keymap['d'] = Actor::Keydef( FloatingActor::CTRL_MOVE_X_POS, 1.0 );
  keymap['s'] = Actor::Keydef( FloatingActor::CTRL_MOVE_Z_POS, 1.0 );
  keymap['w'] = Actor::Keydef( FloatingActor::CTRL_MOVE_Z_NEG, 1.0 );
  keymap[' '] = Actor::Keydef( FloatingActor::CTRL_MOVE_Y_POS, 1.0 );
  keymap['<'] = Actor::Keydef( FloatingActor::CTRL_MOVE_Y_NEG, 1.0 );
  keymap[GLOWViewport::MOUSE_X] =
    Actor::Keydef( FloatingActor::CTRL_ROTATE_Y_NEG, 0.02 );
  keymap[GLOWViewport::MOUSE_Y] =
    Actor::Keydef( FloatingActor::CTRL_ROTATE_X_NEG, 0.02 );
}




/*
 * The middle object of the rotating basis vector system. This object has two
 * connectors: one at the tip of the X axis (CONN_X_AXIS_TIP) and one at the
 * tip of the Y axis (CONN_Y_AXIS_TIP). It is also a subspace and can be used
 * to test the hierarchy conversion between Lifespace and ODE (just insert the
 * other objects into this subspace, or build some hierarchies with this,
 * etc..).
 */
class TestObjectSs :
  public Subspace
{
 public:
  
  enum Connectors {
    CONN_X_AXIS_TIP,
    CONN_Y_AXIS_TIP,
    ConnectorCount
  };
  
  /**
   * Constructs a new test object with connectors at the tip of the x and y
   * axis.
   */
  TestObjectSs( const Object::Params & objectParams ) :
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
    cout << "TestObjectSs prepare:" << endl;
    cout << "  X_CONN angle == "
         << getConnector(CONN_X_AXIS_TIP).
      readSensor(ODEAxleConnector::SENS_ANGLE)
         << endl;
    cout << "  X_CONN anglerate == "
         << getConnector(CONN_X_AXIS_TIP).
      readSensor(ODEAxleConnector::SENS_ANGLERATE)
         << endl;
    
    Subspace::prepare( dt );
  }
  
};


/*
 * The non-middle objects of the rotating basis vector system. This object has
 * only one connector (at the tip of the x axis) and is inherited directly from
 * Object (instead of a Subspace).
 */
class TestObject :
  public Object
{
 public:
  
  enum Connectors {
    CONN_X_AXIS_TIP,
    ConnectorCount
  };
  
  /**
   * Constructs a new test object with a connector at the tip of the x axis
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
  }
};




int main( int argc, char * argv[] )
{
  GLOWDevice::Init( argc, argv );
  GLOWDevice window;
  GLOWViewport viewport( window );
  
  // world
  ODEWorld world;
  
  /*
  ODEWorld odeWorld;
  world.addObject( &odeWorld );
  */
  
  
  // cube object
  /*
  Object * object =
    new Object( new InertiaLocator,
                new BasicVisual( shapes::Basis::TheBasis,
                                 &redMat ) );
  Subspace objSpace( Object( new InertiaLocator ) );
  world.addObject( &objSpace );
  objSpace.addObject( object );
  //world.addObject( object );
  */
  
  // subspace for the 2nd and 3rd objects
  Subspace objectSpace
    ( Object::Params( new BasicLocator( makeVector3d( 0.0, -3.0, 0.0 ) ) ) );
  world.addObject( &objectSpace );
  
  // 2nd basis object (the rotating one)
  Subspace * object2 = new TestObjectSs
    ( Object::Params( new ODELocator( makeVector3d( 0.0, 0.0, -5.0 ) ),
                      new BasicVisual
                      ( new shapes::Scaled( shapes::Basis::TheBasis,
                                            makeVector3d( 2.0, 2.0, 2.0 ) ),
                        &redMat ) ) );
  objectSpace.addObject( object2 );
  
  // 3rd basis object (the one in the background)
  /*
  Object * object3 = new TestObject
    ( Object::Params( new ODELocator( makeVector3d( 0.0, 1.0, -8.0 ) ),
                      new BasicVisual
                      ( new shapes::Scaled( shapes::Basis::TheBasis,
                                            makeVector3d( 2.0, 2.0, 2.0 ) ),
                        &whiteMat ) ) );
  objectSpace.addObject( object3 );
  */

  // 4th basis object, connected to the 2nd (initial position is displaced)
  Object * object4 = new TestObject
    ( Object::Params( new ODELocator( makeVector3d( 0.0, 1.0, -5.0 ) ),
                      new BasicVisual
                      ( new shapes::Scaled( shapes::Basis::TheBasis,
                                            makeVector3d( 2.0, 2.0, 2.0 ) ),
                        &whiteMat ) ) );
  //objectSpace.addObject( object4 );
  world.addObject( object4 );
  //object2->addObject( object4 );

  // 5th basis object, connected to the 2nd's y axis and inserted into its
  // subspace (initial position is displaced)
  Object * object5 = new TestObject
    ( Object::Params( new ODELocator( makeVector3d( 0.0, 1.0, -10.0 ) ),
                      new BasicVisual
                      ( new shapes::Scaled( shapes::Basis::TheBasis,
                                            makeVector3d( 2.0, 2.0, 2.0 ) ),
                        &whiteMat ) ) );
  //object2->addObject( object5 );
  world.addObject( object5 );
  
  world.activate( true );
  
  
  // make it possible to connect before activating..
  
  object2->getConnector(TestObjectSs::CONN_X_AXIS_TIP).connect
    ( object4->getConnector(TestObject::CONN_X_AXIS_TIP) );
  object2->getConnector(TestObjectSs::CONN_Y_AXIS_TIP).connect
    ( object5->getConnector(TestObject::CONN_X_AXIS_TIP) );
  
  
  // assertion test: Assertion `!isLockedToHostSpace()' failed.
  //world.removeObject( object3 );
  
  // light
  Object lightObj
    ( Object::Params( new InertiaLocator( makeVector3d( 2.0, 2.0, -3.0 ) ),
                      new BasicVisual( shapes::Sphere::TheSphere,
                                       &redEmissionMat ) ) );
  world.addObject( &lightObj );
  GLfloat lightatt[3] = { 1.0, 0.0, 0.02 };
  Light light( &redMat, &lightObj, lightatt );
  world.getEnvironment()->addLight( &light );
  
  
  // camera
  
  FloatingActor * camTarget = new FloatingActor;
  
  Actor::Keymap camKeymap; fillCameraKeymap( camKeymap );
  camTarget->setKeymap( &camKeymap );
  
  //Subspace camSpace( Object( new InertiaLocator ) );
  //world.addObject( &camSpace );
  //camSpace.addObject( camTarget );
  world.addObject( camTarget );
  viewport.addActor( camTarget );
  
  Camera cam;
  cam.setTargetObject( camTarget );
  viewport.setCamera( &cam );
  
  
  //object2->getLocator()->addForceAbs( makeVector3d( 0.0, 0.0, -100.0 ) );
  object2->getLocator()->addTorqueAbs( makeVector3d( -500.0, 0.0, 0.0 ) );
  
  /*
  for( int i = 0 ; i < 10 ; i++ ) {
    cout << object->getLocator()->getLoc() << endl;
    object->getLocator()->addForceAbs( makeVector3d( 0.0, 0.0, -1.0 ) );
    world.timestep( 0.1 );
  }
  */
  
  /*
  objSpace.getLocator()->addTorqueAbs( makeVector3d( 5.0, 0.0, 0.0 ) );
  object->getLocator()->addTorqueAbs( makeVector3d( -10.0, 0.0, 0.0 ) );
  */
  
  world.setDefaultDt( 0.05 );
  window.events.addListener( &world );
  
  /*
  //camTarget->getLocator()->addForceAbs( makeVector3d( 3.0, 0.0, 0.0 ) );
  camSpace.getLocator()->addForceAbs( makeVector3d( 3.0, 0.0, 0.0 ) );
  */
  
  GLOWDevice::MainLoop();
  
  return 0;
}
