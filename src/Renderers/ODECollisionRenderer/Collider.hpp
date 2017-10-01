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
 * @file Collider.hpp
 *
 * <brief>
 */

/**
 * @class lifespace::Collider
 * @ingroup ODECollisionRenderer
 *
 * @brief
 * <brief>
 *
 * @warning
 * Memory deallocation is not yet implemented correctly (is currently totally
 * screwed up).
 *
 * @ifdef done_todos
 * @todo
 * done: Rename to just Collider?
 * @endif
 *
 * @todo
 * Create intermediate collision spaces in initGeoms( Subspace & ).
 *
 * @todo
 * Simplify the AreCollisionsInhibited() static function by creating a inhibit
 * targets cache (a set container) to Object and maintain it from
 * Connector::connect() and Connector::disconnect() (remember to update the
 * relevant caches if a connected Connector's inhibit mode is changed).
 *
 * @bug
 * If a Geometry is deleted while it has active contacts, the Contact objects
 * will not be deleted and will contain a dangling pointer afterwards. This
 * cleanup could be made from the Geometry's destructor if Contact had an
 * iterator etc. pointing into the Collider's Contact list.
 */
#ifndef LS_R_COLLIDER_HPP
#define LS_R_COLLIDER_HPP


#include "../../types.hpp"
#include "../../Utility/shapes.hpp"

#include <ode/ode.h>
#include <ode/odecpp.h>
#include <ode/odecpp_collision.h>

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>




namespace lifespace {
  
  
  /* forwards */
  class ODECollisionRenderer;
  class ODEWorld;
  class ObjectNode;
  class Object;
  class Subspace;
  class Contact;
  
  
  
  
  class Collider
  {
    
    typedef std::list<ObjectNode *> objectnodes_t;
    typedef std::list<Contact *> contacts_t;
    
    /** Maximum number of contact joints between two geoms. */
    static const int CONTACTBUF_SIZE;
    
    static void ODECollisionCallback( void * data, dGeomID lhs, dGeomID rhs );
    
    
    ODEWorld & world;
    dSimpleSpace collisionSpace;
    dJointGroup jointGroup;
    boost::scoped_array<dContact> contactBuf;
    
    /** All existing ObjectNode objects of the target world. The nodes are
        owned by this container. */
    objectnodes_t objectNodes;
    
    /** This contains (and owns) \em all contacts that exist in the target
        world. */
    contacts_t allContacts;
    
    /** For quick wiping of old Contact objects. */
    bool currentFlipflop;
    
    
    void initGeom( dSpace & geomSpace, Object & object );
    void initGeoms( dSpace & geomSpace, Object & object );
    void initGeoms( dSpace & geomSpace, Subspace & subspace );
    
    
    friend class ObjectNode;
    
    
  public:
    
    /* constructors/destructors/etc */
    
    Collider( ODEWorld & world_ );
    
    ~Collider();
    
    
    /* accessors */
    
    
    /* operations */
    
    void wipeJoints();
    
    /** Currently no-op, so modifications to Objects' Geometry composites do
        not have any effect! */
    void syncGeomsWithWorld() {}
    
    void collide();
    
  };
  
  
  
  
}   /* namespace lifespace */




#endif   /* LS_R_COLLIDER_HPP */
