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
 * @file BasicIntegrator.hpp
 *
 * This integrator just propagates the prepare() and step() calls to all target
 * objects and doesn't do any other processing.
 */

/**
 * @class lifespace::BasicIntegrator
 * @ingroup Integrators
 *
 * @brief
 * This integrator just propagates the prepare() and step() calls to all target
 * objects and doesn't do any other processing.
 *
 * @sa Integrator
 */
#ifndef LS_T_BASICINTEGRATOR_HPP
#define LS_T_BASICINTEGRATOR_HPP


#include "../types.hpp"
#include "../Structures/Object.hpp"
#include "../Structures/Subspace.hpp"
#include <list>
#include <algorithm>
#include <functional>


namespace lifespace {
  
  
  
  
  class BasicIntegrator :
    public Integrator
  {
  public:
    virtual ~BasicIntegrator() {}
    
    virtual void prepare( Subspace::objects_t & objects, real dt )
    {
      for( Subspace::objects_t::iterator i = objects.begin() ;
           i != objects.end(); i++ ) {
        (*i)->prepare( dt );
      }
    }
    
    virtual void step( Subspace::objects_t & objects )
    {
      for( Subspace::objects_t::iterator i = objects.begin() ;
           i != objects.end(); i++ ) {
        (*i)->step();
      }
    }
  };
  
  
  
  
}   /* namespace lifespace */


#endif   /* LS_T_BASICINTEGRATOR_HPP */
