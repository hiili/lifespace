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
 * @file lifespace.hpp
 *
 * Main header for the Lifespace Simulator.
 *
 * Version: $Name: lifespace-0_0_31 $
 *
 * @if done_todos
 * @todo
 * done: namespace Lifespace --> namespace lifespace.
 * @endif
 *
 * @bug
 * A \em possible bug: if lifespace and ode are compiled with different
 * numerical accuracy (float vs. double) then joint stop disabling \em may not
 * work correctly, as the dInfinity constants get converted to lifespace reals
 * before ending up into ode.
 *
 * @todo
 * Utility module: signal generators (sawtooth, sine, ...), etc..
 *
 * @todo
 * Speed limiter (or a real speed control) to the idle callback (entering
 * gamemode can cause the idle callback to be called in a long burst).
 *
 * @todo
 * Make sure that shared_ptr usage is consistent and does what is intended.
 *
 * @todo
 * Translate the makefiles into English.
 *
 * @todo
 * Finish and fix the cleanup targets in the makefiles.
 *
 * @todo
 * Create a new Shape class hierarchy under Utility and derive basic shapes,
 * location and size modifiers, union operations etc. from it. Put all actual
 * apply functionality into the contexts (BasicVisual, Object::collisionShape,
 * ..) where shapes can be applied (via polymorphism).
 *
 * @todo
 * Remove the automatic rawptr->shared_ptr conversions from the Params classes
 * and start using the shared() helper function.
 *
 * @todo
 * Remove the extra 'doxygen star' from all comment blocks in .cpp files to
 * make them just regular comments (now they do not show up in doxygen's source
 * browser output if STRIP_CODE_COMMENTS is on).
 *
 * @todo
 * Figure out a clean(!) and consistent way of taking temporary raw pointers or
 * references from shared_ptr-managed objects so that use-count is raised
 * correctly for the duration of use (currently a temporary local shared_ptr
 * copy is taken for the time of the operation, as in OpenGLRenderer::render(
 * const BasicVisual & )).
 *
 * @todo
 * Physics module: create a Physics module and move some ODE stuff from
 * Structures and collision detection stuff from Utility into it.
 */
#ifndef LIFESPACE_HPP
#define LIFESPACE_HPP


#include "types.hpp"
#include "Graphics/Graphics.hpp"
#include "Structures/Structures.hpp"
#include "Control/Control.hpp"
#include "Utility/Utility.hpp"
#include "Content/Content.hpp"
#include "Integrators/Integrators.hpp"
#include "Renderers/Renderers.hpp"




namespace lifespace {




}   /* namespace lifespace */




#endif   /* LIFESPACE_HPP */
