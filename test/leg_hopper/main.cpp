/**
 * @file main.cpp
 *
 */

#include <lifespace/lifespace.hpp>
using namespace lifespace;

#include <lifespace/plugins/glow.hpp>
using namespace lifespace::plugins::pglow;

#include <glow/glow.h>

#include <ode/ode.h>
#include <ode/odecpp.h>

#include <iostream>
using std::cout;
using std::endl;

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;
using boost::dynamic_pointer_cast;




#include "sim/resources.hpp"
#include "sim/SimControl.hpp"
#include "sim/RecPlaySystem.hpp"
#include "sim/Floor/Floor.hpp"
#include "sim/Leg/Leg.hpp"




const GLfloat leftLight[4]         = { 0.0, 0.5, 1.0, 1.0 };
const Material leftLightMat( leftLight, leftLight, white,
                             none, polished, GL_FRONT );

const GLfloat legBaseAmbient[4]    = { 0.2, 0.2, 0.0, 1.0 };
const GLfloat legBaseDiffuse[4]    = { 1.0, 1.0, 0.0, 0.5 };
const GLfloat legBaseSpecular[4]   = { 1.0, 1.0, 1.0, 1.0 };
const GLfloat legBaseShininess[1]  = { 10.0 };
const Material legBaseMat( legBaseAmbient, legBaseDiffuse, legBaseSpecular,
                           none, legBaseShininess, GL_FRONT_AND_BACK );




static Controller::ControlMap * newCameraKeymap()
{
  Controller::ControlMap * keymap_ptr = new Controller::ControlMap();
  Controller::ControlMap & keymap = *keymap_ptr;
  
  // define the keyboard controls
  keymap['a'] = Controller::MapTarget(FloatingActor::CTRL_FORCEREL_X_NEG, 5.0);
  keymap['d'] = Controller::MapTarget(FloatingActor::CTRL_FORCEREL_X_POS, 5.0);
  keymap['s'] = Controller::MapTarget(FloatingActor::CTRL_FORCEREL_Z_POS, 5.0);
  keymap['w'] = Controller::MapTarget(FloatingActor::CTRL_FORCEREL_Z_NEG, 5.0);
  keymap[' '] = Controller::MapTarget(FloatingActor::CTRL_FORCEREL_Y_POS, 5.0);
  keymap['<'] = Controller::MapTarget(FloatingActor::CTRL_FORCEREL_Y_NEG, 5.0);

  // define the mouse controls. the last argument controls the sensitivity.
  keymap[GLOWViewport::MOUSE_DX] =
    Controller::MapTarget( FloatingActor::CTRL_TORQUEREL_Y_NEG, 5.0 );
  keymap[GLOWViewport::MOUSE_DY] =
    Controller::MapTarget( FloatingActor::CTRL_TORQUEREL_X_NEG, 5.0 );
  
  return keymap_ptr;
}


static Controller::ControlMap * newSimControlKeymap1()
{
  Controller::ControlMap * keymap_ptr = new Controller::ControlMap();
  Controller::ControlMap & keymap = *keymap_ptr;
  
  keymap[glow::Glow::leftArrowKey] =
    Controller::MapTarget(SimControl::CTRL_INC_DEC, -1.0);
  keymap[glow::Glow::rightArrowKey] =
    Controller::MapTarget(SimControl::CTRL_INC_DEC, 1.0);
  keymap[glow::Glow::upArrowKey] =
    Controller::MapTarget(SimControl::CTRL_SIM_PLAYBACK, 1.0);
  keymap[glow::Glow::downArrowKey] =
    Controller::MapTarget(SimControl::CTRL_PAUSE_SEEK, 1.0);
  keymap['0'] =
    Controller::MapTarget(SimControl::CTRL_SPEED_RESET, 1.0 );
  
  return keymap_ptr;
}


