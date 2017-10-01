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
 * @file FrameState.hpp
 *
 * <brief>
 */

/**
 * @class lifespace::FrameState
 * @ingroup OpenGLRenderer
 *
 * @brief
 * <brief>
 */
#ifndef LS_R_FRAMESTATE_HPP
#define LS_R_FRAMESTATE_HPP


#include "../../types.hpp"




namespace lifespace {
  
  
  /* forwards */
  class OpenGLRenderer;
  class Light;
  class Subspace;
  
  
  
  
  class FrameState
  {
    OpenGLRenderer & renderer;
    int nextLight;
    int maxLights;
    
    
    void enableLight( const Light & light, int lightNum,
                      const Subspace & hostSpace );
    
    void disableLight( int lightNum );
    
    
  public:
    
    FrameState( OpenGLRenderer & renderer, int maxLights );
    
    
    /** Activates a new light. */
    void pushLight( const Light & light, const Subspace & hostSpace );
    
    /** Deactivates the last activated light(s). */
    void popLight( int count = 1 );
    
  };
  
  
  
  
}   /* namespace lifespace */




#endif   /* LS_R_FRAMESTATE_HPP */
