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
 * @file testActor.cpp
 *
 * testcases for lifespace::Actor.
 */
#include "lifespace/lifespace.hpp"
using namespace lifespace;

#include <iostream>
using std::cout;
using std::endl;

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;




class TestActor :
  public Actor,
  public Object
{
  
  
public:
  
  /** The actor's controls. */
  enum Controls {
    CTRL_STRAFE,
    CTRL_WALK,
    CTRL_NEXTITEM
  };
  
  TestActor() :
    Object()
  {
    // create the controls
    controls[CTRL_STRAFE]   = Control( Control::ContinuousMode );
    controls[CTRL_NEXTITEM] = Control( Control::PulseMode );
  }
  
  
  /** Implement the actual functionality behind the controls. */
  void prepare( real dt )
  {
    // no locators implemented yet..
    
    Actor::prepare( dt );
    Object::prepare( dt );
  }
  
  void step()
  {
    Actor::step();
    Object::step();
  }
};




bool testActor()
{
  TestActor actor;
  
  
  // prepare pass
  
  // call is propagated to the associated controller
  actor.useControl( TestActor::CTRL_NEXTITEM, 1.0 );
  actor.useControl( TestActor::CTRL_NEXTITEM, 1.0 );
  
  // call is propagated to the base classes
  actor.prepare( 1.0 );
  
  real acc_x = actor.readControl( TestActor::CTRL_NEXTITEM );
  /* actor.accelerateRel( Vector(acc_x, 0.0, 0.0) ); */
  if( acc_x != 2.0 ) return false;
  
  
  // step pass
  
  // call is propagated to the base classes
  actor.step();
  
  
  return true;
}