static Controller::ControlMap * newSimControlKeymap2()
{
  Controller::ControlMap * keymap_ptr = new Controller::ControlMap();
  Controller::ControlMap & keymap = *keymap_ptr;
  
  keymap[glow::Glow::leftArrowKey] =
    Controller::MapTarget(SimControl::CTRL_SEEK, -700.0 );
  keymap[glow::Glow::rightArrowKey] =
    Controller::MapTarget(SimControl::CTRL_SEEK, 700.0 );
  
  return keymap_ptr;
}


static Controller::ControlMap * newLegTorqueKeymap()
{
  Controller::ControlMap * keymap_ptr = new Controller::ControlMap();
  Controller::ControlMap & keymap = *keymap_ptr;
  
  keymap['r'] = Controller::MapTarget(Leg::CTRL_UPPERPART_TORQUE_X, 1.0);
  keymap['f'] = Controller::MapTarget(Leg::CTRL_UPPERPART_TORQUE_X, -1.0);
  keymap['t'] = Controller::MapTarget(Leg::CTRL_UPPERPART_TORQUE_Y, 0.2);
  keymap['g'] = Controller::MapTarget(Leg::CTRL_UPPERPART_TORQUE_Y, -0.2);
  
  keymap['y'] = Controller::MapTarget(Leg::CTRL_LOWERPART_TORQUE, 0.5);
  keymap['h'] = Controller::MapTarget(Leg::CTRL_LOWERPART_TORQUE, -1.0);
  
  keymap['u'] = Controller::MapTarget(Leg::CTRL_ENDEFFECTOR_TORQUE_X, 0.25);
  keymap['j'] = Controller::MapTarget(Leg::CTRL_ENDEFFECTOR_TORQUE_X, -0.25);
  keymap['i'] = Controller::MapTarget(Leg::CTRL_ENDEFFECTOR_TORQUE_Y, 0.1);
  keymap['k'] = Controller::MapTarget(Leg::CTRL_ENDEFFECTOR_TORQUE_Y, -0.1);
  keymap['o'] = Controller::MapTarget(Leg::CTRL_ENDEFFECTOR_TORQUE_Z, 0.1);
  keymap['l'] = Controller::MapTarget(Leg::CTRL_ENDEFFECTOR_TORQUE_Z, -0.1);
  
  return keymap_ptr;
}


static Controller::ControlMap * newLegTargetVelKeymap()
{
  Controller::ControlMap * keymap_ptr = new Controller::ControlMap();
  Controller::ControlMap & keymap = *keymap_ptr;
  
  keymap['r'] = Controller::MapTarget(Leg::CTRL_UPPERPART_MOTOR_TARGETVEL_X, 5.0);
  keymap['f'] = Controller::MapTarget(Leg::CTRL_UPPERPART_MOTOR_TARGETVEL_X, -5.0);
  keymap['t'] = Controller::MapTarget(Leg::CTRL_UPPERPART_MOTOR_TARGETVEL_Y, 5.0);
  keymap['g'] = Controller::MapTarget(Leg::CTRL_UPPERPART_MOTOR_TARGETVEL_Y, -5.0);
  
  keymap['y'] = Controller::MapTarget(Leg::CTRL_LOWERPART_MOTOR_TARGETVEL, 5.0);
  keymap['h'] = Controller::MapTarget(Leg::CTRL_LOWERPART_MOTOR_TARGETVEL, -5.0);
  
  keymap['u'] = Controller::MapTarget(Leg::CTRL_ENDEFFECTOR_MOTOR_TARGETVEL_X, 5.0);
  keymap['j'] = Controller::MapTarget(Leg::CTRL_ENDEFFECTOR_MOTOR_TARGETVEL_X, -5.0);
  keymap['i'] = Controller::MapTarget(Leg::CTRL_ENDEFFECTOR_MOTOR_TARGETVEL_Y, 5.0);
  keymap['k'] = Controller::MapTarget(Leg::CTRL_ENDEFFECTOR_MOTOR_TARGETVEL_Y, -5.0);
  keymap['o'] = Controller::MapTarget(Leg::CTRL_ENDEFFECTOR_MOTOR_TARGETVEL_Z, 5.0);
  keymap['l'] = Controller::MapTarget(Leg::CTRL_ENDEFFECTOR_MOTOR_TARGETVEL_Z, -5.0);
  
  return keymap_ptr;
}








