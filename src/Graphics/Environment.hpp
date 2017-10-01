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
 * @file Environment.hpp
 *
 * The Environment class contains a set of OpenGL state modification commands,
 * and it can be used to set up a local rendering environment for a
 * Subspace.
 */

/**
 * @class lifespace::Environment
 * @ingroup Graphics
 *
 * @brief
 * The Environment class contains a set of OpenGL state modification commands,
 * and it can be used to set up a local rendering environment for a
 * Subspace.
 */
#ifndef LS_G_ENVIRONMENT_HPP
#define LS_G_ENVIRONMENT_HPP


#include "../types.hpp"
#include <list>
#include <GL/gl.h>


namespace lifespace {
  
  
  /* forwards */
  class Light;
  class Subspace;
  
  
  
  
  class Environment
  {
    
    
  public:
    
    /** Defines a single OpenGL state modification command. */
    struct OGLState {
      GLenum type, name;
      void * value;
      
      OGLState( GLenum type_, GLenum name_, void * value_ ) :
        type( type_ ), name( name_ ), value( value_ )
      {}
    };
    
    
  private:
    std::list<Light *> lights;
    std::list<const OGLState *> oglStates;
    GLbitfield oglStateMask;
    
    void recomputeOglStateMask();
    
    friend class OpenGLRenderer;
    
    
  public:
    Environment() :
      oglStateMask( 0 )
    {}
    
    virtual ~Environment() {}
    
    
    /**
     * @note
     * Lights will be processed in the order they were added.
     */
    void addLight( Light * light )
    { lights.push_back( light ); }
    
    void removeLight( Light * light )
    { lights.remove( light ); }
    
    
    void addOGLState( const OGLState * state )
    {
        oglStates.push_back( state );
        oglStateMask |= state->type;
    }
    
    void removeOGLState( const OGLState * state )
    {
        oglStates.remove( state );
        recomputeOglStateMask();
    }
    
    
    /** currently no-op. can be used for some precomputations/optimizations. */
    virtual void prepare( real dt ) {}
    
    /** currently no-op. can be used for some precomputations/optimizations. */
    virtual void step() {}
  };
  
  
  
  
}   /* namespace lifespace */


#endif   /* LS_G_ENVIRONMENT_HPP */
