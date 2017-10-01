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
 * A test setup which connects three ODE objects with an ODEAxleConnector and
 * an ODESliderConnector. Both connectors and the middle object (which is a
 * FloatingActor) can be controlled from the keyboard.
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


static void fillSsKeymap( Controller::ControlMap & keymap )
{
  keymap['i'] = Controller::MapTarget(FloatingActor::CTRL_FORCEREL_X_NEG, 1.0);
  keymap['k'] = Controller::MapTarget(FloatingActor::CTRL_FORCEREL_X_POS, 1.0);
  keymap['j'] = Controller::MapTarget(FloatingActor::CTRL_FORCEREL_Z_POS, 1.0);
  keymap['l'] = Controller::MapTarget(FloatingActor::CTRL_FORCEREL_Z_NEG, 1.0);
  keymap['u'] = Controller::MapTarget(FloatingActor::CTRL_FORCEREL_Y_POS, 1.0);
  keymap['o'] = Controller::MapTarget(FloatingActor::CTRL_FORCEREL_Y_NEG, 1.0);
}


static void fillSsJoint1Keymap( Controller::ControlMap & keymap )
{
  keymap['t'] = Controller::MapTarget( ODEAxleConnector::CTRL_TORQUE, 1.0 );
  keymap['g'] = Controller::MapTarget( ODEAxleConnector::CTRL_TORQUE, -1.0 );
}


