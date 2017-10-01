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
 * A test setup for testing recursive Cameras. The projector's scaling is
 * commented out because this feature does not currently work correctly.
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
  shared_ptr<Shape> groundShape
    ( shapes::Cube::create( makeVector3d( 10.0, 1.0, 10.0 )));
  Object * ground =
    new Object( Object::Params
                ( new BasicLocator( makeVector3d( 0.0, -0.5, 0.0 )),
                  new BasicVisual( groundShape, &whiteMat ),
                  new BasicGeometry( groundShape,
                                     sptr( new CollisionMaterial
                                           ( 1.0, 1.0, 0.0 )))));
  world.addObject( ground );
  
  shared_ptr<Shape> projectorShape
    ( shapes::Located::create
      ( BasicLocator( makeVector3d( 0.0, -1.0, 0.0 )),
        shapes::Cube::create( makeVector3d( 1.0, 2.0, 1.0 ))));
  Camera * projector = new Camera();
  projector->setLocator
    ( sptr( new BasicLocator( makeVector3d( 4.4, 2.0, -4.4 ))));
  projector->setVisual
    ( sptr( new BasicVisual( projectorShape, &redMat )));
  projector->setGeometry
    ( sptr( new BasicGeometry
            ( projectorShape,
              sptr( new CollisionMaterial( 1.0, 1.0, 0.0 )))));
  world.addObject( projector );
  
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
  cameraObject->getLocator()->setLoc( makeVector3d( 5.5, 1.0, 5.5 ));
  world.addObject( cameraObject );
  Controller::ControlMap cameraKeymap; fillCameraKeymap( cameraKeymap );
  viewport.addActor( cameraObject, &cameraKeymap );
  Camera * camera = new Camera;
  camera->setTargetObject( cameraObject );
  viewport.setCamera( camera );
  
  // configure the projector
  projector->setTargetObject( cameraObject );
  //projector->setScaling( makeVector3d( 0.5, 0.5, 0.5 ));
  
  // start the system
  world.setDefaultDt( 0.05 );
  window.events.addListener( &collisionRenderer );   // order is important!
  window.events.addListener( &world );               // order is important!
  GLOWDevice::MainLoop();
  
  // this is never reached
  return 0;
}
