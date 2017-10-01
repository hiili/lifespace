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
 * @file Renderer.hpp
 *
 * <brief>
 */

/**
 * @class lifespace::Renderer
 * @ingroup Renderers
 *
 * @brief
 * <brief>
 */
#ifndef LS_R_RENDERER_HPP
#define LS_R_RENDERER_HPP


#include "../types.hpp"
#include <boost/shared_ptr.hpp>




namespace lifespace {
  
  
  /* forwards */
  class RenderTarget;
  class RenderSource;
  
  
  
  
  class Renderer
  {
    
    
  public:
    
    /* constructors/destructors/etc */
    
    virtual ~Renderer() {}
    
    
    /* accessors */
    
    virtual void setRenderTarget( RenderTarget * newRenderTarget ) = 0;
    
    virtual void setRenderSource( boost::shared_ptr<const RenderSource>
                                  newRenderSource ) = 0;
    
    
    /* operations */
    
    virtual void render() = 0;
    
  };
  
  
  
  
}   /* namespace lifespace */




#endif   /* LS_R_RENDERER_HPP */
