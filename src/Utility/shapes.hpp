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
 * @file shapes.hpp
 *
 * A collection of classes that define geometric shapes.
 */

/**
 * @defgroup Shapes Shapes
 * @ingroup Utility
 *
 * Various geometric shapes.
 *
 * @todo
 * Rename to lower-case shapes.hpp.
 *
 * @if done_todos
 * @todo
 * Implement factory methods for easily instantiate a shape and wrap it in a
 * shared_ptr. Example:
 * \code
 *   shared_ptr<BasicGeometry> basisGeom =
 *     BasicGeometry::create( Scaled::create( makeVector3d( 2.0, 2.0, 2.0 ),
 *                                            shapes::Basis::create() ),
 *                            CollisionMaterial::create( 1.0, 0.5, 0.0 ));
 * \endcode
 * @endif
 */
#ifndef LS_U_SHAPES_HPP
#define LS_U_SHAPES_HPP


#include "../types.hpp"
#include "../Structures/Vector.hpp"
#include "../Structures/BasisMatrix.hpp"
#include "../Structures/Locator.hpp"
#include "../Structures/BasicLocator.hpp"
#include <boost/shared_ptr.hpp>

#include <list>




namespace lifespace {
  
  
  
  
  /**
   * @class lifespace::Shape
   * @ingroup Utility
   *
   * @brief
   * Base class for all objects representing some geometric shape.
   *
   * All shapes are located at (0,0,0) in current coordinates and have a
   * default "radius" of 1.0 in some sense.
   */
  struct Shape
  {
    /** Make this class polymorphic. */
    virtual ~Shape() {}
  };
  
  
  
  
namespace shapes {  
  
  
  
  
  /* primitives */
  
  
  /**
   * @ingroup Shapes
   *
   * A sphere located at origin and having a default radius of 1.0. 
   */
  struct Sphere :
    public Shape
  {
    real radius;
    
    Sphere( real radius_ = 1.0 ) :
      radius( radius_ )
    {}
    
    static boost::shared_ptr<Sphere> create( real radius = 1.0 )
    { return boost::shared_ptr<Sphere>( new Sphere( radius ) ); }
  };


