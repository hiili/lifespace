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
 * @file glow.hpp
 *
 * The GLOW Toolkit plugin for the Lifespace Simulator.
 */

/**
 * @defgroup glow glow
 * @ingroup plugins
 *
 * The GLOW Toolkit plugin for the Lifespace Simulator.
 *
 * The plugin uses the GLOW Toolkit to implement the following Lifespace
 * Simulator classes:
 *   - Device (GLOWDevice): This is implemented as a GLOW window. Each
 *     GLOWDevice can contain multiple GLOWViewport objects.
 *   - Viewport (GLOWViewport): This is implemented as a GLOW viewport.
 *   - Controller (GLOWViewport): This is implented as a GLOW viewport. User
 *     input is used as the control source and forwarded to all connected Actor
 *     objects.
 *
 * The GLOW viewports display the simulation world from different locations and
 * forward user input to Actor objects in the world. The actual viewing
 * location is represented by a Camera object, which must be inserted to a
 * World to be rendered, and to a Viewport where the image will be shown. The
 * camera can be connected to a viewport's keyboard and mouse input by
 * connecting an actor class (FloatingActor, for example) to the viewport's
 * Controller interface and connecting the camera to the actor.
 *
 * @par GLOW, GLUT and freeglut

 * This plugin is implemented directly with the GLOW Toolkit, which in turn
 * uses GLUT. Using freeglut or openglut instead of GLUT under GLOW is strongly
 * recommended, but not required. Some functionality might not be available
 * with GLUT (especially with older GLUT versions), and using the mousewheel
 * might crash the whole program (a GLUT internal bug). If using freeglut,
 * consider using the newest development version, as the 2.2.0 release does not
 * have key repeat disabling implemented and the 2.4.0 release (the newest
 * release on time of writing) has a cursor bug causing crashes.
 *
 * @par Usage
 * The static method GLOWDevice::Init() must be called exactly once in the
 * whole program before using any functionality in this plugin. Individual GLOW
 * windows can then be created by instantiating objects from this class, and
 * viewports can be created into them with the class GLOWViewport. Finally the
 * system is started by calling the static method GLOWDevice::MainLoop().
 *
 * \par Creating a window with an interactive view into the world
 * To create a new window with an user controllable view into a simulation
 * world:
 *   - Initialize the GLOW windowing system with the static GLOWDevice::Init()
 *     method.
 *   - Create a new GLOWDevice.
 *   - Create a new GLOWViewport and attach it to the GLOW device.
 *   - Create a new World object and fill it with the objects to be simulated.
 *   - Connect the World to the GLOWDevice's eventhost.
 *   - Create a new actor object (FloatingActor, for example), create and
 *     connect a Camera object to it, and then insert the actor object into the
 *     World to a suitable starting location.
 *   - Install a Keymap to the actor object with the Actor::setKeymap method.
 *   - Connect the actor camera to the GLOWViewport with the viewport's
 *     setCamera() and setActor() methods.
 *   - Enter the windowing system's mainloop with the static
 *     GLOWDevice::MainLoop() method.
 *
 * \par
 * Now you should have an automatically timestepped simulation world and a
 * window which displays a user controllable view into the world. The keyboard
 * controls are defined in the keymap that was installed to the actor.
 *
 * @warning
 * Some GLUT 3.7 functionality was added by bypassing GLOW and accessing
 * GLUT/freeglut directly. The implementations are more like a quick hack than
 * a real solution, and at least the following inconsistencies follow:
 *   - GLOW keyboard filters are not applied to key up events.
 *   - deferred operations should not be called within key up callbacks.
 *   - keyboard callbacks may not work at all in other GLOW versions than
 *     1.0.4 (at least not in newer (development) versions, which have this
 *     same functionality).
 * See the GLOWViewport class and source code for more details.
 */
#ifndef LS_P_GLOW_HPP
#define LS_P_GLOW_HPP


#include "glow/GLOWDevice.hpp"
#include "glow/GLOWViewport.hpp"


namespace lifespace { namespace plugins { namespace pglow {
  
  
  
  
  
  
  
  
 }}}   /* lifespace::plugins::pglow */




#endif   /* LS_P_GLOW_HPP */
