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
 * @file Structures.hpp
 *
 * A core module of the Lifespace Simulator.
 */

/**
 * @defgroup Structures Structures
 * 
 * Structures module, one of the core modules in the Lifespace Simulator,
 * provides the classes and management tools for the physical objects and
 * structures in the simulation.
 * 
 * The main entity defining the world is an instance of a class inherited from
 * World. It contains a hierarchial structure of subspaces and objects that are
 * all inherited from the class Object (the class World and Subspace are also
 * inherited from it).
 *
 * @todo
 * Refactor the Locator, BasisMatrix and InertiaLocator
 *
 * @todo
 * Consider if subspaces would contain locators instead of objects, and
 * everything else (dynamics, visuals) would be built around these locators?
 * The world would be a hierarchial collection of locations, each having
 * possibly some extra attributes. (so each thing in the world would have to
 * have a location? hmm, maybe not a good idea.. Also ruins the possibility to
 * integrate external libraries by extending Locators.)
 *
 * @todo
 * Unify the types and forwarding usages of controls, sensors and
 * connectors.
 *
 * @todo
 * Maybe: Change the map containers into vector containers for speedup, but
 * make sure that entries can be added and removed dynamically (removing from
 * the middle whould leave a null entry behind, and adding would need a way to
 * find quickly enough a free slot..).
 */
#ifndef LS_STRUCTURES_HPP
#define LS_STRUCTURES_HPP


#include "../types.hpp"
#include "World.hpp"
#include "ODEWorld.hpp"
#include "Object.hpp"
#include "Subspace.hpp"
#include "Connector.hpp"
#include "ODEMotorAxisParams.hpp"
#include "ODEAxleConnector.hpp"
#include "ODESliderConnector.hpp"
#include "ODEBallConnector.hpp"
#include "ODEUniversalConnector.hpp"
#include "ODEFixedConnector.hpp"
#include "Camera.hpp"
#include "Locator.hpp"
#include "BasicLocator.hpp"
#include "MotionLocator.hpp"
#include "InertiaLocator.hpp"
#include "ODELocator.hpp"




/** */
namespace lifespace {
  
  
  
  
}




#endif   /* LS_STRUCTURES_HPP */
