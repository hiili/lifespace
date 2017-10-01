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
 * @file Controller.hpp
 *
 * The base class for all controllers.
 */

/**
 * @class lifespace::Controller
 * @ingroup Control
 *
 * @brief
 * The base class for all controllers.
 *
 * A Controller object generates control signals, which can be connected to one
 * or more Actor objects to control their behaviour. Control signals have an id
 * and are real-valued. A control id translation map (ControlMap) can be
 * assigned for each connected actor to match the control outputs to the
 * correct inputs in the actors.
 *
 * @sa Actor
 */

#ifndef LS_S_CONTROLLER_HPP
#define LS_S_CONTROLLER_HPP

#include "../types.hpp"
#include "Actor.hpp"
#include <list>

#include <boost/shared_ptr.hpp>




namespace lifespace {
  
  
  
  
  class Controller
  {
    
  public:
    
    /** Defines a single target control that can be used in a ControlMap. */
    struct MapTarget {
      unsigned int controlId;
      float sensitivity;
      
      MapTarget( unsigned int controlId_, float sensitivity_ ) :
        controlId( controlId_ ), sensitivity( sensitivity_ ) {}
      MapTarget() {}
    };
    
    /**
     * Defines a control map for translating the controller's control ids to
     * the correct controls in an actor.
     */
    typedef std::map<unsigned int, MapTarget> ControlMap;
    
    
  private:
    
    struct ActorEntry
    {
      boost::shared_ptr<Actor> actor;
      ControlMap * controlMap;
      
      ActorEntry() {}
      ActorEntry( boost::shared_ptr<Actor> actor_, ControlMap * controlMap_ ) :
        actor( actor_ ), controlMap( controlMap_ ) {}
      bool operator==( boost::shared_ptr<const Actor> other ) const
      { return actor == other; }
      bool operator!=( boost::shared_ptr<const Actor> other ) const
      { return !operator==( other ); }
    };
    
    std::list<ActorEntry> actors;
    
    
  protected:
    
    /**
     * Uses the given control of all connected actors. The control id and force
     * are filtered through any connected control maps.
     */
    void useControl( unsigned int id, real force );
    
    
  public:
    
    Controller() {}
    
    virtual ~Controller() {}


    /**
     * Adds an actor to the end of the actors list.
     *
     * A control map can be supplied to map the controller's control ids and
     * scale the forces before sending them to the actor. This can be used, for
     * example, for translating keyboard keycodes originating from an input
     * device to the appropriate Actor control ids.
     *
     * @note
     * The handling of non-existing codes in useControl() changes when a
     * control map is installed: control maps silently dispose signals with
     * unknown codes, so it is safe to call useControl() with a non-existing
     * control code if a control map is in use.
     *
     * @param controlMap   Pointer to the mapping to be used, or null to
     *                     disable translation. The pointed ControlMap object
     *                     is not copied (only the pointer is taken, so later
     *                     modifications to the target will affect also this
     *                     Actor) and its ownership is not taken.
     */
    void addActor( boost::shared_ptr<Actor> actor,
                   ControlMap * controlMap = 0 );
    
    /**
     * Removes an actor. It is an error to remove a non-existing actor. If the
     * same actor has been inserted multiple times to this controller, then the
     * last instance will be removed.
     */
    void removeActor( boost::shared_ptr<Actor> actor );
    
  };
  
} /* namespace lifespace */

#endif /* LS_S_CONTROLLER_HPP */
