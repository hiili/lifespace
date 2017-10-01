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
 * @file ODECollisionRenderer.hpp
 *
 * Renders World Objects into an ODEWorld as ODE collision geoms.
 */

/**
 * @defgroup ODECollisionRenderer ODECollisionRenderer
 * @ingroup Renderers
 *
 * Renders World Objects into an ODEWorld as ODE collision geoms.
 */

/**
 * @class lifespace::ODECollisionRenderer
 * @ingroup ODECollisionRenderer
 *
 * @brief
 * Renders World Objects into an ODEWorld as ODE collision geoms.
 *
 * All objects that had a Geometry on the moment the collision renderer was
 * connected will be monitored for changes. Object deletion, geometry replacing
 * and geometry removal are supported. Direct geometry changes are not
 * supported (you can make such changes by creating a new geom and replacing
 * the old with it). Objects that did not initially have a geometry and objects
 * whose geometry is once removed will not be monitored and thus later adding a
 * geometry will have no effect.
 *
 * \par Limitations
 * Scaling rotated shapes is allowed only if the rotation is axis-aligned
 * (within EPS limits), because some ODE geoms cannot be scaled in non-axis
 * directions. This is checked in debug mode (with assert()), but in release
 * mode it will just quietly rotate the scaling enough to make it possible.
 *
 * \par
 * Spheres can be scaled only uniformly in each direction (within EPS
 * limits). This is checked in debug mode (with assert()), but in release mode
 * the scaling components are just averaged.
 *
 * @todo
 * This renderer is a total mess, rewrite it!
 *
 * @bug
 * No memory management of any kind yet, will leak like hell if deleting source
 * Objects.
 */
#ifndef LS_R_ODECOLLISIONRENDERER_HPP
#define LS_R_ODECOLLISIONRENDERER_HPP


#include "../../types.hpp"
#include "../Renderer.hpp"
#include "Collider.hpp"
#include "../../Graphics/types.hpp"
#include "../../Structures/ODEWorld.hpp"
#include "../../Utility/Event.hpp"

#include <ode/ode.h>
#include <ode/odecpp.h>
#include <ode/odecpp_collision.h>

#include <iostream>

#include <boost/shared_ptr.hpp>




namespace lifespace {
  
  
  
  
  class ODECollisionRenderer :
    public Renderer,
    public EventListener<GraphicsEvent>
  {
    ODEWorld * renderTarget;
    GraphicsEvents syncEventId;
    
    Collider * collider;
    
    
  public:
    
    /* constructors/destructors/etc */
    
    ODECollisionRenderer( ODEWorld * renderTarget_ ) :
      Renderer(),
      renderTarget( renderTarget_ ),
      syncEventId( GE_TICK ),
      collider( 0 )
    {}
    
    virtual ~ODECollisionRenderer()
    {
      disconnect();
    }
    
    
    /* accessors */
    
    virtual void setRenderTarget( RenderTarget * newRenderTarget )
    { assert(false); /* not (yet?) supported. */ }
    
    virtual void setRenderSource( boost::shared_ptr<const RenderSource>
                                  newRenderSource )
    { assert(false); /* not (yet?) supported. */ }
    
    
    /* operations */
    
    void connect()
    {
      assert( !collider );
      collider = new Collider( *renderTarget );
    }
    
    void disconnect()
    {
      delete collider; collider = 0;
    }
    
    
    /**
     *
     */
    virtual void render()
    {
      // no-op if no rendertarget or not connected
      if( !renderTarget || !collider ) return;
      
      // do the actual rendering
      collider->syncGeomsWithWorld();
      collider->collide();
    }
    
    /** */
    virtual void processEvent( const GraphicsEvent * event )
    {
      if( event->id == syncEventId ) render();
    }
    
  };
  
  
  
  
}   /* namespace lifespace */




#endif   /* LS_R_ODECOLLISIONRENDERER_HPP */
