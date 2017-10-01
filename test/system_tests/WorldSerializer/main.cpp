/**
 * @file main.cpp
 *
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

#include <string>
using std::string;

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;
using boost::dynamic_pointer_cast;




static const GLfloat none[4]         = { 0.0, 0.0, 0.0, 0.0 };
static const GLfloat white[4]        = { 1.0, 1.0, 1.0, 1.0 };
static const GLfloat superwhite[4]   = { 10.0, 10.0, 10.0, 1.0 };
static const GLfloat gray8[4]        = { 0.8, 0.8, 0.8, 1.0 };
static const GLfloat gray6[4]        = { 0.6, 0.6, 0.6, 1.0 };
static const GLfloat gray3[4]        = { 0.3, 0.3, 0.3, 1.0 };
static const GLfloat gray8A[4]       = { 0.8, 0.8, 0.8, 0.7 };
static const GLfloat gray6A[4]       = { 0.6, 0.6, 0.6, 0.7 };
static const GLfloat gray3A[4]       = { 0.3, 0.3, 0.3, 0.7 };
static const GLfloat gray1[4]        = { 0.1, 0.1, 0.1, 1.0 };
static const GLfloat lightRed[4]     = { 1.0, 0.5, 0.5, 1.0 };
static const GLfloat red[4]          = { 1.0, 0.0, 0.0, 1.0 };
static const GLfloat red6[4]         = { 0.6, 0.0, 0.0, 1.0 };
static const GLfloat red3[4]         = { 0.3, 0.0, 0.0, 1.0 };
static const GLfloat blue[4]         = { 0.0, 0.0, 1.0, 1.0 };
static const GLfloat blue8[4]        = { 0.0, 0.0, 0.8, 1.0 };
static const GLfloat blue6[4]        = { 0.0, 0.0, 0.6, 1.0 };
static const GLfloat blue3[4]        = { 0.0, 0.0, 0.3, 1.0 };
static const GLfloat green6[4]       = { 0.0, 0.6, 0.0, 1.0 };
static const GLfloat green3[4]       = { 0.0, 0.3, 0.0, 1.0 };
static const GLfloat polished[1]     = { 40.0 };
static const GLfloat semipolished[1] = { 20.0 };
static const GLfloat dull[1]         = { 10.0 };
static const Material whiteMat( gray3, gray6, white,
                                none, polished, GL_FRONT );
static const Material grayMat( gray1, gray3, white,
                                none, polished, GL_FRONT );
static const Material floorBrightMat( gray3, gray6A, white,
                                      none, semipolished, GL_FRONT );
static const Material floorDarkMat( gray1, gray3A, white,
                                    none, semipolished, GL_FRONT );
static const Material redMat( red3, red6, lightRed,
                              none, polished, GL_FRONT );
static const Material blueMat( blue3, blue6, white,
                              none, dull, GL_FRONT );
static const Material greenMat( green3, green6, white,
                                none, dull, GL_FRONT );
static const Material brightWhiteMat( white, white, white,
                                      none, polished, GL_FRONT );
static const shared_ptr<CollisionMaterial> defaultSurface
( new CollisionMaterial( 0.9, 0.9, 0.001 ));
static const shared_ptr<CollisionMaterial> slipperySurface
( new CollisionMaterial( 0.1, 0.9, 0.001 ));


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
    Controller::MapTarget( FloatingActor::CTRL_TORQUEREL_Y_NEG, 0.4 );
  keymap[GLOWViewport::MOUSE_DY] =
    Controller::MapTarget( FloatingActor::CTRL_TORQUEREL_X_NEG, 0.4 );
}




class TrackerObject :
  public Object
{
  string name;
  
public:
  TrackerObject( const Object::Params & params, const string & name_ ) :
    Object( params ),
    name( name_ )
  {}
  
  void step()
  {
    if( false ) {
      cout << name << ": own y: " << getLocator()->getLoc()(DIM_Y)
           << " world y: " << getWorldLocator()->getLoc()(DIM_Y)
           << "   hosts:";
      const Subspace * s = getHostSpace();
      while( s ) {
        if( s->getLocator() ) {
          cout << " id=" << s;
          cout << ",y=" << s->getLocator()->getLoc()(DIM_Y);
          cout << ",wy=" << s->getWorldLocator()->getLoc()(DIM_Y);
        } else cout << " y=N/A";
        s = s->getHostSpace();
      }
      cout << endl;
    }
    
    Object::step();
  }
};




void makeBallSpaces( shared_ptr<Subspace> subspace, int level )
{
  static int foo = 0;
  
  if( level == 0 ) {
    if( foo == 0 ) {
      subspace->addObject
        ( sptr( new TrackerObject
                ( Object::Params( new ODELocator(),
                                  new BasicVisual
                                  ( shapes::Precomputed::create
                                    ( shapes::Sphere::create( 0.3 )),
                                    &blueMat ),
                                  new BasicGeometry
                                  ( shapes::Sphere::create( 0.3 ),
                                    defaultSurface )),
                  "blue" )));
      foo++;
      return;
    }
    if( foo == 1 ) {
      shared_ptr<Object> obj
        ( new TrackerObject
          ( Object::Params( new InertiaLocator(),
                            new BasicVisual
                            ( shapes::Precomputed::create
                              ( shapes::Sphere::create( 0.3 )),
                              &greenMat ),
                            new BasicGeometry
                            ( shapes::Sphere::create( 0.3 ),
                              defaultSurface )),
            "green" ));
      obj->getLocator()->setVel( makeVector3d( 0.0, -0.1, 0.0 ) );
      subspace->addObject( obj );
      foo++;
      return;
    }
    subspace->addObject
      ( sptr( new Object
              ( Object::Params( new BasicLocator(),
                                new BasicVisual
                                ( shapes::Precomputed::create
                                  ( shapes::Sphere::create( 0.3 )),
                                  &redMat )))));
        //new BasicGeometry
        //( shapes::Sphere::create( 0.3 ),
        //  defaultSurface )))));
    return;
  }
  
  shared_ptr<Subspace> ballSpace1
    ( new Subspace
      ( Object::Params( new BasicLocator
                        ( makeVector3d( -level/2.0, 2.0, -level/2.0 ) ))));
  shared_ptr<Subspace> ballSpace2
    ( new Subspace
      ( Object::Params( new BasicLocator
                        ( makeVector3d( -level/2.0, 2.0, level/2.0 ) ))));
  shared_ptr<Subspace> ballSpace3
    ( new Subspace
      ( Object::Params( new BasicLocator
                        ( makeVector3d( level/2.0, 2.0, -level/2.0 ) ))));
  shared_ptr<Subspace> ballSpace4
    ( new Subspace
      ( Object::Params( new BasicLocator
                        ( makeVector3d( level/2.0, 2.0, level/2.0 ) ))));
  
  subspace->addObject( ballSpace1 );
  makeBallSpaces( ballSpace1, level / 2 );
  subspace->addObject( ballSpace2 );
  makeBallSpaces( ballSpace2, level / 2 );
  subspace->addObject( ballSpace3 );
  makeBallSpaces( ballSpace3, level / 2 );
  subspace->addObject( ballSpace4 );
  makeBallSpaces( ballSpace4, level / 2 );
}




class JointObject :
  public Object,
  public Actor
{
  
public:
  
  enum Connectors {
    CONN_TIP
  };
  
  /** Sensors of the connector. */
  enum Sensors {
    SENS_ANGLE_X,
    SENS_ANGLE_Y,
    SENS_ANGLE_Z,
    SENS_ANGLERATE_X,
    SENS_ANGLERATE_Y,
    SENS_ANGLERATE_Z,
    SensorCount
  };
  
  JointObject( const Locator & location ) :
    Object( Object::Params( new ODELocator
                            ( location.getLoc(),
                              location.getBasis(),
                              5.0, 1.5,            // mass, moment of inertia
                              0.1, 0.01, 0.01,     // normal drag
                              0.1, 0.01, 0.01 ),   // rotational drag
                            new BasicVisual
                            ( shapes::CappedCylinder::create
                              ( 2.0, 0.2 ),
                              &redMat ),
                            new BasicGeometry
                            ( shapes::CappedCylinder::create
                              ( 2.0, 0.2 ),
                              defaultSurface )))
  {
    
    // create and insert the tip connector
    connectors[CONN_TIP] = shared_ptr<Connector>
      ( new ODEBallConnector
        ( Connector( *this, Connector::Any,
                     BasicLocator
                     ( makeVector3d( 0.0, 0.0, 1.1 ),
                       BasisMatrix
                       ( makeVector3d( 1.0, 0.0, 0.0 ),
                         makeVector3d( 0.0, 1.0, 0.0 ) ))),
          ODEMotorAxisParams( 0.0, -M_PI / 4.0, -M_PI / 8.0, 0.9 ), 
          ODEMotorAxisParams( 0.0, -M_PI / 4.0, -M_PI / 8.0, 0.9 ), 
          ODEMotorAxisParams( 0.0, -M_PI / 4.0, -M_PI / 8.0, 0.9 ) ));
    
    // actor init
    sensors.resize( SensorCount );
    
    for( int i = 0 ; i < SensorCount ; i++ ) {
      sensors[i] = connectors[CONN_TIP]->getSensor( i );
    }
    
  }
  
};








