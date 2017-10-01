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
 * @file testbasicLocator.cpp
 *
 * testcases for lifespace::BasicLocator.
 */
#include "lifespace/lifespace.hpp"
#include <cstdlib>
#include <iostream>
using namespace lifespace;
using namespace std;
using boost::shared_ptr;




bool operator==( const BasicLocator & lhs, const BasicLocator & rhs )
{
  Matrix & lb = (Matrix &)lhs.getBasis();
  Matrix & rb = (Matrix &)rhs.getBasis();
  
  assert( lb.size1() == rb.size1() && lb.size2() == rb.size2() );
  
  for( unsigned int i = 0 ; i < lb.size1() ; i++ )
    for( unsigned int j = 0 ; j < lb.size2() ; j++ )
      if( fabs( lb(i,j) - rb(i,j) ) > EPS ) return false;
  
  if( norm_2( lhs.getLoc() - rhs.getLoc() ) > EPS ) return false;
  
  return true;
}




bool testBasicLocator()
{
  bool result = true;
  
  srand(time(0));
  
  
  // test transformations
  
  BasicLocator transformer;
  transformer.setLoc( makeVector3d( FRAND01(), FRAND01(), FRAND01() ) );
  transformer.rotate3dRel( makeVector3d( FRAND01(), FRAND01(), FRAND01() ),
                           M_PI * FRAND01() );
  BasicLocator targetRel;
  targetRel.setLoc( makeVector3d( FRAND01(), FRAND01(), FRAND01() ) );
  targetRel.rotate3dRel( makeVector3d( FRAND01(), FRAND01(), FRAND01() ),
                         M_PI * FRAND01() );
  
  BasicLocator targetAbs( targetRel );
  transformer.transform( targetAbs, Rel2Abs );
  BasicLocator targetRel2( targetAbs );
  transformer.transform( targetRel2, Abs2Rel );
  
  // check Locator::transform()
  if(!( targetRel2 == targetRel )) {
    cout << "Locator::transform() failed!" << endl;
    cout << "  targetRel  == " << targetRel.getLoc() << " - "
         << trans(targetRel.getBasis()) << endl;
    cout << "  targetRel2 == " << targetRel2.getLoc() << " - "
         << trans(targetRel2.getBasis()) << endl;
    result = false;
  }
  
  // check BasicLocator::ResolveTransformation()
  BasicLocator transformer2
    ( BasicLocator::ResolveTransformation( targetAbs, targetRel ) );
  if(!( transformer2 == transformer )) {
    cout << "BasicLocator::ResolveTransformation() failed!" << endl;
    cout << "  targetRel == " << targetRel.getLoc() << " - "
         << trans(targetRel.getBasis()) << endl;
    cout << "  targetAbs == " << targetAbs.getLoc() << " - "
         << trans(targetAbs.getBasis()) << endl;
    cout << "  transformer  loc == " << transformer.getLoc() << endl;
    cout << "  transformer2 loc == " << transformer2.getLoc() << endl;
    cout << "  transformer  basis == " << trans(transformer.getBasis())
         << endl;
    cout << "  transformer2 basis == " << trans(transformer2.getBasis())
         << endl;
    result = false;
  }
  
  return result;
}
