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
 * A test setup for testing the ODECollisionRenderer. Three ODE objects are
 * created and connected with an ODEAxleConnector and an ODEBallConnector. The
 * axle connector and the middle object (which is a FloatingActor) can be
 * controlled from the keyboard. A collision shape is attached to the camera
 * object so that it is visible in front of the camera.
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
      ( new ODEBallConnector
        ( Connector( *this, Connector::Any,
                     BasicLocator
                     ( makeVector3d( 0.0, 2.0, 0.0 ),
                       BasisMatrix
                       ( makeVector3d( 0.0, 1.0, 0.0 ),
                         makeVector3d( 0.0, 0.0, 1.0 ) )))) );
    
    /* init the actor */
    
    
  }
  
  virtual void prepare( real dt )
  {
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
      ( new ODEBallConnector
        ( Connector( *this, Connector::Any,
                     BasicLocator
                     ( makeVector3d( 2.0, 0.0, 0.0 ),
                       BasisMatrix
                       ( makeVector3d( 1.0, 0.0, 0.0 ),
                         makeVector3d( 0.0, 1.0, 0.0 ) )))) );
  }
};




/** Prints the camera location on each frame. */
class PrintLoc :
  public EventListener<GraphicsEvent>
{
  string name;
  const Locator * locator;
  
public:
  PrintLoc( string name_, const Locator * locator_ ) :
    name( name_ ),
    locator( locator_ )
  { assert(locator); }
  
  void processEvent( const GraphicsEvent * event )
  {
    if( event->id == GE_REFRESH_BEGIN ) {
      cout << "PrintLoc (" << name << "): loc == "
           << locator->getLoc() << ", isAA == "
           << locator->getBasis().isAxisAligned() << endl;
    }
  }
};