class Base :
  public Object
{
public:
  
  enum Connectors {
    CONN_BOTTOM_DOWN,
    ConnectorCount
  };
  
  Base()
  {
    
    // object properties
    shared_ptr<Shape> shape
      ( shapes::Cube::create( makeVector3d( 0.7, 0.2, 0.7 ) ));
    shared_ptr<ODELocator> locator
      ( new ODELocator( makeVector3d(),
                        BasisMatrix(),
                        0.0, 0.0,              // mass, moment of inertia
                        0.1, 0.01, 0.01,       // normal drag
                        0.1, 0.01, 0.01 ) );   // rotational drag
    locator->setInertiaShape( shape );
    locator->setDensity( 1.0 );
    //locator->setDensity( 0.2 );
    setLocator( locator );
    setVisual( sptr( new BasicVisual( shape, &legBaseMat ) ));
    setGeometry( sptr( new BasicGeometry( shape, defaultSurface ) ));
    
    // connector
    connectors[CONN_BOTTOM_DOWN] =
      sptr( new Connector( *this, Connector::Slave,
                           BasicLocator( makeVector3d( 0.0, -0.1, 0.0 ),
                                         BasisMatrix
                                         ( makeVector3d( 0.0, -1.0, 0.0 ),
                                           makeVector3d( 0.0, 0.0, 1.0 ) ))));
    
  }

};








