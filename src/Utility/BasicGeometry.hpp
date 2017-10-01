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
 * @file BasicGeometry.hpp
 *
 * This class defines a collision geometry as the combination of a
 * CollisionMaterial (which contains various collision properties) and Shape.
 */

/**
 * @class lifespace::BasicGeometry
 * @ingroup Utility
 *
 * @brief
 * This class defines a collision geometry as the combination of a
 * CollisionMaterial (which contains various collision properties) and Shape.
 */
#ifndef LS_U_BASICGEOMETRY_HPP
#define LS_U_BASICGEOMETRY_HPP


#include "../types.hpp"
#include "Geometry.hpp"
#include "shapes.hpp"
#include "CollisionMaterial.hpp"

#include <boost/shared_ptr.hpp>




namespace lifespace {
  
  
  
  
  struct BasicGeometry :
    public Geometry
  {
    boost::shared_ptr<const Shape> shape;
    boost::shared_ptr<const CollisionMaterial> collisionMaterial;
    
    BasicGeometry( boost::shared_ptr<const Shape> shape_,
                   boost::shared_ptr<const CollisionMaterial>
                   collisionMaterial_ ) :
      Geometry(),
      shape( shape_ ), collisionMaterial( collisionMaterial_ )
    {}
  };




}   /* namespace lifespace */



#endif   /* LS_T_BASICGEOMETRY_HPP */
