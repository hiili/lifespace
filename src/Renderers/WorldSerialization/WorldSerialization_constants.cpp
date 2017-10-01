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
 * @file WorldSerialization_constants.cpp
 */
#include "types.hpp"
using namespace lifespace;

#include <string>
using std::string;

#include <map>
using std::map;

#include <boost/assign/list_of.hpp>
using boost::assign::map_list_of;




map<string, WorldSerialization::PropertyMask>
WorldSerialization::PropertyName2Mask = map_list_of
  ("locator",         WorldSerialization::PROP_LOCATOR )
  ("actor_sensors",   WorldSerialization::PROP_ACTOR_SENSORS );
