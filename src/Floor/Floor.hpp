/**
 * @file Floor.hpp
 *
 * Transparent checkerboard floor
 */

/**
 * @class Floor
 *
 * @brief
 * Transparent checkerboard floor
 */
#ifndef SIM_FLOOR_HPP
#define SIM_FLOOR_HPP


#include <lifespace/lifespace.hpp>




class Floor :
  public lifespace::Subspace
{
  
  
public:
  
  /* constructors/destructors/etc */
  
  /** Creates a new floor object around the origin and of the given
      dimension. The floor surface is at y = 0.0 (visual surface is actually at
      0.001, to avoid rendering artifacts) and the floor will span in x and z
      directions. Sizes must be even. The collision shape of the floor is 2.0
      units thick.
  
      No memory management: some objects will not be deleted on destruction.
  */
  Floor( int xSize, int zSize,
         const lifespace::Material & brightMat,
         const lifespace::Material & darkMat,
         boost::shared_ptr<const lifespace::CollisionMaterial> physicsMat );
  
  
  /* accessors */
  
    
  /* operations */
    
};








#endif   /* SIM_FLOOR_HPP */
