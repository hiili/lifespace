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
 * A test setup for testing collision (contact) feedback.
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

#include <typeinfo>

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;
using boost::dynamic_pointer_cast;




static const GLfloat none[4]        = { 0.0, 0.0, 0.0, 0.0 };
static const GLfloat white[4]       = { 1.0, 1.0, 1.0, 1.0 };
static const GLfloat gray6[4]       = { 0.6, 0.6, 0.6, 1.0 };
static const GLfloat gray3[4]       = { 0.3, 0.3, 0.3, 1.0 };
static const GLfloat red[4]         = { 1.0, 0.0, 0.0, 1.0 };
static const GLfloat red6[4]        = { 0.6, 0.0, 0.0, 1.0 };
static const GLfloat red3[4]        = { 0.3, 0.0, 0.0, 1.0 };
static const GLfloat polished[1]    = { 40.0 };
static const Material whiteMat( gray3, gray6, white,
                                none, polished, GL_FRONT );
static const Material redMat( red3, red6, red,
                              none, polished, GL_FRONT );
static const Material brightWhiteMat( white, white, white,
                                      none, polished, GL_FRONT );
static const shared_ptr<CollisionMaterial> defaultSurface
( new CollisionMaterial( 1.0, 0.9, 0.001 ));


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




class Color
{
  GLfloat components[4];
  
public:
  
  Color() 
  { for( int i=0 ; i<4 ; ++i ) components[i] = 0.0; }
  
  Color( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha )
  {
    components[0] = red; components[1] = green;
    components[2] = blue; components[3] = alpha;
  }
  
  operator const GLfloat * () const
  { return components; }
  
  
  Color operator+( const Color & other )
  {
    return Color( std::min((float)1.0, components[0] + other.components[0]),
                  std::min((float)1.0, components[1] + other.components[1]),
                  std::min((float)1.0, components[2] + other.components[2]),
                  std::min((float)1.0, components[3] + other.components[3]) );
  }
  
  Color & adjustBrightness( float factor )
  {
    for( int i=0 ; i<3 ; ++i ) {
      components[i] = std::min((float)1.0, factor * components[i]);
    }
    return *this;
  }
  
  void interpolate( const Color & target, float amount )
  {
    for( int i=0 ; i<4 ; ++i ) {
      components[i] =
        (1.0 - amount) * components[i] + amount * target.components[i];
    }
  }
  
};




class CollisionResponderObject :
  public virtual Object,
  public Geometry::AddContactEventListener,
  public Geometry::RemoveContactEventListener
{
  
  Color baseColor;
  Color currentColor;
  Color ambientColor, diffuseColor, specularColor;
  Material material;
  bool forceColors;
  
  
  void updateColors()
  {
    const Geometry::contacts_t & contacts = getGeometry()->getContacts();
    Color targetColor( baseColor );
    
    // enumerate current contacts
    for( Geometry::contacts_t::const_iterator i = contacts.begin() ;
         i != contacts.end() ; ++i ) {
      
      // resolve the Object behind the current contact
      const Object * targetObj = i->first->getHostObject();
      
      // is it a CollisionResponder?
      if( const CollisionResponderObject * target =
          dynamic_cast<const CollisionResponderObject *>( targetObj ) ) {
        
        // target is also a CollisionResponder, add its color
        targetColor = targetColor + target->getBaseColor();
        
      }
      
    }
    
    // update current color
    currentColor.interpolate( targetColor, forceColors ? 1.0 : 0.1 );
    forceColors = false;
    
    // update derived colors
    ambientColor = Color(currentColor).adjustBrightness( 0.3 );
    diffuseColor = Color(currentColor).adjustBrightness( 0.6 );
    specularColor = Color(currentColor).adjustBrightness( 1.0 );
  }
  
  
public:
  
  CollisionResponderObject( const Object::Params & objectParams,
                            const Color & baseColor_ ) :
    Object( objectParams ),
    baseColor( baseColor_ ),
    currentColor( baseColor_ ),
    material( ambientColor, diffuseColor, specularColor,
              none, polished, GL_FRONT ),
    forceColors( false )
  {
    updateColors();
    dynamic_pointer_cast<BasicVisual>(getVisual())->material = &material;
    
    getGeometry()->events.addListener( this );
  }
  
  
  const Color & getBaseColor() const
  { return baseColor; }
  
  
  virtual void processEvent( const Geometry::AddContactEvent & event,
                             Geometry & source )
  {
    cout << "CollisionResponderObject -- Geometry::AddContactEvent:" << endl
         << "  this geometry  == " << &source << endl
         << "  other geometry == " << event.other << endl;
    forceColors = true;
  }
  
  virtual void processEvent( const Geometry::RemoveContactEvent & event,
                             Geometry & source )
  {
    cout << "CollisionResponderObject -- Geometry::RemoveContactEvent:" << endl
         << "  this geometry  == " << &source << endl
         << "  other geometry == " << event.other << endl;
  }
  
  
  void localStep()
  {
    /*
    // color update test: slowly fades the object's color to black  
    currentColor.adjustBrightness( 0.995 );
    updateDerivedColors( currentColor );
    */
    updateColors();
  }

  virtual void step()
  {
    localStep();
    Object::step();
  }
  
};




