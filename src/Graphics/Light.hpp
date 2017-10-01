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
 * @file Light.hpp
 *
 * A light source bound to a given locator.
 */

/**
 * @class lifespace::Light
 * @ingroup Graphics
 *
 * @brief
 * A light source bound to a given object.
 *
 * It can be activated by inserting it into an Environment which is attached to
 * an active Subspace.
 *
 * @sa Gfx, Environment, Material
 *
 * @if done_todos
 * @todo
 * done: Consider what happens if the Light and its target Locator are in
 * different subspaces (the light's real location will not be what might be
 * expected).
 * @endif
 */
#ifndef LS_G_LIGHT_HPP
#define LS_G_LIGHT_HPP


#include "Material.hpp"
#include "Viewport.hpp"
#include "../Structures/Vector.hpp"
#include "../Structures/Object.hpp"
#include "../Structures/Locator.hpp"
#include <boost/shared_ptr.hpp>
#include <GL/gl.h>


namespace lifespace {
  
  
  
  
  class Light
  {
    // for all lights
    const Material * material;
    bool directional;
    
    // only for normal lights
    const boost::shared_ptr<Object> object;
    const GLfloat * attenuation;
    
    // only for directional lights
    GLfloat source[4];
    
    /** OpenGLRenderer's helper class FrameState needs access to here. */
    friend class FrameState;
    
    
  public:
    /**
     * Creates a normal light bound to the given light material, object and
     * attenuation parameters.
     *
     * The light will follow the target object. Attenuation can be modified
     * later and it will affect directly this light.
     *
     * @note
     * Currently the target object must be either in the same subspace as the
     * light, or somewhere under it in the subspace hierarchy!
     *
     * @note
     * The Material class does not take local copies from the material
     * properties given to it, and this applies also to the copy contructor
     * that is used here!
     *
     * @sa Material, Locator
     */
    Light( const Material * material_, const boost::shared_ptr<Object> object_,
           const GLfloat attenuation_[3] ) :
      material( material_ ), directional( false ),
      object( object_ ), attenuation( attenuation_ )
    { assert( material && object ); }
    
    /** Creates a directional light. The direction is relative to the
        orientation of the Subspace where the light is inserted. */
    Light( const Material * material_, const Vector & source_ ) :
      material( material_ ), directional( true ),
      object(), attenuation( 0 )
    {
      assert( material );
      source[0] = source_[0]; source[1] = source_[1];
      source[2] = source_[2]; source[3] = 0.0;
    }
    
    virtual ~Light() {}
  };
  
  
  
  
}   /* namespace lifespace */


#endif   /* LS_G_LIGHT_HPP */
