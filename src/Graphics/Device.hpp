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
 * @file Device.hpp
 *
 * An OpenGL graphics device that can contain multiple Viewport objects (OpenGL
 * contexts).
 */

/**
 * @class lifespace::Device
 * @ingroup Graphics
 *
 * @brief
 * An OpenGL graphics device that can contain multiple Viewport objects (OpenGL
 * contexts).
 *
 * @sa Viewport
 */
#ifndef LS_G_DEVICE_HPP
#define LS_G_DEVICE_HPP


#include "../types.hpp"
#include "types.hpp"
#include "../Utility/Event.hpp"




namespace lifespace {
  
  
  
  
  class Device
  {
    
  public:
    
    /** Device events. */
    EventHost<GraphicsEvent> events;
    
  };




}   /* namespace lifespace */


#endif   /* LS_G_DEVICE_HPP */
