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
 * @file main.cpp
 *
 * testcases for Lifespace Structures.
 */
#include <cstdlib>
#include <iostream>
using std::cout;
using std::endl;




/* returning false means test failure */
bool testBasisMatrix();
bool testObject();
bool testSubspace();
bool testActor();
bool testBasicLocator();
bool testInertiaLocator();


/* testcases need to be added here and to the Makefile, and declared above */
#define TESTCASE_COUNT 6
bool (*testcases[TESTCASE_COUNT])() = {
  testBasisMatrix,
  testObject,
  testSubspace,
  testActor,
  testBasicLocator,
  testInertiaLocator
};




int main( int argc, char * argv[] )
{
  int testcaseInd;   // -1 == execute all
  int failures;
  
  
  if( argc == 1 ) {
    testcaseInd = -1;
  } else if( argc == 2 ) {
    testcaseInd = atoi( argv[1] );
  } else {
    cout << "Usage: " << argv[0] << " [<testcase num>]" << endl;
    return 1;
  }
  
  
  failures = 0;
  for( int x = 0; x < TESTCASE_COUNT; x++ ) {
    if( testcaseInd == -1 || x == testcaseInd ) {
      cout << "*** Running testcase " << x << ".." << endl;
      if( !testcases[x]() ) {
        failures++;
        cout << endl << "*** Testcase " << x << " failed!" << endl << endl;
      }
    }
  }
  
  
  cout << endl << endl;
  cout << "Testing finished! Total number of failures: " << failures << endl;
  
  return 0;
}
