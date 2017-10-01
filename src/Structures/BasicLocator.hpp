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
 * @file BasicLocator.hpp
 *
 * A Locator implementation with location and orientation information.
 */

/**
 * @class lifespace::BasicLocator
 * @ingroup Structures
 *
 * @brief
 * A Locator implementation with location and orientation information.
 *
 * This Locator implementation supports all location and orientation related
 * functionality. Methods relating to higher order properties (motion, forces
 * etc.) will assert-fail in debug mode and return dummy values is release
 * mode.
 *
 * @sa Locator, MotionLocator, InertiaLocator
 */
#ifndef LS_S_BASICLOCATOR_HPP
#define LS_S_BASICLOCATOR_HPP


#include "../types.hpp"
#include "Locator.hpp"


namespace lifespace {
  
  
  
  
  class BasicLocator :
    public Locator
  {
    
    /* current state */
    Vector loc;
    BasisMatrix basis;
    
    
  public:
    
    /* constructors */
    
    /**
     * Creates a new locator with location and orientation information.
     */
    BasicLocator( const Vector & loc_ = ZeroVector(3),
                  const BasisMatrix & basis_ = BasisMatrix(3) ) :
      loc( loc_ ),
      basis( basis_ )
    {}
    
    /**
     * Copy constructs from another locator, except the hostObject pointer is
     * set to null.
     */
    BasicLocator( const Locator & other ) :
      loc( other.getLoc() ),
      basis( other.getBasis() )
    {}
    
    /**
     * Assigns the location and orientation information from another
     * locator. The hostObject pointer is not affected.
     */
    BasicLocator & operator=( const Locator & other )
    {
      loc = other.getLoc();
      basis = other.getBasis();
      return *this;
    }
    
    virtual ~BasicLocator() {}
    
    virtual Locator * clone() const
    { return new BasicLocator( *this ); }
    
    
    /* accessors */
    
    virtual const Vector & getLoc() const
    { return loc; }
    virtual const BasisMatrix & getBasis() const
    { return basis; }
    virtual const Vector & getVel() const
    {
      assert(false);
      static const Vector res( ZeroVector(3) );
      return res;
    }
    virtual const Vector & getRotation() const
    {
      assert(false);
      static const Vector res( ZeroVector(3) );
      return res;
    }
    virtual bool isMoving() const
    { assert(false); return false; }
    virtual bool isRotating() const
    { assert(false); return false; }
    
    
    /* mutators */
    
    virtual void setLoc( const Vector & newLoc )
    { loc = newLoc; }
    
    virtual void setBasis( const BasisMatrix & newBasis )
    { basis = newBasis; }
    
    virtual void setVel( const Vector & newVel )
    { assert(false); }   // not implemented
    
    virtual void setRotation( const Vector & newRotation )
    { assert(false); }   // not implemented
    
    void stopMoving() { assert(false); }
    
    void stopRotating() { assert(false); }
    
    
    /* add force in world coordinates */
    virtual void addForceAbs( const Vector & force )
    { assert(false); }   // not implemented
    
    /* add force in local coordinates */
    virtual void addForceRel( const Vector & force )
    { assert(false); }   // not implemented
    
    virtual void addTorqueAbs( const Vector & torque )
    { assert(false); }   // not implemented

    virtual void addTorqueRel( const Vector & torque )
    { assert(false); }   // not implemented
    
    
    /* operations */
    
    /**
     * Rotates the locator in its \em local coordinates (not around origin).
     *
     * @param axis   Rotation axis in relative (local) coordinates.
     * @param angle   Rotation angle in radians.
     */
    virtual void rotate3dRel( const Vector & axis, real angle )
    { basis.rotate3dRel( axis, angle ); }
    
    /** Resolves the transformation that would produce the given absolute
        locator from the given relative locator, and returns it as a
        BasicLocator. */
    static inline BasicLocator
    ResolveTransformation( const Locator & absoluteLocator,
                           const Locator & relativeLocator );
    
    
    /** */
    virtual void prepare( real dt_ ) {}
    
    /** */
    virtual void step() {}
  };
  
  
  
  
  /* inlines */
  
  
  inline BasicLocator
  BasicLocator::ResolveTransformation( const Locator & absoluteLocator,
                                       const Locator & relativeLocator )
  {
    BasicLocator result;
    
    // compute the reverse rotation for A=TR:
    //   (A = absolute basis, T = transformation, R = relative basis)
    // A=T*R <=> Tinv*A=R <=> Tinv=R*Ainv <=> T=(R*Ainv)inv
    result.setBasis
      ( BasisMatrix( trans( prod( relativeLocator.getBasis(),
                                  absoluteLocator.getBasis().inverted() )),
                     true ));
    
    // remove the relativeLocator's rotated position from the absoluteLocator's
    // position
    result.setLoc( absoluteLocator.getLoc() -
                   prod( result.getBasis(), relativeLocator.getLoc() ));
    
    return result;
  }
  
  
  
  
}   /* namespace lifespace */


#endif   /* LS_S_BASICLOCATOR_HPP */
