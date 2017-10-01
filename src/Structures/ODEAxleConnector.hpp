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
 * @file ODEAxleConnector.hpp
 *
 * An ODE axle connector, which allows rotation about the connector's z-axis.
 */

/**
 * @class lifespace::ODEAxleConnector
 * @ingroup Structures
 *
 * @brief
 * An ODE axle connector, which allows rotation about the connector's z-axis.
 *
 * This connector is an abstraction of the ODE hinge joint. At least one of the
 * objects being connected must have an ODELocator.
 *
 * @sa Connector, ODELocator
 */
#ifndef LS_S_ODEAXLECONNECTOR_HPP
#define LS_S_ODEAXLECONNECTOR_HPP


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
  
  
  
  
  class ODEAxleConnector :
    public Connector,
    public dHingeJoint
  {
  public:
    
    /** All controls are continuous, see Actor::Control for details. */
    enum Controls {
      CTRL_MOTOR_TARGETVEL,
      CTRL_MOTOR_FORCEFACTOR,
      CTRL_TORQUE,
      ControlCount
    };
    
    /** Sensors of the connector. */
    enum Sensors {
      SENS_ANGLE,
      SENS_ANGLERATE,
      SensorCount
    };
    
    
  private:
    
    /**
     * @name static properties
     * @{ */
    real maxMotorForce;
    real loStopAngle, hiStopAngle;
    real stopBounciness;
    /** @} */
    
    /**
     * @name sensor readers
     * @{ */
    mem_fun_generator<real, dReal, dHingeJoint> angleReader;
    mem_fun_generator<real, dReal, dHingeJoint> angleRateReader;
    /** @} */
    
    
    /** Initializes the controls and sensors. */
    void init();
    
    
  public:
    
    /**
     * Creates a new axle connector.
     *
     * The rotation axis will be along negative z axis, with positive rotation
     * turning the positive y axis towards positive x axis (right-hand rule).
     *
     * Stops can be disabled by setting them to positive and negative infinity
     * (with the GNU INFINITY and -INFINITY macros, for example). Either or
     * both stops can be disabled individually. Stops are defined in radians.
     */
    ODEAxleConnector( const Connector & connector,
                      real maxMotorForce,
                      real loStopAngle, real hiStopAngle,
                      real stopBounciness );
    
    
    virtual void connect( boost::shared_ptr<Connector> target,
                          Aligning aligning );
    
    virtual void disconnect();
    
    
    virtual void prepare( real dt );
    
  };
  
  
  
  
}   /* namespace lifespace */


#endif   /* LS_S_ODEAXLECONNECTOR_HPP */
