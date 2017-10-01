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
 * @file ODEBallConnector.hpp
 *
 * An ODE ball and socket connector, which locks the connectors to the same
 * position while allowing (optionally motored) rotation.
 */

/**
 * @class lifespace::ODEBallConnector
 * @ingroup Structures
 *
 * @brief
 * An ODE ball and socket connector, which locks the connectors to the same
 * position while allowing (optionally motored) rotation.
 *
 * This connector is an abstraction for the ODE ball and socket joint and
 * AMotor joint. At least one of the objects being connected must have an
 * ODELocator.
 *
 * @sa Connector, ODELocator
 */
#ifndef LS_S_ODEBALLCONNECTOR_HPP
#define LS_S_ODEBALLCONNECTOR_HPP


#include "../types.hpp"
#include "../Control/Actor.hpp"
#include "Connector.hpp"
#include "ODEMotorAxisParams.hpp"
#include "Vector.hpp"
#include "BasisMatrix.hpp"
#include "Object.hpp"
#include "ODELocator.hpp"
#include <ode/ode.h>
#include <ode/odecpp.h>

#include <boost/shared_ptr.hpp>




namespace lifespace {
  
  
  
  
  class ODEBallConnector :
    public Connector,
    public dBallJoint,
    public dAMotorJoint
  {
  public:
    
    /** All controls are continuous, see Actor::Control for details. */
    enum Controls {
      CTRL_MOTOR_TARGETVEL_X,
      CTRL_MOTOR_TARGETVEL_Y,
      CTRL_MOTOR_TARGETVEL_Z,
      CTRL_MOTOR_FORCEFACTOR_X,
      CTRL_MOTOR_FORCEFACTOR_Y,
      CTRL_MOTOR_FORCEFACTOR_Z,
      CTRL_TORQUE_X,
      CTRL_TORQUE_Y,
      CTRL_TORQUE_Z,
      ControlCount
    };
    
    /** Sensors of the connector. */
    enum Sensors {
      SENS_ANGLE_X,
      SENS_ANGLE_Y,
      SENS_ANGLE_Z,
      SENS_ANGLERATE_X,
      SENS_ANGLERATE_Y,
      SENS_ANGLERATE_Z,
      SensorCount
    };
    
    
  private:
    
    /** Params for all motor axes. */
    ODEMotorAxisParams motorAxes[3];
    
    /** Is motor control enabled? */
    bool isMotored;
    
    class AngleDataReader : public Actor::Sensor
    {
      ODEBallConnector * host;
      int order;
      int axis;
      bool active;
      
    public:
      AngleDataReader() : active( false ) {}
      AngleDataReader( ODEBallConnector * host_, int order_, int axis_ ) :
        host( host_ ), order( order_ ), axis( axis_ ), active( false ) {}
      
      void activate( bool newState ) { active = newState; }
      virtual real operator()();
    };
    
    /** Sensor readers. */
    AngleDataReader angleDataReaders[2][3];
    
    
    /** Initializes the controls and sensors. */
    void init();
    
    
  public:
    
    /**
     * Creates a new ball connector. The connector's orientation does not
     * affect the connection (but the target connector's orientation is still
     * aligned when connecting).
     */
    ODEBallConnector( const Connector & connector );
    
    /**
     * Creates a new motored ball connector. The motor axis will correspond to
     * the master connector's x, y and z axes. The motor is an abstraction of
     * the ODE AMotor joint and it operates in Euler mode: the x axis is locked
     * to the master object, the z axis is locked to the slave object and the y
     * axis is computed on each step to be perpendicular to the other
     * axes.
     * 
     * @warning
     * Remember that this mode causes a possibility of gimbal lock and
     * instability if the y axis is allowed to rotate to +90' or -90'. Consider
     * using joint stops on y axis to prevent this. See the ODE manual for more
     * information.
     */
    ODEBallConnector( const Connector & connector, 
                      const ODEMotorAxisParams & motorAxisParamsX,
                      const ODEMotorAxisParams & motorAxisParamsY,
                      const ODEMotorAxisParams & motorAxisParamsZ );
    
    
    virtual void connect( boost::shared_ptr<Connector> target,
                          Aligning aligning );
    
    virtual void disconnect();
    
    
    virtual void prepare( real dt );
    
  };
  
  
  
  
}   /* namespace lifespace */


#endif   /* LS_S_ODEBALLCONNECTOR_HPP */
