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
 * @file ODELocator.cpp
 *
 * Implementation for the ODELocator class.
 */
#include "../types.hpp"
#include "ODELocator.hpp"
#include "Vector.hpp"
#include "Locator.hpp"
#include <cassert>
using namespace lifespace;
using namespace std;




void ODELocator::prepare( real dt )
{
  assert( isActive() );
  
  /* Apply linear and rotational air drag. The drag works in world
     coordinates, so we can use directly the worldLocator and world
     coordinates. Moment of inertia shape is not taken into account in
     preventing rotational "nodding" when stopping.
     
     The sqrt calls in vector normalizations cause a somewhat considerable
     performance hit. The test system_tests/ODELocator_performance can be used
     to test this.
  */
  
  // linear drag
  if( worldLocator->isMoving() ) {
    
    // read the current velocity
    const Vector & vel = worldLocator->getVel();
    real velMagn = norm_2( vel ), dragMagn;
    
    // try to stop completely if velocity < EPS, else apply drag
    if( velMagn < EPS ) {
      worldLocator->setVel( makeVector3d( 0.0, 0.0, 0.0 ) );
    } else {
      // compute the drag, prevent "nodding" to the opposite direction
      dragMagn = velConstantDrag +
        velLinearDrag * velMagn + velQuadraticDrag * SQUARE(velMagn);
      if( (dragMagn / mass) * dt > velMagn ) {
        dragMagn = mass * (velMagn / dt);
      }
      // apply the drag force
      worldLocator->addForceAbs( dragMagn * -(vel / velMagn) );
    }
    
  }
  
  // rotational drag
  if( worldLocator->isRotating() ) {
    
    // read the current velocity
    const Vector & rotation = worldLocator->getRotation();
    real rotMagn = norm_2( rotation ), dragMagn;
    
    // try to stop completely if velocity < EPS, else apply drag
    if( rotMagn < EPS ) {
      worldLocator->setRotation( makeVector3d( 0.0, 0.0, 0.0 ) );
    } else {
      // compute the drag, prevent "nodding" to the opposite direction
      // (moment of inertia shape is not taken into account!)
      dragMagn = rotConstantDrag +
        rotLinearDrag * rotMagn + rotQuadraticDrag * SQUARE(rotMagn);
      if( dragMagn * dt > rotMagn ) {
        dragMagn = rotMagn / dt;
      }
      // apply the drag force
      worldLocator->addTorqueAbs( dragMagn * -(rotation / rotMagn) );
    }
    
  }
  
}




void ODELocator::step()
{
  assert( isActive() );
  worldLocator->invalidateCache();
  invalidateCache();
  
  BasicLocator::step();
}
