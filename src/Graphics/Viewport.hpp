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
 * @file Viewport.hpp
 *
 * A Viewport represents a rendering context within an OpenGL device, and
 * connects the simulation world to it by rendering the output of a Camera
 * object into the viewport's area on the graphics device.
 */

/**
 * @class lifespace::Viewport
 * @ingroup Graphics
 *
 * @brief
 * A Viewport represents a rendering context within an OpenGL device, and
 * connects the simulation world to it by rendering the output of a Camera
 * object into the viewport's area on the graphics device.
 *
 * A viewport is attached to a Device upon construction, and it usually cannot
 * be detached without destructing it. A video feed from a Camera can be sent
 * to the viewport's display area with the setCamera() method. The Viewport
 * class contains also some low-level state abstractions on top of the
 * underlying OpenGL rendering context.
 *
 * \par Timestepping the world
 * A Camera is also a normal object in the simulation world, so the world must
 * be timestepped from somewhere to allow the camera to move. The world can be
 * stepped explicitly by calling its timestep() method, or it can be connected
 * to the eventhost of a Graphics Device, in which case it will timestep
 * automatically on each received GE_TICK event. See the class World for
 * details.
 *
 * @sa Device, World
 */
#ifndef LS_G_VIEWPORT_HPP
#define LS_G_VIEWPORT_HPP


#include "../types.hpp"
#include "types.hpp"
#include "../Renderers/Renderer.hpp"
#include "../Renderers/RenderTarget.hpp"
#include "../Structures/Camera.hpp"

#include <pthread.h>
#include <GL/gl.h>
#include <list>
#include <cassert>

#include <boost/shared_ptr.hpp>




namespace lifespace {
  
  
  /* forwards */
  class Device;
  
  
  
  
  class Viewport :
    public RenderTarget,
    public EventListener<GraphicsEvent>
  {
    
    /** Mutex for all static thread-safe methods. */
    static pthread_mutex_t mutex;
    
    
    /** A thread -> Viewport map. */
    typedef std::map<pthread_t, Viewport *> viewports_t;
    
    /** Current viewports for each rendering thread. These are set
        automatically in the Viewport's applyCameraToGfx(). */
    static viewports_t currentViewports;
    
    /**
     * Sets the current Viewport for the calling thread. Is thread-safe.
     */
    static void setCurrentViewport( Viewport * viewport );
    
    
    /** Current camera whose OpenGL stream is rendered to the viewport. */
    boost::shared_ptr<const Camera> camera;
    
    
  protected:

    /**
     * Initializes the OpenGL context with some general default values. This
     * will be called from the viewport's constructor.
     */
    virtual void initContext();
    
    /**
     * Renders the world seen by the current camera to the underlying OpenGL
     * context. No state preparations are done here anymore: the OpenGL
     * rendering command stream defining the contents of the view will start
     * immediately. If the current camera is null, then nothing is done.
     */
    void applyCameraToGfx();
    
    
  public:
    
    /**
     * This can be used during a rendering pass to find the Viewport that is
     * currently being rendered in the calling thread. This must not be called
     * outside a rendering pass! Is thread-safe.
     *
     * @todo
     * Remove this (should be unused now when the OpenGLRenderer exists)
     */
    inline static Viewport * getCurrentViewport();
    
    
    /** Viewport events. */
    EventHost<GraphicsEvent> events;
    
    
    /* constructors/destructors/etc */
    
    /**
     * Creates a new viewport into the given device. Ownership of the renderer
     * object is taken and it is deleted in the destructor.
     */
    Viewport( Device & parentDevice, Renderer * renderer );
    
    /**
     * Creates a new viewport into the given device. OpenGLRenderer is used as
     * the renderer.
     */
    Viewport( Device & parentDevice );
    
    virtual ~Viewport();
    
    /**
     * Causes this Viewport to be closed and marked for automatic deletion as
     * soon as it becomes possible. Any pointers to this object should be
     * considered invalid immediately after this call.
     */
    virtual void close() {}
    
    
    /* accessors */
    
    /**
     * Sets the camera whose OpenGL stream is rendered to this viewport. If the
     * viewport already has a camera set, then the new one will replace the old
     * one. Giving a null pointer disables rendering of this viewport.
     */
    void setCamera( boost::shared_ptr<const Camera> newCamera )
    {
      camera = newCamera;
      if( renderer ) renderer->setRenderSource( camera );
    }
    
    /**
     * Returns the current camera, or null if no camera is attached.
     */
    boost::shared_ptr<const Camera> getCamera() const
    { return camera; }
    
    
    virtual void setRenderer( Renderer * newRenderer )
    {
      if( renderer ) renderer->setRenderTarget( 0 );
      RenderTarget::setRenderer( newRenderer );
      if( renderer ) renderer->setRenderTarget( this );
    }
    
    
    /**
     * Handler for received graphics events. A GE_TICK event should cause a
     * derived Viewport class to redraw itself.
     */
    virtual void processEvent( const GraphicsEvent * event ) {}
    
  };
  
  
  
  
  /* inlines */
  
  
  Viewport * Viewport::getCurrentViewport()
  {
    Viewport * result;
    
    pthread_mutex_lock( &mutex );
    viewports_t::iterator i = currentViewports.find( pthread_self() );
    assert( i != currentViewports.end() );
    result = i->second;
    pthread_mutex_unlock( &mutex );
    
    return result;
  }
  
  
  
  
}   /* namespace lifespace */




#endif   /* LS_G_VIEWPORT_HPP */
