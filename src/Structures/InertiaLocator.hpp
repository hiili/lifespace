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
 * @file InertiaLocator.hpp
 */

/**
 * @class lifespace::InertiaLocator
 * @ingroup Structures
 *
 * @brief
 * A locator with inertia and air resistance.
 *
 *
 * @invariant
 * \code
 * (vel.length2() >= EPS && moving) || (vel == (0,0,0) && !moving)
 * \endcode
 *
 * @invariant
 * \code
 * (rotation.length2() >= EPS && rotating)
 *   || (rotation == (0,0,0) && !rotating)
 * \endcode
 *
 *
 * @if done_todos
 * @todo
 * done: Move visual scaling away from here, this is clearly the wrong place.
 * @endif
 *
 * @if done_todos
 * @todo
 * done: Implement rotation with momentum to avoid breaking the prepare-step
 * guidelines.
 * @endif
 *
 * @if done_todos
 * @todo
 * done: Finish and enable the mInertia coefficient. Add mass (turn the
 * acceleration methods to addForce methods).
 * @endif
 *
 * @todo
 * Consider creating a Params nested class (whose default-constructor fills in
 * the default values), which could be used to pass parameters to a
 * constructor?
 *
 * @if done_todos
 * @todo
 * done differently: name --> BasicLocator
 * @endif
 *
 * @todo
 * Create a copy ctor and assignment operator.
 *
 * @sa Locator
 */
#ifndef LS_S_INERTIALOCATOR_HPP
#define LS_S_INERTIALOCATOR_HPP


#include "../types.hpp"
#include "MotionLocator.hpp"
#include <cmath>
#include <iostream>
#include <GL/gl.h>


