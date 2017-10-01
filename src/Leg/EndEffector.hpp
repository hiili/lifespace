/**
 * @file EndEffector.hpp
 *
 * <brief>
 */

/**
 * @class EndEffector
 *
 * @brief
 * <brief>
 */
#ifndef ENDEFFECTOR_HPP
#define ENDEFFECTOR_HPP


#include "../resources.hpp"

#include <lifespace/lifespace.hpp>




class EndEffector :
  public lifespace::Subspace,
  public lifespace::Actor
{
    
  /** joint parameters */
  static const lifespace::real MAX_MOTOR_FORCE_X, LO_STOP_ANGLE_X, HI_STOP_ANGLE_X,
    STOP_BOUNCINESS_X;
  static const lifespace::real MAX_MOTOR_FORCE_Y, LO_STOP_ANGLE_Y, HI_STOP_ANGLE_Y,
    STOP_BOUNCINESS_Y;
  static const lifespace::real MAX_MOTOR_FORCE_Z, LO_STOP_ANGLE_Z, HI_STOP_ANGLE_Z,
    STOP_BOUNCINESS_Z;
  
  
public:
  
  enum Connectors {
    CONN_BASE,
    ConnectorCount
  };
  
  
  /* constructors/destructors/etc */
  
  EndEffector();
  
  
};




#endif   /* ENDEFFECTOR_HPP */
