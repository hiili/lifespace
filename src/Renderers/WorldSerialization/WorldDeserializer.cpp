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
 * @file WorldDeserializer.cpp
 *
 * Implementations for the WorldDeserializer class.
 */
#include "WorldDeserializer.hpp"
#include "types.hpp"
#include "../../types.hpp"
#include "../../Graphics/types.hpp"
#include "../../Structures/Vector.hpp"
#include "../../Structures/BasisMatrix.hpp"
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

#include <istream>
using std::istream;

#include <cstdio>
using std::sscanf;

#include <list>
using std::list;

#include <map>
using std::map;
using std::pair;

#include <string>
using std::string;
using std::getline;

#include <sstream>
using std::istringstream;

#include <algorithm>
using std::find;




const int WorldDeserializer::DataVersion = 1;




void WorldDeserializer::addSourceStream( istream * stream )
{
  assert_user( stream, "The provided stream pointer is null!" );
  
  // add to list
  streams.push_back( stream );
}


void WorldDeserializer::removeSourceStream( istream * stream )
{
  assert_user( stream, "The provided stream pointer is null!" );
  
  // check that the stream is on the list
  assert_user( find( streams.begin(), streams.end(), stream ) != streams.end(),
               "The provided stream is not on the source stream list!" );
  
  // remove from list
  streams.remove( stream );
}




void
WorldDeserializer::addTargetObject( shared_ptr<Object> object,
                                    WorldSerialization::PropertyMask properties,
                                    bool recursive )
{
  assert_user( object, "The provided Object pointer is null!" );
  
  // init object data
  ObjectData data;
  data.object = object;
  data.properties = properties;
  data.fullName = object->getFullName();
  
  // add to list
  pair<objects_t::iterator, bool> result =
    objects.insert( make_pair( data.fullName, data ) );
  assert_user( result.second,
               "A target object with the same absolute name '"
               << data.fullName << "' already exists!" );
  
  // listen for modification events
  object->events.addListener( this );
  
  // recurse if recursion flag is true and is a Subspace
  shared_ptr<Subspace> subspace;
  if( recursive &&
      ( subspace = dynamic_pointer_cast<Subspace>( object ) )) {
    
    for( Subspace::objects_t::iterator i = subspace->getObjects().begin() ;
         i != subspace->getObjects().end() ; i++ ) {
      addTargetObject( *i, properties, recursive );
    }
    
  }
}


void WorldDeserializer::removeTargetObject( shared_ptr<Object> object,
                                            bool recursive )
{
  assert_user( object, "The provided Object pointer is null!" );
  
  // stop listening for the object's events
  object->events.removeListener( this );
  
  // remove (first occurence) from list
  objects_t::iterator i;
  for( i = objects.begin() ;
       i != objects.end() && i->second.object != object ; i++ );
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
      removeTargetObject( *i, recursive );
    }
    
  }
}




void WorldDeserializer::processEvent( const Object::ObjectEvent * event )
{
  switch( event->id )
    {
    case Object::OE_OBJECT_DYING:
      assert_user( false,
                   "Objects added for deserialization cannot "
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


void WorldDeserializer::processEvent( const GraphicsEvent * event )
{
  if( event->id == GE_TICK ) deserialize();
}








/* -- deserialization methods begin -- */




Vector WorldDeserializer::deserialize3dVector( istream & streambuf )
{
  Vector result(3);
  
  streambuf >> result(0);
  streambuf >> result(1);
  streambuf >> result(2);
  
  return result;
}


void WorldDeserializer::deserializeLocator( istream & streambuf,
                                            shared_ptr<Locator> locator )
{
  assert_internal( locator );
  
  // position
  locator->setLoc( deserialize3dVector( streambuf ) );
  
  // orientation
  BasisMatrix basis;
  basis.getBasisVec( DIM_X ) = deserialize3dVector( streambuf );
  basis.getBasisVec( DIM_Y ) = deserialize3dVector( streambuf );
  basis.getBasisVec( DIM_Z ) = deserialize3dVector( streambuf );
  locator->setBasis( basis );
  
}


void
WorldDeserializer::deserializeActorSensors( std::istream & streambuf,
                                            boost::shared_ptr<Actor> actor )
{
  // TODO: implement
  /*
  assert_internal( actor );
  
  char buf[30];
  
  // sensor values
  unsigned int sensorCount = actor->getSensorCount();
  for( unsigned int sensor = 0 ; sensor < sensorCount ; sensor++ ) {
    
    sprintf( buf, "%24.16e ", actor->readSensor( sensor ) );
    streambuf << buf;
    
  }
  */
}


void WorldDeserializer::deserializeEntry( const string & entry )
{
  string objectFullName, propertyName, propertyData;
  WorldSerialization::PropertyMask property;
  
  // parse buffer contents (mörköparsintaa..)
  objectFullName = entry.substr( 0, entry.find( '.' ) );
  propertyName   = entry.substr( objectFullName.length() + 1,
                                 entry.find( ':',
                                             objectFullName.length() + 1 ) -
                                 (objectFullName.length() + 1) );
  propertyData   = entry.substr( objectFullName.length() + 1 +
                                 propertyName.length() + 2 );
  assert_user( WorldSerialization::PropertyName2Mask.find( propertyName )
               != WorldSerialization::PropertyName2Mask.end(),
               "Unrecognized property name '" << propertyName << "'!" );
  property = WorldSerialization::PropertyName2Mask[propertyName];
  
  // find target object
  objects_t::iterator object_i = objects.find( objectFullName );
  
  // return if target object not found
  if( object_i == objects.end() ) return;
  
  // return if the property contained in the current entry is not selected for
  // deserialization for this object
  if( object_i->second.properties & property == 0 ) return;
  
  // deserialize
  shared_ptr<Object> & object = object_i->second.object;
  istringstream propertyDataStream( propertyData );
  switch( property )
    {
    case WorldSerialization::PROP_LOCATOR:
      
      // locator
      if( object->getLocator() ) {
        deserializeLocator( propertyDataStream,
                            object->getLocator() );
      }
      break;
      
    case WorldSerialization::PROP_ACTOR_SENSORS:
      
      // actor
      if( dynamic_pointer_cast<Actor>( object ) ) {
        deserializeActorSensors( propertyDataStream,
                                 dynamic_pointer_cast<Actor>( object ) );
      }
      break;
    };
}


void WorldDeserializer::deserialize()
{
  // loop through all input streams and deserialize one serialization block
  // from each
  for( streams_t::iterator stream = streams.begin() ;
       stream != streams.end() ; stream++ ) {
    
    deserializeFromStream( **stream );
    
  }
  
}


void WorldDeserializer::deserializeFromStream( istream & stream )
{
  string buf;
    
  // scan for header
  int retcode, version;
  do {
    getline( stream, buf );
    retcode =
      sscanf( buf.c_str(),
              "======== WorldSerializer begin - version %d ========",
              &version );
  } while( retcode != 1 && stream );
  assert_user( stream,
               "No new serialization data block found from "
               "the current stream!" );
  
  // check version
  assert_user( version == DataVersion,
               "Version mismatch in a serialization data block!"
               " (should be " << DataVersion << ", is " << version << ")" );
  
  // deserialize until trailer encountered
  for( getline( stream, buf ) ;
       buf != "======== WorldSerializer end ========" ;
       getline( stream, buf ) ) {
    
    // deserialize
    deserializeEntry( buf );
    
  }
  
}
