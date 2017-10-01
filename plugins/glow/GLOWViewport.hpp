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
 * @file GLOWViewport.hpp
 *
 * The Graphics Viewport and Controller implementation of the GLOW Toolkit
 * plugin.
 */

/**
 * @class lifespace::plugins::pglow::GLOWViewport
 * @ingroup glow
 *
 * @brief
 * The Graphics Viewport and Controller implementation of the GLOW Toolkit
 * plugin.
 *
 * A GLOWViewport is attached to a GLOWDevice upon construction, and it cannot
 * be detached without destructing it. A video feed from a Camera can be sent
 * to the viewport's display area with the setCamera() method. User input from
 * the viewport can be forwarded to an Actor with the addActor() method in the
 * Controller interface.
 *
 * @par User input
 * Keyboard and mouse events are received with the OnKeyboard, OnKeyboardUp,
 * OnMouseMotion and OnMouseDrag events. These can be overridden if special
 * handling is needed. By default they direct the events to the Actors that
 * have been attached with addActor() method. The key codes can be translated
 * to the correct actor control ids by providing a ControlMap with the Actor
 * object. Some special keys are used to control the Viewport itself (including
 * its input handling behaviour), see below for details.
 *
 * @par
 * Different input is handled as follows:
 *   - Some special key events are consumed directly by the Viewport:
 *       - F9: switch inverse mouse on/off (on by default).
 *       - F10: connect/disconnect mouse to the viewport under the pointer.
 *              (same as middle mouse button)
 *       - ESC: quit the program immediately (exit()).
 *   - Other keyboard events are always sent to the Actor if this Viewport is
 *     active (the mouse pointer is over the viewport).
 *   - Key down events are sent with the keycode as the control id, and 1.0 as
 *     the control value.
 *   - Key up events are sent with the keycode as the control id, and 0.0 as
 *     the control value. (note that these events will not be generated with
 *     old GLUTs, see GLOWDevice::IsOldGlut() method)
 *   - Mouse button events are sent with one of the ExtraControls enumeration
 *     values as the control id, and 1.0 (button down) or 0.0 (button up) as
 *     value.
 *   - Mouse motion events are sent \em only if the mouse is "connected" to the
 *     GLOWViewport by clicking the middle mouse button (or F10) over the
 *     viewport. When connected, the pointer is made invisible and all motion
 *     is sent as delta motion events directly to the actor(s): control ids are
 *     taken from the ExtraControls enumeration and the value is the amount of
 *     motion (in pixels) since the last motion event in the corresponding
 *     direction. The mouse can be "disconnected" by clicking the middle mouse
 *     button (or F10) again.
 *
 * \par Graphics Events
 * The following events are emitted by the GLOWViewport:
 *   - GE_REFRESH_BEGIN: Is sent from the GLOW OnBeginPaint() callback,
 *     i.e. when the contents of the viewport is about to be rendered. GLOW
 *     will block until all listeners have processed the event.
 *   - GE_REFRESH_END: Is sent from the GLOW OnEndPaint() callback, i.e. when
 *     the contents of the viewport is rendered. GLOW will block until
 *     all listeners have processed the event.
 * The following events are accepted by the GLOWViewport:
 *   - GE_TICK: The viewport will be scheduled to be redrawn. This is
 *     equivalent for calling the refresh() method. This will have no effect if
 *     auto refresh is enabled (it is enabled by default).
 *
 * @note
 * The constructors register directly some GLUT callbacks for a GLOW window
 * (which is not recommended by the GLOW manual). This works at least with GLOW
 * version 1.0.4, but check this if having problems with keyboard callbacks
 * with another version.
 * 
 * @ifdef done_todos
 * @todo
 * Inherit from the Controller class when it has been created.
 * @endif
 * 
 * @sa GLOWDevice, Device, Viewport, Controller
 */
#ifndef LS_P_GLOW_GLOWVIEWPORT_HPP
#define LS_P_GLOW_GLOWVIEWPORT_HPP


#include <lifespace/types.hpp>
#include <lifespace/Graphics/Viewport.hpp>
#include <lifespace/Control/Controller.hpp>
#include "GLOWDevice.hpp"
#include <glow/glow.h>
#include <GL/gl.h>
#include <list>
#include <cassert>




