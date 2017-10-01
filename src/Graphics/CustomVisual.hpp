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
 * @file CustomVisual.hpp
 *
 * Base class for visuals which have a custom OpenGL implementation.
 */

/**
 * @class lifespace::CustomVisual
 * @ingroup Graphics
 *
 * @brief
 * Base class for visuals which have a custom OpenGL implementation.
 *
 * Derive from this abstract base class if you need to use OpenGL directly to
 * render a visual. The render() method will be called whenever the visual
 * needs to be rendered to OpenGL.
 *
 * \par Context data
 * Renderers offer storage for context-specific data, which can be used, for
 * example, to store display lists or textures. The context storage works as
 * follows:
 *   - The first time a renderer calls a CustomVisual's render() method, a null
 *     pointer is provided as the context argument. Derive your own context
 *     class from CustomVisual::Context, initialize it and return it as the
 *     method's return value.
 *   - Later, when the \em same renderer calls your CustomVisual's render(), a
 *     pointer to the previously returned context is given as the context
 *     argument. You can modify its contents as you need, or even delete it and
 *     create a new one, but remember to return it again at the end if you want
 *     to get it back also at the next time!
 *   - A single context object is created for a single Renderer - CustomVisual
 *     pair: if there are two renderers active in the system, then your
 *     render() method is called initially twice with a null context. This
 *     holds even if the current CustomVisual instance is used in multiple
 *     places: the first time a renderer sees a CustomVisual instance, a
 *     context is stored for it, and the same context is offered back if the
 *     same instance is encountered again somewhere else.
 *
 * \par Ownership of the context objects
 * The ownership of the context objects is on the renderer while the context is
 * stored in it, and on the CustomVisual instance while the render() method is
 * running. This means that you can delete your context in render() and just
 * return a null pointer to get rid of a context. But note that this also means
 * that the renderer might also delete the context at any time while it is
 * stored in it. For this reason the context object should have an appropriate
 * destructor if it contains resources that should be freed at the end, and the
 * CustomVisual should be prepared to receive again a null context at any time!
 *
 * \par Things to remember:
 *   - There might be more than one renderer active at the same time, so your
 *     CustomVisual might have to create multiple contexts. This means that if
 *     you need some common data in the contexts (textures, for example), then
 *     load it somewhere else to a common storage and do only the
 *     context-specific binding etc. with the context object.
 *   - Each renderer stores a single context for \em every CustomVisual
 *     instance it encounters, so try to re-use the same instances as much as
 *     possible (to avoid, for example, binding a large texture to multiple
 *     names within the same context).
 *   - The context object should have an appropriate destructor (see "Ownership
 *     of the context objects" above).
 *   - The CustomVisual should be prepared to receive a null context at \em any
 *     time (see "Ownership of the context objects" above).
 */
#ifndef LS_G_CUSTOMVISUAL_HPP
#define LS_G_CUSTOMVISUAL_HPP


#include "../types.hpp"
#include "Visual.hpp"




namespace lifespace {
  
  
  
  
  class CustomVisual :
    public Visual
  {
    
  public:
    
    /** - */
    class Context {};
    
    /** */
    virtual Context * render( Context * context ) const = 0;
    
  };
  
  
  
  
}   /* namespace lifespace */


#endif   /* LS_G_CUSTOMVISUAL_HPP */
