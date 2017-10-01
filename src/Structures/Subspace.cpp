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
 * @file Subspace.cpp
 *
 * Implementations for the Subspace class.
 */
#include "Subspace.hpp"
#include "Locator.hpp"
#include "ODEWorld.hpp"
#include "../Graphics/Environment.hpp"
#include "../Graphics/Visual.hpp"
#include "../Graphics/Viewport.hpp"
#include "../Integrators/Integrator.hpp"
#include "../Integrators/BasicIntegrator.hpp"
using namespace lifespace;

#include <list>
using std::list;

#include <algorithm>
using std::for_each;

#include <functional>
using std::mem_fun;

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;




Subspace::Params::Params() :
  objectParams(),
  environment( shared_ptr<Environment>( new Environment ) ),
  integrator( shared_ptr<Integrator>( new BasicIntegrator ) ),
  selfCollide( true )
{}


Subspace::Params::Params( const Object::Params & objectParams_ ) :
  objectParams( objectParams_ ),
  environment( shared_ptr<Environment>( new Environment ) ),
  integrator( shared_ptr<Integrator>( new BasicIntegrator ) ),
  selfCollide( true )
{}


Subspace::Params::Params( const Object::Params & objectParams_,
                          boost::shared_ptr<Environment> environment_,
                          boost::shared_ptr<Integrator> integrator_ ) :
  objectParams( objectParams_ ),
  environment( environment_ ),
  integrator( integrator_ ),
  selfCollide( true )
{}


Subspace::Params::Params( const Object::Params & objectParams_,
                          bool selfCollide_ ) :
  objectParams( objectParams_ ),
  environment( shared_ptr<Environment>( new Environment ) ),
  integrator( shared_ptr<Integrator>( new BasicIntegrator ) ),
  selfCollide( selfCollide_ )
{}


Subspace::Params::Params( const Object::Params & objectParams_,
                          boost::shared_ptr<Environment> environment_,
                          boost::shared_ptr<Integrator> integrator_,
                          bool selfCollide_ ) :
  objectParams( objectParams_ ),
  environment( environment_ ),
  integrator( integrator_ ),
  selfCollide( selfCollide_ )
{}


Subspace::Subspace( const Subspace::Params & params ) :
  Object( params.objectParams ),
  environment( params.environment ),
  integrator( params.integrator ),
  selfCollide( params.selfCollide )
{
  assert_user( selfCollide == true,
               "Disabling self-collide is not yet implemented, so currently "
               "the selfCollide flag must always be set true!" );
}


Subspace::~Subspace()
{
  // deactivate self and all contained Objects if in active state. Note that
  // activation is currently checked from the status of isLockedToHostSpace()!
  if( isLockedToHostSpace() ) ODEWorld::Activate( this, 0 );
  
  assert( objects.empty() );
}




bool Subspace::transformToWorldCoordinates( Locator & target ) const
{
  if( !hostSpace ) return false;
  
  if( locator && locator->getDirectWorldLocator() ) {
    locator->getDirectWorldLocator()->transform( target );
    return true;
  } else {
    if( locator ) locator->transform( target );
    return hostSpace->transformToWorldCoordinates( target );
  }
}


bool Subspace::transformToWorldCoordinates( Vector & target ) const
{
  if( !hostSpace ) return false;
  
  if( locator && locator->getDirectWorldLocator() ) {
    locator->getDirectWorldLocator()->transform( target );
    return true;
  } else {
    if( locator ) locator->transform( target );
    return hostSpace->transformToWorldCoordinates( target );
  }
}


bool Subspace::transformToWorldCoordinates( Matrix & target ) const
{
  if( !hostSpace ) return false;
  
  if( locator && locator->getDirectWorldLocator() ) {
    locator->getDirectWorldLocator()->transform( target );
    return true;
  } else {
    if( locator ) locator->transform( target );
    return hostSpace->transformToWorldCoordinates( target );
  }
}


bool Subspace::transformToSubspaceCoordinates( const Subspace * subspace,
                                               Locator & target ) const
{
  if( this == subspace ) return true;
  if( !hostSpace ) return false;
  
  // directworldlocators could be used also here as a shortcut..
  if( locator ) locator->transform( target );
  return hostSpace->transformToSubspaceCoordinates( subspace, target );
}




void Subspace::localPrepare( real dt )
{
  if( environment ) environment->prepare( dt );
  if( integrator ) integrator->prepare( objects, dt );
}


void Subspace::localStep()
{
  if( environment ) environment->step();
  if( integrator ) integrator->step( objects );
}




void Subspace::prepare( real dt )
{
  localPrepare( dt );
  Object::prepare( dt );
}


void Subspace::step()
{
  localStep();
  Object::step();
}
