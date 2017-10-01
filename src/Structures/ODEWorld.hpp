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
 * @file ODEWorld.hpp
 *
 * An extension of the World class that can contain ODE (Open Dynamics Engine)
 * Subspaces and Objects.
 */

/**
 * @class lifespace::ODEWorld
 * @ingroup Structures
 *
 * @brief
 * An extension of the World class that can contain Objects managed by ODE
 * (Open Dynamics Engine).
 *
 * Objects can be assigned to be managed by ODE by using ODELocators as the
 * Objects' locators, inserting the Objects under an ODEWorld and activating
 * the world (ODEWorld::activate()). This causes all ODELocator-located Objects
 * to be collision checked and timestepped by ODE.
 *
 * @par World hierarchy and ODE's flat world
 * ODE's integrator part does not support hierarchial worlds, so a Lifespace's
 * subspace hierarchy must be flattened when connected to ODE. This is done
 * almost transparently by the ODELocator class: it flattens the world when
 * inserting it into ODE, and reconstructs the relative view when information
 * is read back from it. It can be used almost in the same way as normal
 * locators, see the documentation of the class ODELocator for details.
 *
 * @par
 * The world hierarchy is composed as follows:
 *   - A subspace hierarchy is composed normally from Subspaces and
 *     Objects. ODELocator objects can be used in objects that should be
 *     managed by ODE, but they must have an ODEWorld object above them in the
 *     hierarchy (the ODEWorld can be, but does not have to be the root node:
 *     worlds can be embedded within other worlds). Normal locators can be used
 *     also under an ODEWorld where ODE is not needed.
 *   - The ODELocators are activated by calling the ODEWorld::activate()
 *     method. This will also lock to their current host spaces all Objects
 *     that contain ODELocators (so that the ODELocators cannot be disconnected
 *     from the ODEWorld without first deactivating them), see below for more
 *     details.
 *
 * @par Activating and deactivating an ODEWorld
 * The ODELocator objects must be activated before they are fully
 * functional. This can be done only after they are inserted into a subspace
 * hierarchy so that they reside under an ODEWorld. The activation is done with
 * the ODEWorld::activate() method, which effectively activates all ODELocators
 * that are found under it.
 *
 * @par Locking of active hierarchies
 * Active ODELocators must not get disconnected from the host ODEWorld or
 * relocated in the hierarchy in any way. This is enforced by locking all
 * relevant objects and subspaces to their host spaces with the
 * Object::lockToHostSpace() method during activation. This makes it impossible
 * to remove them from their current host spaces (attempting will fail an
 * assertion). If a locked subspace branch needs modifications, it can be
 * deactivated with the static ODEWorld::Activate() method and reactivated with
 * the same method after maintenance. Deleting a locked branch is allowed
 * without first deactivating it. Adding new objects or branches to a locked
 * hierarchy is allowed, but the new object or branch must still be activated
 * separately after it has been inserted to the hierarchy (by calling
 * ODEWorld::Activate() directly for the new Object or Subspace).
 *
 * @sa ODELocator, World, Subspace, Object
 */
#ifndef LS_S_ODEWORLD_HPP
#define LS_S_ODEWORLD_HPP


#include "../types.hpp"
#include "World.hpp"
#include <ode/ode.h>
#include <ode/odecpp.h>
#include <boost/shared_ptr.hpp>




namespace lifespace {
  
  
  
  
  class ODEWorld :
    public World,
    public dWorld
  {
    real dt;
    
    
  public:
    
    /**
     * Activates or deactivates the target object or subspace (and recursively
     * all entities that are connected to it).
     *
     * ODELocator objects need to be connected internally directly into the
     * root ODEWorld to be fully functional, and this method does this
     * connecting. One restriction follows:
     *
     * @warning
     * The subspace hierarchy leading to Objects containing activated
     * ODELocators must not be broken or rearranged in any way!
     *
     * This restriction is enforced by locking all target objects and subpaces
     * (even those that do not contain ODELocators), to their host spaces
     * (Object::lockToHostSpace()). Locked objects cannot be removed from their
     * host subspaces. They will be automatically unlocked upon deactivation.
     *
     * (De)activating an object or subspace hierarchy twice is an
     * error. Deleting a locked Object/Subspace is allowed and will cause it to
     * be deactivated first. Adding new Objects or Subspaces to a locked
     * Subspace is allowed (but they must still be activated separately after
     * insertion).
     *
     * @param hostODEWorld   The host ODEWorld to activate, or null to
     *                       deactivate.
     *
     * @sa activate()
     */
    static void Activate( Object * target, ODEWorld * hostODEWorld );
    
    
    /**
     * Creates a new ODE-managed world.
     *
     * @param subspaceParams   Constructor params for the subspace base class.
     */
    ODEWorld( const Subspace::Params & subspaceParams ) :
      World( subspaceParams )
    {}
    
    /**
     * Creates a new world with a default constructed Subspace base class.
     */
    ODEWorld() :
      World()
    {}
    
    
    /**
     * Activates or deactivates the whole world (all connected entities).
     *
     * This method calls the static ODEWorld::Activate() method with this world
     * as the target object, see its documentation for details.
     *
     * @sa Activate()
     */
    void activate( bool activation )
    { Activate( (Object *)this, activation ? this : 0 ); }
    
    
    /**
     * Sets the world gravity.
     *
     * This is currently just a wrapper for the inherited dWorld::setGravity()
     * method, but this might change in the future so it is recommended to use
     * this method instead.
     */
    void setGravityVector( const Vector & gravity )
    {
      assert( gravity.size() == 3 );
      dWorld::setGravity( gravity(0), gravity(1), gravity(2) );
    }
    
    /**
     * Returns the current gravity vector.
     *
     * The result is returned by value, and currently the contents come
     * directly from the dWorld::getGravity() method.
     */
    Vector getGravityVector() const
    {
      dVector3 result;
      dWorld::getGravity( result );
      return makeVector3d( result[0], result[1], result[2] );
    }
    
    
    virtual void prepare( real dt_ )
    {
      dt = dt_;
      World::prepare( dt_ );
    }
    
    virtual void step()
    {
      dWorld::step( dt );
      World::step();
    }
  };
  
  
  
  
}   /* namespace lifespace */


#endif   /* LS_S_ODEWORLD_HPP */
