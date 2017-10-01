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
 * @file CollisionMaterial.hpp
 *
 * Collision and friction properties of a surface.
 */

/**
 * @class lifespace::CollisionMaterial
 * @ingroup Utility
 *
 * @brief
 * Collision and friction properties of a surface.
 *
 * @todo
 * Implement a factory method for easily instantiate a collision material and
 * wrap it in a shared_ptr.
 */
#ifndef LS_G_COLLISIONMATERIAL_HPP
#define LS_G_COLLISIONMATERIAL_HPP


#include "../types.hpp"




namespace lifespace {
  
  
  
  
  struct CollisionMaterial
  {
    float friction;
    float bounciness;
    float bounceMinVel;
    
    /**
     * Creates a new CollisionMaterial object, which can be used with
     * BasicGeometry.
     *
     * @param friction_       Coulomb friction coefficient e [0 - INFINITY].
     * @param bounciness_     Bounciness e [0 - 1], 0 == no bouncing, 1 ==
     *                        maximum bounciness.
     * @param bounceMinVel_   Minimum collision velocity needed to result in
     *                        bouncing.
     */
    CollisionMaterial( float friction_, float bounciness_,
                       float bounceMinVel_ ) :
      friction( friction_ ),
      bounciness( bounciness_ ),
      bounceMinVel( bounceMinVel_ )
    {}
  };
  
  
  
  
}   /* namespace lifespace */


#endif   /* LS_G_COLLISIONMATERIAL_HPP */
