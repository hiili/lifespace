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
 * @file Subspace.hpp
 *
 * A Subspace contains a group of objects and other Subspaces, it may have its
 * own rendering environment (lights etc.), and it is associated with an
 * integrator.
 */

/**
 * @class lifespace::Subspace
 * @ingroup Structures
 *
 * @brief
 * A Subspace contains a group of objects and other Subspaces, it may have its
 * own rendering environment (lights etc.), and it is associated with an
 * integrator.
 *
 * @todo
 * Params struct, like in Object.
 *
 * @sa Object, Integrator, Environment
 */
#ifndef LS_S_SUBSPACE_HPP
#define LS_S_SUBSPACE_HPP


#include "../types.hpp"
#include "Object.hpp"
#include "Vector.hpp"
#include "Matrix.hpp"
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <list>
#include <algorithm>


namespace lifespace {
  
  
  /* forwards */
  class Integrator;
  class Environment;
  class Locator;
  
  
  
  
  class Subspace :
    public virtual Object
  {
  public:
    typedef std::list< boost::shared_ptr<Object> > objects_t;
    
    
  private:
    objects_t objects;
    boost::shared_ptr<Environment> environment;
    boost::shared_ptr<Integrator> integrator;
    
    /** Should the Objects in this Subspace collide with each other? */
    bool selfCollide;
    
    
  public:
    
    /* constructors/destructors/etc */
    
    /**
     * Subspace constructor params.
     *
     * The default constructor will create params for an empty Object, create a
     * new Environment and instantiate a BasicIntegrator for the
     * Subspace. Self-collide will be enabled by default.
     *
     * @sa Object::Params
     */
    struct Params {
      
      /** Params for the Object base class. */
      Object::Params objectParams;
      
      /** The Environment for this Subspace. */
      boost::shared_ptr<Environment> environment;
      
      /** The Integrator for this Subspace. */
      boost::shared_ptr<Integrator> integrator;
      
      /**
       * Should the Objects in this Subspace collide with each other?
       *
       * @note
       * Handling is not yet implemented, and currently this must always be set
       * true (is checked with an assert in debug mode)!
       */
      bool selfCollide;
      
      Params();
      Params( const Object::Params & objectParams );
      Params( const Object::Params & objectParams,
              boost::shared_ptr<Environment> environment,
              boost::shared_ptr<Integrator> integrator );
      Params( const Object::Params & objectParams,
              bool selfCollide );
      Params( const Object::Params & objectParams,
              boost::shared_ptr<Environment> environment,
              boost::shared_ptr<Integrator> integrator,
              bool selfCollide );
    };
    
    /**
     * Constructs a new Subspace from the given params, or from defaults if the
     * params are omitted.
     *
     * @sa Subspace::Params
     */
    Subspace( const Subspace::Params & params = Subspace::Params() );
    
    /**
     * Destructs the Subspace. The Subspace and all contained Objects will be
     * deactivated if they are active. Contained Objects are not explicitly
     * deleted: the shared_ptrs pointing to them are just released.
     */
    virtual ~Subspace();
    
    
    /**
     * Transforms the target locator, vector or matrix to absolute world
     * coordinates by following the subspace hierarchy upwards until a World
     * object is encountered.
     *
     * @return   True on success, false on failure (this subspace is not inside
     *           a world).
     *
     * @note
     * The target locator might get modified even on failure!
     */
    virtual bool transformToWorldCoordinates( Locator & target ) const;
    virtual bool transformToWorldCoordinates( Vector & target ) const;
    virtual bool transformToWorldCoordinates( Matrix & target ) const;
    
    /**
     * Transforms the target locator to the coordinates of the given subspace
     * by following the subspace hierarchy upwards until the given subspace is
     * encountered.
     *
     * @return   True on success, false on failure (the given subspace is not a
     *           parent of this subspace).
     *
     * @note
     * The target locator might get modified even on failure!
     */
    bool transformToSubspaceCoordinates( const Subspace * subspace,
                                         Locator & target ) const;
    
    
    /* mutators */
    
    /**
     * Adds the given Object to the Subspace.
     *
     * @note
     * An object can belong only to \em one subspace at a time (and no multiple
     * instances of the same object are allowed within one subspace). It is an
     * error to call this method with an object already contained in a
     * subspace.
     *
     * @internal
     * The previous requirement is enforced in the Object::setHostSpace()
     * method.
     *
     * @note
     * Objects will always be processed in the order they were added.
     *
     * @remarks
     * An object is not allowed to exist multiple times even in a \em single
     * subspace, because otherwise its prepare() and step() methods could get
     * called multiple times. Simultaneous existence in multiple subspaces
     * would break the hierarchial structure of the world.
     *
     * @sa removeObject()
     */
    virtual void addObject( boost::shared_ptr<Object> object )
    {
      objects.push_back( object );
      object->setHostSpace( this );
    }
    
    /**
     * Removes the given Object from the Subspace. It is an error to call this
     * method if the object is not contained in the subspace.
     *
     * @sa addObject()
     */
    virtual void removeObject( boost::shared_ptr<Object> object )
    {
      // assert that the object really is contained in this subspace
      assert( object->getHostSpace() == this &&
              std::find( objects.begin(), objects.end(), object )
              != objects.end() );
      
      objects.remove( object );
      object->setHostSpace( 0 );
    }
    
    
    boost::shared_ptr<const Environment> getEnvironment() const
    { return environment; }
    
    boost::shared_ptr<Environment> getEnvironment()
    { return environment; }
    
    const objects_t & getObjects() const
    { return objects; }
    
    objects_t & getObjects()
    { return objects; }
    
    
    void localPrepare( real dt );
    void localStep();
    
    virtual void prepare( real dt );
    virtual void step();
  };
  
  
  
  
}   /* namespace lifespace */



#endif   /* LS_S_SUBSPACE_HPP */
