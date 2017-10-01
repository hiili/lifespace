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
 * @file Object.hpp
 *
 * The base class for all entities in the world hierarchy.
 */

/**
 * @class lifespace::Object
 * @ingroup Structures
 *
 * @brief
 * The base class for all entities in the Lifespace's world hierarchy.
 *
 * The main components of an Object are the Visual, Locator and Geometry
 * objects, which are contained as composite members and are all optional. A
 * Visual defines visual properties to be used by a graphics renderer
 * (OpenGLRenderer, for example), thus making this object visible in the
 * world. A Locator defines the object's position, orientation, speed, mass
 * etc.. A Geometry defines the object's shape and attributes from the
 * viewpoint of collision detection. The object has also a Connector container,
 * which can be filled with the joint connection points of the object.
 *
 * The Object itself does not contain any actual data or properties: its main
 * function is to glue together other parts and relate them to each
 * other. Therefore making a copy of an Object is not very meaningful operation
 * (and would be ambiguous), so the class has been made noncopyable.
 *
 * \par Object events
 *
 * The class contains also an EventHost (public member field \c events) for
 * some simple event dispatch and callback registration functionality.
 * Currently supported events:
 *   - OE_OBJECT_DYING: Is sent when the object is being deleted. It is
 *     guaranteed that no more messages will be sent from this object after
 *     this. A pointer to this object is included in the message (the 'source'
 *     field). Note that this message is sent from the Object's destructor and
       the object is already under deletion: all derived parts are probably
       already destructed at this point!
 *   - OE_LOCATOR_CHANGING: The locator is being added/removed/replaced.
 *   - OE_VISUAL_CHANGING: The visual is being added/removed/replaced.
 *   - OE_GEOMETRY_CHANGING: The geometry is being added/removed/replaced.
 *   - OE_HOSTSPACE_CHANGING: The object is being (dis)connected to a host
 *     subspace. Either the object's current hostSpace or the new hostSpace is
 *     always null.
 *
 * The ObjectEventData struct passed along with the OE_*_CHANGING events will
 * contain a pointer to the source Object (which still has the old target
 * object) and a pointer to the new composite object that is being inserted to
 * the object.
 *
 * @note
 * The OE_*_CHANGING events are sent even if the corresponding target is not
 * really changing: for example, calling setLocator() with the current locator
 * \em will trigger the event. This can be used to force a re-scan of the
 * target by an attached renderer.
 *
 * \par Virtual inheritance
 *
 * All classes inheriting from Object (Subspace and Camera, for example)
 * inherit it as a virtual base class. In practice, this means three things:
 * (\b note: only the first has to be considered if multiple inheritance is not
 * used!)
 *   - Whenever initializing a derived object, the virtual base Object can be
 *     also constructed explicitly at the "most derived" class, by supplying an
 *     instance of Object::Params to Object in the initializer list (it \em
 *     must to be initialized this way if a non-empty base object is wanted,
 *     initialization from intermediate classes will have no effect).
 *   - prepare() and step() \em must be overrided to forward the calls to all
 *     base classes, and it gets a bit complicated when using multiple
 *     inheritance with a common base Object: The base implementations must
 *     always be called at the end of the overriding implementation. Now,
 *     instead of directly calling the overridden implementation (which one?),
 *     one must call the "local" implementations of the corresponding methods
 *     in all intermediate base classes (Subspace::localPrepare(), for
 *     example), \em and then finally the underlying Object's implementation
 *     (Object::prepare(), for example).
 *     @code
 *       class ActorSubspace :
 *         public Actor,
 *         public Subspace
 *       {
 *         ...
 *         virtual void prepare( dt )
 *         {
 *           <overriding implementation goes here>
 *           ...
 *           Actor::localPrepare( dt );
 *           Subspace::localPrepare( dt );
 *           Object::prepare( dt );
 *         }
 *     @endcode
 *     (this will be simplified in the future)
 *   - Remember to use \c dynamic_cast when downcasting.
 *
 * @if done_todos
 * @todo
 * done: Consider removing the OE_HOSTSPACE_CHANGED object event.
 * @endif
 *
 * @todo
 * Add onPrepare and onStep eventhosts (would send begin and end events, or
 * just end events).
 *
 * @todo
 * Protect the prepare and step methods from user and create own corresponding
 * methods for the user (userPrepare() or customPrepare(), for example).
 *
 * @todo
 * Remove the default constructor to avoid accidents with the virtual
 * inheritance (passing an empty Params object could be used to
 * default-construct an Object explicitly).
 */
