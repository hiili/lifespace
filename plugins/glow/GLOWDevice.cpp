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
 * @file GLOWDevice.cpp
 */


#include <lifespace/types.hpp>
#include <lifespace/Utility/Event.hpp>
#include "GLOWDevice.hpp"
#include "GLOWViewport.hpp"
#include <glow/glow.h>
#include <GL/glut.h>
using namespace lifespace;
using namespace lifespace::plugins::pglow;
using namespace glow;




#if (GLUT_XLIB_IMPLEMENTATION >= 15)
const bool GLOWDevice::IS_OLD_GLUT = false;
#else
#ifdef FREEGLUT
const bool GLOWDevice::IS_OLD_GLUT = false;
#else
const bool GLOWDevice::IS_OLD_GLUT = true;
#endif
#endif




void GLOWDevice::Init( int & argc, char ** argv )
{ glow::Glow::Init( argc, argv ); }




void GLOWDevice::MainLoop()
{ glow::Glow::MainLoop(); }




GLOWDevice::GLOWDevice() :
  GlowWindow( "",
              GlowWindow::autoPosition,
              GlowWindow::autoPosition,
              INITIAL_WIDTH, INITIAL_HEIGHT,
              (Glow::BufferType)0,
              Glow::noEvents ),
  fullWindowViewport( 0 )
{
  Glow::RegisterIdle( this );
}




void GLOWDevice::OnMessage( const GlowIdleMessage & message )
{
  // send the graphics events
  GraphicsEvent event = { GE_TICK, 0 };
  events.sendEvent( &event );
}


void GLOWDevice::OnReshape( int width, int height )
{
  if( fullWindowViewport ) {
    dynamic_cast<glow::GlowSubwindow *>( fullWindowViewport )->
      Reshape( width, height );
  }
}
