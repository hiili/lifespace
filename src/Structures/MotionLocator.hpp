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
 * @file MotionLocator.hpp
 *
 * A Locator implementation with location, orientation and motion
 * functionality.
 */

/**
 * @class lifespace::MotionLocator
 * @ingroup Structures
 *
 * @brief
 * A Locator implementation with location, orientation and motion
 * functionality.
 *
 * This Locator implementation supports all location, orientation and motion
 * related functionality. Methods relating to higher order properties
 * (i.e. forces) will assert-fail in debug mode and do nothing in release mode.
 *
 * @warning
 * Not yet fully implemented!
 *
 * @if done_todos
 * @todo
 * done:Implement the missing methods (step() at least).
 * @endif
 *
 * @todo
 * Copy stopMoving() and stopRotating() and corresponding is* methods to
 * Locator as pure virtuals?
 *
 * @sa Locator
 */
#ifndef LS_S_MOTIONLOCATOR_HPP
#define LS_S_MOTIONLOCATOR_HPP


#include "../types.hpp"
#include "BasicLocator.hpp"


namespace lifespace {
  
  
  
  
  class MotionLocator :
    public BasicLocator
  {
    
    /* current state */
    Vector vel;        // in absolute (host) coordinates
    Vector rotation;   // in absolute (host) coordinates
    bool moving;
    bool rotating;
    
  protected:
    
    /* prepared next state */
    real dt;   // if NAN, then the step is not prepared and nextLoc and
               // nextBasis are not valid
    Vector nextLoc;
    BasisMatrix nextBasis;
    
    
  public:
    
    /* constructors */
    
    /** */
    MotionLocator( const Vector & loc = ZeroVector(3),
                   const BasisMatrix & basis = BasisMatrix(3) ) :
      BasicLocator( loc, basis ),
      vel( ZeroVector(3) ),
      rotation( ZeroVector(3) ),
      moving( false ),
      rotating( false ),
      dt( NAN ),
      nextLoc( ZeroVector(3) ),
      nextBasis( BasisMatrix(3) )
    {}
    
    /**
     * Copy constructs from another locator, except the hostObject pointer is
     * set to null and a possibly prepared step is discarded (you have to call
     * prepare() separately for this locator before step()).
     *
     * @note
     * Will attempt to read velocities, which will cause an assert-fail if the
     * given locator does not support velocity functionality!
     */
    MotionLocator( const Locator & other ) :
      BasicLocator( other ),
      dt( NAN ),
      nextLoc( ZeroVector(3) ),
      nextBasis( BasisMatrix(3) )
    {
      setVel( other.getVel() );
      setRotation( other.getRotation() );
    }
    
    /**
     * Assigns from another locator, except the hostObject pointer is not
     * affected and a possibly prepared step is discarded (you have to call
     * prepare() again before step()).
     *
     * @note
     * Will attempt to read velocities, which will cause an assert-fail if the
     * given locator does not support velocity functionality!
     */
    MotionLocator & operator=( const Locator & other )
    {
      *(BasicLocator *)this = other;
      setVel( other.getVel() );
      setRotation( other.getRotation() );
      dt = NAN;
      return *this;
    }
    
    virtual ~MotionLocator() {}
    
    virtual Locator * clone() const
    { return new MotionLocator( *this ); }
    
    
    /* accessors */
    
    virtual const Vector & getVel() const
    { return vel; }
    virtual const Vector & getRotation() const
    { return rotation; }
    virtual bool isMoving() const
    { return moving; }
    virtual bool isRotating() const
    { return rotating; }
    
    
    /* mutators */
    
    virtual void setVel( const Vector & newVel )
    {
      if( lengthSquared( newVel ) >= EPS ) {
        vel = newVel; moving = true;
      } else stopMoving();
    }
    
    virtual void setRotation( const Vector & newRotation )
    {
      if( lengthSquared( newRotation ) >= EPS ) {
        rotation = newRotation; rotating = true;
      } else stopRotating();
    }
    
    void stopMoving()
    { vel = ZeroVector(3); moving = false; }
    
    void stopRotating()
    { rotation = ZeroVector(3); rotating = false; }
    
    
    /* operations */
    
    /** Calculates the new location. */
    virtual void prepare( real dt_ )
    {
      // store dt
      dt = dt_; 
      
      // calculate next location, velocity and rotation if no collisions
      nextLoc = getLoc() + vel * dt;
      
      // calculate next orientation if no collisions
      if( rotating ) {
        
        Vector axis = rotation * dt;
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
      
      // reset the step length
      dt = NAN;
    }
  };




}   /* namespace lifespace */


#endif   /* LS_S_MOTIONLOCATOR_HPP */
