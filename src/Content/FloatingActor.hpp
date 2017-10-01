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
 * @file FloatingActor.hpp
 *
 * The FloatingActor class defines an Actor, which is also an Object, and has
 * some common properties and controls of a freely movable physical entity with
 * mass and air resistance, etc.
 */

/**
 * @class lifespace::FloatingActor
 * @ingroup Content
 *
 * @brief
 * The FloatingActor class defines an Actor, which is also an Object, and has
 * some common properties and controls of a freely movable physical entity with
 * mass and air resistance, etc.
 *
 * For example, the associated Locator can be set to the targetLocator of a
 * Camera object, and that camera will then follow this FloatingActor. The
 * Actor interface could then be connected to a window's user input.
 *
 * The FloatingActor contains also an auto-roll feature, which tries to keep
 * the actor in "normal" roll angle (basis x-axis y-component is
 * minimized). Note that if using auto-roll with ODELocators, then the
 * locator's rotational drag (quadratic drag at least) should be set quite high
 * and the auto-roll force low to minimize oscillation.
 *
 * @if done_todos
 * @todo
 * done: Implement a Keymapper object to be used between a viewport and an
 * actor. It would translate the key codes to control enums.
 * @endif
 *
 * @if done_todos
 * @todo
 * done: A viewport could also have multiple actors connected, and nonmapped
 * commands would be just ignored by the Keymapper. This would also solve the
 * problem when not allowing undefined control commands.
 * @endif
 *
 * @if done_todos
 * @todo
 * Consider renaming and/or moving into the Control module.
 * @endif
 *
 * @todo
 * Try to remove the negative direction controls. (but first think how to
 * handle simultaneus conflicting keypresses originating from GLOWViewport)
 *
 * @todo
 * Change the auto-roll default value to false.
 *
 * @sa Actor, Object
 */
#ifndef LS_C_FLOATINGACTOR_HPP
#define LS_C_FLOATINGACTOR_HPP


#include "../types.hpp"
#include "../Control/Actor.hpp"
#include "../Structures/Object.hpp"


