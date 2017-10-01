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
 * @file src/Graphics/types.hpp
 * @ingroup Graphics
 *
 * Common types for the Graphics module.
 */
#ifndef LS_G_TYPES_HPP
#define LS_G_TYPES_HPP


#include "../types.hpp"
#include "../Utility/Event.hpp"


namespace lifespace {
  
  
  
  
  /** Event types originating from and accepted by the graphics system. The
      responsible graphics classes should take care of sending these events
      through their EventHost whenever they occur. */
  enum GraphicsEvents {
    
    /** This event is sent whenever the graphics device has restarted its
        rendering cycle (it may or may not block while the event is being
        processed, use some of the other events if blocking is explicitly
        needed). This event is also accepted by Viewport and causes it to
        schedule itself to be redrawn. */
    GE_TICK,
    
    /** Is sent when the device or viewport is about to start refreshing
        itself. The source blocks until all listeners have processed the
        event. */
    GE_REFRESH_BEGIN,
    
    /** Is sent when the device or viewport has refreshed itself. The refresh
       operation not announced to be finished until all listeners have
       processed the event. */
    GE_REFRESH_END
    
  };
  
  /**
   * The generic event type emitted by the graphics system.
   */
  typedef Event<GraphicsEvents,void *> GraphicsEvent;
  
  
  
  
}   /* namespace lifespace */




#endif   /* LS_G_TYPES_HPP */
