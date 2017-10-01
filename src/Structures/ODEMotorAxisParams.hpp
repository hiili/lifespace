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
 * @file ODEMotorAxisParams.hpp
 *
 * Lifespace Matrices.
 */

/**
 * @class lifespace::ODEMotorAxisParams
 * @ingroup Structures
 *
 * @brief
 * Common parameters for all ODE joint motors.
 */
#ifndef LS_S_ODEMOTORAXISPARAMS_HPP
#define LS_S_ODEMOTORAXISPARAMS_HPP


#include "../types.hpp"
#include "ode/ode.h"
#include "ode/odecpp.h"




namespace lifespace {
  
  
  
  
  /** Defines params of one motor axis for an ODE motor. */
  struct ODEMotorAxisParams {
    
    /**
     * Maximum force output of the motor for reaching the target velocity.
     *
     * @note
     * This does \em not affect the direct force controls.
     */
    dReal maxMotorForce;
    
    /** Low stop angle in radians. Use \code -dInfinity to disable. */
    dReal loStop;
    
    /** High stop angle in radians. Use \code -dInfinity to disable. */
    dReal hiStop;
    
    /** Bounciness of the stops. 0.0 = not bouncy, 1.0 = full bounciness. */
    dReal stopBounciness;
    
    
    ODEMotorAxisParams() :
      maxMotorForce( 0.0 ),
      loStop( -dInfinity ),
      hiStop( +dInfinity ),
      stopBounciness( 0.0 )
    {}
    
    ODEMotorAxisParams( real maxMotorForce_, real loStop_, real hiStop_,
                        real stopBounciness_ ) :
      maxMotorForce( maxMotorForce_ ),
      loStop( loStop_ ),
      hiStop( hiStop_ ),
      stopBounciness( stopBounciness_ )
    {}
    
  };
  
  
  
  
}   /* namespace lifespace */


#endif   /* LS_S_ODEMOTORAXISPARAMS_HPP */
