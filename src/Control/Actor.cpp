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
 * @file Actor.cpp
 *
 * Implementation for the Actor base class.
 */
#include "../types.hpp"
#include "Actor.hpp"
#include <map>
#include <algorithm>
#include <functional>
#include <cassert>
using namespace lifespace;




Actor::Control::Control( Control * proxyTarget_ ) :
  value( 0.0 ), mode( ProxyTarget ),
  proxyTarget( proxyTarget_ ), activeProxiesFromThis( 0 )
{}


Actor::Control::Control( Mode mode_ ) :
  value( 0.0 ), mode( mode_ ),
  proxyTarget( 0 ), activeProxiesFromThis( 0 )
{}


Actor::Control::~Control()
{
  if( proxyTarget ) {
    proxyTarget->activeProxiesFromThis--;
  } else {
    assert( activeProxiesFromThis == 0 );
  }
}




Actor::Control Actor::Control::makeProxy()
{
  if( proxyTarget ) {
    return proxyTarget->makeProxy();
  } else {
    activeProxiesFromThis++;
    return Actor::Control( this );
  }
}


void Actor::Control::setMode( Mode mode_ )
{ mode = mode_; }


void Actor::Control::use( real force )
{
  // forward call if is a proxy control
  if( proxyTarget ) {
    proxyTarget->use( force );
    return;
  }
  
  // mode?
  if( mode == PulseMode ) {
    // pulse
    value += force;
  } else if( mode == ContinuousMode ) {
    // continuous
    value = force;
  } else assert( false );
}


real Actor::Control::read() const
{
  // forward call if is a proxy control
  if( proxyTarget ) return proxyTarget->read();
  
  return value;
}


void Actor::Control::step( real dt )
{
  // no-op if is a proxy control
  if( proxyTarget ) return;

  if( mode == PulseMode ) value = 0.0;
}








/* -- Actor methods begin -- */




Actor::Actor( unsigned int controlCount, unsigned int sensorCount ) :
  dt( 0.0 )
{
  controls.resize( controlCount, Control() );
  sensors.resize( sensorCount, 0 );
}


Actor::~Actor() {}




unsigned int Actor::getControlCount() const
{ return controls.size(); }


unsigned int Actor::getSensorCount() const
{ return sensors.size(); }




void Actor::useControl( unsigned int id, real force )
{ controls.at(id).use( force ); }


real Actor::readControl( unsigned int id ) const
{ return controls.at(id).read(); }


Actor::Control Actor::makeControlProxy( unsigned int id )
{ return controls.at(id).makeProxy(); }


Actor::Sensor * Actor::getSensor( unsigned int id )
{ return sensors.at(id); }


real Actor::readSensor( unsigned int id ) const
{
  assert_user( sensors.at(id),
               "The sensor being read is not initialized!" );
  
  return (*sensors.at(id))();
}




void Actor::prepare( real dt_ )
{ dt = dt_; }


void Actor::step()
{
  for( controls_t::iterator i = controls.begin() ;
       i != controls.end() ; i++ ) {
    i->step( dt );
  }
}