  /**
   * @ingroup Shapes
   *
   * A cube located at origin and having a default edge length of 2.0 (unit
   * "radius").
   *
   * @todo
   * Rename to Box.
   */
  struct Cube :
    public Shape
  {
    Vector size;
    
    Cube( const Vector & size_ = makeVector3d( 2.0, 2.0, 2.0 ) ) :
      size( size_ )
    {}
    
    static boost::shared_ptr<Cube>
    create( const Vector & size = makeVector3d( 2.0, 2.0, 2.0 ) )
    { return boost::shared_ptr<Cube>( new Cube( size ) ); }
  };
  
  
  /**
   * @ingroup Shapes
   *
   * A capped cylinder (a cylinder with half-spheres at the ends).
   *
   * It is located at origin, aligned along z-axis and has a default length of
   * 2.0 (without the caps) and radius of 1.0.
   *
   * \par Supported in:
   *   - OpenGLRenderer
   *   - ODECollisionRenderer
   */
  struct CappedCylinder :
    public Shape
  {
    real length;
    real radius;
    
    CappedCylinder( real length_ = 2.0, real radius_ = 1.0 ) :
      length( length_ ), radius( radius_ )
    {}
    
    static boost::shared_ptr<CappedCylinder>
    create( real length = 2.0, real radius = 1.0 )
    {
      return boost::shared_ptr<CappedCylinder>
        ( new CappedCylinder( length, radius ) );
    }
  };
  
  
  
  
  /* filter types */
  
  
  /**
   * @ingroup Shapes
   * 
   * Scales the contained shape.
   */
  struct Scaled :
    public Shape
  {
    Vector scale;
    boost::shared_ptr<Shape> target;
    
    Scaled( const Vector & scale_,
            boost::shared_ptr<Shape> target_ ) :
      scale( scale_ ),
      target( target_ )
    {}

    static boost::shared_ptr<Scaled>
    create( const Vector & scale, boost::shared_ptr<Shape> target )
    { return boost::shared_ptr<Scaled>( new Scaled( scale, target ) ); }
  };
  
  
  /**
   * @ingroup Shapes
   *
   * Moves and rotates the contained shape.
   */
  struct Located :
    public Shape
  {
    BasicLocator location;
    boost::shared_ptr<Shape> target;
    
    Located( const BasicLocator & location_,
             boost::shared_ptr<Shape> target_ ) :
      location( location_ ),
      target( target_ )
    {}
    
    static boost::shared_ptr<Located>
    create( const BasicLocator & location, boost::shared_ptr<Shape> target )
    { return boost::shared_ptr<Located>( new Located( location, target ) ); }
  };
  
  
  /**
   * @ingroup Shapes
   *
   * Instructs a renderer to precompute the contained shape, if possible.
   */
  struct Precomputed :
    public Shape
  {
    boost::shared_ptr<Shape> target;
    
    Precomputed( boost::shared_ptr<Shape> target_ ) :
      target( target_ )
    {}
    
    static boost::shared_ptr<Precomputed>
    create( boost::shared_ptr<Shape> target )
    { return boost::shared_ptr<Precomputed>( new Precomputed( target ) ); }
  };
  
  
  
  
  /* group types */
  
  
  /**
   * @ingroup Shapes
   *
   * Combines multiple shapes into one.
   *
   * @todo
   * Add initialization with boost assignment library.
   *
   * @todo
   * Parameterize this with a template type.
   */
  struct Union :
    public Shape
  {
    typedef std::list< boost::shared_ptr<Shape> > targets_t;
    targets_t targets;
    
    Union( boost::shared_ptr<Shape> shape0 = boost::shared_ptr<Shape>(),
           boost::shared_ptr<Shape> shape1 = boost::shared_ptr<Shape>(),
           boost::shared_ptr<Shape> shape2 = boost::shared_ptr<Shape>(),
           boost::shared_ptr<Shape> shape3 = boost::shared_ptr<Shape>(),
           boost::shared_ptr<Shape> shape4 = boost::shared_ptr<Shape>(),
           boost::shared_ptr<Shape> shape5 = boost::shared_ptr<Shape>(),
           boost::shared_ptr<Shape> shape6 = boost::shared_ptr<Shape>(),
           boost::shared_ptr<Shape> shape7 = boost::shared_ptr<Shape>(),
           boost::shared_ptr<Shape> shape8 = boost::shared_ptr<Shape>(),
           boost::shared_ptr<Shape> shape9 = boost::shared_ptr<Shape>() )
    {
      if( shape0 ) targets.push_back( shape0 );
      if( shape1 ) targets.push_back( shape1 );
      if( shape2 ) targets.push_back( shape2 );
      if( shape3 ) targets.push_back( shape3 );
      if( shape4 ) targets.push_back( shape4 );
      if( shape5 ) targets.push_back( shape5 );
      if( shape6 ) targets.push_back( shape6 );
      if( shape7 ) targets.push_back( shape7 );
      if( shape8 ) targets.push_back( shape8 );
      if( shape9 ) targets.push_back( shape9 );
    }
    
    static boost::shared_ptr<Union>
    create( boost::shared_ptr<Shape> shape0 = boost::shared_ptr<Shape>(),
            boost::shared_ptr<Shape> shape1 = boost::shared_ptr<Shape>(),
            boost::shared_ptr<Shape> shape2 = boost::shared_ptr<Shape>(),
            boost::shared_ptr<Shape> shape3 = boost::shared_ptr<Shape>(),
            boost::shared_ptr<Shape> shape4 = boost::shared_ptr<Shape>(),
            boost::shared_ptr<Shape> shape5 = boost::shared_ptr<Shape>(),
            boost::shared_ptr<Shape> shape6 = boost::shared_ptr<Shape>(),
            boost::shared_ptr<Shape> shape7 = boost::shared_ptr<Shape>(),
            boost::shared_ptr<Shape> shape8 = boost::shared_ptr<Shape>(),
            boost::shared_ptr<Shape> shape9 = boost::shared_ptr<Shape>() )
    { return boost::shared_ptr<Union>( new Union( shape0, shape1,
                                                  shape2, shape3,
                                                  shape4, shape5,
                                                  shape6, shape7,
                                                  shape8, shape9 ) ); }
  };
  
  
  
  
  /* base classes for custom functionality */
  
  
  /**
   * @ingroup Shapes
   *
   * Abstract base class for \em visual shapes with custom OpenGL
   * implementation. The render() method will be called whenever the shape
   * needs to be rendered to OpenGL. Note that this class produces results
   * only in OpenGL renderers. The context system works as in CustomVisual, see
   * its documentation for details and some important things to remember.
   *
   * @sa CustomVisual
   */
  class CustomOpenGLShape :
    public Shape
  {
    
  public:
    
    /** */
    class Context {};
    
    /** */
    virtual Context * render( Context * context ) const = 0;
    
  };
  
  
  
  
  /* composite types */
  
  
  /**
   * @ingroup Shapes
   *
   * A combination of three bars extending from origin along each axis, each
   * having a length of 1.0. The x-axis bar is thicker than the others.
   */
  struct Basis :
    public Union
  {
    static const float THICKNESS_X, THICKNESS_Y, THICKNESS_Z;
    
    Basis() :
      Union( Located::create
             ( BasicLocator( makeVector3d( 0.5, 0.0, 0.0 ) ),
               Cube::create( makeVector3d( 1.0,
                                           THICKNESS_X,
                                           THICKNESS_X ) )),
             Located::create
             ( BasicLocator( makeVector3d( 0.0, 0.5, 0.0 ) ),
               Cube::create( makeVector3d( THICKNESS_Y,
                                           1.0,
                                           THICKNESS_Y ) )),
             Located::create
             ( BasicLocator( makeVector3d( 0.0, 0.0, 0.5 ) ),
               Cube::create( makeVector3d( THICKNESS_Z,
                                           THICKNESS_Z,
                                           1.0 ) )))
    {}
    
    static boost::shared_ptr<Basis> create()
    { return boost::shared_ptr<Basis>( new Basis() ); }
  };
  
  
  
  
}   /* namespace lifespace::shapes */


}   /* namespace lifespace */


#endif   /* LS_U_SHAPES_HPP */