namespace lifespace {
  
  
  
  
  class InertiaLocator :
    public MotionLocator
  {
    
    /* constants */
    static const real DEFAULT_VEL_CONSTANT_FRICTION;
    static const real DEFAULT_VEL_LINEAR_FRICTION;
    static const real DEFAULT_ROT_CONSTANT_FRICTION;
    static const real DEFAULT_ROT_LINEAR_FRICTION;
    
    
    /* static properties */
    real mass, mInertia;
    real velConstantFriction, velLinearFriction;
    real rotConstantFriction, rotLinearFriction;
    
    /* force accumulators */
    Vector force;   // this is available also during the next prepare pass,
                    // and indicates the force during _previous_
                    // timestep.
    Vector extForce;
    Vector linearAcc;
    Vector torque;   // this is available also during the next prepare pass,
                     // and indicates the torque during _previous_ timestep.
    Vector extTorque;
    Vector rotationalAcc;
    
  protected:
    
    /* prepared next state (dt, nextLoc and nextBasis are inherited from
       MotionLocator) */
    Vector nextVel;
    Vector nextRotation;
    
    
  public:
    
    /* constructors */
    
    /** */
    InertiaLocator( const Vector & loc = ZeroVector(3),
                    const BasisMatrix & basis = BasisMatrix(3),
                    real mass_ = 1.0, real mInertia_ = 1.0,
                    real velConstantFriction_ = DEFAULT_VEL_CONSTANT_FRICTION,
                    real velLinearFriction_ = DEFAULT_VEL_LINEAR_FRICTION,
                    real rotConstantFriction_ = DEFAULT_ROT_CONSTANT_FRICTION,
                    real rotLinearFriction_ = DEFAULT_ROT_LINEAR_FRICTION ) :
      MotionLocator( loc, basis ),
      mass( mass_ ), mInertia( mInertia_ ),
      velConstantFriction( velConstantFriction_ ),
      velLinearFriction( velLinearFriction_ ),
      rotConstantFriction( rotConstantFriction_ ),
      rotLinearFriction( rotLinearFriction_ ),
      force( ZeroVector(3) ),
      extForce( ZeroVector(3) ),
      linearAcc( ZeroVector(3) ),
      torque( ZeroVector(3) ),
      extTorque( ZeroVector(3) ),
      rotationalAcc( ZeroVector(3) ),
      nextVel( ZeroVector(3) ),
      nextRotation( ZeroVector(3) )
    {}
    
    /**
     * Copy constructs from another InertiaLocator.
     *
     * Only the location, orientation, motion, mass and friction information is
     * copied: the hostObject pointer is set to null and all force accumulators
     * and a possibly prepared step are cleared.
     *
     * @warning
     * The state of the force accumulators is \em not copied along, neither is
     * a possibly prepared step! (the newly created copy is in the same state
     * as if its step() was just called)
     */
    InertiaLocator( const InertiaLocator & other ) :
      MotionLocator( other ),
      mass( other.mass ), mInertia( other.mInertia ),
      velConstantFriction( other.velConstantFriction ),
      velLinearFriction( other.velLinearFriction ),
      rotConstantFriction( other.rotConstantFriction ),
      rotLinearFriction( other.rotLinearFriction ),
      force( ZeroVector(3) ),
      extForce( ZeroVector(3) ),
      linearAcc( ZeroVector(3) ),
      torque( ZeroVector(3) ),
      extTorque( ZeroVector(3) ),
      rotationalAcc( ZeroVector(3) ),
      nextVel( ZeroVector(3) ),
      nextRotation( ZeroVector(3) )
    {}
    
    virtual ~InertiaLocator() {}
    
    virtual Locator * clone() const
    { return new InertiaLocator( *this ); }
    
    
    /* accessors */
    
    
    /* mutators */
    
    /* add force in world coordinates */
    virtual void addForceAbs( const Vector & force )
    { extForce += force; }
    
    /* add force in local coordinates */
    virtual void addForceRel( const Vector & force )
    { extForce += prod( getBasis(), force ); }
    
    virtual void addTorqueAbs( const Vector & torque )
    { extTorque += torque; }

    virtual void addTorqueRel( const Vector & torque )
    { extTorque += prod( getBasis(), torque ); }
    
    
    /* operations */
    
    /**
     * Calculates the new acceleration and location. Acceleration consists of
     * external acceleration commands and velocity damp force.
     *
     * The external acceleration accumulator will not be cleared in this
     * method, so all external accelerations applied after the last step() call
     * will always be used. Note however that accelerations applied after the
     * last prepare call will not have any effect!
     *
     * @bug
     * Friction computation would produce a NAN friction vector if the velocity
     * were zero and isMoving would say that the object is moving (divide by
     * velMagn). See ODELocator for a fix.
     *
     * @sa Object::prepare()
     */
    virtual void prepare( real dt_ )
    {
      const Vector vel( getVel() );
      const Vector rotation( getRotation() );
      
      // store dt
      dt = dt_; 
      
      // apply externally initiated forces
      force = extForce;
      torque = extTorque;
      
      // add linear friction force
      if( isMoving() ) {
        real velMagn = norm_2( vel ), frictionMagn = 0.0;
        
        frictionMagn = velConstantFriction + velLinearFriction * velMagn;
        if( (frictionMagn / mass) * dt > velMagn ) {
          frictionMagn = mass * (velMagn / dt);
        }
        force += frictionMagn * -(vel / velMagn);
      }
      
      // add rotational friction force
      if( isRotating() ) {
        real rotMagn = norm_2( rotation ), frictionMagn = 0.0;
        
        frictionMagn = rotConstantFriction + rotLinearFriction * rotMagn;
        if( (frictionMagn / mInertia) * dt > rotMagn ) {
          frictionMagn = mInertia * (rotMagn / dt);
        }
        torque += frictionMagn * -(rotation / rotMagn);
      }
      
      // convert forces to accelerations
      linearAcc = force / mass;
      rotationalAcc = torque / mInertia;
      
      // calculate next location, velocity and rotation if no collisions
      nextLoc = getLoc() + vel * dt + 0.5 * linearAcc * SQUARE(dt);
      nextVel = vel + linearAcc * dt;
      nextRotation = rotation + rotationalAcc * dt;
      
      // calculate next orientation if no collisions
      if( isRotating() || lengthSquared( rotationalAcc ) > EPS ) {
        
        Vector axis = rotation * dt + 0.5 * rotationalAcc * SQUARE(dt);
        real angle = norm_2( axis );
        axis /= angle;
        
        nextBasis = getBasis();
        nextBasis.rotate3dAbs( axis, angle );
        
      } else {
        nextBasis = getBasis();
      }
    }
    
    /** Commit the prepared step. */
    virtual void step()
    {
      assert( !std::isnan(dt) );
      
      // move to next state
      setLoc( nextLoc );
      setBasis( nextBasis );
      setVel( nextVel );
      setRotation( nextRotation );
      
      // reset the step length and acceleration accumulators
      dt = NAN;
      extForce = ZeroVector(3);
      extTorque = ZeroVector(3);
    }
  };




}   /* namespace lifespace */


#endif   /* LS_S_INERTIALOCATOR_HPP */
