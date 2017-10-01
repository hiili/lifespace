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
 * @file World.hpp
 *
 * Encapsulates a simulation subspace hierarchy into a single object, which can
 * be timestepped with a single method, or automatically on each display or
 * viewport refresh by connecting it to the eventhost of a Graphics Device or
 * Viewport.
 */

/**
 * @class lifespace::World
 * @ingroup Structures
 *
 * @brief
 * Encapsulates a simulation subspace hierarchy into a single object, which can
 * be timestepped with a single method, or automatically on each display or
 * viewport refresh by connecting it to the eventhost of a Graphics Device or
 * Viewport.
 *
 * If using graphics events for automatic timestepping, then the default step
 * length must also be set with setDefaultDt().
 *
 * @todo
 * Rethink what it means if a World has a locator (and is it interpreted
 * consistently when doing absolute<->relative coordinate transformations).
 *
 * @sa Subspace, Object, Device, Viewport
 */
#ifndef LS_S_WORLD_HPP
#define LS_S_WORLD_HPP


#include "../types.hpp"
#include "../Graphics/types.hpp"
#include "Subspace.hpp"
#include "../Utility/Event.hpp"
#include <boost/shared_ptr.hpp>
#include <cmath>




namespace lifespace {
  
  
  /* forwards */
  class Integrator;
  class Environment;
  
  
  
  
  class World :
    public Subspace,
    public EventListener<GraphicsEvent>
  {
    double worldTime;
    long long worldIteration;
    
    real defaultDt;
    GraphicsEvents syncEventId;
    
    
  public:
    
    /**
     * Creates a new world.
     *
     * @param subspaceParams   Constructor params for the Subspace base class.
     */
    World( const Subspace::Params & subspaceParams ) :
      Subspace( subspaceParams ),
      worldTime( 0.0 ),
      worldIteration( 0 ),
      defaultDt( NAN ),
      syncEventId( GE_TICK )
    {}
    
    /**
     * Creates a new world with a default constructed Subspace base class.
     */
    World() :
      Subspace(),
      defaultDt( NAN ),
      syncEventId( GE_TICK )
    {}
    
    
    /**
     * Returns a pointer to this world. This method is the end point for a
     * search initiated from an Object::getHostWorld() method.
     */
    virtual const World * getHostWorld() const
    { return this; }

    /** */
    virtual World * getHostWorld()
    { return this; }
    
    
    /**
     * Does nothing and returns always true. This method is the end point for a
     * transformation chain initiated from an Object::getWorldLocator() method.
     */
    virtual bool transformToWorldCoordinates( Locator & target ) const
    { return true; }
    virtual bool transformToWorldCoordinates( Vector & target ) const
    { return true; }
    virtual bool transformToWorldCoordinates( Matrix & target ) const
    { return true; }
    
    
    /**
     * Takes a timestep of the given length.
     *
     * This method encapsulates all individual computation passes that would be
     * needed to run the simulation if working directly with subspaces and
     * objects.
     *
     * The world can be stepped also by calling the individual methods
     * directly, but the time and iteration counters of the world object will
     * not get incremented in that case.
     *
     * @sa setDefaultDt, Device::events, Viewport::events
     */
    virtual void timestep( real dt )
    {
      prepare( dt );
      step();
      
      worldTime += dt;
      worldIteration++;
    }
    
    /** Returns the elapsed simulation time. */
    double getWorldTime() const
    { return worldTime; }
    
    /** Returns the current simulation iteration number, that is, the number of
        times the timestep() method has been called. */
    long long getWorldIteration() const
    { return worldIteration; }
    
    /**
     * Sets the default timestep length that will be used if the world is
     * timestepped automatically by the Window::Ticker.
     */
    void setDefaultDt( real dt )
    { defaultDt = dt; }
    
    /** */
    virtual void processEvent( const GraphicsEvent * event )
    {
      if( event->id == syncEventId ) {
        assert( !std::isnan( defaultDt ) );
        timestep( defaultDt );
      }
    }
    
  };
  
  
  
  
}   /* namespace lifespace */


#endif   /* LS_S_WORLD_HPP */
