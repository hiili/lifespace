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
 * @file Content_constants.cpp
 */
#include "../types.hpp"
#include "FloatingActor.hpp"
using namespace lifespace;




/* FloatingActor */
const float FloatingActor::DEFAULT_AUTOROLL_FORCE = 20.0;
const real FloatingActor::VEL_CONSTANT_FRICTION = 0.1;
const real FloatingActor::VEL_LINEAR_FRICTION = 1.0;
const real FloatingActor::ROT_CONSTANT_FRICTION = 0.1;
const real FloatingActor::ROT_LINEAR_FRICTION = 7.0;
