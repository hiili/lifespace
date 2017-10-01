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
 * @file Graphics.hpp
 *
 * A core module of the Lifespace Simulator.
 */

/**
 * @defgroup Graphics Graphics
 *
 * Graphics module, one of the core modules in the Lifespace Simulator,
 * contains the user interface and the rendering engine for rendering the
 * Lifespace world.
 *
 * \par The user interface
 * The graphics output target is represented by the abstract class Device. Each
 * graphics device can contain multiple viewports (class Viewport), which in
 * turn display the simulation world from different locations. The actual
 * viewing location is represented by a Camera object, which must be connected
 * to a World to be rendered, and to a Viewport where the image will be shown.
 *
 * \par
 * Individual graphics device implementations exist as lifespace plugins which
 * inherit from the Device and Viewport classes. Currently there are plugins
 * implementing these classes with the GLOW Toolkit ("glow" plugin) and with VR
 * Juggler ("vrjuggler" plugin). User input is abstracted through the Control
 * module instead of graphics devices, but currently at least the glow plugin
 * implements also the Controller class to provide keyboard and mouse input
 * through it. See the plugin's documentation for details.
 *
 * \par timestepping the world
 * The camera is also a normal object in the simulation world, so the world
 * must be timestepped from somewhere to allow the camera to move. The world
 * can be stepped explicitly by calling its timestep() method, or it can be
 * connected to the events subobject of a Device or Viewport class to receive
 * timestep commands automatically (to avoid the need for multiple threads
 * after entering the graphics system's mainloop).
 */
#ifndef LS_GRAPHICS_HPP
#define LS_GRAPHICS_HPP


#include "../types.hpp"
#include "Visual.hpp"
#include "BasicVisual.hpp"
#include "CustomVisual.hpp"
#include "Environment.hpp"
#include "Light.hpp"
#include "Material.hpp"
#include "Device.hpp"
#include "Viewport.hpp"




namespace lifespace {
  
  
  
  
  
  
  
  
}




#endif   /* LS_GRAPHICS_HPP */
