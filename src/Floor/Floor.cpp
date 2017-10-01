/**
 * @file Floor.cpp
 *
 * Implementations for the Floor class.
 */
#include "Floor.hpp"

#include <lifespace/lifespace.hpp>
using namespace lifespace;

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include <GL/gl.h>




Floor::Floor( int xSize, int zSize,
              const Material & brightMat, const Material & darkMat,
              shared_ptr<const CollisionMaterial> physicsMat ) :
  Subspace()
{
  // check that both sizes are even
  assert( xSize % 2 == 0 && zSize % 2 == 0 );
  
  // enable alpha blending
  getEnvironment()->addOGLState
    ( new Environment::OGLState( GL_ENABLE_BIT, GL_BLEND,
                                 new GLboolean(GL_TRUE) ));
  
  shared_ptr<shapes::Union> floorHalfShapeUnion = shapes::Union::create();
  for( int i = -xSize / 2 ; i < xSize / 2 ; i++ ) {
    for( int j = -zSize / 2 ; j < zSize / 2 ; j++ ) {
      if( (i + j) % 2 ) {
        floorHalfShapeUnion->targets.push_back
          ( shapes::Located::create( makeVector3d( i + 0.5, -0.001, j + 0.5 ),
                                     shapes::Cube::create
                                     ( makeVector3d( 1.0, 0.002, 1.0 ) )));
      }
    }
  }
  
  shared_ptr<Shape> floorHalfShape =
    shapes::Precomputed::create( floorHalfShapeUnion );

  shared_ptr<Object> floorB
    ( new Object( Object::Params( new BasicLocator
                                  ( makeVector3d( 0.0, 0.0, 0.0 ),
                                    BasisMatrix() ),
                                  new BasicVisual( floorHalfShape, &brightMat ),
                                  new BasicGeometry
                                  ( shapes::Located::create
                                    ( makeVector3d( 0.0, -1.0, 0.0 ),
                                      shapes::Cube::create
                                      ( makeVector3d( xSize, 2.0, zSize ) )),
                                    physicsMat ))));
  addObject( floorB );
  
  shared_ptr<Object> floorD
    ( new Object( Object::Params( new BasicLocator
                                  ( makeVector3d( 0.0, 0.0, 0.0 ),
                                    BasisMatrix
                                    ( makeVector3d( 1.0, 0.0, 0.0 ),
                                      makeVector3d( 0.0, 1.0, 0.0 ) )),
                                  new BasicVisual( floorHalfShape, &darkMat ),
                                  0 )));
  addObject( floorD );
}
