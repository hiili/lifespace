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
 * @file FrameState.cpp
 */


#include "../../types.hpp"
#include "FrameState.hpp"
#include "OpenGLRenderer.hpp"
#include "../../Graphics/Light.hpp"
using namespace lifespace;

#include <GL/gl.h>




FrameState::FrameState( OpenGLRenderer & renderer_, int maxLights_ ) :
  renderer( renderer_ ),
  nextLight( 0 ),
  maxLights( maxLights_ )
{}




void FrameState::enableLight( const Light & light, int lightNum,
                              const Subspace & hostSpace )
{
  static const GLfloat origin[4] = { 0.0, 0.0, 0.0, 1.0 };
  
  renderer.render( *light.material, lightNum );
  if( light.directional ) {
    glLightfv( GL_LIGHT0 + lightNum, GL_POSITION, light.source );
  } else {
    
    // compute the target object's location relative to the current
    // subspace
    boost::shared_ptr<const Locator> locator
      ( light.object->getSubspaceLocator( &hostSpace ) );
    assert( locator );
    
    // locate and configure the light
    glPushMatrix();
    renderer.render( *locator );
    glLightf( GL_LIGHT0 + lightNum, GL_CONSTANT_ATTENUATION,
              light.attenuation[0] );
    glLightf( GL_LIGHT0 + lightNum, GL_LINEAR_ATTENUATION,
              light.attenuation[1] );
    glLightf( GL_LIGHT0 + lightNum, GL_QUADRATIC_ATTENUATION,
              light.attenuation[2] );
    glLightfv( GL_LIGHT0 + lightNum, GL_POSITION, origin );
    glPopMatrix();
    
  }
  
  glEnable( GL_LIGHT0 + lightNum );
}


void FrameState::disableLight( int lightNum )
{
  glDisable( GL_LIGHT0 + lightNum );
}




void FrameState::pushLight( const Light & light, const Subspace & hostSpace )
{
  if( nextLight < maxLights ) {
    enableLight( light, nextLight, hostSpace );
  }
  nextLight++;
}


void FrameState::popLight( int count )
{
  for( int i = 0 ; i < count ; i++ ) {
    nextLight--;
    if( nextLight < maxLights ) disableLight( nextLight );
  }
}
