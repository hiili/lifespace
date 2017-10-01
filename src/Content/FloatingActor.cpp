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
 * @file FloatingActor.cpp
 */
#include "../types.hpp"
#include "FloatingActor.hpp"
#include "../Structures/InertiaLocator.hpp"
#include <boost/shared_ptr.hpp>
#include <cmath>
#include <iostream>
using namespace lifespace;




void FloatingActor::init()
{
  // change mode for all pulse controls
  for( int control = CTRL_TORQUEREL_X_POS ;
       control <= CTRL_TORQUEREL_Z_NEG ; control++ ) {
    controls[control].setMode( Control::PulseMode );
  }
  
  // no sensors
  
}


void FloatingActor::applyAutoRoll()
{
  real worldY = getLocator()->getBasis().getBasisVec( DIM_X )( DIM_Y );
  real angle = std::asin( worldY );
  
  getLocator()->addTorqueRel( (autoRollForce * angle)
                              * makeVector3d( 0.0, 0.0, -1.0 ) );
}




FloatingActor::FloatingActor() :
  Object( Object::Params
          ( new InertiaLocator( ZeroVector(3), BasisMatrix(3),
                                1.0, 1.0,
                                VEL_CONSTANT_FRICTION,
                                VEL_LINEAR_FRICTION,
                                ROT_CONSTANT_FRICTION,
                                ROT_LINEAR_FRICTION ) ) ),
  Actor( ControlCount, SensorCount ),
  autoRoll( true ),
  autoRollForce( DEFAULT_AUTOROLL_FORCE ),
  basisOrthonormalization( true )
{ init(); }


FloatingActor::FloatingActor( const Object::Params & objectParams ) :
  Object( objectParams ),
  Actor( ControlCount, SensorCount ),
  autoRoll( true ),
  autoRollForce( DEFAULT_AUTOROLL_FORCE ),
  basisOrthonormalization( true )
{ init(); }




void FloatingActor::localPrepare( real dt )
{
  // handle the CTRL_SETLOC_n controls
  if( readControl(CTRL_SETLOC) > 0.5 ) {
    getLocator()->setLoc( makeVector3d( readControl(CTRL_LOC_X),
                                        readControl(CTRL_LOC_Y),
                                        readControl(CTRL_LOC_Z) ));
  }
  
  // handle the CTRL_SETBASIS_n controls
  if( readControl(CTRL_SETBASIS) > 0.5 ) {
    Matrix m(3,3);
    for( int col = 0 ; col < 3 ; col++ ) {
      for( int row = 0 ; row < 3 ; row++ ) {
        m(row,col) = readControl(CTRL_BASIS_XX + col * 3 + row);
      }
    }
    getLocator()->setBasis( BasisMatrix( m, !basisOrthonormalization ) );
  }
  
  // handle the CTRL_SETVEL_n controls
  if( readControl(CTRL_SETVEL) > 0.5 ) {
    getLocator()->setVel( makeVector3d( readControl(CTRL_VEL_X),
                                        readControl(CTRL_VEL_Y),
                                        readControl(CTRL_VEL_Z) ));
  }
  
  // handle the CTRL_SETROTATION_n controls
  if( readControl(CTRL_SETROTATION) > 0.5 ) {
    getLocator()->setRotation( makeVector3d( readControl(CTRL_ROTATION_X),
                                             readControl(CTRL_ROTATION_Y),
                                             readControl(CTRL_ROTATION_Z) ));
  }
  
  // handle the CTRL_FORCEREL_n controls
  getLocator()->addForceRel
    ( makeVector3d( readControl( CTRL_FORCEREL_X_POS )
                    - readControl( CTRL_FORCEREL_X_NEG ),
                    readControl( CTRL_FORCEREL_Y_POS )
                    - readControl( CTRL_FORCEREL_Y_NEG ),
                    readControl( CTRL_FORCEREL_Z_POS )
                    - readControl( CTRL_FORCEREL_Z_NEG ) ));

  // handle the CTRL_TORQUEREL_n controls
  getLocator()->addTorqueRel
    ( makeVector3d( readControl( CTRL_TORQUEREL_X_POS )
                    - readControl( CTRL_TORQUEREL_X_NEG ),
                    readControl( CTRL_TORQUEREL_Y_POS )
                    - readControl( CTRL_TORQUEREL_Y_NEG ),
                    readControl( CTRL_TORQUEREL_Z_POS )
                    - readControl( CTRL_TORQUEREL_Z_NEG ) ));
  if( autoRoll ) applyAutoRoll();
}


void FloatingActor::localStep()
{}


void FloatingActor::prepare( real dt )
{
  localPrepare( dt );
  Actor::prepare( dt );
  Object::prepare( dt );
}

void FloatingActor::step()
{
  localStep();
  Actor::step();
  Object::step();
}
