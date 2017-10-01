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
 * @file GLOWViewport.cpp
 */


#include <lifespace/types.hpp>
#include <lifespace/Graphics/Light.hpp>
#include <lifespace/Structures/Camera.hpp>
#include <lifespace/Control/Controller.hpp>

#include "GLOWViewport.hpp"
#include "GLOWDevice.hpp"

#include <glow/glow.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <algorithm>

using namespace lifespace;
using namespace lifespace::plugins::pglow;
using namespace glow;




/**
 * @warning
 * A static cast is made with the assumption that key up callbacks will not be
 * registered to non-Viewport windows.
 */
void GLOWViewport::KeyboardUpFunc( unsigned char key, int x, int y )
{
  GLOWViewport * viewport =
    (GLOWViewport *)( Glow::ResolveWindow( glutGetWindow() ) );
  
  if( viewport ) {
    viewport->OnKeyboardUp( Glow::KeyCode(key), x, y,
                            Glow::Modifiers( glutGetModifiers() ) );
  }
}


/**
 * @warning
 * A static cast is made with the assumption that key up callbacks will not be
 * registered to non-Viewport windows.
 */
void GLOWViewport::SpecialUpFunc( int key, int x, int y )
{
  GLOWViewport * viewport =
    (GLOWViewport *)( Glow::ResolveWindow( glutGetWindow() ) );
  
  if( viewport ) {
    viewport->OnKeyboardUp( Glow::KeyCode(key+Glow::specialKeyOffset), x, y,
                            Glow::Modifiers( glutGetModifiers() ) );
  }
}




void GLOWViewport::initContext()
{
  Glow::RegisterIdle( this );
  
  // *** NOTE: This is against recommendations in the GLOW manual, but does
  // ***       work at least with GLOW version 1.0.4. Check this if having
  // ***       problems with keyboard callbacks.
  
  // register the key up callback and switch of key repeat if supported
  if( !GLOWDevice::IsOldGlut() ) {
    GlowSubwindow::MakeCurGlutWindow();
    glutIgnoreKeyRepeat( true );
    glutKeyboardUpFunc( GLOWViewport::KeyboardUpFunc );
    glutSpecialUpFunc( GLOWViewport::SpecialUpFunc );
  }
  
  Viewport::initContext();
}


/** This viewport must be the GLUT's current window. */
void GLOWViewport::setMouseConnected( bool connect )
{
  mouseConnected = connect;
  SetCursor( connect ? GLUT_CURSOR_NONE : GLUT_CURSOR_INHERIT );
  if( connect ) glutWarpPointer( Width() / 2, Height() / 2 );
}


/** This viewport must be the GLUT's current window. */
void GLOWViewport::handleMouseMotion( int x, int y )
{
  if( !mouseConnected ) return;
  
  // has the mouse really moved?
  if( x != Width() / 2 || y != Height() / 2 ) {
    // send the motion data to actors
    useControl( MOUSE_DX, x - Width() / 2 );
    useControl( MOUSE_DY,
                (inverseMouse ? -1.0 : 1.0) * (y - Height() / 2) );
    // reset the mouse pointer to the middle of the viewport
    glutWarpPointer( Width() / 2, Height() / 2 );
  }
}




GLOWViewport::GLOWViewport( GLOWDevice & parentDevice_ ) :
  Viewport( parentDevice_ ),
  GlowSubwindow( &parentDevice_,
                 0, 0,
                 GlowSubwindow::parentWindowSize,
                 GlowSubwindow::parentWindowSize,
                 Glow::rgbBuffer | Glow::depthBuffer
                 | Glow::doubleBuffer,
                 Glow::keyboardEvents | Glow::mouseEvents
                 | Glow::dragEvents | Glow::motionEvents ),
  parentDevice( parentDevice_ ),
  mouseConnected( false ),
  inverseMouse( DEFAULT_INVERSE_MOUSE ),
  autoRefresh( true ),
  fullWindow( true )
{
  parentDevice.setFullWindowViewport( this );
  initContext();
}


