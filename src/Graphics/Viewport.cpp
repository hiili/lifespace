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
 * @file Viewport.cpp
 */


#include "../types.hpp"
#include "Viewport.hpp"
#include "Device.hpp"
#include "../Renderers/OpenGLRenderer/OpenGLRenderer.hpp"
#include "../Structures/Camera.hpp"
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <algorithm>
using namespace lifespace;

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;




/* static member initializations */

pthread_mutex_t Viewport::mutex = PTHREAD_MUTEX_INITIALIZER;
std::map<pthread_t, Viewport *> Viewport::currentViewports;




/**
 * Initializes the OpenGL context with some general default values, which
 * include:
 *   - clear color: black with zero alpha.
 *   - depth testing: on
 *   - lighting: on, local viewer mode on
 *   - culling: back-face culling on
 *   - alpha:
 *       - blending: function initialized, actual blending disabled
 *       - testing: enabled, tolerance alpha == 0.1
 *   - normalizing: on
 */
void Viewport::initContext()
{
  // clearing
  glClearColor(0.0, 0.0, 0.0, 0.0);
  
  // depth buffer
  glDepthFunc( GL_LEQUAL );
  glEnable( GL_DEPTH_TEST );
      
  // lighting
  glShadeModel( GL_SMOOTH );
  glLightModelf( GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE );
  glEnable( GL_LIGHTING );
      
  // culling
  glCullFace( GL_BACK );
  glEnable( GL_CULL_FACE );
  
  // alpha blending and testing
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glAlphaFunc( GL_GREATER, 0.1 );
  //glEnable( GL_ALPHA_TEST );
  //glEnable( GL_BLEND );
  
  // normalizing
  glEnable( GL_NORMALIZE );
}




void Viewport::applyCameraToGfx()
{
  // no-op if no renderer attached
  if( !renderer ) return;
  
  setCurrentViewport( this );
  renderer->render();    
  setCurrentViewport( 0 );
}




Viewport::Viewport( Device & parentDevice, Renderer * renderer ) :
  RenderTarget( renderer ),
  camera()
{
  renderer->setRenderTarget( this );
  initContext();
}


Viewport::Viewport( Device & parentDevice ) :
  RenderTarget( new OpenGLRenderer( 0, shared_ptr<Camera>() ) ),
  camera()
{
  renderer->setRenderTarget( this );
  initContext();
}


Viewport::~Viewport()
{
  delete renderer; renderer = 0;
}




void Viewport::setCurrentViewport( Viewport * viewport )
{
  pthread_mutex_lock( &mutex );
  if( viewport ) {
    currentViewports[ pthread_self() ] = viewport;
  } else {
    currentViewports.erase( pthread_self() );
  }
  pthread_mutex_unlock( &mutex );
}
