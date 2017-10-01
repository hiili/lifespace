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
 * @file Structures_constants.cpp
 */
#include "../types.hpp"
#include "BasisMatrix.hpp"
#include "InertiaLocator.hpp"
#include "ODELocator.hpp"
#include "Camera.hpp"
using namespace lifespace;




/* BasisMatrix */
const int BasisMatrix::ORTHONR_FREQ = 100;


/* InertiaLocator */
const real InertiaLocator::DEFAULT_VEL_CONSTANT_FRICTION = 0.01;
const real InertiaLocator::DEFAULT_VEL_LINEAR_FRICTION   = 0.4;
const real InertiaLocator::DEFAULT_ROT_CONSTANT_FRICTION = 0.01;
const real InertiaLocator::DEFAULT_ROT_LINEAR_FRICTION   = 0.4;


/* ODELocator */
const real ODELocator::DEFAULT_VEL_CONSTANT_DRAG    = 0.01;
const real ODELocator::DEFAULT_VEL_LINEAR_DRAG      = 0.01;
const real ODELocator::DEFAULT_VEL_QUADRATIC_DRAG   = 0.01;
const real ODELocator::DEFAULT_ROT_CONSTANT_DRAG    = 0.01;
const real ODELocator::DEFAULT_ROT_LINEAR_DRAG      = 0.01;
const real ODELocator::DEFAULT_ROT_QUADRATIC_DRAG   = 0.01;


/* Camera */
const float Camera::DEFAULT_FOV = 65.0;
