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
 * A test setup which connects three ODE objects with an ODEAxleConnector and a
 * motored ODEBallConnector.
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


static void fillCameraKeymap( Controller::ControlMap & keymap )
{
  // define the keyboard controls
  keymap['a'] = Controller::MapTarget(FloatingActor::CTRL_FORCEREL_X_NEG, 2.0);
  keymap['d'] = Controller::MapTarget(FloatingActor::CTRL_FORCEREL_X_POS, 2.0);
  keymap['s'] = Controller::MapTarget(FloatingActor::CTRL_FORCEREL_Z_POS, 2.0);
  keymap['w'] = Controller::MapTarget(FloatingActor::CTRL_FORCEREL_Z_NEG, 2.0);
  keymap[' '] = Controller::MapTarget(FloatingActor::CTRL_FORCEREL_Y_POS, 2.0);
  keymap['<'] = Controller::MapTarget(FloatingActor::CTRL_FORCEREL_Y_NEG, 2.0);
  
  // define the mouse controls. the last argument controls the sensitivity.
  keymap[GLOWViewport::MOUSE_DX] =
    Controller::MapTarget( FloatingActor::CTRL_TORQUEREL_Y_NEG, 0.4 );
  keymap[GLOWViewport::MOUSE_DY] =
    Controller::MapTarget( FloatingActor::CTRL_TORQUEREL_X_NEG, 0.4 );
}


static void fillMoJoint1Keymap( Controller::ControlMap & keymap )
{
  keymap['r'] = Controller::MapTarget( ODEAxleConnector::CTRL_TORQUE, 1.0 );
  keymap['f'] = Controller::MapTarget( ODEAxleConnector::CTRL_TORQUE, -1.0 );
}


static void fillMoJoint2Keymap( Controller::ControlMap & keymap )
{
  keymap['t'] = Controller::MapTarget( ODEBallConnector::CTRL_TORQUE_X, 0.5 );
  keymap['g'] = Controller::MapTarget( ODEBallConnector::CTRL_TORQUE_X, -0.5 );
  keymap['y'] = Controller::MapTarget( ODEBallConnector::CTRL_TORQUE_Y, 0.5 );
  keymap['h'] = Controller::MapTarget( ODEBallConnector::CTRL_TORQUE_Y, -0.5 );
  keymap['u'] = Controller::MapTarget( ODEBallConnector::CTRL_TORQUE_Z, 0.5 );
  keymap['j'] = Controller::MapTarget( ODEBallConnector::CTRL_TORQUE_Z, -0.5 );
}




/*
 * The middle object of the rotating basis vector system. This object has two
 * connectors: one at the tip of the X axis (CONN_X_AXIS_TIP) and one at the
 * tip of the Y axis (CONN_Y_AXIS_TIP).
 */
class MiddleObject :
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
  MiddleObject( const Object::Params & objectParams ) :
    Object( objectParams )
  {
    /* init the connectors */
    
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
    /*
    connectors[CONN_Y_AXIS_TIP] = shared_ptr<Connector>
      ( new ODEBallConnector
        ( Connector( *this, Connector::Any,
                     BasicLocator
                     ( makeVector3d( 0.0, 2.0, 0.0 ),
                       BasisMatrix
                       ( makeVector3d( 0.0, 0.0, -1.0 ),
                         makeVector3d( 0.0, 1.0, 0.0 ) )))));
    */
    connectors[CONN_Y_AXIS_TIP] = shared_ptr<Connector>
      ( new ODEBallConnector
        ( Connector( *this, Connector::Any,
                     BasicLocator
                     ( makeVector3d( 0.0, 2.0, 0.0 ),
                       BasisMatrix
                       ( makeVector3d( 0.0, 0.0, -1.0 ),
                         makeVector3d( 0.0, 1.0, 0.0 ) ))),
          ODEMotorAxisParams( 1.0, -0.4 * M_PI, 0.4 * M_PI, 0.5 ),
          ODEMotorAxisParams( 1.0, -0.05 * M_PI, 0.05 * M_PI, 0.5 ),
          ODEMotorAxisParams( 1.0, -0.4 * M_PI, 0.4 * M_PI, 0.5 ) ));
    
    /* init the actor */
    
    
  }
  
  /**
   * Outputs the readings of the sensors of the x axis connector to standard
   * output.
   */
  virtual void prepare( real dt )
  {
    cout << "MiddleObject prepare:" << endl;
    cout << "  X_CONN angle == "
         << getConnector(CONN_X_AXIS_TIP).
      readSensor(ODEAxleConnector::SENS_ANGLE)
         << endl;
    cout << "  X_CONN anglerate == "
         << getConnector(CONN_X_AXIS_TIP).
      readSensor(ODEAxleConnector::SENS_ANGLERATE)
         << endl;
    
    Object::prepare( dt );
  }
  
  virtual void step()
  {
    Object::step();
  }
};


