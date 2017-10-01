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
 * @file Visual.hpp
 *
 * The base interface for all renderable entities.
 */

/**
 * @class lifespace::Visual
 * @ingroup Graphics
 *
 * @brief
 * The base interface for all renderable entities.
 *
 * Attaching this kind of object to an Object makes it visible in the
 * world. Implement the pure virtual method applyToGfx() to provide the actual
 * rendering implementation.
 *
 * Every Object connected to the world structure will have its applyToGfx()
 * method called on every rendering pass. By default, this method just forwards
 * the call to the same named method of an attached Visual, if available.
 *
 * @if done_todos
 * @todo
 * done: Implement scaling somewhere here (scaling of visuals was previously
 * implemented in BasicLocator, which is clearly not the right place).
 * @endif
 *
 * @sa Object
 */
#ifndef LS_G_VISUAL_HPP
#define LS_G_VISUAL_HPP


namespace lifespace {
  
  
  
  
  struct Visual
  {
    /** Make the class polymorphic. */
    virtual ~Visual() {}
  };
  
  
  
  
}   /* namespace lifespace */


#endif   /* LS_G_VISUAL_HPP */
