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
 * @file Actor.hpp
 *
 * The base class for entities that have some externally operable controls.
 */

/**
 * @class lifespace::Actor
 * @ingroup Control
 *
 * @brief
 * The base class for entities that have some externally operable controls.
 *
 * The external operator can be, for example, a Controller or a window's user
 * input stream. A controller could be implemented, for example, into a parent
 * container object in the world hierarchy. The actual control signal concept
 * is defined with the nested class Control. The Actor class is meant to be a
 * base class for individual actor classes, which define their actual controls
 * and the functionality behind them.
 *
 * You can define the controls with an enumeration and then insert them into
 * the controls map in your constructor:
 * @code
 *   enum Controls {
 *     CTRL_TURN,
 *     CTRL_WALK
 *   };
 *   
 *   void init()
 *   {
 *     controls[CTRL_TURN] = Control( Control::ContinuousMode );
 *     controls[CTRL_WALK] = Control( Control::ContinuousMode );
 *   }
 * @endcode
 * Then override the prepare() method and implement the actual functionality
 * there (read the controls and apply forces to the base Object,
 * etc.). Remember to finally call the underlying Actor::prepare(). A control
 * can also be forwarded directly to a control in another actor class with the
 * makeControlProxy() method.
 *
 * @par Usage limitations
 * When using a controller instead of a user input stream to control the actor,
 * then the controls should be used (with useControl()) during the prepare
 * pass, and the Actor::prepare() method should be called @em after the
 * controls have been set properly. To ensure this order, currently only parent
 * objects (in the world hierarchy) and the object itself are allowed to use
 * the controls during a prepare pass. The controls are not allowed to be used
 * at all during a step pass. From outside the prepare and step passes (from a
 * window callback function, for example) the controls can be used without any
 * limitations.
 *
 * @note
 * Remember: overriding versions of prepare() should call the underlying
 * prepare() after having finished their own actions!
 *
 * @todo
 * Find a way to combine controls and sensors of multiple actors in multiple
 * inheritance.
 *
 * @bug
 * Pulse controls will accumulate between repeated prepare() calls. Fix: get
 * rid of the possibility of repeated prepare() calls.
 *
 * @sa Actor::Control, Controller
 */
#ifndef LS_S_ACTOR_HPP
#define LS_S_ACTOR_HPP


#include "../types.hpp"
#include <map>
#include <vector>
#include <algorithm>
#include <functional>


namespace lifespace {
  
  
  
  
  class Actor
  {
    
  public:
    
    /**
     * Defines a single control.
     *
     * A control signal can be sent to a control with the use() method. The
     * signal can have a certain force (a real number), which affects the
     * control's value. The value can be queried anytime except during a step
     * pass (the preferred time is during a prepare pass).
     *
     * @par Pulse and continuous controls
     * Controls can be either pulse controls or continuous controls:
     *   - Pulse controls accumulate all control signals that come though the
     *     use() method, and reset to zero on each step() call. A "next item"
     *     control could be a pulse control, for example. Note that when
     *     applying impulse forces with pulse controls, the current timestep
     *     length must be taken into account in the implementation of the
     *     prepare() method. See the FloatingActor class for an example.
     *   - Continuous controls always keep the last control signal received
     *     through the use() method, and do not reset on step() calls. An
     *     "engine power" or "walk forward" control could be a continuous
     *     control, for example.
     */
    class Control
    {
    public:
      enum Mode { PulseMode, ContinuousMode, ProxyTarget };
      
    private:
      real value;
      Mode mode;
      Control * proxyTarget;
      int activeProxiesFromThis;
      
      /** Creates a proxy control for the given target control. */
      Control( Control * proxyTarget );
      
    public:
      
      /** Create a new control. Mode can be changed later with setMode(). */
      Control( Mode mode = ContinuousMode );
      
      ~Control();
      
