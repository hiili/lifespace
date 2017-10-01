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
 * @file BasisMatrix.hpp
 *
 * Lifespace coordinate system basis matrices.
 */

/**
 * @class lifespace::BasisMatrix
 * @ingroup Structures
 *
 * @brief
 * Lifespace coordinate system basis matrices.
 *
 *
 * @todo
 * dynamic dimensionality.
 *
 * @todo
 * Combine location and orientation into a single 4x4 matrix (or maybe not).
 *
 * @internal
 * Make sure that orthonormalize() is called whenever the basis is modified!
 */
#ifndef LS_S_BASISMATRIX_HPP
#define LS_S_BASISMATRIX_HPP


#include "../types.hpp"
#include "Vector.hpp"
#include "Matrix.hpp"
#include <GL/gl.h>
#include <iostream>




namespace lifespace {
  
  
  
  
  class BasisMatrix :
    public Matrix
  {
    
    /** Basis orthonormalization frequency. The basis vectors are
        orthonormalized on every ORTHONR_FREQ:th call to orthonormalize(),
        which is usually called from step() on each simulation cycle. */
    static const int ORTHONR_FREQ;
    
    
    int orthonrPhase;
    
    
    /**
     * 2d-rotate the given vectors in their plane. The rotation is skewed if x
     * and y are not orthonormal.
     *
     * @param x   A Vector or a uBLAS vector_expression.
     * @param y   A Vector or a uBLAS vector_expression.
     * @param angle    Rotation angle in radians. If x points right and y
     *                 points up, then positive angle rotates
     *                 counter-clockwise.
     */
    template<class E1, class E2>
    void rotate( E1 x, E2 y, real angle )
    {
      Vector x0( x );
      x = x * cos(angle) - y * sin(angle);
      y = x0 * sin(angle) + y * cos(angle);
    }
    
    /**
     * Orthonormalizes the basis on every Nth call (always with the 'forced'
     * flag). If it's not orthogonal, then X axis is dominant, Y comes next and
     * so on.
     * 
     * @todo
     * n-dimensional version (maybe using normal Gram-Schmidt
     * orthonormalization, because the basis is usually already almost
     * orthonormal..)
     */
    void orthonormalize( bool forced )
    {
      assert( size2() == 3 );   // n-dimensionality not yet supported
      
      orthonrPhase = (orthonrPhase + 1) % ORTHONR_FREQ;
      if( orthonrPhase == 0 || forced ) {
        basisvec_reference_t basisVecX( getBasisVec(DIM_X) );
        normalize( basisVecX );
        getBasisVec(DIM_Y) = normalized( getBasisVec(DIM_Y) -
                                         projection( getBasisVec(DIM_Y),
                                                     getBasisVec(DIM_X) ));
        getBasisVec(DIM_Z) = normalized( getBasisVec(DIM_Z) -
                                         projection( getBasisVec(DIM_Z),
                                                     getBasisVec(DIM_X) ) -
                                         projection( getBasisVec(DIM_Z),
                                                     getBasisVec(DIM_Y) ));
        orthonrPhase = 0;
      }
    }
    
    
  public:
    
    /* constructors */
    
    /** Init with identity matrix. */
    BasisMatrix( unsigned int dim = 3 ) :
      Matrix( IdentityMatrix( dim ) ),
      orthonrPhase( 0 )
    {}
    
    /** Init from another Matrix. It will be orthonormalized first unless the
        isOrthonormal flag is set to true. */
    BasisMatrix( const Matrix & matrix, bool isOrthonormal = false ) :
      Matrix( matrix ),
      orthonrPhase( 0 )
    {
      if( !isOrthonormal ) orthonormalize( true );
    }
    
    /** Copy construct from another BasisMatrix, which will \em not be
        orthonormalized (it is assumed that it is already orthonormal). */
    BasisMatrix( const BasisMatrix & other ) :
      Matrix( other ),
      orthonrPhase( 0 )
    {}
    
    /**
     * Init from ahead and up -vectors. They don't have to be normalized. If
     * they are not orthogonal, then "up" is dominant.
     *
     * @todo
     * n-dimensional version (will be ambiquous, how should it be solved?)
     */
    BasisMatrix( const Vector & ahead, const Vector & up ) :
      Matrix( 3, 3 ),
      orthonrPhase( 0 )
    {
      column(*this,0).assign( crossProduct( ahead, up ) );
      column(*this,1).assign( up );
      column(*this,2).assign( ahead * -1.0 );
      orthonormalize( true );
    }
    
    
    /* accessors */
    
    typedef const ublas::matrix_column< const ublas::matrix<real> >
    basisvec_const_reference_t;
    
    basisvec_const_reference_t getBasisVec( unsigned int d ) const
    { return column( *(const Matrix *)this, d ); }
    
    typedef ublas::matrix_column< ublas::matrix<real> >
    basisvec_reference_t;
    
    basisvec_reference_t getBasisVec( unsigned int d )
    { return column( *(Matrix *)this, d ); }
    
    const Vector getAhead() const
    { return (real)-1.0 * getBasisVec(DIM_Z); }
    
    const Vector getUp() const
    { return getBasisVec(DIM_Y); }
    
    const Vector getRight() const
    { return getBasisVec(DIM_X); }
    
    /**
     * Returns true if the basis is axis-aligned within EPS limits.
     */
    bool isAxisAligned() const
    {
      for( unsigned int i = 0 ; i < size2() ; i++ )
        if( norm_inf( column( *this, i ) ) < 1.0 - EPS ) return false;
      return true;
    }
    
    
    /* operations */
    
    /**
     * Inverts the basis. This is quite fast operation because the basis is
     * always orthonormal and thus the inverse can be taken by transposing.
     */
    void invert()
    {
      *(Matrix *)this = trans( *this );
    }

    /**
     * Returns an inverted copy of the basis. This is quite fast operation
     * because the basis is always orthonormal and thus the inverse can be
     * taken by transposing.
     */
    BasisMatrix inverted() const
    {
      return BasisMatrix( trans( *this ), true );
    }
    
    /**
     * 3d-rotates around the given axis, which is given in absolute
     * coordinates.
     *
     * The basis must represent a 3-dimensional orientation (3x3-matrix).
     *
     * @param axis    A normalized rotation axis in absolute coordinates.
     * @param angle   rotation angle in radians.
     */
    template<typename E>
    void rotate3dAbs( const ublas::vector_expression<E> & axis, real angle )
    {
      assert( size1() == 3 && size2() == 3 );
      
      Matrix rotM(3,3);
      real c = std::cos(angle), s = std::sin(angle); real t = 1.0 - c;
      rotM(0,0) = t * axis()(0) * axis()(0) + c;
      rotM(0,1) = t * axis()(0) * axis()(1) - axis()(2) * s;
      rotM(0,2) = t * axis()(0) * axis()(2) + axis()(1) * s;
      rotM(1,0) = t * axis()(0) * axis()(1) + axis()(2) * s;
      rotM(1,1) = t * axis()(1) * axis()(1) + c;
      rotM(1,2) = t * axis()(1) * axis()(2) - axis()(0) * s;
      rotM(2,0) = t * axis()(0) * axis()(2) - axis()(1) * s;
      rotM(2,1) = t * axis()(1) * axis()(2) + axis()(0) * s;
      rotM(2,2) = t * axis()(2) * axis()(2) + c;
      *(Matrix *)this = prod( rotM, *this );
    }
    
    /**
     * 3d-rotates around the given axis, which is given in relative (local)
     * coordinates.
     *
     * The basis must represent a 3-dimensional orientation (3x3-matrix).
     *
     * @param axis    A normalized rotation axis in relative coordinates.
     * @param angle   rotation angle in radians.
     */
    template<typename E>
    void rotate3dRel( const ublas::vector_expression<E> & axis, real angle )
    {
      rotate3dAbs( prod( *this, axis ), angle );
    }
    
    /*
    void yaw( real amount )
    {
      rotate( basisVec(DIM_X), basisVec(DIM_Z), -amount );
      orthonormalize( false );
    }
    
    void pitch( real amount )
    {
      rotate( basisVec(DIM_Z), basisVec(DIM_Y), -amount );
      orthonormalize( false );
    }
    
    void roll( real amount )
    {
      rotate( basisVec(DIM_Y), basisVec(DIM_X), -amount );
      orthonormalize( false );
    }
    */
  };




}   /* namespace lifespace */


#endif   /* LS_S_BASISMATRIX_HPP */
