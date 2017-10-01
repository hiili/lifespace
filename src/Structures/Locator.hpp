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
 * @file Locator.hpp
 *
 * The abstract base class for all entities with a location, orientation,
 * motion etc.\ in some space.
 */

/**
 * @class lifespace::Locator
 * @ingroup Structures
 *
 * @brief
 * The abstract base class for all entities with a location, orientation,
 * motion etc. in some space.
 *
 * A Locator that is connected to an Object represents a relative position and
 * orientation. The reference frame is defined by the host object: if the
 * Locator's host object is connected to a Subspace chain that ends to a World,
 * then that chain defines the transformations that relate the locator to the
 * world coordinates.
 *
 * Some locators might have an internal world-relative representation of their
 * location. This representation, if it exists, can be accessed with the
 * getDirectWorldLocator() method. This method will always either return a \em
 * direct modifiable reference (not a computed temporary) to the internal
 * world-relative representation, or null if it doesn't have one. A computed
 * temporary representing the same information is always available through the
 * Object::getWorldLocator() method.
 *
 * An unconnected Locator can always be used just as a general abstraction for
 * the associated properties. For example, the BasicLocator is a commonly used
 * abstraction for the combination of a position and orientation within any
 * reference frame.
 *
 * @par Get and set methods
 * All results obtained from the get methods are valid only until the locator's
 * prepare() or step() methods are called the next time.
 * 
 * @par
 * The set methods (setLoc(), setBasis(), setVel(), setRotation()) must not be
 * called between prepare and step passes! When using them during a prepare
 * pass, the locator's prepare() should be called (possibly again) \em after
 * using these methods (otherwise they do not have any effect).
 *
 * @par Forces, torques and the timestep length
 * The methods for adding forces and torques will apply the accumulated force
 * and torque for the whole duration of the next timestep \em without scaling
 * them first. This implies that to apply constant magnitude impulses (instead
 * of continuous forces), the given values should be divided with the timestep
 * length before calling the accumulator methods with them.
 *
 * @todo
 * @par Dimensionality:
 * The dimensionality of the locator is dynamic, and it doesn't have to equal
 * the dimension of the surrounding space (which is also defined with a locator
 * of this type and thus is also dynamic).
 *
 * @todo
 * Consider using vector expressions as function arguments to avoid
 * temporaries. This will become more important with high-dimensional locators
 * (some clean typedef etc. for the vector_expression would be good, just to
 * keep the code more readable). Also ublas direct assignment should be
 * considered (would be more efficient, but makes the code less readable).
 *
 * @if done_todos
 * @todo
 * done differently: Move some basic location, orientation and velocity
 * information from FloatingLocator to here. Maybe this could contain "current
 * state" information (loc, basis, vel), and derived locators could then add
 * some force applying functionality.
 * @endif
 *
 * @if done_todos
 * @todo
 * done: Add momentum and maybe change move() and accelerate() to addForce().
 * @endif
 *
 * @todo
 * Add addTorqueRel() method and implement getPrevStepForce() and
 * getPrevStepTorque() methods (which return the corresponding forces from the
 * last step), and fix InertiaLocator not to overwrite these in prepare
 * (separate fields).
 *
 * @if done_todos
 * @todo
 * done: New locator hierarchy: 0th order == Locator (loc&basis), 1st order ==
 * MotionLocator (+vel&rotation), 2nd order == InertiaLocator (+force
 * accumulators). transform() needs to be fixed also when this is done.
 * @endif
 *
 * @todo
 * !!! Fix transform() and alignWith() !!! (currently they do not affect other
 * than BasicLocator attributes). This could be done with polymorphism, but a
 * more efficient way would be desirable in these methods.
 *
 * @todo
 * Rename getLoc() etc. --> getPos(). (currently the naming confuses the
 * locator concept with position) .. Or maybe not. The current naming relates
 * the Locator to its most basic property. The naming is also already used in
 * so many places within the documentation..
 *
 * @todo
 * Mark some getter methods pure?
 */
#ifndef LS_S_LOCATOR_HPP
#define LS_S_LOCATOR_HPP


#include "../types.hpp"
#include "Vector.hpp"
#include "BasisMatrix.hpp"
#include <boost/shared_ptr.hpp>
#include <cmath>




namespace lifespace {
  
  
  /* forwards */
  class Object;
  
  
  
  
  class Locator
  {
    Object * hostObject;
    
    
  protected:
    
    virtual void setHostObject( Object * newHostObject )
    { hostObject = newHostObject; }
    
    /** Object needs access to the private setHostObject() method. */
    friend class Object;
    
    
  public:
    
    /**
     * Creates a new locator.
     */
    Locator() :
      hostObject( 0 )
    {}
    
    /**
     * Copy constructs a new locator. Currently no attributes are copied from
     * the given locator, including the hostObject pointer which will be set to
     * null.
     */
    Locator( const Locator & other ) :
      hostObject( 0 )
    {}
    
    virtual ~Locator() {}
    
    /**
     * Returns a deep copy of the locator. The hostObject pointer is never
     * copied and is always set to null in the newly created locator.
     */
    virtual Locator * clone() const = 0;
    
    
    /* accessors */
    
