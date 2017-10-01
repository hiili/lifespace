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
 */

#include <lifespace/lifespace.hpp>
using namespace lifespace;

#include <lifespace/plugins/glow.hpp>
using namespace lifespace::plugins::pglow;

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include <iostream>
using std::cout;
using std::endl;

#include <GL/gl.h>




static const GLfloat none[4]        = { 0.0, 0.0, 0.0, 0.0 };
static const GLfloat white[4]       = { 1.0, 1.0, 1.0, 1.0 };
static const GLfloat gray6[4]       = { 0.6, 0.6, 0.6, 1.0 };
static const GLfloat gray3[4]       = { 0.3, 0.3, 0.3, 1.0 };
static const GLfloat red[4]         = { 1.0, 0.0, 0.0, 1.0 };
static const GLfloat polished[1]    = { 40.0 };
static const Material redMat( none, red, white,
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




int main( int argc, char * argv[] )
{
  GLOWDevice::Init( argc, argv );
  GLOWDevice window;
  GLOWViewport viewport( window );
  
  // world
  World world;
  
  // cube object
  Object * object =
    new Object( Object::Params( new InertiaLocator,
                                new BasicVisual( shapes::Basis::TheBasis,
                                                 &redMat ) ) );
  Subspace objSpace
    ( Subspace::Params( Object::Params( new InertiaLocator ) ) );
  world.addObject( &objSpace );
  objSpace.addObject( object );
  //world.addObject( object );
  
  // 2nd cube object
  Object * object2 =
    new Object( Object::Params
                ( new InertiaLocator( makeVector3d( 0.0, 0.0, -5.0 ) ),
                  new BasicVisual
                  ( new shapes::Scaled( shapes::Basis::TheBasis,
                                        makeVector3d( 2.0, 2.0, 2.0 ) ),
                    &redMat ) ) );
  world.addObject( object2 );
  
  // light
  Object lightObj( Object::Params
                   ( new InertiaLocator( makeVector3d( 2.0, 2.0, -3.0 ) ),
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
  
  Subspace camSpace
    ( Subspace::Params( Object::Params( new InertiaLocator ) ) );
  world.addObject( &camSpace );
  camSpace.addObject( camTarget );
  //world.addObject( camTarget );
  viewport.addActor( camTarget );
  
  Camera cam;
  cam.setTargetObject( camTarget );
  viewport.setCamera( &cam );
  
  
  for( int i = 0 ; i < 10 ; i++ ) {
    cout << object->getLocator()->getLoc() << endl;
    object->getLocator()->addForceAbs( makeVector3d( 0.0, 0.0, -1.0 ) );
    world.timestep( 0.1 );
  }
  
  objSpace.getLocator()->addTorqueAbs( makeVector3d( 5.0, 0.0, 0.0 ) );
  object->getLocator()->addTorqueAbs( makeVector3d( -10.0, 0.0, 0.0 ) );
  
  world.setDefaultDt( 0.05 );
  window.events.addListener( &world );
  
  //camTarget->getLocator()->addForceAbs( makeVector3d( 3.0, 0.0, 0.0 ) );
  camSpace.getLocator()->addForceAbs( makeVector3d( 3.0, 0.0, 0.0 ) );
  
  GLOWDevice::MainLoop();
  
  return 0;
}