GLOWViewport::GLOWViewport( GLOWDevice & parentDevice_,
                            int x, int y, int width, int height ) :
  Viewport( parentDevice_ ),
  GlowSubwindow( &parentDevice_,
                 x, y, width, height,
                 Glow::rgbBuffer | Glow::depthBuffer
                 | Glow::doubleBuffer,
                 Glow::keyboardEvents | Glow::mouseEvents
                 | Glow::dragEvents | Glow::motionEvents ),
  parentDevice( parentDevice_ ),
  mouseConnected( false ),
  inverseMouse( DEFAULT_INVERSE_MOUSE ),
  autoRefresh( true ),
  fullWindow( false )
{ initContext(); }


void GLOWViewport::close()
{
  setMouseConnected( false );
  
  Viewport::close();
  GlowSubwindow::Close();
}




void GLOWViewport::setAutoRefresh( bool state )
{ autoRefresh = state; }


bool GLOWViewport::isFullWindow()
{ return fullWindow; }


void GLOWViewport::refresh()
{ Refresh(); }


void GLOWViewport::processEvent( const GraphicsEvent * event )
{
  if( event->id == GE_TICK ) refresh();
}




void GLOWViewport::OnMessage( const GlowIdleMessage & message )
{
  if( autoRefresh ) Refresh();
}


void GLOWViewport::OnReshape( int width, int height )
{
  glViewport( 0, 0, width, height );
}


bool GLOWViewport::OnBeginPaint()
{
  // send a graphics event
  GraphicsEvent event = { GE_REFRESH_BEGIN, 0 };
  events.sendEvent( &event );
  
  // init the graphics context
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  if( getCamera() ) {
    // init the OpenGL matrices
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( getCamera()->getFov(), (float)Width() / Height(),
                    0.1, 1000.0 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    
    // render
    applyCameraToGfx();
  }
  
  return true;
}


void GLOWViewport::OnEndPaint()
{
  // send a graphics event
  GraphicsEvent event = { GE_REFRESH_END, 0 };
  events.sendEvent( &event );
}


void GLOWViewport::OnKeyboard( Glow::KeyCode key, int x, int y,
                               Glow::Modifiers modifiers )
{
  switch( key )
    {
    case Glow::f9Key:
      inverseMouse = !inverseMouse;
      break;
    case Glow::f10Key:
      setMouseConnected( !mouseConnected );
      break;
    case Glow::escapeKey:
      exit(0);
      break;
    default:
      useControl( key, 1.0 );
      break;
    }
}


void GLOWViewport::OnKeyboardUp( Glow::KeyCode key, int x, int y,
                                 Glow::Modifiers modifiers )
{
  switch( key )
    {
    case Glow::f9Key:
      break;
    case Glow::f10Key:
      break;
    case Glow::escapeKey:
      break;
    default:
      useControl( key, 0.0 );
      break;
    }
}


void GLOWViewport::OnMouseDown( Glow::MouseButton mouseButton, int x, int y,
                                Glow::Modifiers modifiers )
{
  switch( mouseButton )
    {
    case Glow::leftButton:
      useControl( MOUSE_LBUTTON, 1.0 );
      break;
    case Glow::middleButton:
      setMouseConnected( !mouseConnected );
      break;
    case Glow::rightButton:
      useControl( MOUSE_RBUTTON, 1.0 );
      break;
    }
}


void GLOWViewport::OnMouseUp( Glow::MouseButton mouseButton, int x, int y,
                              Glow::Modifiers modifiers )
{
  switch( mouseButton )
    {
    case Glow::leftButton:
      useControl( MOUSE_LBUTTON, 0.0 );
      break;
    case Glow::middleButton:
      break;
    case Glow::rightButton:
      useControl( MOUSE_RBUTTON, 0.0 );
      break;
    }
}


void GLOWViewport::OnMouseMotion( int x, int y )
{ handleMouseMotion( x, y ); }


void GLOWViewport::OnMouseDrag( int x, int y )
{ handleMouseMotion( x, y ); }
