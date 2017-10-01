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
 * @file WorldSerializer.hpp
 *
 * Serializes selected simulation world content into a stream.
 */

/**
 * @class lifespace::WorldSerializer
 * @ingroup WorldSerialization
 *
 * @brief
 * Serializes selected simulation world content into a stream.
 *
 * foo
 */
#ifndef LS_R_WORLDSERIALIZER_HPP
#define LS_R_WORLDSERIALIZER_HPP


#include "types.hpp"
#include "../../types.hpp"
#include "../../Graphics/types.hpp"
#include "../../Structures/Object.hpp"
#include "../../Structures/Subspace.hpp"
#include "../../Utility/Event.hpp"

#include <boost/shared_ptr.hpp>

#include <ostream>
#include <sstream>
#include <list>
#include <string>




namespace lifespace {
  
  
  
  
  /* forwards */
  class Actor;
  
  
  
  
  class WorldSerializer :
    public EventListener<Object::ObjectEvent>,
    public EventListener<GraphicsEvent>
  {
    
    /** Data version number */
    static const int DataVersion;
    
    /** Data for an individual serialization source object. */
    struct ObjectData {
      
      /** The source object. */
      boost::shared_ptr<Object> object;
      
      /** Properties to be serialized. */
      WorldSerialization::PropertyMask properties;
      
      /** The full (absolute) name of the object. */
      std::string fullName;
      
    };
    
    
    typedef std::list<ObjectData> objects_t;
    typedef std::list<std::ostream *> streams_t;
    
    /** Source objects. */
    objects_t objects;
    
    /** Target streams. */
    streams_t streams;
    
    
    /** Serialize a 3d vector into the given character buffer. */
    void serialize3dVector( std::ostream & streambuf,
                            const Vector & vec );
    
    void serializeLocator( std::ostream & buf,
                           boost::shared_ptr<const Locator> locator );
    
    void serializeActorSensors( std::ostream & streambuf,
                                boost::shared_ptr<const Actor> actor );
    
    
  public:
    
    /* constructors/destructors/etc */
    
    /** */
    WorldSerializer() {}
    
    
    /* accessors */
    
    /** Adds an ostream to the list of streams that the serialized data will be
        sent into upon each serialize() call. */
    void addTargetStream( std::ostream * stream );
    
    /** Removes the given stream from the target stream list. */
    void removeTargetStream( std::ostream * stream );
    
    /** Adds an Object to be serialized on each serialize() call. The full
        (absolute) name of the Object is constructed when this method is called
        and will not get updated if the Object's name or a name of its host
        Subspaces change. If the given Object is a Subspace and the recursive
        flag is set to true, then all of its contents will be also added. */
    void addSourceObject( boost::shared_ptr<Object> object,
                          WorldSerialization::PropertyMask properties,
                          bool recursive = false );
    
    /** Removes the given Object from the list of Objects to be
        serialized. Trying to remove an Object not on the list is considered as
        an error if the recursive flag is false (checked with an assert). Set
        the recursive flag to true if the Object is a Subspace and all of its
        \em current contents should be also removed from the list. In this case
        it is not an error if the Object or any of its children is not on the
        list. */
    void removeSourceObject( boost::shared_ptr<Object> object,
                             bool recursive = false );
    
    
    /* operations */
    
    /** Serialize all source objects into all target streams. */
    void serialize();
    
    /** Serialize all source objects into a stream. */
    void serializeToStream( std::ostream & stream );
    
    
    /** Handle object events. */
    virtual void processEvent( const Object::ObjectEvent * event );
    
    /** Handle tick events. */
    virtual void processEvent( const GraphicsEvent * event );
    
  };
  
  
  
  
}   /* namespace lifespace */




#endif   /* LS_R_WORLDSERIALIZER_HPP */
