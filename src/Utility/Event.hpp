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
 * @file Event.hpp
 *
 * Classes for an event notification / callback hook system.
 *
 * An EventListener class can be registered to an EventHost class. New Events
 * can be sent to the EventHost, which delivers them to all registered
 * listeners. The host can be used to act as a one-way event sender or as a
 * host for a callback hook or filter chain (callback/filter mode not yet
 * implemented).
 *
 * Both the EventHost and EventListener classes are templated and the Event can
 * be anything, but only host/listener pairs of same types can be connected.
 * Some simple event classes are provided for convenience.
 *
 * An eventlistener inherits from the EventListener class and implements the
 * pure virtual method processEvent() to handle events. Then it registers
 * itself to an EventHost.
 *
 * An eventhost is constructed with no arguments. To send an event, create a
 * new Event object and EventHost.sendEvent() it.
 *
 * @todo
 * Replace the event enums etc with an exception-like eventclass hierarchy.
 *
 * @todo
 * Replace the event system with libsigc++.
 */
#ifndef LS_U_EVENT_HPP
#define LS_U_EVENT_HPP


#include "../types.hpp"
#include <list>
#include <map>
#include <algorithm>
#include <functional>
#include <cassert>


namespace lifespace {
  
  
  
  
  /**
   * @class lifespace::EventHostEx
   * @ingroup Utility
   *
   * @brief
   * An extended EventHost that allows listeners to use overloading instead of
   * switching to parse the event types.
   */
  template<class Listener, class Source>
  class EventHostEx
  {
    
    Source & source;
    
    /** Container type for storing active listeners. */
    typedef std::list<Listener *> listeners_t;
    
    /** Container for active listeners. Note that it is stored as reversed! */
    listeners_t listeners;
    
    
  public:
    
    EventHostEx( Source & source_ ) :
      source( source_ )
    {}
    
    
    /**
     * Adds the given listener which must be non-null. The listener will be
     * added to the end of the list.
     */
    void addListener( Listener * listener )
    {
      assert( listener );
      listeners.push_front( listener );
    }
    
    /**
     * Removes the last inserted occurence of the given listener (the order
     * becomes significant if the event system is used as a filter chain). It
     * is an error to remove a non-existing listener.
     */
    void removeListener( Listener * listener )
    {
      typename listeners_t::iterator i = listeners.find( listener );
      assert( i != listeners.end() );
      listeners.erase( i );
    }
    
    /**
     * Delivers the event to each listener (in same order as they were
     * added).
     */
    template<class Event>
    void sendEvent( const Event & event ) const
    {
      // for_each( listeners )
      for( typename listeners_t::const_iterator i = listeners.begin() ;
           i != listeners.end() ; ++i ) {
        // do
        (*i)->processEvent( event, source );
      }
    }
    
  };
  
  
  
  
  /* ==== the old classes ==== */
  
  
  
  
  /* Pre-defined event classes */
  
  
  /**
   * @class lifespace::Event
   * @ingroup Utility
   *
   * @brief
   * A simple event with templated id and data fields.
   *
   * @deprecated
   * Will be replaced with an exception-like eventclass hierarchy.
   */
  template<typename IdType, typename DataType>
  class Event
  {
  public:
    IdType id;
    DataType data;
  };


  /**
   * @class lifespace::MapEvent
   * @ingroup Utility
   *
   * @brief
   * An event with multiple templated data fields, retrievable by unique ids.
   */
  template<typename IdType, typename DataType>
  class MapEvent
  {
  public:
    std::map<IdType, DataType> data;
  };




  /* Actual event system */


  /**
   * @class lifespace::EventListener
   * @ingroup Utility
   *
   * @brief
   * Class for receiving events.
   */
  template<typename Event>
  class EventListener
  {
  public:
    virtual ~EventListener() {}
    virtual void processEvent( const Event * event ) = 0;
  };




  /**
   * @class lifespace::EventHost
   * @ingroup Utility
   *
   * Class for delivering events.
   */
  template<typename Event>
  class EventHost
  {
    typedef std::list<EventListener<Event> *> listeners_t;
    listeners_t listeners;   // stored as reversed!
  
  
  public:
    /**
     * Adds the given listener which must be non-null. The listener will be
     * added to the end of the list.
     */
    void addListener( EventListener<Event> * listener )
    {
      assert( listener );
      listeners.push_front( listener );
    }
  
    /**
     * Removes the last inserted occurence of the given listener (the order
     * becomes significant if the event system is used as a filter chain). It
     * is an error to remove a non-existing listener.
     */
    void removeListener( EventListener<Event> * listener )
    {
      typename listeners_t::iterator i =
        std::find( listeners.begin(), listeners.end(), listener );
      assert( i != listeners.end() );
      listeners.erase( i );
    }
  
    /**
     * Delivers the event to each listener (in same order as they were
     * added). Listeners are allowed to remove themselves (but no other
     * listeners!) from this eventhost during event processing.
     */
    void sendEvent( const Event * event ) const
    {
      typename listeners_t::const_reverse_iterator i;
      
      // this works even if a listener deletes itself, because the reverse
      // iterator is actually based on the next entry
      for( i = listeners.rbegin() ; i != listeners.rend() ; i++ ) {
        (*i)->processEvent( event );
      }
    }
  };
  
  
  
  
}   /* namespace lifespace */


#endif   /* LS_U_EVENT_HPP */