      /**
       * Creates a proxy control that forwards all use and read actions to this
       * control (or directly to the final target of this if this is also a
       * proxy). Proxies can be made also from outside of Actors with the
       * Actor::makeControlProxy() method.
       *
       * step() calls are not forwarded by proxies.
       *
       * \par Example:
       * @code
       * controls[CTRL_RAISE_HEAD] =
       *   neck.headConnector.controls[CTRL_ADDTORQUE_X].makeProxy();
       * @endcode
       *
       * @sa Actor::makeControlProxy()
       */
      Control makeProxy();
      
      /** Set the operating mode of the control. */
      void setMode( Mode mode );
      
      void use( real force );
      
      real read() const;
      
      void step( real dt );
    };
    
    /** Defines a single sensor. */
    typedef generator<real> Sensor;
    
    
  private:
    
    real dt;
    
    
  protected:

    /** A mapping from control ids to the actual control objects. */
    typedef std::vector<Control> controls_t;
    
    /** A mapping from sensor ids to the actual sensor objects. */
    typedef std::vector<Sensor *> sensors_t;
    
    /**
     * The actor's controls. Fill this with the actual control ids mapped to an
     * instance of class Control, initialized with appropriate attributes. A
     * control can be forwarded to another Actor object (maybe to a higher
     * level representation of the same object) by creating a proxy control
     * with the makeControlProxy() method and inserting it into the other
     * Actor's control map.
     */
    controls_t controls;
    
    /**
     * The actor's sensors. Fill this with the actual sensor ids mapped to an
     * instance of class Sensor, initialized with appropriate attributes. Note
     * that this container contains only pointers to the actual sensors, and
     * these pointers can be freely copied to other Actor objects (maybe to
     * higher level representations of the same object) to achieve forwarded
     * sensors.
     */
    sensors_t sensors;
    
    
  public:
    
    /**
     * Creates a new Actor with the specified number of controls and sensors.
     * Controls are initialized as ContinuousMode controls. This can be changed
     * later with setMode(). Sensors will \em not be initialized and must be
     * initialized explicitly by storing a Sensor pointer into each entry in
     * the sensors array. Using them without initialization will fail an
     * assertion.
     *
     * Both the control and the sensor arrays can be resized after construction
     * (in some derived class, for example).
     */
    Actor( unsigned int controlCount = 0, unsigned int sensorCount = 0 );
    
    virtual ~Actor();
    
    
    /** Returns the number of controls associated with this actor. */
    unsigned int getControlCount() const;
    
    /** Returns the number of sensors associated with this actor. */
    unsigned int getSensorCount() const;
    
    
    /**
     * Sends the given control signal to the control identified by the id. It
     * is an error to try to use a non-existing control.
     *
     * @par Usage limitations
     * This method must not be called during a step pass. When calling it
     * during a prepare pass (recommended), only parent objects (in the world
     * hierarchy) and the object itself are allowed to use this
     * method. (outside the prepare and step passes it can be used freely,
     * though)
     *
     * @internal
     * @note
     * If access is needed also from other, non-parent objects, then
     * readControl could have a one cycle delay to keep the operations
     * order-independent. A doublebuffered value field would be the
     * implementation: useControl modifies the back buffer and the actor's
     * prepare uses the front buffer, and the controls' step switches the
     * buffers.
     */
    void useControl( unsigned int id, real force );
    
    /**
     * Returns the value of the control identified by the id. It is an error to
     * try to read a non-existing control.
     *
     * @par Usage limitations
     * This method must not be called during a step pass.
     */
    real readControl( unsigned int id ) const;
    
    /**
     * Creates and returns a control proxy for the control with the given id.
     */
    Control makeControlProxy( unsigned int id );
    
    
    /**
     * Returns a pointer to the sensor identified by the id. The pointer can be
     * directly used in another sensor map to achieve sensor forwarding. Note
     * however, that disabling a forwarded sensor would actually disable the
     * actual underlying sensor.
     */
    Sensor * getSensor( unsigned int id );
    
    /**
     * Returns the value of the sensor identified by the id. It is an error to
     * try to read a non-existing sensor.
     */
    real readSensor( unsigned int id ) const;
    
    
    /** */
    virtual void prepare( real dt );
    
    /** */
    virtual void step();
  };
  
  
  
  
}   /* namespace lifespace */


#endif   /* LS_S_ACTOR_HPP */