int main( int argc, char * argv[] )
{
  // graphics and user interface
  GLOWDevice::Init( argc, argv );
  GLOWDevice window;
  GLOWViewport viewport( window );


  // world
  ODEWorld world;
  world.setName( "world" );
  world.setGravityVector( makeVector3d( 0.0, -0.981, 0.0 ));
  //world.setCFM( 0.000001 );
  world.setAutoDisableFlag( true );
  world.setAutoDisableLinearThreshold( 0.05 );
  world.setAutoDisableAngularThreshold( 0.05 );

  // collision detection
  ODECollisionRenderer collisionRenderer( &world );




  /* create the world content */
  
  // mirror camera
  shared_ptr<Camera> mirrorCamera( new Camera( makeVector3d( 1.0, -1.0, 1.0 ) ) );
  world.addObject( mirrorCamera );
  
  // above floor content space
  shared_ptr<Subspace> mainSpace( new Subspace() );
  mainSpace->setName( "mainSpace" );
  world.addObject( mainSpace );
  
  // floor space
  shared_ptr<Subspace> floorSpace( new Subspace() );
  // enable alpha blending
  floorSpace->getEnvironment()->addOGLState
    ( new Environment::OGLState( GL_ENABLE_BIT, GL_BLEND,
                                 new GLboolean(GL_TRUE) ));
  world.addObject( floorSpace );
  
  // floor
  shared_ptr<shapes::Union> floorHalfShapeUnion = shapes::Union::create();
  for( int i = -10 ; i < 10 ; i++ ) {
    for( int j = -10 ; j < 10 ; j++ ) {
      if( (i + j) % 2 ) {
        floorHalfShapeUnion->targets.push_back
          ( shapes::Located::create( makeVector3d( i + 0.5, 0.0, j + 0.5 ),
                                     shapes::Cube::create
                                     ( makeVector3d( 1.0, 0.0, 1.0 ) )));
      }
    }
  }
  shared_ptr<Shape> floorHalfShape =
    shapes::Precomputed::create( floorHalfShapeUnion );

  shared_ptr<Object> floorB
    ( new Object
      ( Object::Params( new BasicLocator
                        ( makeVector3d( 0.0, 0.0, 0.0 ),
                          BasisMatrix() ),
                        new BasicVisual( floorHalfShape, &floorBrightMat ),
                        new BasicGeometry
                        ( shapes::Located::create
                          ( makeVector3d( 0.0, -1.0, 0.0 ),
                            shapes::Cube::create
                            ( makeVector3d( 20.0, 2.0, 20.0 ) )),
                          defaultSurface ))));
  floorSpace->addObject( floorB );
  
  shared_ptr<Object> floorD
    ( new Object
      ( Object::Params( new BasicLocator
                        ( makeVector3d( 0.0, 0.0, 0.0 ),
                          BasisMatrix
                          ( makeVector3d( 1.0, 0.0, 0.0 ),
                            makeVector3d( 0.0, 1.0, 0.0 ) )),
                        new BasicVisual( floorHalfShape, &floorDarkMat ),
                        0 )));
  floorSpace->addObject( floorD );
  
  // red tube
  shared_ptr<Object> tube
    ( new Object( Object::Params( new ODELocator
                                  ( makeVector3d( 0.0, 3.0, -3.0 ),
                                    BasisMatrix(),
                                    5.0, 1.5,            // mass, moment of inertia
                                    0.1, 0.01, 0.01,     // normal drag
                                    0.1, 0.01, 0.01 ),   // rotational drag
                                  new BasicVisual
                                  ( shapes::CappedCylinder::create
                                    ( 2.0, 0.2 ),
                                    &redMat ),
                                  new BasicGeometry
                                  ( shapes::CappedCylinder::create
                                    ( 2.0, 0.2 ),
                                    defaultSurface ))));
  tube->setName( "tube" );
  mainSpace->addObject( tube );
  
  // blue cube
  shared_ptr<Object> cube
    ( new Object( Object::Params( new ODELocator
                                  //( makeVector3d( -8.0, 1.5, -3.0 ),
                                  ( makeVector3d( -3.0, 1.1, -3.5 ),
                                    BasisMatrix(),
                                    15.0, 1.0,           // mass, moment of inertia
                                    0.1, 0.01, 0.01,     // normal drag
                                    0.1, 0.01, 0.01 ),   // rotational drag
                                  new BasicVisual
                                  ( shapes::Cube::create
                                    ( makeVector3d( 2.0, 0.5, 1.0 ) ),
                                    &blueMat ),
                                  new BasicGeometry
                                  ( shapes::Cube::create
                                    ( makeVector3d( 2.0, 0.5, 1.0 ) ),
                                    slipperySurface ))));
  cube->setName( "cube" );
  mainSpace->addObject( cube );
  
  // balls
  shared_ptr<Subspace> ballSpace( new Subspace() );
  ballSpace->setName( "ballSpace" );
  mainSpace->addObject( ballSpace );
  makeBallSpaces( ballSpace, 2 );
  
  
  // joint objects
  
  shared_ptr<JointObject> jointObject1
    ( new JointObject( BasicLocator( makeVector3d( -5.0, 4.0, -8.0 ) )));
  jointObject1->setName( "jointObject1" );
  mainSpace->addObject( jointObject1 );
  
  shared_ptr<JointObject> jointObject2
    ( new JointObject( BasicLocator( makeVector3d( -5.0, 6.0, -7.0 ) )));
  jointObject2->setName( "jointObject2" );
  mainSpace->addObject( jointObject2 );
  
  
  
  
  /* lights */

  // light object
  shared_ptr<Object> lightObject
    ( new Object
      ( Object::Params( new BasicLocator( makeVector3d( 2.0, 2.0, -4.0 ) ),
                        new BasicVisual( shapes::Sphere::create( 0.5 ),
                                         &brightWhiteMat ))));
  mainSpace->addObject( lightObject );

  // the actual light
  GLfloat lightAttenuation[3] = { 1.0, 0.0, 0.02 };
  Light * light = new Light( &brightWhiteMat, lightObject, lightAttenuation );
  world.getEnvironment()->addLight( light );


  // camera object
  shared_ptr<FloatingActor> cameraObject
    ( new FloatingActor
      ( Object::Params( new InertiaLocator
                        ( makeVector3d( 0.0, 2.0, 3.0 ), BasisMatrix(),
                          1.0, 2.0,      // mass, moment of inertia
                          0.01, 0.5,     // normal drag
                          0.1, 5.0 ),    // rotational drag
                        0,
                        0 )));
  world.addObject( cameraObject );
  Controller::ControlMap cameraKeymap; fillCameraKeymap( cameraKeymap );
  viewport.addActor( cameraObject, &cameraKeymap );
  
  // the actual camera
  shared_ptr<Camera> camera( new Camera() );
  camera->setTargetObject( cameraObject );
  viewport.setCamera( camera );
  
  // mirror camera object
  shared_ptr<Object> mirrorCameraObject
    ( new Object( Object::Params( new BasicLocator() ) ));
  mainSpace->addObject( mirrorCameraObject );
  mirrorCamera->setTargetObject( mirrorCameraObject );
  
  
  
  
  // activate the world
  world.activate( true );
  collisionRenderer.connect();


  // connect the connectors
  
  jointObject1->getConnector( JointObject::CONN_TIP )->connect
    ( jointObject2->getConnector( JointObject::CONN_TIP ),
      Connector::DontAlign );
  
  dynamic_pointer_cast<ODELocator>( tube->getLocator() )->
    setRotation( makeVector3d( 1.0, 4.0, 0.1 ) );
  //dynamic_pointer_cast<ODELocator>( cube->getLocator() )->
  //  setRotation( makeVector3d( 0.0, 1.5, 0.3 ) );
  //dynamic_pointer_cast<ODELocator>( cube->getLocator() )->
  //  setVel( makeVector3d( 0.5, 1.0, 0.0 ) );
  dynamic_pointer_cast<ODELocator>( cube->getLocator() )->
    setRotation( makeVector3d( 0.0, 0.0, 0.1 ) );
  
  
  
  
  // init serialization
  WorldSerializer serializer;
  serializer.addSourceObject( cameraObject, WorldSerialization::PROP_ALL );
  serializer.addSourceObject( jointObject1, WorldSerialization::PROP_ALL );
  serializer.addSourceObject( tube, WorldSerialization::PROP_ALL );
  serializer.addSourceObject( cube, WorldSerialization::PROP_ALL );
  serializer.addSourceObject( ballSpace, WorldSerialization::PROP_ALL, true );
  serializer.addTargetStream( &cout );
  
  
  
  // start the system
  viewport.setAutoRefresh( false );
  world.setDefaultDt( 0.05 );
  window.events.addListener( &collisionRenderer );   // order is important!
  window.events.addListener( &world );               // order is important!
  window.events.addListener( &serializer );
  window.events.addListener( &viewport );
  GLOWDevice::MainLoop();

  // this is never reached
  return 0;
}
