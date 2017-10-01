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
 * @file Material.hpp
 *
 * An abstraction for the basic OpenGL material properties: ambient, diffuse,
 * specular, emission and shininess.
 */

/**
 * @class lifespace::Material
 * @ingroup Graphics
 *
 * @brief
 * An abstraction for the basic OpenGL material properties: ambient, diffuse,
 * specular, emission and shininess.
 *
 * Can be also used to define a light source's "material", in which case only
 * ambient, diffuse and specular are used.
 *
 * @sa BasicVisual, Light
 *
 * @todo
 * (written before the separate OpenGLRenderer was created!)
 * Pre-defined dynamic materials (like Shapes): move the current 5 basic color
 * vectors to BasicMaterial, and do not specialize this base class (so that
 * textures can be also made with materials, and animated textures would also
 * be possible in future). Or not. Instead combine materials with shapes, they
 * are actually the same (think about multi-pass texturing, this would be
 * impossible with the current shape-material side-by-side design but would
 * need shape-inside-material design to allow multiple iterations). However,
 * the basic OpenGL material properties are still needed for lights.. Hmm,
 * advanced lighting, e.g. lightmaps or shadows, would already need more
 * information, which would even be into the direction of materials..
 */
#ifndef LS_G_MATERIAL_HPP
#define LS_G_MATERIAL_HPP


#include <GL/gl.h>


namespace lifespace {
  
  
  
  
  struct Material
  {
    const GLfloat * ambient;
    const GLfloat * diffuse;
    const GLfloat * specular;
    const GLfloat * emission;
    const GLfloat * shininess;
    const GLenum face;
    
    /**
     * Creates a new material object, which can be used with BasicVisual and
     * Light.
     *
     * @note
     * No local copies are made from the material properties, so all changes to
     * them will affect also this material.
     *
     * @param face_   Should be GL_FRONT, GL_BACK, or GL_FRONT_AND_BACK.
     */
    Material( const GLfloat ambient_[4], const GLfloat diffuse_[4],
              const GLfloat specular_[4], const GLfloat emission_[4],
              const GLfloat shininess_[1],
              GLenum face_ ) :
      ambient( ambient_ ),
      diffuse( diffuse_ ),
      specular( specular_ ),
      emission( emission_ ),
      shininess( shininess_ ),
      face( face_ )
    {}
  };
  
  
  
  
}   /* namespace lifespace */


#endif   /* LS_G_MATERIAL_HPP */
