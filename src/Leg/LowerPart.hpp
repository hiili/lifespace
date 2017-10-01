/**
 * @file LowerPart.hpp
 *
 * <brief>
 */

/**
 * @class LowerPart
 *
 * @brief
 * <brief>
 */
#ifndef LOWERPART_HPP
#define LOWERPART_HPP


#include "../resources.hpp"

#include <lifespace/lifespace.hpp>




class LowerPart :
  public lifespace::Subspace,
  public lifespace::Actor
{
    
  /** joint parameters */
  static const lifespace::real MAX_MOTOR_FORCE, LO_STOP_ANGLE, HI_STOP_ANGLE,
    STOP_BOUNCINESS;
  
  
public:
  
  enum Connectors {
    CONN_BASE,
    CONN_BOTTOM,
    ConnectorCount
  };
  
  
  /* constructors/destructors/etc */
  
  LowerPart();
  
  
};




#endif   /* LOWERPART_HPP */
