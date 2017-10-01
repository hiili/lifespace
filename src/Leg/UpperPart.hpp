/**
 * @file UpperPart.hpp
 *
 * <brief>
 */

/**
 * @class UpperPart
 *
 * @brief
 * <brief>
 */
#ifndef UPPERPART_HPP
#define UPPERPART_HPP


#include "../resources.hpp"

#include <lifespace/lifespace.hpp>




class UpperPart :
  public lifespace::Subspace,
  public lifespace::Actor
{
    
  /** joint parameters */
  static const lifespace::real MAX_MOTOR_FORCE_X, LO_STOP_ANGLE_X, HI_STOP_ANGLE_X,
    STOP_BOUNCINESS_X;
  static const lifespace::real MAX_MOTOR_FORCE_Y, LO_STOP_ANGLE_Y, HI_STOP_ANGLE_Y,
    STOP_BOUNCINESS_Y;
  
  
public:
  
  enum Connectors {
    CONN_BASE,
    CONN_BOTTOM,
    ConnectorCount
  };
  
  
  /* constructors/destructors/etc */
  
  UpperPart();
  
  
};




#endif   /* UPPERPART_HPP */
