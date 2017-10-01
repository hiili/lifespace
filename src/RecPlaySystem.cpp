/**
 * @file RecPlaySystem.cpp
 *
 * Implementations for the RecPlaySystem class.
 */
#include "RecPlaySystem.hpp"

#include <lifespace/lifespace.hpp>
using namespace lifespace;

#include <string>
using std::string;

#include <sstream>
using std::istringstream;
using std::ostringstream;

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;




RecPlaySystem::RecPlaySystem() {}




void RecPlaySystem::addObject( shared_ptr<Object> object,
                               WorldSerialization::PropertyMask properties,
                               bool recursive )
{
  serializer.addSourceObject( object, properties, recursive );
  deserializer.addTargetObject( object, properties, recursive );
}


void RecPlaySystem::removeObject( shared_ptr<Object> object,
                                  bool recursive )
{
  serializer.removeSourceObject( object, recursive );
  deserializer.removeTargetObject( object, recursive );
}




void RecPlaySystem::serializeState( unsigned long long t )
{
  ostringstream datastream;
  
  // serialize
  serializer.serializeToStream( datastream );
  
  // insert into allData (resize if needed)
  if( allData.size() <= t ) allData.resize( t + 1 );
  allData[t] = sptr( new string( datastream.str() ));
}


void RecPlaySystem::deserializeState( unsigned long long t )
{
  assert_user( allData.size() > t && allData[t],
               "No serialization data associated with the "
               "requested time index " << t << "!" );
  
  // retrieve content
  istringstream datastream( *allData[t] );
  
  // deserialize
  deserializer.deserializeFromStream( datastream );
}
