/**
 * @file RecPlaySystem.hpp
 *
 * Recording and playback control.
 */

/**
 * @class RecPlaySystem
 *
 * @brief
 * Recording and playback functionality.
 *
 * World snapshots are stored in a vector as a consequence of the assumption of
 * dense (mostly sequential) distribution of time indices. Storage space will
 * be allocated for a pointer for every time index from 0 to the largest
 * encountered t.
 *
 * ( outdated: ) Functionality:
 *
 *   - simulation speed control: speedup implemented by frameskip, over 2x
 *     slowdown implemented by frame duplication (no interpolation support)
 *
 *   - recording: all object locations and velocities are stored on each
 *     simulation cycle. No internal state data in objects is stored!
 *
 *   - playback: stop simulating and feed the visualization with stored
 *     location information. Location information is directly written into all
 *     locators. The world is not timestepped during playback.
 *
 *   - (not supported: (re-simulate: rewind and re-simulate, possibly with
 *   different user input)
 *
 *
 * \par Compatibility with controllers
 *
 * Simulation speed control does not affect the simulation content or the
 * dataflow to controllers. Re-simulation does, of course, affect also the
 * controllers.
 *
 * @todo
 * Rename to SimControl?
 *
 * @warning
 * Only object locations and velocities are stored (and restored when
 * re-simulating)- All internal state data in objects will NOT be stored or
 * restored!
 */
#ifndef SIM_RECPLAYSYSTEM_HPP
#define SIM_RECPLAYSYSTEM_HPP


#include <lifespace/lifespace.hpp>

#include <vector>
#include <string>

#include <boost/shared_ptr.hpp>




class RecPlaySystem
{
  
  lifespace::WorldSerializer serializer;
  lifespace::WorldDeserializer deserializer;
  
  std::vector< boost::shared_ptr<std::string> > allData;
  
  
public:
  
  
  /* constructors/destructors/etc */
  
  /** */
  RecPlaySystem();
  
  /** */
  virtual ~RecPlaySystem() {}
  
  
  /* accessors */
  
  void addObject( boost::shared_ptr<lifespace::Object> object,
                  lifespace::WorldSerialization::PropertyMask properties,
                  bool recursive = false );
  
  void removeObject( boost::shared_ptr<lifespace::Object> object,
                     bool recursive = false );
  
  
  /* operations */
  
  /**
   * Stores the current state of all selected objects. The state will be
   * associated with the time index t. If a state serialization already exists
   * with the same time index, it will be overwritten. Objects can be added and
   * removed from serialization via the methods addObject() and removeObject().
   */
  void serializeState( unsigned long long t );
  
  /** Reverts the selected objects to the serialized state associated with time
      index t. It is an error to try to deserialize with a time index with no
      content (fails an assertion in debug mode and is a no-op in release
      mode). */
  void deserializeState( unsigned long long t );
  
};




#endif   /* SIM_RECPLAYSYSTEM_HPP */
