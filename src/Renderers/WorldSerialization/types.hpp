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
 * @file src/Renderers/WorldSerialization/types.hpp
 * @ingroup WorldSerialization
 *
 * Common types for the WorldSerialization renderer.
 */
#ifndef LS_R_WS_TYPES_HPP
#define LS_R_WS_TYPES_HPP


#include <boost/static_assert.hpp>

#include <string>
#include <map>




namespace lifespace {
  
  
  
  
  /** . */
  class WorldSerialization {
    
    BOOST_STATIC_ASSERT( sizeof(int) >= 4 );
    
    
  public:
    
    /** Mask of OR'ed WorldSerialization::SerializableProperties to be
        (de)serialized from (to) each object. */
    typedef int PropertyMask;
    
    /** Available Object properties to be (de)serialized. */
    enum SerializableProperties {
      PROP_LOCATOR         = 2^0,    /**< the Locator component, if exists */
      PROP_ACTOR_SENSORS   = 2^1,    /**< the Actor base class, if inherited
                                          from */
      PROP_ALL             = (2^2) - 1 /**< all available properties */
    };
    
    
    /** For converting property names into corresponding property masks. Use it
        as an std::map<string, PropertyMask>. (non-const to allow use of []) */
    static std::map<std::string, PropertyMask> PropertyName2Mask;
    
  };
  
  
  
  
}   /* namespace lifespace */




#endif   /* LS_R_WS_TYPES_HPP */
