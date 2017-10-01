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
 * Tests the Precomputed -shape (OpenGL display lists) and automatic display
 * list generation. Automatic generation can be enabled or disabled on the line
 * just after creating the Viewport. A Precomputed-shape can be commented in or
 * out at the bugUnionShape variable initialization.
 *
 * This test case extends the ODECollisionRenderer test case.
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

#include <iomanip>

#include <sys/time.h>

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




/** Dumps the current framerate to cout. */ 
class DumpFPS :
  public EventListener<GraphicsEvent>
{
  double prev;
  float avgFPS;

  double getTime()
  {
    timeval tv; gettimeofday( &tv, 0 );
    return ((double)tv.tv_sec) + ((double)tv.tv_usec) * 1e-6;
  }
  
public:
  
  DumpFPS() :
    prev( getTime() ),
    avgFPS( 0.0 )
  {}
  
  void processEvent( const GraphicsEvent * event )
  {
    double time = getTime();
    
    if( event->id == GE_REFRESH_BEGIN ) {
      float fps = time - prev;
      prev = time;
      avgFPS = 0.75 * avgFPS + 0.25 * fps;
      
      cout << "current fps: " << std::fixed << std::setprecision(1)
           << (1.0 / fps) << ", avg fps: " << (1.0 / avgFPS) << endl;
    }
  }
  
};








int main( int argc, char * argv[] )
{
  GLOWDevice::Init( argc, argv );
  GLOWDevice window;
  GLOWViewport viewport( window );
  
  // switch on automatic display list generation
  ((OpenGLRenderer *)viewport.getRenderer())->setAutoDisplaylisting( true );
  
  // attach a framerate counter
  window.events.addListener( new DumpFPS() );
  
  
  // world
  ODEWorld world;
  
  // collision detection
  ODECollisionRenderer collisionRenderer( &world );
  
  
  
  
  shared_ptr<Shape> camBasisShape
    ( new shapes::Located
      ( BasicLocator( makeVector3d( -0.3, -0.5, -1.5 ),
                      BasisMatrix
                      ( makeVector3d( -1.0, 0.0, -1.0 ),
                        //( makeVector3d( 0.0, 0.0, -1.0 ),
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
  
  
  
  
  // create the big union object
  
  shared_ptr<shapes::Union> bigUnion( new shapes::Union() );
  shared_ptr<Shape> theSphere( new shapes::Sphere( 0.1) );
  for( int i = 0 ; i < 100 ; i++ ) {
    bigUnion->targets.push_back
      ( shapes::Located::create
        ( BasicLocator( makeVector3d( ((real)i) / 2.0, 0.0, 0.0 ) ),
          theSphere ));
  }
  
  // select to use or not to use display lists
  //shared_ptr<Shape> bigUnionShape( shapes::Precomputed::create( bigUnion ));
  shared_ptr<Shape> bigUnionShape( bigUnion );
  
  Object unionObject
    (( Object::Params( sptr( new ODELocator( makeVector3d(-25.0, 0.0, -5.0) )),
                       sptr( new BasicVisual( bigUnionShape, &redMat )),
                       sptr( new BasicGeometry
                             ( bigUnionShape, sptr( new CollisionMaterial
                                                    ( 0.5, 0.5, 0.0 )))))));
  world.addObject( &unionObject );
  
  
  
  
  // light object (use double parentheses to avoid a gcc parser 'bug')
  Object lightObj
    (( Object::Params( sptr( new ODELocator( makeVector3d( 2.0, 2.0, -3.0 ))),
                       sptr( new BasicVisual( sptr( new shapes::Sphere ),
                                              &redEmissionMat )))));
  world.addObject( &lightObj );
  
  // camera object
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
        camGeom ));
  //camTarget->setAutoRoll( false );
  Controller::ControlMap camKeymap; fillCameraKeymap( camKeymap );
  world.addObject( camTarget );
  viewport.addActor( camTarget, &camKeymap );
  
  
  
  
  world.activate( true );
  collisionRenderer.connect();
  
  
  
  
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