/*
 * The axle-connected non-middle object of the rotating basis vector
 * system. This object has only one connector (at the tip of the x axis) and is
 * inherited directly from Object (instead of a Subspace).
 */
class TestObjectAxle :
  public Object
{
 public:
  
  enum Connectors {
    CONN_X_AXIS_TIP
  };
  
  /**
   * Constructs a new test object with a connector at the tip of the x axis
   */
  TestObjectAxle( const Object::Params & objectParams ) :
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
 * The balljoint-connected non-middle object of the rotating basis vector
 * system. This object has only one connector (at the tip of the x axis) and is
 * inherited directly from Object (instead of a Subspace).
 */
class TestObjectBall :
  public Object
{
 public:
  
  enum Connectors {
    CONN_X_AXIS_TIP
  };
  
  /**
   * Constructs a new test object with a connector at the tip of the x axis
   */
  TestObjectBall( const Object::Params & objectParams ) :
    Object( objectParams )
  {
    // create and insert the x axis connector
    connectors[CONN_X_AXIS_TIP] = shared_ptr<Connector>
      ( new Connector( *this, Connector::Slave,
                       BasicLocator
                       ( makeVector3d( 0.0, 0.0, 0.0 ),
                         BasisMatrix
                         ( makeVector3d( 0.0, 0.0, 1.0 ),
                           makeVector3d( 0.0, 1.0, 0.0 ) ))));
  }
};








int main( int argc, char * argv[] )
{
  GLOWDevice::Init( argc, argv );
  GLOWDevice window;
  GLOWViewport viewport( window );
  
  // world
  ODEWorld world;
  
  
  
  
  Object * middleObject = new MiddleObject
    ( Object::Params
      ( new ODELocator( makeVector3d( -1.0, -2.0, -5.0 ) ),
        new BasicVisual
        ( sptr( new shapes::Scaled( makeVector3d( 2.0, 2.0, 2.0 ),
                                    sptr( new shapes::Basis ))),
          &redMat )));
  world.addObject( middleObject );
  Controller::ControlMap moJoint1Keymap; fillMoJoint1Keymap( moJoint1Keymap );
  viewport.addActor
    ( &middleObject->getConnector( MiddleObject::CONN_X_AXIS_TIP ),
      &moJoint1Keymap );
  Controller::ControlMap moJoint2Keymap; fillMoJoint2Keymap( moJoint2Keymap );
  viewport.addActor
    ( &middleObject->getConnector( MiddleObject::CONN_Y_AXIS_TIP ),
      &moJoint2Keymap );
  
  Object * axleObject = new TestObjectAxle
    ( Object::Params
      ( new ODELocator( makeVector3d( 0.0, 1.0, -5.0 ) ),
        new BasicVisual
        ( sptr( new shapes::Scaled( makeVector3d( 2.0, 2.0, 2.0 ),
                                    sptr( new shapes::Basis ))),
          &whiteMat )));
  world.addObject( axleObject );

  Object * ballObject = new TestObjectBall
    ( Object::Params
      ( new ODELocator( makeVector3d( 0.0, 1.0, -10.0 ) ),
        new BasicVisual
        ( sptr( new shapes::Scaled( makeVector3d( 2.0, 2.0, 2.0 ),
                                    sptr( new shapes::Basis ))),
          &whiteMat )));
  world.addObject( ballObject );
  
  
  
  
  world.activate( true );
  
  
  middleObject->getConnector(MiddleObject::CONN_X_AXIS_TIP).connect
    ( axleObject->getConnector(TestObjectAxle::CONN_X_AXIS_TIP) );
  middleObject->getConnector(MiddleObject::CONN_Y_AXIS_TIP).connect
    ( ballObject->getConnector(TestObjectBall::CONN_X_AXIS_TIP) );
  
  
  // light
  Object lightObj
    ( Object::Params( new InertiaLocator( makeVector3d( 2.0, 2.0, -3.0 ) ),
                      new BasicVisual( sptr( new shapes::Sphere ),
                                       &redEmissionMat )));
  world.addObject( &lightObj );
  GLfloat lightatt[3] = { 1.0, 0.0, 0.02 };
  Light light( &whiteMat, &lightObj, lightatt );
  world.getEnvironment()->addLight( &light );
  
  
  // camera
  
  FloatingActor * camTarget = new FloatingActor;
  Controller::ControlMap camKeymap; fillCameraKeymap( camKeymap );
  world.addObject( camTarget );
  viewport.addActor( camTarget, &camKeymap );
  
  Camera cam;
  cam.setTargetObject( camTarget );
  viewport.setCamera( &cam );
  
  
  world.setDefaultDt( 0.05 );
  window.events.addListener( &world );
  
  GLOWDevice::MainLoop();
  
  return 0;
}