int main( int argc, char * argv[] )
{
  if( argc != 2 ) {
    cout << "Käyttis: " << argv[0] << " torques|targetvels" << endl;
    exit(1);
  }
  
  enum ControlMode {CM_TORQUES, CM_TARGETVELS};
  ControlMode controlMode = CM_TORQUES;
  if( 0 == strcmp( argv[1], "torques" ) )
    controlMode = CM_TORQUES;
  else if( 0 == strcmp( argv[1], "targetvels" ) )
    controlMode = CM_TARGETVELS;
  else {
    cout << "Virheellinen 1. argumentti!" << endl;
    exit(1);
  }
  
  
  /* simulator initialization */
  
  // graphics and user interface
  GLOWDevice::Init( argc, argv );
  GLOWDevice window;
  GLOWViewport viewport( window );
  
  // recording and playback handling
  shared_ptr<RecPlaySystem> recPlaySystem( new RecPlaySystem() );
  
  // sim control
  shared_ptr<SimControl> simControl( new SimControl( recPlaySystem ) );
  //simControl->setDefaultVisualizationDtMultiplier( 2.0 );
  simControl->setControlDtMultiplier( 1 );
  viewport.addActor( simControl, newSimControlKeymap1() );
  viewport.addActor( simControl, newSimControlKeymap2() );
  
  // TCP remote controller for Matlab-controlled actors
  //TCPRemoteController tcpRemoteController;
  
  
  /* simulation world initialization */
  
  // simulation world
  ODEWorld world;
  world.setGravityVector( makeVector3d( 0.0, -9.81, 0.0 ));
  
  // collision detection
  ODECollisionRenderer collisionRenderer( &world );
  
  
  
  
  
  
  
  
  /* create the world content */
  
  
  // floor mirror
  shared_ptr<Object> mirrorProjectorTarget
    ( new Object( Object::Params( new BasicLocator() ) ));
  shared_ptr<Camera> mirrorProjector( new Camera( makeVector3d( 1.0, -1.0, 1.0 ) ));
  mirrorProjector->setTargetObject( mirrorProjectorTarget );
  world.addObject( mirrorProjector );   // add before floor
  
  // floor
  shared_ptr<Floor> floor( new Floor( 20, 20, floorBrightMat, floorDarkMat,
                                      defaultSurface ) );
  world.addObject( floor );
  
  // above floor content space
  shared_ptr<Subspace> mainSpace( new Subspace() );
  mainSpace->addObject( mirrorProjectorTarget );
  world.addObject( mainSpace );
  
  // above floor transparent content space (alpha blending enabled and backface
  // culling disabled, is added to the mainSpace after all other stuff)
  shared_ptr<Subspace> mainSpaceTransparents( new Subspace() );
  mainSpaceTransparents->getEnvironment()->addOGLState
    ( new Environment::OGLState( GL_ENABLE_BIT, GL_BLEND,
                                 new GLboolean(GL_TRUE) ));
  mainSpaceTransparents->getEnvironment()->addOGLState
    ( new Environment::OGLState( GL_ENABLE_BIT, GL_CULL_FACE,
                                 new GLboolean(GL_FALSE) ));
  
  
  
  
  /* create mainSpace content */
  
  // red tube
  shared_ptr<Object> tube( new Object() );
  shared_ptr<Shape> tubeShape( shapes::CappedCylinder::create( 2.0, 0.2 ) );
  shared_ptr<ODELocator> tubeLocator
    ( new ODELocator( makeVector3d( 0.0, 5.0, -3.0 ),
                      BasisMatrix(),
                      0.0, 0.0,              // mass, moment of inertia
                      0.1, 0.01, 0.01,       // normal drag
                      0.1, 0.01, 0.01 ) );   // rotational drag
  tubeLocator->setInertiaShape( tubeShape );
  tubeLocator->setDensity( 1.0 );
  tube->setLocator( tubeLocator );
  tube->setVisual( sptr( new BasicVisual( tubeShape, &redMat ) ));
  tube->setGeometry( sptr( new BasicGeometry( tubeShape, defaultSurface ) ));
  //mainSpace->addObject( tube );
  
  
  
  
  // add leg stuff into world
  
  
  // yellow leg base
  shared_ptr<Object> base( new Base() );
  base->getLocator()->setLoc( makeVector3d( -2.0, 1.5, -4.0 ) );
  mainSpaceTransparents->addObject( base );
  
  
  // leg
  
  shared_ptr<Leg> leg( new Leg() );
  mainSpace->addObject( leg );
  
  Controller::ControlMap * legKeymap;
  switch( controlMode )
    {
    case CM_TORQUES:
      legKeymap = newLegTorqueKeymap();
      break;
    case CM_TARGETVELS:
      legKeymap = newLegTargetVelKeymap();
      leg->useControl( Leg::CTRL_UPPERPART_MOTOR_FORCEFACTOR_X, 1.0 );
      leg->useControl( Leg::CTRL_UPPERPART_MOTOR_FORCEFACTOR_Y, 1.0 );
      leg->useControl( Leg::CTRL_LOWERPART_MOTOR_FORCEFACTOR, 1.0 );
      leg->useControl( Leg::CTRL_ENDEFFECTOR_MOTOR_FORCEFACTOR_X, 1.0 );
      leg->useControl( Leg::CTRL_ENDEFFECTOR_MOTOR_FORCEFACTOR_Y, 1.0 );
      leg->useControl( Leg::CTRL_ENDEFFECTOR_MOTOR_FORCEFACTOR_Z, 1.0 );
      break;
    }
  viewport.addActor( leg, legKeymap );
  
  
  
  
  
  
  
  
  /* lights */
  
  
  // light object 1
  shared_ptr<Object> lightObject1
    ( new Object( Object::Params( new BasicLocator( makeVector3d( 3.0, 2.0, -2.0 ) ),
                                  new BasicVisual( shapes::Sphere::create( 0.5 ),
                                                   &brightWhiteMat ))));
  mainSpace->addObject( lightObject1 );
  
  // actual light 1
  GLfloat lightAttenuation1[3] = { 1.0, 0.0, 0.02 };
  Light * light1 = new Light( &brightWhiteMat, lightObject1,
                              lightAttenuation1 );
  world.getEnvironment()->addLight( light1 );
  //mainSpace->getEnvironment()->addLight( light1 );
  //floor->getEnvironment()->addLight( light1 );

  // light object 2
  shared_ptr<Object> lightObject2
    ( new Object( Object::Params( new BasicLocator( makeVector3d( -4.0, 2.0, -8.0 ) ),
                                  new BasicVisual( shapes::Sphere::create( 0.25 ),
                                                   &brightBlueMat ))));
  mainSpace->addObject( lightObject2 );
  
  // actual light 2
  GLfloat lightAttenuation2[3] = { 2.0, 0.0, 0.02 };
  Light * light2 = new Light( &brightBlueMat, lightObject2,
                              lightAttenuation2 );
  world.getEnvironment()->addLight( light2 );
  
  
  
  
  /* camera */
  
  
  // camera subspace with no integrator (separates its timestepping from
  // simulation time)
  shared_ptr<Subspace> cameraSpace
    ( new Subspace( Subspace::Params( Object::Params(),
                                      shared_ptr<Environment>( new Environment() ),
                                      shared_ptr<Integrator>() ) ));
  world.addObject( cameraSpace );
  
  // camera target object
  shared_ptr<FloatingActor> cameraObject
    ( new FloatingActor( Object::Params( new InertiaLocator
                                         ( makeVector3d( 0.0, 2.0, 3.0 ), BasisMatrix(),
                                           0.2, 2.0,      // mass, moment of inertia
                                           0.01, 0.5,     // normal drag
                                           0.1, 5.0 ),    // rotational drag
                                         0,      // visual
                                         0 )));   // collision shape
  cameraSpace->addObject( cameraObject );
  viewport.addActor( cameraObject, newCameraKeymap() );
  
  // the actual camera
  shared_ptr<Camera> camera( new Camera() );
  camera->setTargetObject( cameraObject );
  viewport.setCamera( camera );
  
  
  
  
  /* world finalization */
  
  // add transparent stuff into mainSpace
  mainSpace->addObject( mainSpaceTransparents );
  
  // activate the world
  world.activate( true );
  collisionRenderer.connect();
  
  
  // connect all connectors
  leg->getConnector( Leg::CONN_BASE )->connect
    ( base->getConnector( Base::CONN_BOTTOM_DOWN ), Connector::AlignMaster );
  leg->connect();
  
  // set initial velocities
  /*
  dynamic_pointer_cast<ODELocator>( tube->getLocator() )->
    setRotation( makeVector3d( 3.0, 12.0, 0.3 ) );
  dynamic_pointer_cast<ODELocator>( tube->getLocator() )->
    setVel( makeVector3d( -1.0, 0.0, 0.0 ) );
  */
  
  // adjust world softness
  world.setCFM( 0.1 );
  
  
  
  
  /* start the system */
  
  world.setDefaultDt( 0.008 );
  viewport.setAutoRefresh( false );
  window.events.addListener( simControl.get() );
  
  // pass the camera object to simControl for timestepping
  simControl->setCameraObjectAndDt( cameraObject.get(), 0.016 );
  
  // rec/play
  recPlaySystem->addObject( leg, WorldSerialization::PROP_LOCATOR, true );
  recPlaySystem->addObject( base, WorldSerialization::PROP_LOCATOR, true );
  
  // control
  //simControl.doControlEvent.addListener( &TCPRemoteController );
  
  // simulate
  simControl->doSimulateEvent.addListener( &collisionRenderer );   // order is important!
  simControl->doSimulateEvent.addListener( &world );               // order is important!
  
  // render
  simControl->doRenderEvent.addListener( &viewport );
  
  // enter GLOW mainloop (will not return)
  GLOWDevice::MainLoop();
  
  
  // this is never reached
  return 0;
}
