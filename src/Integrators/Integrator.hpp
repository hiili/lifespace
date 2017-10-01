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
 * @file Integrator.hpp
 *
 * Interface for actual timestepping.
 */

/**
 * @class lifespace::Integrator
 * @ingroup Integrators
 *
 * @brief
 * Interface for actual timestepping.
 *
 * @sa BasicIntegrator
 */
#ifndef LS_T_INTEGRATOR_HPP
#define LS_T_INTEGRATOR_HPP


#include "../types.hpp"
#include "../Structures/Subspace.hpp"
#include <list>


namespace lifespace {
  
  
  /* forwards */
  class Object;
  
  
  
  
  class Integrator
  {
  public:
    virtual ~Integrator() {}
    virtual void prepare( Subspace::objects_t & objects, real dt ) = 0;
    virtual void step( Subspace::objects_t & objects ) = 0;
  };
  
  
  
  
}   /* namespace lifespace */


#endif   /* LS_T_INTEGRATOR_HPP */
