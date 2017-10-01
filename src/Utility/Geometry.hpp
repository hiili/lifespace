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
 * @file Geometry.hpp
 *
 * This class defines the geometry of an object from the viewpoint of collision
 * detection.
 */

/**
 * @class lifespace::Geometry
 * @ingroup Utility
 *
 * @brief
 * This class defines the geometry of an object from the viewpoint of collision
 * detection.
 */
#ifndef LS_U_GEOMETRY_HPP
#define LS_U_GEOMETRY_HPP


#include "../types.hpp"
#include "Event.hpp"
#include <map>




namespace lifespace {
  
  
  /* forwards */
  class Object;
  class Contact;
  
  
  
  
  class Geometry
  {
    
  public:
    
    /** A mapping for active physical contacts between Geometry objects. The
        key type is the target Geometry's raw pointer, and the value type is a
        raw pointer to the Contact object representing the contact. The Contact
        objects are owned by the collision detector. */
    typedef std::map<const Geometry *, Contact *> contacts_t;
    
    
    /* events */
    
    struct GeometryEvent {};
    
    /** This event is sent when the destructor starts executing. */
    struct DestructorEvent : public GeometryEvent
    {};
    
    /** Sent when a new Contact is about to be added (this is sent \em before
        the addition takes place, and before argument validity checks). */
    struct AddContactEvent : public GeometryEvent
    {
      const Geometry * other;
      Contact * contact;
      
      AddContactEvent( const Geometry * other_,
                       Contact * contact_ ) :
        other( other_ ), contact( contact_ ) {}
    };
    
    /** Sent when a new Contact is about to be removed (this is sent \em before
        the removal takes place, and before argument validity checks). */
    struct RemoveContactEvent : public GeometryEvent
    {
      const Geometry * other;
      
      RemoveContactEvent( const Geometry * other_ ) :
        other( other_ ) {}
    };
    
    struct EventListener
    {
      virtual ~EventListener() {}
      virtual void processEvent( const DestructorEvent & event,
                                 Geometry & source ) {}
      virtual void processEvent( const AddContactEvent & event,
                                 Geometry & source ) {}
      virtual void processEvent( const RemoveContactEvent & event,
                                 Geometry & source ) {}
    };

    struct DestructorEventListener : public virtual EventListener
    {
      virtual ~DestructorEventListener() {}
      virtual void processEvent( const DestructorEvent & event,
                                 Geometry & source ) = 0;
    };
    struct AddContactEventListener : public virtual EventListener
    {
      virtual ~AddContactEventListener() {}
      virtual void processEvent( const AddContactEvent & event,
                                 Geometry & source ) = 0;
    };
    struct RemoveContactEventListener : public virtual EventListener
    {
      virtual ~RemoveContactEventListener() {}
      virtual void processEvent( const RemoveContactEvent & event,
                                 Geometry & source ) = 0;
    };
    
    
    
  private:
    
    /** The current host Object (a Geometry cannot belong to multiple Objects
        at the same time. */
    Object * hostObject;
    
    /** All currently active contacts (for objects which support collision
        detection). The Contact objects (the value types of the map) are owned
        by the collision detector. */
    contacts_t contacts;
    
    
    void setHostObject( Object * newHostObject )
    { hostObject = newHostObject; }
    
    
    /** Object needs access to the private setHostObject() method. */
    friend class Object;
    
    
  public:
    
    Geometry();
    
    /** Make the class polymorphic */
    virtual ~Geometry();
    
    
    /** Geometry events are sent through this. */
    EventHostEx<Geometry::EventListener, Geometry> events;
    
    
    /* accessors */
    
    /** Returns a pointer to the Object where the Locator has been inserted
        (null if not inserted). */
    const Object * getHostObject() const
    { return hostObject; }
    Object * getHostObject()
    { return hostObject; }
    
    const contacts_t & getContacts() const
    { return contacts; }
    
    contacts_t & getContacts()
    { return contacts; }
    
    
    /* operations */
    
    void addContact( const Geometry * other, Contact * contact );
    
    void removeContact( const Geometry * other );
    
  };




}   /* namespace lifespace */



#endif   /* LS_T_GEOMETRY_HPP */
