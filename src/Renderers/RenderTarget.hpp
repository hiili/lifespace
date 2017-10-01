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
 * @file RenderTarget.hpp
 *
 * Base class for all rendering targets.
 */

/**
 * @class lifespace::RenderTarget
 * @ingroup Renderers
 *
 * @brief
 * Base class for all rendering targets.
 *
 * The renderer field has default accessors to read and set it. Remember to
 * override these if you need special handling when the renderer is being
 * changed.
 */
#ifndef LS_R_RENDERTARGET_HPP
#define LS_R_RENDERTARGET_HPP


#include "../types.hpp"
#include "Renderer.hpp"




namespace lifespace {
  
  
  
  
  class RenderTarget
  {
    
  protected:
    
    /** Renderer for this target. The pointed object is owned by this
        object. */
    Renderer * renderer;
    
    
  public:
    
    /* constructors/destructors/etc */
    RenderTarget( Renderer * renderer_ = 0 ) :
      renderer( renderer_ )
    {}
    
    /** Make the class polymorphic. */
    virtual ~RenderTarget() {}
    
    
    /* accessors */
    
    virtual const Renderer * getRenderer() const
    { return renderer; }
    
    virtual Renderer * getRenderer()
    { return renderer; }

    virtual void setRenderer( Renderer * newRenderer )
    { delete renderer; renderer = newRenderer; }
    
    
    /* operations */
    
  };
  
  
  
  
}   /* namespace lifespace */




#endif   /* LS_R_RENDERTARGET_HPP */