namespace lifespace {
  
  
  
  
  class FloatingActor :
    public Actor,
    public virtual Object
  {
    
    /** Defines the default auto-roll force. */
    static const float DEFAULT_AUTOROLL_FORCE;
    
    /* friction constants */
    static const real VEL_CONSTANT_FRICTION;
    static const real VEL_LINEAR_FRICTION;
    static const real ROT_CONSTANT_FRICTION;
    static const real ROT_LINEAR_FRICTION;
    
    bool autoRoll;
    float autoRollForce;
    bool basisOrthonormalization;
    
    
    /** */
    void init();
    
    /** */
    void applyAutoRoll();
    
    
  public:
    
    /**
     * Controls of the actor object. All SET controls are continuous (so they
     * stay active until you deactivate them). All controls with dimension
     * suffixes are guaranteed to stay in the current order so that you can use
     * offsetting to refer to the dimensions (CTRL_LOC_Y == CTRL_LOC_X + 1, for
     * example). Note that this does \em not apply to the POS and NEG suffixes!
     *
     * Linear force accumulator controls are continuous, but torque controls
     * operate in pulse mode, see prepare() for details.
     */
    enum Controls {
      
      /** Set this to 1.0 (or >0.5 to be exact) to lock the locator's location
          to the value of the CTRL_LOC_n controls. */
      CTRL_SETLOC,
      CTRL_LOC_X,   /**< - */
      CTRL_LOC_Y,   /**< - */
      CTRL_LOC_Z,   /**< - */
      
      /** Set this to 1.0 (or >0.5 to be exact) to lock the locator's
          orientation to the value of the CTRL_BASIS_nn controls. */
      CTRL_SETBASIS,
      CTRL_BASIS_XX,   /**< X axis, X element */
      CTRL_BASIS_XY,   /**< X axis, Y element */
      CTRL_BASIS_XZ,   /**< ... */
      CTRL_BASIS_YX,   /**< - */
      CTRL_BASIS_YY,   /**< - */
      CTRL_BASIS_YZ,   /**< - */
      CTRL_BASIS_ZX,   /**< - */
      CTRL_BASIS_ZY,   /**< - */
      CTRL_BASIS_ZZ,   /**< - */
      
      /** Set this to 1.0 (or >0.5 to be exact) to lock the locator's velocity
          to the value of the CTRL_VEL_n controls. */
      CTRL_SETVEL,
      CTRL_VEL_X,   /**< - */
      CTRL_VEL_Y,   /**< - */
      CTRL_VEL_Z,   /**< - */
      
      /** Set this to 1.0 (or >0.5 to be exact) to lock the locator's rotation
          to the value of the CTRL_ROTATION_n controls. */
      CTRL_SETROTATION,
      CTRL_ROTATION_X,   /**< - */
      CTRL_ROTATION_Y,   /**< - */
      CTRL_ROTATION_Z,   /**< - */
      
      /** Relative forces and torques to be added to the force accumulators. */
      CTRL_FORCEREL_X_POS,
      CTRL_FORCEREL_X_NEG,   /**< - */
      CTRL_FORCEREL_Y_POS,   /**< - */
      CTRL_FORCEREL_Y_NEG,   /**< - */
      CTRL_FORCEREL_Z_POS,   /**< - */
      CTRL_FORCEREL_Z_NEG,   /**< - */
      CTRL_TORQUEREL_X_POS,   /**< - */
      CTRL_TORQUEREL_X_NEG,   /**< - */
      CTRL_TORQUEREL_Y_POS,   /**< - */
      CTRL_TORQUEREL_Y_NEG,   /**< - */
      CTRL_TORQUEREL_Z_POS,   /**< - */
      CTRL_TORQUEREL_Z_NEG,   /**< - */
      
      ControlCount
    };
    
    /** No sensors. */
    enum Sensors {
      SensorCount
    };
    
    
    /**
     * Creates a new FloatingActor with an Object using an InertiaLocator and
     * rather high friction values defined in this class. Orthonormalization of
     * basis set with CTRL_BASIS_nn is enabled, see
     * setBasisOrthonormalization().
     */
    FloatingActor();
    
    /**
     * Creates a new FloatingActor with an Object constructed with the given
     * Object params. Orthonormalization of basis set with CTRL_BASIS_nn is
     * enabled, see setBasisOrthonormalization().
     */
    FloatingActor( const Object::Params & objectParams );
    
    
    /** */
    void setAutoRoll( bool state )
    { autoRoll = state; }
    
    /**
     * Sets the auto-roll force. Negative values cause the "normal" orientation
     * to be upside down. Too large values will cause instability. Meaningful
     * values are somewhere around 1.0.
     */
    void setAutoRollForce( float force )
    { autoRollForce = force; }
    
    /**
     * Sets whether the basis set with CTRL_BASIS_nn should be orthonormalized
     * when inserting it into the locator. Default value is true.
     */
    void setBasisOrthonormalization( bool orthonormalize )
    { basisOrthonormalization = orthonormalize; }
    
    
    /** */
    void localPrepare( real dt );
    
    /** */
    void localStep();
    
    /**
     * Transfers the control signals to the locator and applies auto-roll (if
     * enabled).
     *
     * Linear forces are not scaled by dt because they are considered to result
     * from a continuous force (the corresponding controls are also continuous
     * controls), whereas the torque is currently a pulse control and thus
     * scaled by dt (this is just to make it easier to use mouse motion to feed
     * these controls, and this might change in the future).
     */
    virtual void prepare( real dt );
    
    /** */
    virtual void step();
    
  };
  
  
  
  
}   /* namespace lifespace */


#endif   /* LS_C_FLOATINGACTOR_HPP */