static void fillSsJoint2Keymap( Controller::ControlMap & keymap )
{
  keymap['y'] = Controller::MapTarget( ODESliderConnector::CTRL_FORCE, 1.0 );
  keymap['h'] = Controller::MapTarget( ODESliderConnector::CTRL_FORCE, -1.0 );
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
  public Subspace,
  public FloatingActor
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
  TestObjectSs( const Object::Params & objectParams ) :
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
    connectors[CONN_Y_AXIS_TIP] = shared_ptr<Connector>
      ( new ODESliderConnector
        ( Connector( *this, Connector::Any,
                     BasicLocator
                     ( makeVector3d( 0.0, 2.0, 0.0 ),
                       BasisMatrix
                       ( makeVector3d( 0.0, 1.0, 0.0 ),
                         makeVector3d( 0.0, 0.0, 1.0 ) ))),
          1.0, -2.0, 0.0, 0.2 ) );
    
    /* init the actor */
    
    
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
    
    Subspace::localPrepare( dt );
    FloatingActor::localPrepare( dt );
    Object::prepare( dt );
  }
  
  virtual void step()
  {
    Subspace::localStep();
    FloatingActor::localStep();
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
 * The slider-connected non-middle object of the rotating basis vector
 * system. This object has only one connector (at the tip of the x axis) and is
 * inherited directly from Object (instead of a Subspace).
 */
class TestObjectSlider :
  public Object
{
 public:
  
  enum Connectors {
    CONN_X_AXIS_TIP
  };
  
  /**
   * Constructs a new test object with a connector at the tip of the x axis
   */
  TestObjectSlider( const Object::Params & objectParams ) :
    Object( objectParams )
  {
    // create and insert the x axis connector
    connectors[CONN_X_AXIS_TIP] = shared_ptr<Connector>
      ( new ODESliderConnector
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
  if( argc != 4 ) {
    cout << "Usage: " << argv[0] << " <slider cfm> <slider stop erp> "
         << "<slider stop cfm>" << endl;
    return 1;
  }
  
  GLOWDevice::Init( argc, argv );
  GLOWDevice window;
  GLOWViewport viewport( window );
  
  // world
  ODEWorld world;
  
  
  // subspace for the 2nd and 3rd objects
  Subspace objectSpace
    ( Object::Params( new BasicLocator( makeVector3d( 0.0, -3.0, 0.0 ) ) ) );
  world.addObject( &objectSpace );
  
  // 2nd basis object (the rotating one)
  Subspace * object2 = new TestObjectSs
    ( Object::Params
      ( new ODELocator( makeVector3d( 0.0, 0.0, -5.0 ) ),
        new BasicVisual
        ( sptr( new shapes::Scaled( makeVector3d( 2.0, 2.0, 2.0 ),
                                    sptr( new shapes::Basis ))),
          &redMat )));
  objectSpace.addObject( object2 );
  Controller::ControlMap ssKeymap; fillSsKeymap( ssKeymap );
  viewport.addActor( dynamic_cast<Actor *>(object2),
                     &ssKeymap );
  dynamic_cast<FloatingActor *>(object2)->setAutoRoll( false );
  Controller::ControlMap ssJoint1Keymap; fillSsJoint1Keymap( ssJoint1Keymap );
  viewport.addActor( &object2->getConnector( TestObjectSs::CONN_X_AXIS_TIP ),
                     &ssJoint1Keymap );
  Controller::ControlMap ssJoint2Keymap; fillSsJoint2Keymap( ssJoint2Keymap );
  viewport.addActor( &object2->getConnector( TestObjectSs::CONN_Y_AXIS_TIP ),
                     &ssJoint2Keymap );
  
  
  // 4th basis object, connected to the 2nd (initial position is displaced)
  Object * object4 = new TestObjectAxle
    ( Object::Params
      ( new ODELocator( makeVector3d( 0.0, 1.0, -5.0 ) ),
        new BasicVisual
        ( sptr( new shapes::Scaled( makeVector3d( 2.0, 2.0, 2.0 ),
                                    sptr( new shapes::Basis ))),
          &whiteMat )));
  //objectSpace.addObject( object4 );
  world.addObject( object4 );
  //object2->addObject( object4 );

  // 5th basis object, connected to the 2nd's y axis and inserted into its
  // subspace (initial position is displaced)
  Object * object5 = new TestObjectSlider
    ( Object::Params
      ( new ODELocator( makeVector3d( 0.0, 1.0, -10.0 ) ),
        new BasicVisual
        ( sptr( new shapes::Scaled( makeVector3d( 2.0, 2.0, 2.0 ),
                                    sptr( new shapes::Basis ))),
          &whiteMat )));
  //object2->addObject( object5 );
  world.addObject( object5 );
  
  world.activate( true );
  
  
  // make it possible to connect before activating..
  
  object2->getConnector(TestObjectSs::CONN_X_AXIS_TIP).connect
    ( object4->getConnector(TestObjectAxle::CONN_X_AXIS_TIP) );
  object2->getConnector(TestObjectSs::CONN_Y_AXIS_TIP).connect
    ( object5->getConnector(TestObjectSlider::CONN_X_AXIS_TIP) );
  
  ((ODESliderConnector &)
   (object2->getConnector(TestObjectSs::CONN_Y_AXIS_TIP)))
    .setParam( dParamCFM, atof(argv[1]) );
  ((ODESliderConnector &)
   (object2->getConnector(TestObjectSs::CONN_Y_AXIS_TIP)))
    .setParam( dParamStopERP, atof(argv[2]) );
  ((ODESliderConnector &)
   (object2->getConnector(TestObjectSs::CONN_Y_AXIS_TIP)))
    .setParam( dParamStopCFM, atof(argv[3]) );
  
  
  /*
  object2->getConnector(TestObjectSs::CONN_X_AXIS_TIP).useControl
    ( ODEAxleConnector::CTRL_MOTOR_TARGETVEL, 0.0 );
  object2->getConnector(TestObjectSs::CONN_X_AXIS_TIP).useControl
    ( ODEAxleConnector::CTRL_MOTOR_FORCEFACTOR, 1.0 );
  object2->getConnector(TestObjectSs::CONN_Y_AXIS_TIP).useControl
    ( ODEAxleConnector::CTRL_MOTOR_TARGETVEL, 0.0 );
  object2->getConnector(TestObjectSs::CONN_Y_AXIS_TIP).useControl
    ( ODEAxleConnector::CTRL_MOTOR_FORCEFACTOR, 1.0 );
  */
  
  
  // assertion test: Assertion `!isLockedToHostSpace()' failed.
  //world.removeObject( object3 );
  
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
  
  //Subspace camSpace( Object( new InertiaLocator ) );
  //world.addObject( &camSpace );
  //camSpace.addObject( camTarget );
  world.addObject( camTarget );
  viewport.addActor( camTarget, &camKeymap );
  
  Camera cam;
  cam.setTargetObject( camTarget );
  viewport.setCamera( &cam );
  
  
  //object2->getLocator()->addTorqueAbs( makeVector3d( -500.0, 0.0, 0.0 ) );
  
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
