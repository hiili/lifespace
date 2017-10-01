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
 * @file GLOWDevice.hpp
 *
 * The Graphics Device implementation of the GLOW Toolkit plugin.
 */

/**
 * @class lifespace::plugins::pglow::GLOWDevice
 * @ingroup glow
 *
 * @brief
 * The Graphics Device implementation of the GLOW Toolkit plugin.
 *
 * \par Graphics Events
 * The following events are emitted by the GLOWDevice:
 *   - GE_TICK: Is sent from the GLOW idle callback, i.e. when the window
 *     refresh cycle is about to start again. GLOW will block until all
 *     listeners have processed the event.
 */
#ifndef LS_P_GLOW_GLOWDEVICE_HPP
#define LS_P_GLOW_GLOWDEVICE_HPP


#include <lifespace/types.hpp>
#include <lifespace/Graphics/Device.hpp>
#include <lifespace/Utility/Event.hpp>
#include <glow/glow.h>




namespace lifespace { namespace plugins { namespace pglow {
  
  
  
  
  /* forwards */
  class GLOWViewport;
  
  
  
  
  class GLOWDevice :
    public Device,
    private glow::GlowWindow,
    private glow::GlowIdleReceiver
  {
    friend class GLOWViewport;
    
    
    /** This is set true if the current GLUT version does not support keyboard
        up events, key repeat disabling and fullscreen mode (GameMode). */
    static const bool IS_OLD_GLUT;
    
    static const int INITIAL_WIDTH = 320;
    static const int INITIAL_HEIGHT = 240;
    
    
    GLOWViewport * fullWindowViewport;
    
    
  public:
    
    /**
     * Returns true if the current GLUT version does not support keyboard up
     * events, key repeat disabling and fullscreen mode (GameMode). In this
     * case the GLOWViewport::OnKeyboardUp callback will never be called, and
     * GLOWViewport::OnKeyboard will receive key repeat events.
     *
     * Key repeat can be coped with, for example, in Actor classes by using
     * pulse control mode instead of continuous control mode with keyboard
     * controls.
     */
    static bool IsOldGlut()
    { return IS_OLD_GLUT; }
    
    
    void setFullWindowViewport( GLOWViewport * viewport )
    { fullWindowViewport = viewport; }
    
    GLOWViewport * getFullWindowViewport()
    { return fullWindowViewport; }
    
    
    /**
     * Initializes the windowing system. This must be called exactly once
     * before using any other windowing functionality. argc and argv are passed
     * directly to GLOW.
     */
    static void Init( int & argc, char ** argv );
    
    
    /**
     * Enters the windowing system's mainloop. This will never return.
     */
    static void MainLoop();
    
    
    /** Creates a new GLOW window with fixed size. */
    GLOWDevice();

    virtual ~GLOWDevice() {}
    
    
    virtual void OnMessage( const glow::GlowIdleMessage & message );
    
    virtual void OnReshape( int width, int height );
    
  };




 }}}   /* namespace lifespace::plugins::pglow */


#endif   /* LS_P_GLOW_GLOWDEVICE_HPP */
