/**
 * @file Leg.hpp
 *
 * <brief>
 */

/**
 * @class Leg
 *
 * @brief
 * <brief>
 */
#ifndef LEG_HPP
#define LEG_HPP


#include "UpperPart.hpp"
#include "LowerPart.hpp"
#include "EndEffector.hpp"
#include <lifespace/lifespace.hpp>
#include <boost/shared_ptr.hpp>




class Leg :
  public lifespace::Subspace,
  public lifespace::Actor
{
  
  boost::shared_ptr<UpperPart> upperPart;
  boost::shared_ptr<LowerPart> lowerPart;
  boost::shared_ptr<EndEffector> endEffector;
  
  
public:
  
  /* materials */
  
  static const boost::shared_ptr<lifespace::Material> MainVisMaterial;
  
  static const boost::shared_ptr<lifespace::CollisionMaterial>
  MainPhysMaterial;
  
  
  enum Connectors {
    CONN_BASE,
    ConnectorCount
  };
  
  enum Sensors {
    
    SENS_UPPERPART_ANGLE_X,
    SENS_UPPERPART_ANGLE_Y,
    
    SENS_LOWERPART_ANGLE,
    
    SENS_ENDEFFECTOR_ANGLE_X,
    SENS_ENDEFFECTOR_ANGLE_Y,
    SENS_ENDEFFECTOR_ANGLE_Z,
    
    SENS_UPPERPART_ANGLERATE_X,
    SENS_UPPERPART_ANGLERATE_Y,
    
    SENS_LOWERPART_ANGLERATE,
    
    SENS_ENDEFFECTOR_ANGLERATE_X,
    SENS_ENDEFFECTOR_ANGLERATE_Y,
    SENS_ENDEFFECTOR_ANGLERATE_Z,
    
    SensorCount
  };
  
  enum Controls {

    CTRL_UPPERPART_MOTOR_TARGETVEL_X,
    CTRL_UPPERPART_MOTOR_TARGETVEL_Y,

    CTRL_LOWERPART_MOTOR_TARGETVEL,

    CTRL_ENDEFFECTOR_MOTOR_TARGETVEL_X,
    CTRL_ENDEFFECTOR_MOTOR_TARGETVEL_Y,
    CTRL_ENDEFFECTOR_MOTOR_TARGETVEL_Z,

    CTRL_UPPERPART_MOTOR_FORCEFACTOR_X,
    CTRL_UPPERPART_MOTOR_FORCEFACTOR_Y,

    CTRL_LOWERPART_MOTOR_FORCEFACTOR,

    CTRL_ENDEFFECTOR_MOTOR_FORCEFACTOR_X,
    CTRL_ENDEFFECTOR_MOTOR_FORCEFACTOR_Y,
    CTRL_ENDEFFECTOR_MOTOR_FORCEFACTOR_Z,

    CTRL_UPPERPART_TORQUE_X,
    CTRL_UPPERPART_TORQUE_Y,

    CTRL_LOWERPART_TORQUE,

    CTRL_ENDEFFECTOR_TORQUE_X,
    CTRL_ENDEFFECTOR_TORQUE_Y,
    CTRL_ENDEFFECTOR_TORQUE_Z,

    ControlCount
  };
  
  
  /* constructors/destructors/etc */
  
  Leg();
  
  
  /* accessors */
  
  //...
  
  
  /* operations */
  
  /** Connect all parts. This should be called after the ODEWorld has been
      activated. */
  void connect();
  
};




#endif   /* LEG_HPP */