#ifndef LS_S_OBJECT_HPP
#define LS_S_OBJECT_HPP


#include "../Utility/Event.hpp"
#include "../types.hpp"

#include <list>
#include <string>

#include <stdlib.h>

#include <GL/gl.h>

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>




namespace lifespace {
  
  
  
  
  /* forwards */
  class Visual;
  class Locator;
  class Geometry;
  class Subspace;
  class World;
  class Connector;
  
  
  
  
  class Object :
    public boost::enable_shared_from_this<Object>,
    private boost::noncopyable
  {
    
  public:
    
    /** A mapping from connector ids to the actual connector objects. */
    typedef std::map< unsigned int, boost::shared_ptr<Connector> >
    connectors_t;
    
    
  private:
    
    boost::shared_ptr<Locator> locator;
    boost::shared_ptr<Visual> visual;
    boost::shared_ptr<Geometry> geometry;
    Subspace * hostSpace;
    int lockedToHostSpace;
    std::string name;
    
    
    /**
     * Sets the object's current hostspace pointer, or marks the object as
     * disconnected (if a null pointer is given).
     * 
     * It is an error to disconnect an object twice, to set the hostspace of an
     * already connected object, or to disconnect an object that is locked to
     * its hostspace. These are assertion checked.
     *
     * Note that this does not actually insert the object to the given Subspace
     * (and therefore shouldn't be called directly): this must be done with the
     * Subspace::addObject() method, which then in turn calls this method.
     */
    void setHostSpace( Subspace * newHostSpace );
    
    /** Subspace::addObject() and Subspace::removeObject() need access to the
        private setHostSpace() method. */
    friend class Subspace;
    
    
  protected:
    
    /**
     * The object's connectors. Fill this with the actual connector ids mapped
     * to an instance of class Connector, initialized with appropriate
     * attributes. Connectors cannot be currently forwarded to other Objects,
     * although this can be implemented quite easily when needed.
     *
     * @internal
     * Connector forwarding could be done with a separate list for "native"
     * connectors, and this list would be used when propagating prepare() and
     * step() calls. The connectors_t's second type would be changed to
     * shared_ptr.
     *
     * @todo
     * Move this to the private section and add accessors to add and remove
     * Connectors.
     */
    connectors_t connectors;
    
    
  public:
    
    /** Event types originating from the Object's EventHost. */
    enum ObjectEvents {
      OE_OBJECT_DYING,
      OE_LOCATOR_CHANGING,
      OE_VISUAL_CHANGING,
      OE_GEOMETRY_CHANGING,
      OE_HOSTSPACE_CHANGING,
      OE_LOCATOR_MODIFIED,
      OE_VISUAL_MODIFIED,
      OE_GEOMETRY_MODIFIED,
    };
    
    /** ObjectEvent data. */
    struct ObjectEventData {
      Object * source;
      union {
        Locator * locator;
        Visual * visual;
        Geometry * geometry;
        Subspace * hostSpace;
      } changingTarget;
    };
    
    /** Event emitted by an Object. */
    typedef Event<ObjectEvents,ObjectEventData> ObjectEvent;
    
    /* constructors/destructors/etc */
    
    /**
     * Object constructor params.
     *
     * The default constructor will create params for an empty Object. If raw
     * pointers are given, they are first wrapped inside shared_ptr objects,
     * thus effectively taking their ownership (so the caller should \em not
     * delete them at any point later after giving them to this constructor,
     * they will be destructed automatically when they are no more needed).
     */
    struct Params {
      // fields
      boost::shared_ptr<Locator> locator;
      boost::shared_ptr<Visual> visual;
      boost::shared_ptr<Geometry> geometry;
      // constructors
      Params();
      Params( Locator * locator,
              Visual * visual = 0,
              Geometry * geometry = 0 );
      Params( boost::shared_ptr<Locator> locator,
              boost::shared_ptr<Visual> visual
              = boost::shared_ptr<Visual>(),
              boost::shared_ptr<Geometry> geometry
              = boost::shared_ptr<Geometry>() );
    };
    
    /**
     * Constructs a new Object from the given params, or from defaults if the
     * params are omitted.
     *
     * @sa Object::Params
     */
    Object( const Object::Params & params = Object::Params() );
    
    /**
     * Releases all bound composite objects and destructs the object.
     *
     * @note
     * The object should not be attached to a host Subspace when calling this! 
     */
    virtual ~Object();
    
    enum LockActions { Unlock = -1, Lock = 1 };
    
    /** Locks or unlocks the object to the current hostspace. (Un)locking an
        already (un)locked object is an error. */
    void lockToHostSpace( enum LockActions action )
    {
      lockedToHostSpace += action;
      assert( lockedToHostSpace == 0 || lockedToHostSpace == 1 );
    }
    
    bool isLockedToHostSpace() const
    { return lockedToHostSpace > 0; }
    
    
    /** Object events. */
    EventHost<ObjectEvent> events;
    
    
    /* accessors (may return null!) */
    
    /** Returns the name of the object. */
    const std::string & getName() const
    { return name; }
    
    /** Returns the full (absolute) name of the object. The full name is
        composed from the object's name, preceded by the names of all host
        Subspaces and the root World separated by the '/' character. Example:
        "theworld/space2/object5"
    */
    std::string getFullName() const;
    
    /**
     * Returns a direct pointer to the internal Locator. Usually this locator
     * represents this Object's location relative to the host Subspace.
     *
     * @sa getSubspaceLocator(), getWorldLocator()
     */
    boost::shared_ptr<const Locator> getLocator() const
    { return locator; }
    
    boost::shared_ptr<const Visual> getVisual() const
    { return visual; }
    
    boost::shared_ptr<const Geometry> getGeometry() const
    { return geometry; }
    
    /**
     * Returns a direct pointer to the internal Locator. Usually this locator
     * represents this Object's location relative to the host Subspace.
     *
     * @sa getSubspaceLocator(), getWorldLocator()
     */
    boost::shared_ptr<Locator> getLocator()
    { return locator; }
    
    boost::shared_ptr<Visual> getVisual()
    { return visual; }
    
    boost::shared_ptr<Geometry> getGeometry()
    { return geometry; }
    
    /** Returns a pointer to the Subspace where the Object has been inserted
        (can be null). */
    const Subspace * getHostSpace() const
    { return hostSpace; }
    
    /** */
    Subspace * getHostSpace()
    { return hostSpace; }
    
    /** Returns a pointer to the host World, i.e.\ the root node of the
        subspace hierarchy where the Object has been inserted (can be null). */
    virtual const World * getHostWorld() const;
    
    /** */
    virtual World * getHostWorld();
    
    
    /**
     * Returns a locator representing the Object's relative location and
     * orientation within the given Subspace (can be null if not under the
     * given Subspace).
     *
     * Note that this operation might be quite expensive if the locator was not
     * precomputed and cached somewhere! The returned locator is most probably,
     * but not necessarily a temporary and is therefore defined as const.
     *
     * @sa getLocator(), getWorldLocator()
     */
    boost::shared_ptr<const Locator>
    getSubspaceLocator( const Subspace * subspace ) const;
    
    /**
     * Returns a locator representing the Object's absolute location and
     * orientation within the host World (can be null if not within a World).
     *
     * Note that this operation might be quite expensive if the locator was not
     * precomputed and cached somewhere! The returned locator is most probably,
     * but not necessarily a temporary and is therefore defined as const.
     *
     * @todo
     * Combine this and getSubspaceLocator() to getRelativeLocator(), with null
     * pointer representing the host world, etc.. (or not?)
     *
     * @sa getLocator(), getSubspaceLocator()
     */
    boost::shared_ptr<const Locator> getWorldLocator() const;
    
    /**
     * Find and return the connector with the given id. It is an error to try
     * to get a connector which is not present in the Object (is asserted in
     * debug mode).
     */
    boost::shared_ptr<const Connector> getConnector( unsigned int id ) const;
    boost::shared_ptr<Connector> getConnector( unsigned int id );
    
    /**
     * Return a direct reference to the internal connector container.
     */
    const connectors_t & getConnectors() const
    { return connectors; }
    connectors_t & getConnectors()
    { return connectors; }
    
    
    /* mutators */
    
    /** Sets the name of the object. Object names may not contain dots, colons
        nor slashes! */
    void setName( const std::string & newName );
    
    void setLocator( boost::shared_ptr<Locator> newLocator );
    
    void setVisual( boost::shared_ptr<Visual> newVisual );
    
    void setGeometry( boost::shared_ptr<Geometry> newGeometry );
    
    
    /* method dispatch to Locator */
    
    /**
     * Call point for the prepare pass, which is used to precompute the effect
     * of a timestep. By default the call is just forwarded to the internal
     * Locator, if available, and all connectors. The argument dt defines the
     * length of the timestep under preparation.
     *
     * This method may be called multiple times: the last call will always
     * overwrite the preparations done by previous calls. However, this \em
     * must be called at least once before the next step() call!
     *
     * This method can be overridden and used for activities which \em plan for
     * the next step or add some forces etc. to other objects, rather than
     * actually alter anything. Finally the underlying prepare() should be
     * called. The Locator's prepare() finally calculates the next state from
     * the current state and external forces (acceleration), which step() then
     * (maybe) applies.
     *
     * In short: A prepare method should not modify anything that might be used
     * by another prepare method. The results of the last step pass should be
     * used as the data source.
     *
     * If, however, you \em have to write execution order -sensitive prepare
     * methods, then try to keep the order-dependent objects on different
     * levels in the same branch of the world hierarchy to keep the actual
     * execution order predictable: the prepare method calls propagate in the
     * world hierarchy from top to down, and in insertion order within
     * container objects. This means that the order within containers is not
     * very well defined.
     *
     * @note
     * Remember: overriding versions of prepare() should call the underlying
     * prepare() after having finished their own actions!
     *
     * @sa step()
     */
    virtual void prepare( real dt );
    
    /**
     * Call point for the step pass, which is used to apply the previously
     * prepared timestep. prepare() must have been called at least once before
     * calling this!
     *
     * By default the call is just forwarded to the internal Locator, if
     * available, and to all connectors.
     *
     * Overriding versions of step() may apply modifications which somehow
     * reflect a concrete change in the state (resulting from time passing by
     * dt, as given in last prepare() call). A step method should not use any
     * data that might be modified by another step method. Instead the results
     * of the last prepare pass should be used as the data source. If you have
     * to execute possibly conflicting operations inside step methods, then
     * using an integrator (or implementing a specialized one) should be
     * considered to solve the result.
     *
     * @note
     * Remember: overriding versions of step() should call the underlying
     * step() after having finished their own actions!
     *
     * @sa prepare()
     */
    virtual void step();
  };
  
  
  
  
}   /* namespace lifespace */




#endif   /* LS_S_OBJECT_HPP */