namespace lifespace {
  
  
  
  
  /* forwards */
  class Light;
  class Camera;
  class Actor;
  class Subspace;
  
  
  
  
namespace plugins { namespace pglow {
  
  
  /* forwards */
  class GLOWDevice;
  
  
  
  
  class GLOWViewport :
    public Viewport,
    public Controller,
    private glow::GlowSubwindow,
    private glow::GlowIdleReceiver
  {
    friend class GLOWDevice;
    
    static const bool DEFAULT_INVERSE_MOUSE = true;
    
    GLOWDevice & parentDevice;
    
    bool mouseConnected;
    bool inverseMouse;
    bool autoRefresh;
    
    /**True if the viewport was opened to fill the whole device (window). This
       affects the handling of window resize events: if fullWindow is true,
       then window resizing will cause this viewport to be resized also
       automatically to fill the new window size. */
    bool fullWindow;
    
    
    static void KeyboardUpFunc( unsigned char key, int x, int y );
    static void SpecialUpFunc( int key, int x, int y );
    
    
    /**
     * @warning
     * This method registers directly some GLUT callbacks for a GLOW window
     * (which is not recommended by the GLOW manual). This works at least with
     * GLOW version 1.0.4, but check this if having problems with keyboard
     * callbacks with another version.
     */
    virtual void initContext();
    
    void setMouseConnected( bool connect );
    void handleMouseMotion( int x, int y );
    
    
  public:
    
    /**
     * These codes are used as the keycode when the corresponding events
     * occur. Mouse motion controls have the location delta (in pixels) as the
     * value. Button controls have the value 1.0 on button down event, and
     * value 0.0 on button up event.
     *
     * See the GLOWViewport documentation for details about user input.
     *
     * @sa GLOWViewport
     */
    enum ExtraControls {
      MOUSE_LBUTTON = 0x100,
      MOUSE_RBUTTON = 0x101,
      MOUSE_DX      = 0x102,
      MOUSE_DY      = 0x103
    };
    
    
    /**
     * Creates a new viewport into the given GLOW window. The viewport will
     * cover the entire window. */
    GLOWViewport( GLOWDevice & parentDevice );
    
    /**
     * Creates a new viewport into the given GLOW window. The viewport's size
     * and location are given as arguments.
     */
    GLOWViewport( GLOWDevice & parentDevice,
                  int x, int y, int width, int height );
    
    virtual ~GLOWViewport()
    {
      if( parentDevice.getFullWindowViewport() == this ) {
        parentDevice.setFullWindowViewport( 0 );
      }
    }
    
    /**
     * Causes the viewport to be closed and this Viewport object deleted after
     * the current callback finishes. Any pointers to this object will be
     * invalid after that.
     */
    virtual void close();
    
    
    /**
     * Enables or disables auto-refresh. If this is enabled, then the viewport
     * schedules itself to be redrawn on every idle message received from
     * GLOW. Otherwise the client application is responsible for calling the
     * refresh() method whenever a refresh is required.
     */
    void setAutoRefresh( bool state );
    
    bool isFullWindow();
    
    /**
     * Schedules the viewport to be redrawn when control is returned to
     * GLOW. This can be called, for example, whenever a GE_TICK event is
     * received from the GLOWDevice. This will have no effect if auto refresh
     * is enabled.
     */ 
    void refresh();
    
    /**
     * Handler for received graphics events.
     *
     * Accepted events:
     *   - GE_TICK: The viewport will be scheduled to be redrawn. This is
     *     equivalent for calling the refresh() method. This will have no
     *     effect if auto refresh is enabled (it is enabled by default).
     */
    virtual void processEvent( const GraphicsEvent * event );
    
    
    virtual void OnMessage( const glow::GlowIdleMessage & message );
    
    virtual void OnReshape( int width, int height );
    
    /**
     * @todo
     * Set the depth buffer range more nicely (is now hardcoded).
     */
    virtual bool OnBeginPaint();
    
    virtual void OnEndPaint();
    
    
    virtual void OnKeyboard( glow::Glow::KeyCode key, int x, int y,
                             glow::Glow::Modifiers modifiers );
    /**
     * Override this to receive key up events.
     *
     * @note
     * This won't be called if the current GLUT implementation does not support
     * key up callbacks. See Window::IsOldGlut()
     *
     * @warning
     *   - keyboard filters are not applied to key up events.
     *   - deferred GLOW operations should not be called here.
     */
    virtual void OnKeyboardUp( glow::Glow::KeyCode key, int x, int y,
                               glow::Glow::Modifiers modifiers );
    virtual void OnMouseDown( glow::Glow::MouseButton mouseButton,
                              int x, int y, glow::Glow::Modifiers modifiers );
    virtual void OnMouseUp( glow::Glow::MouseButton mouseButton,
                            int x, int y, glow::Glow::Modifiers modifiers );
    virtual void OnMouseMotion( int x, int y );
    virtual void OnMouseDrag( int x, int y );
    
  };
  
  
  
  
 }}}   /* namespace lifespace::plugins::pglow */


#endif   /* LS_P_GLOW_GLOWVIEWPORT_HPP */
