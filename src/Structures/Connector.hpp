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
 * @file Connector.hpp
 *
 * Defines a connection point in an Object for Object-Object joints (only ODE
 * joints are implemented and thus at least one of the objects being connected
 * must have an ODELocator).
 */

/**
 * @class lifespace::Connector
 * @ingroup Structures
 *
 * @brief
 * Defines a connection point in an Object for Object-Object joints (only ODE
 * joints are implemented and thus at least one of the objects being connected
 * must have an ODELocator).
 *
 * Connectors are inherited from BasicLocator and Actor classes. The
 * BasicLocator's location and orientation define the connection point's static
 * location and resting angle/state relative to the host Object. The connector
 * itself will never move or rotate due to forces exerted by attached
 * joints. The joint motors and current state can be accessed through the Actor
 * interface: motors are shown as controls and the current state as sensors in
 * the interface.
 *
 * Two connectors can be connected with the connect() method. A connector can
 * be connected only to a single target connector at a time. When connecting,
 * the connector whose connect() method was called becomes the master connector
 * of the connection, and the other becomes the slave connector. Only the
 * master's actor interface is usable, and only it can be used to
 * disconnect(). The slave can be of any connector type (including this
 * Connector base class): only its BasicLocator part is used.
 *
 * Cyclic connections are not explicitly forbidden, but may cause more or less
 * problems depending on the particular implementation in the underlying
 * physics engine. The Connector class will just forcefully relocate the
 * objects upon connection so that the involved connectors get aligned, but
 * this will tear all existing conflicting connections to illegal states and it
 * is the physics engine's job to correct the situation. The result depends on
 * the physics engine, but at least ODE v0.5 seems to solve such situations
 * quite nicely.
 *
 * @if done_todos
 * @todo
 * done: Add a flag to allow "smooth-connecting" connectors (so that they slide
 * with ERP to alignment instead of warping to the final position).
 * @endif
 *
 * @todo
 * Create a general slaveConnector which doesn't have any specific connector
 * type (it acts only as a connect point). Or allow this class to be used as
 * such directly!
 *
 * @todo
 * Add a flag to select if the target connector should be flipped or not when
 * connecting.
 *
 * @todo
 * ODEConnector-specific: implement connecting to the static background
 * (non-ODELocator in one of the objects).
 *
 * @bug
 * This class can be currently instantiated and also defined as Master or
 * Any. However, connecting \em from this connector (so that it becomes master)
 * will not actually connect anything.
 */
#ifndef LS_S_CONNECTOR_HPP
#define LS_S_CONNECTOR_HPP


#include "../types.hpp"
#include "../Control/Actor.hpp"
#include "Vector.hpp"
#include "BasisMatrix.hpp"
#include "Object.hpp"
#include "BasicLocator.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>