    /** Returns a pointer to the Object where the Locator has been inserted
        (null if not inserted). */
    const Object * getHostObject() const
    { return hostObject; }
    
    /**
     * Returns a locator representing this Locator's absolute location and
     * orientation within the host World (can be null if not within a World or
     * a world relative locator is not directly available).
     *
     * This method will never do any computation: if a world relative locator
     * is not already interally available, then null is returned.
     */
    virtual boost::shared_ptr<const Locator> getDirectWorldLocator() const
    { return boost::shared_ptr<const Locator>(); }
    
    virtual boost::shared_ptr<Locator> getDirectWorldLocator()
    { return boost::shared_ptr<Locator>(); }
    
    virtual const Vector & getLoc() const = 0;
    virtual const BasisMatrix & getBasis() const = 0;
    virtual const Vector & getVel() const = 0;
    virtual const Vector & getRotation() const = 0;
    /** Always returns true if the locator is moving, but might return true
        also for a non-moving locator (so \c false means that the locator is
        definitely not moving). */
    virtual bool isMoving() const = 0;
    /** Always returns true if the locator is rotating, but might return true
        also for a non-rotating locator (so \c false means that the locator is
        definitely not rotating). */
    virtual bool isRotating() const = 0;
    
    
    /* mutators */
    
    virtual void setLoc( const Vector & newLoc ) = 0;
    virtual void setBasis( const BasisMatrix & newBasis ) = 0;
    virtual void setVel( const Vector & newVel ) = 0;
    virtual void setRotation( const Vector & newRotation ) = 0;
    
    /** add linear force in world coordinates */
    virtual void addForceAbs( const Vector & force ) = 0;
    
    /** add linear force in local coordinates */
    virtual void addForceRel( const Vector & force ) = 0;
    
    /** add torque force in world coordinates */
    virtual void addTorqueAbs( const Vector & torque ) = 0;

    /** add torque force in local coordinates */
    virtual void addTorqueRel( const Vector & torque ) = 0;
    
    
    /* operations */
    
    /**
     * Aligns the locator with the given reference locator. Currently only the
     * location and orientation are affected.
     */
    inline void alignWith( const Locator & refLocator );
    
    /**
     * Applies this locator's transformation to the target locator (i.e.\
     * "left-multiplies" it if the locators are seen as homogeneous
     * transformation matrices).
     *
     * Note that only the location and orientation of this locator will affect
     * the result, and currently only the location and orientation of the
     * target are modified.
     *
     * @param direction
     * Selects the direction of the transformation: use Rel2Abs (or Normal) to
     * transform the target from relative (relative to this locator) to
     * absolute coordinates, and Abs2Rel (or Reverse) to transform the target
     * from absolute to relative coordinates. In homogeneous transformation
     * matrix terms, the latter direction is equal to left-multiplying the
     * target with the inverse of this locator.
     *
     * @internal
     * not implemented: the target's velocity and rotation are transformed
     * to this locator's coordinate system.
     */
    inline void transform( Locator & target,
                           Direction direction = Rel2Abs ) const;

    inline void transform( Vector & target,
                           Direction direction = Rel2Abs ) const;

    inline void transform( Matrix & target,
                           Direction direction = Rel2Abs ) const;
    
    
    /**
     * Calculates the new external acceleration vector (extAcc) and
     * precalculates the next location assuming there are no collisions. This
     * method may be called multiple times before a call to step(). It \em must
     * be called at least once between the last mutator call and the following
     * step() call!
     */
    virtual void prepare( real dt ) = 0;
    
    /**
     * Commits the prepared step. moves the locator forward in time according
     * to calculations made by the last prepare() invocation.
     */
    virtual void step() = 0;
    
  };
  
  
  
  
  /* inlines */
  
  
  inline void Locator::alignWith( const Locator & refLocator )
  {
    setLoc( refLocator.getLoc() );
    setBasis( refLocator.getBasis() );
  }
  
  
  inline void Locator::transform( Locator & target, Direction direction ) const
  {
    switch( direction )
      {
      case Rel2Abs:
        target.setLoc( getLoc() + prod( getBasis(), target.getLoc() ) );
        target.setBasis( BasisMatrix( prod( getBasis(), target.getBasis() ),
                                      true ) );
        break;
      case Abs2Rel:
        target.setLoc( prod( getBasis().inverted(),
                             target.getLoc() - getLoc() ) );
        target.setBasis( BasisMatrix( prod( getBasis().inverted(),
                                            target.getBasis() ),
                                      true ) );
        break;
      default:
        assert(false);
      }
  }
  
  
  inline void Locator::transform( Vector & target, Direction direction ) const
  {
    switch( direction )
      {
      case Rel2Abs:
        target = getLoc() + prod( getBasis(), target );
        break;
      case Abs2Rel:
        target = prod( getBasis().inverted(), target - getLoc() );
        break;
      default:
        assert(false);
      }
  }


  inline void Locator::transform( Matrix & target, Direction direction ) const
  {
    switch( direction )
      {
      case Rel2Abs:
        target = prod( getBasis(), target );
        break;
      case Abs2Rel:
        target = prod( getBasis().inverted(), target );
        break;
      default:
        assert(false);
      }
  }
  
  
  
  
}   /* namespace lifespace */


#endif   /* LS_S_LOCATOR_HPP */
