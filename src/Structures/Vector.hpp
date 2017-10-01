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
 * @file Vector.hpp
 *
 * Lifespace vectors.
 */

/**
 * @class lifespace::Vector
 * @ingroup Structures
 *
 * @brief
 * Lifespace vectors.
 *
 * @todo
 * Dynamic dimensionality, or use a bounded array storage.
 */
#ifndef LS_S_VECTOR_HPP
#define LS_S_VECTOR_HPP


#include "../types.hpp"
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/vector_expression.hpp>
#include <boost/numeric/ublas/io.hpp>

#include <cmath>




namespace lifespace {
  
  
  
  
  typedef ublas::vector<real> Vector;
  
  typedef ublas::zero_vector<real> ZeroVector;
  
  
  inline Vector makeVector3d( real a, real b, real c )
  {
    Vector v(3);
    v(0) = a; v(1) = b; v(2) = c;
    return v;
  }

  inline Vector makeVector3d()
  {
    Vector v(3);
    v(0) = v(1) = v(2) = 0.0;
    return v;
  }
  
  
  /**
   * Returns the squared length of the vector.
   */
  template<class E>
  inline real lengthSquared( const ublas::vector_expression<E> & vector )
  { return inner_prod( vector, vector ); }
  
  /** Performs an element-wise abs operation for the vector and returns the result as a new vector (by value). */
  template<class E>
  Vector abs( const ublas::vector_expression<E> & vector )
  {
    Vector res( vector );
    for( unsigned int i = 0 ; i < res.size() ; i++ )
      res(i) = std::abs(res(i));
    return res;
  }
  
  /**
   * Normalizes the given vector.
   *
   * @param vector   A Vector or a uBLAS vector_expression.
   * @return   Nothing is returned to avoid accidental mis-use.
   *
   * @warning
   * If the vector's length is < EPS, then the vector is converted to a NaN
   * vector!
   *
   * @todo
   * Re-think the handling of almost-zero vectors. Is a NaN vector really a
   * good result? Some kind of math exception could be one solution, although
   * it might result in quite complicated usage..
   * 
   * @internal
   * Why a temporary was originally used in vector division? 
   * 
   * @sa normalized()
   */
  template<class E>
  inline void normalize( ublas::vector_expression<E> & vector )
  {
    real len = norm_2( vector );
    vector() /= (len > EPS) ? len : (real)NAN;
  }
  
  /**
   * Returns a normalized copy of the vector.
   *
   * @warning
   * If the vector's length is < EPS, then the resulting vector is a NaN
   * vector!
   *
   * @sa normalize()
   */
  inline Vector normalized( Vector vector )
  {
    normalize( vector );
    return vector;
  }
  
  /**
   * Projects the lhs vector onto the rhs vector and returns the result.
   */
  template<class E1, class E2>
  inline Vector projection( const ublas::vector_expression<E1> & lhs,
                            const ublas::vector_expression<E2> & rhs )
  {
    return (inner_prod( lhs, rhs ) / lengthSquared(rhs)) * rhs;
  }

  /**
   * Computes the cross product of the given vectors and returns the result.
   *
   * @deprecated
   * Not generally defined in higher dimensions, so this will be removed from
   * an n-dimensional version.
   */
  inline Vector crossProduct( const Vector & lhs, const Vector & rhs )
  {
    // only 3-dim version available
    assert( lhs.size() == 3 && rhs.size() == 3 );
    
    Vector res(3);
    res(0) = lhs(1) * rhs(2) - lhs(2) * rhs(1);
    res(1) = -( lhs(0) * rhs(2) - lhs(2) * rhs(0) );
    res(2) = lhs(0) * rhs(1) - lhs(1) * rhs(0);
    
    return res;
  }









  
#ifdef IS_OBSOLETE
  class Vector :
    public ublas::vector<real>
  {
    
  public:
    
    /* ctor/dtor/etc */
    
    Vector( const ublas::vector<real> & other ) :
      ublas::vector<real>( other )
    {}
    
    Vector & operator=( const ublas::vector<real> & other )
    {
      *this = other;
      return *this;
    }
    
    
    /* accessors */
    
    
    /* mutators */
    
    /**
     * Normalizes the vector.
     *
     * @warning
     * If the vector's length is < EPS, then the vector is converted to a NaN
     * vector!
     *
     * @todo
     * Re-think the handling of almost-zero vectors. Is a NaN vector really a
     * good result? Some kind of math exception could be one solution, although
     * it might result in quite complicated usage..
     *
     * @sa getNormalized()
     */
    void normalize()
    {
      real len = norm_2( *this );
      if( len > EPS ) {
        *this /= len;
      } else {
        *this *= NAN;
      }
    }
    
    
    /* operations */
    
    
    
    /**
     * Returns a normalized copy of the vector.
     *
     * @warning
     * If the vector's length is < EPS, then the resulting vector is a NaN
     * vector!
     *
     * @sa normalize()
     */
    Vector getNormalized() const
    {
      Vector temp( *this );
      temp.normalize();
      return temp;
    }
    
    /**
     * Projects this vector onto the given vector and returns the result.
     */
    Vector projectOn( const Vector & other ) const
    {
      return Vector( (inner_prod( *this, other ) /
                      inner_prod( other, other )) * other );
    }
  };
#endif




}   /* namespace lifespace */


#endif   /* LS_S_VECTOR_HPP */
