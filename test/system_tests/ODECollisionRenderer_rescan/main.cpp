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




/**
 * The ground Object.
 */
class Ground :
  public Object
{
  shared_ptr<shapes::Located> floorPlane;
  shared_ptr<Shape> shape;
  real dt;
  float wavePhase;
  
  
 public:
  
  Ground( Locator * locator ) :
    Object( Object::Params( locator ) ),
    wavePhase( 0.0 )
  {
    // create the floor plane, which can be rotated with the Located filter
    // shape
    floorPlane = shapes::Located::create
      ( BasicLocator( makeVector3d( 0.0, -0.25, 0.0 )),
        shapes::Cube::create( makeVector3d( 9.0, 0.5, 9.0 )));
    
    // create the complete shape
    shape = shapes::Union::create
      ( floorPlane,
        shapes::Located::create
        ( BasicLocator( makeVector3d( -5.0, 0.0, 0.0 )),
          shapes::Cube::create( makeVector3d( 1.0, 2.0, 10.0 ))),
        shapes::Located::create
        ( BasicLocator( makeVector3d( 5.0, 0.0, 0.0 )),
          shapes::Cube::create( makeVector3d( 1.0, 2.0, 10.0 ))),
        shapes::Located::create
        ( BasicLocator( makeVector3d( 0.0, 0.0, -5.0 )),
          shapes::Cube::create( makeVector3d( 10.0, 2.0, 1.0 ))),
        shapes::Located::create
        ( BasicLocator( makeVector3d( 0.0, 0.0, 5.0 )),
          shapes::Cube::create( makeVector3d( 10.0, 2.0, 1.0 ))));
    
    // create the visual and geometry
    setVisual( sptr( new BasicVisual( shape, &whiteMat )));
    setGeometry( sptr( new BasicGeometry
                       ( shape,
                         sptr( new CollisionMaterial( 1.0, 0.5, 0.0 )))));
  }
  
  
  virtual void prepare( real dt_ )
  {
    dt = dt_;
    Object::prepare( dt );
  }
  
  
  virtual void step()
  {
    wavePhase = fmod( wavePhase + 0.5 * dt, 2.0 * M_PI );
    floorPlane->location.setBasis
      ( BasisMatrix( makeVector3d( 0.0, 0.0, -1.0 ),
                     makeVector3d( 0.3 * cos(wavePhase), 1.0, 0.0 )));
    setGeometry( getGeometry() );
    
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
  
  
  
  
  // create the objects
  Ground * ground =
    new Ground( new BasicLocator
                ( makeVector3d( 0.0, -2.0, -3.0 ),
                  BasisMatrix( makeVector3d( 0.0, 0.0, -1.0 ),
                               makeVector3d( 0.0, 1.0, 0.0 ))));
  world.addObject( ground );
  Object * ball =
    new Object( Object::Params
                ( new ODELocator( makeVector3d( 1.0, 2.0, -2.0 )),
                  new BasicVisual( shapes::Sphere::create( 0.5 ),
                                   &whiteMat ),
                  new BasicGeometry( shapes::Sphere::create( 0.5 ),
                                     sptr( new CollisionMaterial
                                           ( 1.0, 1.0, 0.0 )))));
  world.addObject( ball );
  
  
  // activate the world
  world.activate( true );
  collisionRenderer.connect();
  
  
  // connect the connectors
  
  
  
  
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