int main( int argc, char * argv[] )
{
  if( argc != 3 ) {
    cout << "Usage: " << argv[0] << " <ball erp> <ball cfm>" << endl;
    return 1;
  }
  
  GLOWDevice::Init( argc, argv );
  GLOWDevice window;
  GLOWViewport viewport( window );
  
  // world
  ODEWorld world;
  
  // collision detection
  ODECollisionRenderer collisionRenderer( &world );
  
  
  
  
  // ---- some test geoms ----
  
  shared_ptr<BasicGeometry> basisGeom
    ( new BasicGeometry
      ( sptr( new shapes::Scaled( makeVector3d( 2.0, 2.0, 2.0 ),
                                  sptr( new shapes::Basis ))),
        sptr( new CollisionMaterial( 1.0, 0.5, 0.0 ))));
  /*
  shared_ptr<BasicGeometry> basisGeom
    ( new BasicGeometry
      ( sptr( new shapes::Basis ),
        sptr( new CollisionMaterial( 1.0, 0.5, 0.0 ))));
  */
  shared_ptr<BasicGeometry> testGeom
    ( new BasicGeometry( sptr( new shapes::Sphere( 1.0 )),
                         sptr( new CollisionMaterial( 1.0, 0.5, 0.0 ))));
  shared_ptr<BasicGeometry> testGeom2
    ( new BasicGeometry
      ( sptr( new shapes::Located
              ( BasicLocator( makeVector3d( 1.0, 1.0, 0.0 )),
                sptr( new shapes::Sphere( 0.5 )))),
        sptr( new CollisionMaterial( 1.0, 0.5, 0.0 ))));
  shared_ptr<Shape> testShape3
    ( new shapes::Located
      ( BasicLocator( makeVector3d( 0.5, 0.0, -1.5 )),
        sptr( new shapes::Cube( makeVector3d( 1.0, 0.2, 0.2 )))));
  shared_ptr<Shape> testShape3x2
    ( new shapes::Located
      ( BasicLocator( makeVector3d( 0.5, 0.0, -1.5 )),
        sptr( new shapes::Scaled
              ( makeVector3d( 2.0, 2.0, 2.0 ),
                sptr( new shapes::Cube( makeVector3d( 1.0, 0.2, 0.2 )))))));
  
  shared_ptr<Shape> camBasisShape
    ( new shapes::Located
      ( BasicLocator( makeVector3d( -0.3, -0.5, -1.5 ),
                      BasisMatrix
                      //( makeVector3d( -1.0, 0.0, -1.0 ),
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
  
  
  
  
  // subspace for the middle part
  Subspace objectSpace
    ( Object::Params( new BasicLocator( makeVector3d( 0.0, -3.0, 0.0 ) ) ) );
  world.addObject( &objectSpace );
  
  
  // middle part
  Subspace * middlePart = new TestObjectSs
    ( Object::Params
      ( sptr( new ODELocator( makeVector3d( 0.0, 0.0, -5.0 ))),
        sptr( new BasicVisual
              ( sptr( new shapes::Scaled( makeVector3d( 2.0, 2.0, 2.0 ),
                                          sptr( new shapes::Basis ))),
                                          &redMat )),
        basisGeom ));
  objectSpace.addObject( middlePart );
  Controller::ControlMap ssKeymap; fillSsKeymap( ssKeymap );
  viewport.addActor( dynamic_cast<Actor *>(middlePart),
                     &ssKeymap );
  dynamic_cast<FloatingActor *>(middlePart)->setAutoRoll( false );
  Controller::ControlMap ssJoint1Keymap; fillSsJoint1Keymap( ssJoint1Keymap );
  viewport.addActor
    ( &middlePart->getConnector( TestObjectSs::CONN_X_AXIS_TIP ),
      &ssJoint1Keymap );
  
  
  // axle-connected end part (initial position is displaced)
  Object * axleEndPart = new TestObjectAxle
    ( Object::Params
      ( sptr( new ODELocator( makeVector3d( 0.0, 1.0, -5.0 ))),
        sptr( new BasicVisual
              ( sptr( new shapes::Scaled( makeVector3d( 2.0, 2.0, 2.0 ),
                                          sptr( new shapes::Basis ))),
                &whiteMat )),
        basisGeom ));
  world.addObject( axleEndPart );

  // ball-connected end part (initial position is displaced)
  Object * ballEndPart = new TestObjectBall
    ( Object::Params
      ( sptr( new ODELocator( makeVector3d( 0.0, 1.0, -10.0 ))),
        sptr( new BasicVisual
              ( sptr( new shapes::Scaled( makeVector3d( 2.0, 2.0, 2.0 ),
                                    sptr( new shapes::Basis ))),
                &whiteMat )),
        basisGeom ));
  world.addObject( ballEndPart );
  
  
  // light object (use double parentheses to avoid a gcc parser 'bug')
  Object lightObj
    (( Object::Params( sptr( new ODELocator( makeVector3d( 2.0, 2.0, -3.0 ))),
                       sptr( new BasicVisual( sptr( new shapes::Sphere ),
                                              &redEmissionMat )),
                       testGeom )));
  world.addObject( &lightObj );
  
  // camera object
  //Shape * camObjectShape =
  //  new Union( sptr( new 
  FloatingActor * camTarget =
    new FloatingActor
    ( Object::Params
      ( sptr( new ODELocator
              ( makeVector3d( 0.0, 0.0, 0.0 ),
                BasisMatrix(3),
                1.0, 2.0,
                0.01, 0.2, 1.0,     // normal drag
                0.1, 0.5, 2.0 )),   // rotation drag
        camVis,
        /*
          sptr( new BasicVisual
              ( sptr( new shapes::Located
                      ( BasicLocator( makeVector3d( -0.3, -0.5, -1.5 ),
                                      BasisMatrix
                                      ( makeVector3d( 0.0, 0.0, -1.0 ),
                                        //( makeVector3d( -1.0, 0.0, -1.0 ),
                                        makeVector3d( 0.0, 1.0, 0.0 ))),
                        sptr( new shapes::Basis ))),
                &redMat )),
        */
        /*
        sptr( new BasicGeometry
              ( sptr( new shapes::Located
                      ( BasicLocator( makeVector3d( -0.3, -0.5, -1.5 ),
                                      BasisMatrix
                                      ( makeVector3d( 0.0, 0.0, -1.0 ),
                                        //( makeVector3d( -1.0, 0.0, -1.0 ),
                                        makeVector3d( 0.0, 1.0, 0.0 ))),
                        sptr( new shapes::Basis ))),
                sptr( new CollisionMaterial( 1.0, 0.5, 0.0 ))))));
        */
        camGeom ));
  //camTarget->setAutoRoll( false );
  Controller::ControlMap camKeymap; fillCameraKeymap( camKeymap );
  world.addObject( camTarget );
  viewport.addActor( camTarget, &camKeymap );
  viewport.events.addListener( new PrintLoc( string("camera"),
                                             camTarget->getLocator().get() ));
  
  
  
  
  world.activate( true );
  collisionRenderer.connect();
  
  
  middlePart->getConnector(TestObjectSs::CONN_X_AXIS_TIP).connect
    ( axleEndPart->getConnector(TestObjectAxle::CONN_X_AXIS_TIP) );
  middlePart->getConnector(TestObjectSs::CONN_Y_AXIS_TIP).connect
    ( ballEndPart->getConnector(TestObjectBall::CONN_X_AXIS_TIP) );
  
  ((ODEBallConnector &)
   (middlePart->getConnector(TestObjectSs::CONN_Y_AXIS_TIP)))
    .setParam( dParamERP, atof(argv[1]) );
  ((ODEBallConnector &)
   (middlePart->getConnector(TestObjectSs::CONN_Y_AXIS_TIP)))
    .setParam( dParamCFM, atof(argv[2]) );
  
  
  
  
  // light
  GLfloat lightatt[3] = { 1.0, 0.0, 0.02 };
  Light light( &whiteMat, &lightObj, lightatt );
  world.getEnvironment()->addLight( &light );
  
  // camera
  Camera cam;
  cam.setTargetObject( camTarget );
  viewport.setCamera( &cam );
  
  
  world.setDefaultDt( 0.05 );
  window.events.addListener( &collisionRenderer );
  window.events.addListener( &world );
  
  GLOWDevice::MainLoop();
  
  return 0;
}
