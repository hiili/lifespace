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
 * @file WorldDeserializer.hpp
 *
 * Deserializes selected simulation world content from a stream.
 */

/**
 * @class lifespace::WorldDeserializer
 * @ingroup WorldSerialization
 *
 * @brief
 * Deserializes selected simulation world content from a stream.
 *
 * 
 *
 */
#ifndef LS_R_WORLDDESERIALIZER_HPP
#define LS_R_WORLDDESERIALIZER_HPP


#include "types.hpp"
#include "../../types.hpp"
#include "../../Graphics/types.hpp"
#include "../../Structures/Object.hpp"
#include "../../Structures/Subspace.hpp"
#include "../../Utility/Event.hpp"

#include <boost/shared_ptr.hpp>

#include <istream>
#include <sstream>
#include <list>
#include <map>
#include <string>




namespace lifespace {
  
  
  
  
  /* forwards */
  class Actor;
  
  
  
  
  class WorldDeserializer :
    public EventListener<Object::ObjectEvent>,
    public EventListener<GraphicsEvent>
  {
    
    /** Data version number */
    static const int DataVersion;
    
    /** Data for an individual deserialization target object. */
    struct ObjectData {
      
      /** The target object. */
      boost::shared_ptr<Object> object;
      
      /** Properties to be deserialized. */
      WorldSerialization::PropertyMask properties;
      
      /** The full (absolute) name of the object. */
      std::string fullName;
      
    };
    
    
    typedef std::map<std::string, ObjectData> objects_t;
    typedef std::list<std::istream *> streams_t;
    
    /** Target objects. */
    objects_t objects;
    
    /** Source streams. */
    streams_t streams;
    
    
    /** Deserialize a 3d vector into the given character buffer. */
    Vector deserialize3dVector( std::istream & streambuf );
    
    void deserializeLocator( std::istream & streambuf,
                             boost::shared_ptr<Locator> locator );
    
    void deserializeActorSensors( std::istream & streambuf,
                                  boost::shared_ptr<Actor> actor );
    
    void deserializeEntry( const std::string & entry );
    
      
  public:
    
    /* constructors/destructors/etc */
    
    /** */
    WorldDeserializer() {}
    
    
    /* accessors */
    
    /** Adds an istream to the list of streams that the deserialized data will be
        read from upon each deserialize() call. */
    void addSourceStream( std::istream * stream );
    
    /** Removes the given stream from the source stream list. */
    void removeSourceStream( std::istream * stream );
    
    /** Adds an Object to be deserialized on each deserialize() call. The full
        (absolute) name of the Object is constructed when this method is called
        and will not get updated if the Object's name or a name of its host
        Subspaces change. If the given Object is a Subspace and the recursive
        flag is set to true, then all of its contents will be also added. */
    void addTargetObject( boost::shared_ptr<Object> object,
                          WorldSerialization::PropertyMask properties,
                          bool recursive = false );
    
    /** Removes the given Object from the list of Objects to be
        deserialized. Trying to remove an Object not on the list is considered as
        an error if the recursive flag is false (checked with an assert). Set
        the recursive flag to true if the Object is a Subspace and all of its
        \em current contents should be also removed from the list. In this case
        it is not an error if the Object or any of its children is not on the
        list. */
    void removeTargetObject( boost::shared_ptr<Object> object,
                             bool recursive = false );
    
    
    /* operations */
    
    /** Try to deserialize all target objects from all source streams. One
        serialization data block is read from each stream. Every target object
        mentioned in some of the data blocks is deserialized. Target objects
        not mentioned in the data blocks and objects mentioned but not on the
        target object list will be left intact. */
    void deserialize();
    
    /** Deserialize from a stream. Only the first encountered serialization
        block will be deserialized. */
    void deserializeFromStream( std::istream & stream );
    
    
    /** Handle object events. */
    virtual void processEvent( const Object::ObjectEvent * event );
    
    /** Handle tick events. */
    virtual void processEvent( const GraphicsEvent * event );
    
  };
  
  
  
  
}   /* namespace lifespace */




#endif   /* LS_R_WORLDDESERIALIZER_HPP */
