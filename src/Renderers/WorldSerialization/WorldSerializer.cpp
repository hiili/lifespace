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
 * @file WorldSerializer.cpp
 *
 * Implementations for the WorldSerializer class.
 */
#include "WorldSerializer.hpp"
#include "types.hpp"
#include "../../types.hpp"
#include "../../Graphics/types.hpp"
#include "../../Structures/Object.hpp"
#include "../../Structures/Subspace.hpp"
#include "../../Structures/Locator.hpp"
#include "../../Structures/MotionLocator.hpp"
#include "../../Structures/ODELocator.hpp"
#include "../../Control/Actor.hpp"
#include "../../Utility/Event.hpp"
using namespace lifespace;

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;
using boost::dynamic_pointer_cast;

#include <iostream>
using std::cout;
using std::endl;

#include <ostream>
using std::ostream;

#include <list>
using std::list;

#include <string>
using std::string;

#include <sstream>
using std::ostringstream;

#include <algorithm>
using std::find;




const int WorldSerializer::DataVersion = 1;




void WorldSerializer::addTargetStream( ostream * stream )
{
  assert_user( stream, "The provided stream pointer is null!" );
  
  // add to list
  streams.push_back( stream );
}


void WorldSerializer::removeTargetStream( ostream * stream )
{
  assert_user( stream, "The provided stream pointer is null!" );
  
  // check that the stream is on the list
  assert_user( find( streams.begin(), streams.end(), stream ) != streams.end(),
               "The provided stream is not on the target stream list!" );
  
  // remove from list
  streams.remove( stream );
}




void
WorldSerializer::addSourceObject( shared_ptr<Object> object,
                                  WorldSerialization::PropertyMask properties,
                                  bool recursive )
{
  assert_user( object, "The provided Object pointer is null!" );
  
  // init object data
  ObjectData data;
  data.object = object;
  data.properties = properties;
  data.fullName = object->getFullName();
  assert_internal( data.fullName.find_first_of( ".:" ) == string::npos );
  
  // add to list
  objects.push_back( data );
  
  // listen for modification events
  object->events.addListener( this );
  
  // recurse if recursion flag is true and is a Subspace
  shared_ptr<Subspace> subspace;
  if( recursive &&
      ( subspace = dynamic_pointer_cast<Subspace>( object ) )) {
    
    for( Subspace::objects_t::iterator i = subspace->getObjects().begin() ;
         i != subspace->getObjects().end() ; i++ ) {
      addSourceObject( *i, properties, recursive );
    }
    
  }
}


void WorldSerializer::removeSourceObject( shared_ptr<Object> object,
                                          bool recursive )
{
  assert_user( object, "The provided Object pointer is null!" );
  
  // stop listening for the object's events
  object->events.removeListener( this );
  
  // remove (first occurence) from list
  objects_t::iterator i;
  for( i = objects.begin() ; i != objects.end() && i->object != object ; i++ );
  if( i != objects.end() ) {
    // was found, remove it (do not check for duplicates!)
    objects.erase( i );
  } else {
    // assert that the object was on the list if recursive flag is not set
    if( !recursive ) assert_user( false, "The given Object was not found!" );
  }
  
  // recurse if recursion flag is true and is a Subspace
  shared_ptr<Subspace> subspace;
  if( recursive &&
      ( subspace = dynamic_pointer_cast<Subspace>( object ) ) ) {
    
    for( Subspace::objects_t::iterator i = subspace->getObjects().begin() ;
         i != subspace->getObjects().end() ; i++ ) {
      removeSourceObject( *i, recursive );
    }
    
  }
}




void WorldSerializer::processEvent( const Object::ObjectEvent * event )
{
      switch( event->id )
        {
        case Object::OE_OBJECT_DYING:
          assert_user( false,
                       "Objects added for serialization cannot "
                       "be currently deleted!" );
          break;
        case Object::OE_HOSTSPACE_CHANGING:
          // host subspace changes will go undetected..
          assert_user( false,
                       "Dynamic tracking not yet implemented!" );
          break;
        default:
          // no-op
          break;
        }
}


void WorldSerializer::processEvent( const GraphicsEvent * event )
{
  if( event->id == GE_TICK ) serialize();
}








/* -- serialization methods begin -- */




void WorldSerializer::serialize3dVector( ostream & streambuf,
                                         const Vector & vec )
{
  char buf[100];
  
  sprintf( buf, "%24.16e %24.16e %24.16e ", vec(0), vec(1), vec(2) );
  streambuf << buf;
}


void WorldSerializer::serializeLocator( ostream & streambuf,
                                        shared_ptr<const Locator> locator )
{
  assert_internal( locator );
  
  // position
  serialize3dVector( streambuf, locator->getLoc() );
  
  // orientation
  serialize3dVector( streambuf, locator->getBasis().getBasisVec( DIM_X ) );
  serialize3dVector( streambuf, locator->getBasis().getBasisVec( DIM_Y ) );
  serialize3dVector( streambuf, locator->getBasis().getBasisVec( DIM_Z ) );
  
}


void
WorldSerializer::serializeActorSensors( std::ostream & streambuf,
                                        boost::shared_ptr<const Actor> actor )
{
  assert_internal( actor );
  
  char buf[30];
  
  // sensor values
  unsigned int sensorCount = actor->getSensorCount();
  for( unsigned int sensor = 0 ; sensor < sensorCount ; sensor++ ) {
    
    sprintf( buf, "%24.16e ", actor->readSensor( sensor ) );
    streambuf << buf;
    
  }
  
}


void WorldSerializer::serialize()
{
  ostringstream datastream;
  
  // serialize
  serializeToStream( datastream );
  
  // iterate through target streams and write data to each
  for( streams_t::iterator stream = streams.begin() ;
       stream != streams.end() ; stream++ ) {
    
    // send data to the current stream
    **stream << datastream.str();
    
  }
  
}


void WorldSerializer::serializeToStream( ostream & stream )
{
  // write header
  stream << "======== WorldSerializer begin - version "
         << DataVersion << " ========" << endl;
  
  // iterate through objects
  for( objects_t::iterator object_i = objects.begin() ;
       object_i != objects.end() ; object_i++ ) {
    
    // write locator stuff
    if( object_i->properties & WorldSerialization::PROP_LOCATOR &&
        object_i->object->getLocator() ) {
      stream << object_i->fullName << ".locator: ";
      serializeLocator( stream, object_i->object->getLocator() );
      stream << endl;
    }

    // write actor stuff
    if( object_i->properties & WorldSerialization::PROP_ACTOR_SENSORS &&
        dynamic_pointer_cast<Actor>( object_i->object ) ) {
      stream << object_i->fullName << ".actor_sensors: ";
      serializeActorSensors( stream,
                             dynamic_pointer_cast<Actor>( object_i->object ) );
      stream << endl;
    }
    
  }
  
  // write trailer
  stream << "======== WorldSerializer end ========" << endl << endl;
}
