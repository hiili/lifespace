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
 * @file WorldSerialization.hpp
 *
 * Provides simulation world serialization functionality.
 */

/**
 * @defgroup WorldSerialization WorldSerialization
 * @ingroup Renderers
 *
 * Provides simulation world serialization functionality.
 *
 * Currently only some predefined state attributes of world objects can be
 * serialized. World structure and internal composite structure of world
 * objects are not preserved: the world hierarchy and objects must already
 * exist during deserialization, which then just writes the stored attributes
 * into the corresponding objects. Objects are identified by their name (can be
 * set with lifespace::Object::setName()).
 */
#ifndef LS_R_WORLDSERIALIZATION_HPP
#define LS_R_WORLDSERIALIZATION_HPP


// top-down includes for client programs
#include "WorldSerializer.hpp"
#include "WorldDeserializer.hpp"


#endif   /* LS_R_WORLDSERIALIZATION_HPP */
