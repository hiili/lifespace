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
 * @file ODESliderConnector.hpp
 *
 * An ODE slider connector, which allows movement along the connector's z-axis.
 */

/**
 * @class lifespace::ODESliderConnector
 * @ingroup Structures
 *
 * @brief
 * An ODE slider connector, which allows movement along the connector's z-axis.
 *
 * This connector is an abstraction of the ODE slider joint. At least one of
 * the objects being connected must have an ODELocator.
 * 
 * @sa Connector, ODELocator
 */
#ifndef LS_S_ODESLIDERCONNECTOR_HPP
#define LS_S_ODESLIDERCONNECTOR_HPP


#include "../types.hpp"
#include "../Control/Actor.hpp"
#include "Connector.hpp"
#include "Vector.hpp"
#include "BasisMatrix.hpp"
#include "Object.hpp"
#include "ODELocator.hpp"
#include <ode/ode.h>
#include <ode/odecpp.h>

#include <boost/shared_ptr.hpp>




namespace lifespace {
  
  
  
  
  class ODESliderConnector :
    public Connector,
    public dSliderJoint
  {
  public:
    
    /** All controls are continuous, see Actor::Control for details. */
    enum Controls {
      CTRL_MOTOR_TARGETVEL,
      CTRL_MOTOR_FORCEFACTOR,
      CTRL_FORCE,
      ControlCount
    };
    
    /** Sensors of the connector. */
    enum Sensors {
      SENS_LENGTH,
      SENS_VELOCITY,
      SensorCount
    };
    
    
  private:
    
    /**
     * @name static properties
     * @{ */
    real maxMotorForce;
    real loStop, hiStop;
    real stopBounciness;
    /** @} */
    
    /**
     * @name sensor readers
     * @{ */
    mem_fun_generator<real, dReal, dSliderJoint> lengthReader;
    mem_fun_generator<real, dReal, dSliderJoint> velocityReader;
    /** @} */
    
    
    /** Initializes the controls and sensors. */
    void init();
    
    
  public:
    
    /**
     * Creates a new slider connector.
     *
     * The slider axis will be along negative z axis, with positive movement
     * pushing the target connector to negative z direction.
     *
     * Stops can be disabled by setting them to positive and negative infinity
     * (with the GNU INFINITY and -INFINITY macros, for example). Either or
     * both stops can be disabled individually.
     */
    ODESliderConnector( const Connector & connector,
                        real maxMotorForce,
                        real loStop, real hiStop,
                        real stopBounciness );
    
    
    virtual void connect( boost::shared_ptr<Connector> target,
                          Aligning aligning );
    
    virtual void disconnect();
    
    
    virtual void prepare( real dt );
    
  };
  
  
  
  
}   /* namespace lifespace */


#endif   /* LS_S_ODESLIDERCONNECTOR_HPP */