class CameraObject :
  public CollisionResponderObject,
  public FloatingActor,
  public Subspace
{
public:
  CameraObject( const Object::Params & objectParams,
                const Color & baseColor ) :
    Object( objectParams ),
    CollisionResponderObject( objectParams, baseColor )
  {
    // enable alpha blending
    getEnvironment()->addOGLState
      ( new Environment::OGLState( GL_ENABLE_BIT, GL_BLEND,
                                   new GLboolean(GL_TRUE) ));
    // disable back face culling
    getEnvironment()->addOGLState
      ( new Environment::OGLState( GL_ENABLE_BIT, GL_CULL_FACE,
                                   new GLboolean(GL_FALSE) ));
  }
  
  virtual void prepare( real dt )
  {
    // doesn't have one: CollisionResponderObject::localPrepare( dt );
    FloatingActor::localPrepare( dt );
    Subspace::localPrepare( dt );
    Object::prepare( dt );
  }

  virtual void step()
  {
    //CollisionResponderObject::localStep();
    FloatingActor::localStep();
    Subspace::localStep();
    Object::step();
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
  world.setGravityVector( makeVector3d( 0.0, -9.81, 0.0 ));
  
  // collision detection
  ODECollisionRenderer collisionRenderer( &world );
  
  
  
  
  /* create the world content */
  
  
  // ground
  shared_ptr<Shape> groundShape
    ( shapes::Cube::create( makeVector3d( 10.0, 1.0, 10.0 )));
  CollisionResponderObject * ground = new CollisionResponderObject
    ( Object::Params
      ( new BasicLocator( makeVector3d( 0.0, -0.5, 0.0 )),
        new BasicVisual( groundShape, &whiteMat ),
        new BasicGeometry( groundShape, defaultSurface )),
      Color( 0.5, 0.5, 0.5, 1.0 ));
  world.addObject( ground );
  cout << "ground object: " << ground
       << ", ground geometry: "
       << ground->getGeometry().get() << endl;
  
  // ball 1
  CollisionResponderObject * ball = new CollisionResponderObject
    ( Object::Params( new ODELocator( makeVector3d( 1.0, 2.0, -2.0 )),
                      new BasicVisual( shapes::Sphere::create( 0.5 ),
                                       &whiteMat ),
                      new BasicGeometry( shapes::Sphere::create( 0.5 ),
                                         defaultSurface )),
      Color( 1.0, 0.0, 0.0, 1.0 ));
  world.addObject( ball );
  cout << "ball object: " << ball
       << ", ball geometry: "
       << ball->getGeometry().get() << endl;
  
  // ball 2
  CollisionResponderObject * ball2 = new CollisionResponderObject
    ( Object::Params( new ODELocator( makeVector3d( -0.5, 3.0, -2.0 )),
                      new BasicVisual( shapes::Sphere::create( 0.5 ),
                                       &whiteMat ),
                      new BasicGeometry( shapes::Sphere::create( 0.5 ),
                                         defaultSurface )),
      Color( 0.0, 1.0, 0.0, 1.0 ));
  world.addObject( ball2 );
  cout << "ball 2 object: " << ball2
       << ", ball 2 geometry: "
       << ball2->getGeometry().get() << endl;
  
  
  
  
  /* lights */
  
  // light object
  Object * lightObject = new Object
    ( Object::Params( new BasicLocator( makeVector3d( 2.0, 2.0, -2.0 ) ),
                      new BasicVisual( shapes::Sphere::create( 0.5 ),
                                       &brightWhiteMat )));
  world.addObject( lightObject );
  cout << "light object: " << lightObject
       << ", light geometry: "
       << lightObject->getGeometry().get() << endl;
  
  // the actual light
  GLfloat lightAttenuation[3] = { 1.0, 0.0, 0.02 };
  Light * light = new Light( &brightWhiteMat, lightObject, lightAttenuation );
  world.getEnvironment()->addLight( light );
  
  
  /* camera */
  
  // camera components
  shared_ptr<Shape> camBasisShape
    ( new shapes::Located( BasicLocator( makeVector3d( -0.3, -0.5, -1.5 ),
                                         BasisMatrix
                                         ( makeVector3d( 0.0, 0.0, -1.0 ),
                                           makeVector3d( 0.0, 1.0, 0.0 ))),
                           sptr( new shapes::Scaled
                                 ( makeVector3d( 1.0, 1.0, 1.0 ),
                                   sptr( new shapes::Basis )))));
  shared_ptr<BasicVisual> camVis
    ( new BasicVisual( camBasisShape, &whiteMat ));
  shared_ptr<BasicGeometry> camGeom
    ( new BasicGeometry( camBasisShape, defaultSurface ));
  
  // camera object
  CameraObject * cameraObject = new CameraObject
    ( Object::Params( sptr( new ODELocator
                            ( makeVector3d( 0.0, 2.0, 1.0 ), BasisMatrix(),
                              1.0, 2.0,           // mass, moment of inertia
                              0.01, 0.1, 1.0,     // normal drag
                              0.1, 1.0, 10.0 )),   // rotational drag
                      camVis,
                      camGeom ),
      Color( 0.5, 0.5, 1.0, 0.5 ) );
  dynamic_pointer_cast<ODELocator>( cameraObject->getLocator() )->
    setGravityEnabled( false );
  world.addObject( cameraObject );
  Controller::ControlMap cameraKeymap; fillCameraKeymap( cameraKeymap );
  viewport.addActor( cameraObject, &cameraKeymap );
  cout << "camera object: " << cameraObject
       << ", camera geometry: "
       << cameraObject->getGeometry().get() << endl;
  
  // the actual camera
  Camera * camera = new Camera;
  camera->setTargetObject( cameraObject );
  viewport.setCamera( camera );
  
  
  
  
  // activate the world
  world.activate( true );
  collisionRenderer.connect();
  
  
  // connect the connectors
  
  
  
  
  // start the system
  world.setDefaultDt( 0.05 );
  window.events.addListener( &collisionRenderer );   // order is important!
  window.events.addListener( &world );               // order is important!
  GLOWDevice::MainLoop();
  
  // this is never reached
  return 0;
}
