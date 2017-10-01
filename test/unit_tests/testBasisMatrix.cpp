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
 * @file testBasisMatrix.cpp
 *
 * testcases for lifespace::BasisMatrix.
 */
#include "lifespace/lifespace.hpp"
#include "boost/numeric/ublas/io.hpp"
using namespace lifespace;

#include <cstdlib>
#include <iostream>
using std::cout;
using std::endl;




bool operator==( const Matrix & lhs, const Matrix & rhs )
{
  assert( lhs.size1() == rhs.size1() && lhs.size2() == rhs.size2() );
  
  for( unsigned int i = 0 ; i < lhs.size1() ; i++ )
    for( unsigned int j = 0 ; j < lhs.size2() ; j++ )
      if( fabs( lhs(i,j) - rhs(i,j) ) > EPS ) return false;
  
  return true;
}




bool testBasisMatrix()
{
  bool result = true;
  
  srand(time(0));
  
  
  // test BasisMatrix::getInverted() (and thus BasisMatrix::invert())
  Matrix m(3,3);
  for( int i = 0 ; i < 3 ; i++ )
    for( int j = 0 ; j < 3 ; j++ )
      m(i,j) = FRAND01();
  BasisMatrix bm(m);
  BasisMatrix bmInv( bm.inverted() );
  if(!( prod( bm, bmInv ) == IdentityMatrix(3) )) {
    cout << "BasisMatrix::inverted() failed!" << endl;
    result = false;
  }
  
  return result;
}