namespace lifespace {
  
  
  
  
  class Connector :
    public BasicLocator,
    public Actor,
    public boost::enable_shared_from_this<Connector>
  {
  public:
    
    /** Defines the connector's possible roles in a connection. */
    enum Role { Master, Slave, Any, _Invalid };
    
    /** Defines which one, or neither, of the connectors should be moved to get
        them aligned. */
    enum Aligning { AlignMaster, AlignSlave, DontAlign };
    
    
  private:
    
    /**
     * @name static properties
     * @{ */
    Object & hostObject;
    Role allowedRole;
    Role activeRole;
    Vector flipAxis;
    bool inhibitCollisions;
    /** @} */
    
    BasicLocator * locationSnapshot;
    
    
  protected:
    
    boost::shared_ptr<Connector> targetConnector;
    
    /**
     * Stores either Object's location internally and returns an aligning mode
     * which will move the stored Object.
     *
     * @sa RestoreLocation()
     */
    Aligning snapshotEitherLocation( const Connector & master,
                                     const Connector & slave );
    
    /**
     * Restores the location stored with snapshotEitherLocation() and discards
     * the internal copy of the location.
     *
     * @param aligning   This must be the same that was returned by
     *                   snapshotEitherLocation()!
     */
    void restoreLocation( Connector & master, Connector & slave,
                          Aligning aligning );
    
    
  public:
    
    /**
     * Creates a connection point to the host Object.
     *
     * The connector must still be inserted to the host object's connector map
     * before it can be used.
     *
     * @param hostObject           Reference to the host Object.
     * @param role                 Allowed role (Master, Slave or Any)
     * @param locator              The connection point's relative location
     *                             within the host Object. Note that a copy of
     *                             this Locator is taken, rather than storing a
     *                             pointer to the given locator.
     * @param inhibitCollisions    Should this connector inhibit detection of
     *                             collisions between the connected Objects.
     */
    Connector( Object & hostObject, Role role,
               const BasicLocator & locator,
               bool inhibitCollisions = true );
    
    /** Copies the connector parameters \em and the host object, but not any
        active connections. */
    Connector( const Connector & other );
    
    /** */
    virtual ~Connector();
    
    /** Copies the connector parameters, but not any active connections nor the
        host Object reference. */
    Connector & operator=( const Connector & other );
    
    
    const Object & getHostObject() const
    { return hostObject; }
    
    Object & getHostObject()
    { return hostObject; }

    boost::shared_ptr<const Connector> getTargetConnector() const
    { return targetConnector; }
    
    boost::shared_ptr<Connector> getTargetConnector()
    { return targetConnector; }
    
    bool isConnected() const
    { return targetConnector ? true : false; }
    
    bool isConnectedAndMaster() const
    { return targetConnector && activeRole == Master; }
    
    /** Does this connector inhibit detection of collisions between the
        connected Objects. */
    bool doesInhibitCollisions() const
    { return inhibitCollisions; }
    
    
    /** Sets whether this connector should inhibit detection of collisions
        between the connected Objects. */
    void setInhibitCollisions( bool inhibitCollisions_ )
    { inhibitCollisions = inhibitCollisions_; }
    
    
    /**
     * Connects this connector to the target connector.
     *
     * This connector becomes the master and the targetConnector becomes the
     * slave connector: all motor forces and stop params are taken from the
     * master, only the master can apply forces and control the joint motor
     * (through the Actor interface), and only the master can be used to
     * disconnect().
     *
     * This connector's role must be either Master or Any, and the target must
     * be either Slave or Any. One of the connectors (and its host Object with
     * it) is aligned with the other connector and then flipped 180' along the
     * \em other connector's flip axis (flipAxis, which defaults to the
     * relative y axis and cannot be changed currently). The aligned side can
     * be selected with the \c aligning parameter.
     *
     * After connection, all sensors in the master connector get activated and
     * can be used to get information from the connection's state. Sensors of
     * an unconnected connector or the slave connector will always produce zero
     * value when queried.
     *
     * @param target   The target Connector for the connection
     */
    virtual void connect( boost::shared_ptr<Connector> target,
                          Aligning aligning = AlignSlave );
    
    /**
     * Disconnects the connector from the current target (must be connected).
     */
    virtual void disconnect();
    
    
    /**
     * Activates (or deactivates) force feedback for the connector.
     *
     * 
     */
    //void activateForceFeedback( bool state );
    
    
    /* methods from BasicLocator */
    
    /** Returns a deep copy of the locator representing the connector's
        relative location within the host Object. */
    virtual Locator * clone() const
    { return new Connector( *this ); }
    
    
    /* methods from BasicLocator and Actor */
    
    virtual void prepare( real dt )
    {
      BasicLocator::prepare( dt );
      Actor::prepare( dt );
    }
    
    virtual void step()
    {
      BasicLocator::step();
      Actor::step();
    }
    
  };
  
  
  
  
}   /* namespace lifespace */


#endif   /* LS_S_CONNECTOR_HPP */
