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
 * @file ObjectNode.hpp
 *
 * <brief>
 */

/**
 * @class lifespace::ObjectNode
 * @ingroup ODECollisionRenderer
 *
 * @brief
 * <brief>
 *
 * @warning
 * The ObjectNode creates an own collision space, and sets it cleanup mode to
 * 1, so that it \em will be cleaned up automatically upon destruction. This
 * implies that you shouldn't add any geoms created with the ODE C++ wrapper
 * interface into this space! Also take care to not add the sub-objects of a
 * subspace into an ObjectNode's collision space.
 */
#ifndef LS_R_OBJECTNODE_HPP
#define LS_R_OBJECTNODE_HPP


#include "../../types.hpp"
#include "../../Structures/BasicLocator.hpp"
#include "../../Structures/Vector.hpp"
#include "../../Structures/Object.hpp"
#include "../../Structures/Subspace.hpp"
#include "../../Structures/ODELocator.hpp"
#include "../../Utility/shapes.hpp"
#include "../../Utility/Geometry.hpp"
#include "../../Utility/BasicGeometry.hpp"
#include "../../Utility/CollisionMaterial.hpp"
#include "../../Utility/Contact.hpp"

#include <ode/ode.h>
#include <ode/odecpp.h>
#include <ode/odecpp_collision.h>

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>

#include <iostream>
#include <cassert>
#include <cmath>




namespace lifespace {
  
  
  /* forwards */
  class ODECollisionRenderer;
  class ODEWorld;
  //class Object;
  //class Subspace;
  
  
  
  
  class ObjectNode :
    public EventListener<Object::ObjectEvent>
  {
    Collider & collider;
    dSpace & hostGeomSpace;
    dSimpleSpace * geomSpace;
    Object & object;
    dBodyID objectBodyID;
    //boost::scoped_ptr<dGeom> geom;
    
    /** Deletes all Contact objects connected with the current geometry of the
        target object. */
    void deleteContacts()
    {
      Geometry::contacts_t & contacts =
        object.getGeometry()->getContacts();
      
      for( Geometry::contacts_t::iterator i = contacts.begin() ;
           i != contacts.end() ; ) {
        Geometry::contacts_t::iterator j = i++;
        collider.allContacts.remove( j->second );
        delete j->second;
      }
      
      assert_internal( contacts.empty() );
    }
    
    void applyLocatorToGeom( dGeomID geom, const Locator & locator )
    {
      dGeomSetPosition( geom,
                        locator.getLoc()(0),
                        locator.getLoc()(1),
                        locator.getLoc()(2) );
      dMatrix3 odeBasis;
      dACCESS33(odeBasis,0,0) = locator.getBasis().getBasisVec(DIM_X)[0];
      dACCESS33(odeBasis,1,0) = locator.getBasis().getBasisVec(DIM_X)[1];
      dACCESS33(odeBasis,2,0) = locator.getBasis().getBasisVec(DIM_X)[2];
      dACCESS33(odeBasis,0,1) = locator.getBasis().getBasisVec(DIM_Y)[0];
      dACCESS33(odeBasis,1,1) = locator.getBasis().getBasisVec(DIM_Y)[1];
      dACCESS33(odeBasis,2,1) = locator.getBasis().getBasisVec(DIM_Y)[2];
      dACCESS33(odeBasis,0,2) = locator.getBasis().getBasisVec(DIM_Z)[0];
      dACCESS33(odeBasis,1,2) = locator.getBasis().getBasisVec(DIM_Z)[1];
      dACCESS33(odeBasis,2,2) = locator.getBasis().getBasisVec(DIM_Z)[2];
      dGeomSetRotation( geom, odeBasis );
    }
      
    // old boost in use still..
    Vector element_prod3( const Vector & lhs, const Vector & rhs )
    {
      assert( lhs.size() == 3 && rhs.size() == 3 );
        
      Vector res(3);
      res(0) = lhs(0) * rhs(0);
      res(1) = lhs(1) * rhs(1);
      res(2) = lhs(2) * rhs(2);
      return res;
    }
      
    /* shapes */
      
    dGeomID makeGeom( dSpace & geomSpace, const CollisionMaterial & material,
                      const BasicLocator * location, const Vector * scaling,
                      const Shape & shape )
    {
      // attempt downcasts to supported types
      if( const shapes::Sphere * sphere =
          dynamic_cast<const shapes::Sphere *>( &shape ) )
        return makeGeom( geomSpace, material, location, scaling, *sphere );
      if( const shapes::Cube * cube =
          dynamic_cast<const shapes::Cube *>( &shape ) )
        return makeGeom( geomSpace, material, location, scaling, *cube );
      if( const shapes::CappedCylinder * cappedCylinder =
          dynamic_cast<const shapes::CappedCylinder *>( &shape ) )
        return makeGeom( geomSpace, material, location, scaling,
                         *cappedCylinder );
      if( const shapes::Scaled * scaled =
          dynamic_cast<const shapes::Scaled *>( &shape ) )
        return makeGeom( geomSpace, material, location, scaling, *scaled );
      if( const shapes::Located * located =
          dynamic_cast<const shapes::Located *>( &shape ) )
        return makeGeom( geomSpace, material, location, scaling, *located );
      if( const shapes::Precomputed * precomputed =
          dynamic_cast<const shapes::Precomputed *>( &shape ) )
        return precomputed->target ?
          makeGeom( geomSpace, material, location, scaling,
                    *precomputed->target ) :
          0;
      if( const shapes::Union * shapeUnion =
          dynamic_cast<const shapes::Union *>( &shape ) )
        return makeGeom( geomSpace, material, location, scaling,
                         *shapeUnion );
        
      assert(false); return 0;
    }
      
    // note: scaling is averaged..
    dGeomID makeGeom( dSpace & geomSpace, const CollisionMaterial & material,
                      const BasicLocator * location, const Vector * scaling,
                      const shapes::Sphere & sphere )
    {
      // only uniform scaling allowed
      assert( !scaling ||
              ( scaling->size() == 3 &&
                fabs((*scaling)(0) - (*scaling)(1)) < EPS &&
                fabs((*scaling)(0) - (*scaling)(2)) < EPS ));
      
      dGeomID result;
      real radius = sphere.radius *
        ( scaling ?
          ((*scaling)(0) + (*scaling)(1) + (*scaling)(2)) / 3.0 :
          1.0 );
      
      if( !location ) {
        result = dCreateSphere( geomSpace, radius );
      } else {
        result = dCreateGeomTransform( geomSpace );
        dGeomID target = dCreateSphere( 0, radius );
        applyLocatorToGeom( target, *location );
        dGeomTransformSetGeom( result, target );
        dGeomTransformSetInfo( result, 1 );
        dGeomTransformSetCleanup( result,  1 );
      }
        
      if( objectBodyID ) {
        dGeomSetBody( result, objectBodyID );
      } else {
        applyLocatorToGeom( result,
                            ( object.getLocator() ?
                              (const Locator &)*object.getLocator() : (const Locator &)BasicLocator() ) );
      }
      dGeomSetData( result, (void *)&object );
      return result;
    }
      
    dGeomID makeGeom( dSpace & geomSpace, const CollisionMaterial & material,
                      const BasicLocator * location, const Vector * scaling,
                      const shapes::Cube & cube )
    {
      assert( cube.size.size() == 3 );
        
      dGeomID result;
      Vector size( cube.size );
        
      if( scaling ) size = element_prod3( *scaling, size );
        
      if( !location ) {
        result = dCreateBox( geomSpace, size(0), size(1), size(2) );
      } else {
        result = dCreateGeomTransform( geomSpace );
        dGeomID target = dCreateBox( 0, size(0), size(1), size(2) );
        applyLocatorToGeom( target, *location );
        dGeomTransformSetGeom( result, target );
        dGeomTransformSetInfo( result, 1 );
        dGeomTransformSetCleanup( result, 1 );
      }
        
      if( objectBodyID ) {
        dGeomSetBody( result, objectBodyID );
      } else {
        applyLocatorToGeom( result,
                            ( object.getLocator() ?
                              (const Locator &)*object.getLocator() : (const Locator &)BasicLocator() ) );
      }
      dGeomSetData( result, (void *)&object );
      return result;
    }
    
    // note: scaling is averaged..
    dGeomID makeGeom( dSpace & geomSpace, const CollisionMaterial & material,
                      const BasicLocator * location, const Vector * scaling,
                      const shapes::CappedCylinder & cc )
    {
      // only uniform scaling along x and y allowed
      assert( !scaling ||
              ( scaling->size() == 3 &&
                fabs((*scaling)(0) - (*scaling)(1)) < EPS ));
      
      dGeomID result;
      real radius = cc.radius *
        ( scaling ?
          ((*scaling)(0) + (*scaling)(1)) / 2.0 :
          1.0 );
      real length = cc.length * ( scaling ? ((*scaling)(2)) : 1.0 );
      
      if( !location ) {
        result = dCreateCapsule( geomSpace, radius, length );
      } else {
        result = dCreateGeomTransform( geomSpace );
        dGeomID target = dCreateCapsule( 0, radius, length );
        applyLocatorToGeom( target, *location );
        dGeomTransformSetGeom( result, target );
        dGeomTransformSetInfo( result, 1 );
        dGeomTransformSetCleanup( result,  1 );
      }
        
      if( objectBodyID ) {
        dGeomSetBody( result, objectBodyID );
      } else {
        applyLocatorToGeom( result,
                            ( object.getLocator() ?
                              (const Locator &)*object.getLocator() : (const Locator &)BasicLocator() ) );
      }
      dGeomSetData( result, (void *)&object );
      return result;
    }
    
    dGeomID makeGeom( dSpace & geomSpace, const CollisionMaterial & material,
                      const BasicLocator * location, const Vector * scaling,
                      const shapes::Scaled & scaled )
    {
      assert( scaled.scale.size() == 3 );
        
      if( !scaling ) {
        return makeGeom( geomSpace, material, location, &scaled.scale,
                         *scaled.target );
      } else {
        Vector newScaling( element_prod3( scaled.scale, *scaling ) );
        return makeGeom( geomSpace, material, location, &newScaling,
                         *scaled.target );
      }
    }
      
    dGeomID makeGeom( dSpace & geomSpace, const CollisionMaterial & material,
                      const BasicLocator * location, const Vector * scaling,
                      const shapes::Located & located )
    {
      BasicLocator newLocation( located.location );
      if( scaling ) {
        newLocation.setLoc( element_prod3( *scaling, newLocation.getLoc() ));
      }
      if( location ) location->transform( newLocation );
        
      if( !scaling ) {
        return makeGeom( geomSpace, material, &newLocation, 0,
                         *located.target );
      } else {
          
        // works only if the transformation's basis is axis-aligned, because
        // some geoms cannot be scaled in non-axis directions.
        assert( located.location.getBasis().isAxisAligned() );
          
        // rotate our scaling into the transformed target space
        Vector newScaling( abs( prod( located.location.getBasis().inverted(),
                                      *scaling )));
          
        return makeGeom( geomSpace, material, &newLocation, &newScaling,
                         *located.target );
      }
    }
      
    dGeomID makeGeom( dSpace & geomSpace, const CollisionMaterial & material,
                      const BasicLocator * location, const Vector * scaling,
                      const shapes::Union & shapeUnion )
    {
      // dump the return values for now..
        
      // for_each( shapeUnion.targets )
      for( shapes::Union::targets_t::const_iterator i =
             shapeUnion.targets.begin() ;
           i != shapeUnion.targets.end() ; i++ ) {
        // do
        makeGeom( geomSpace, material, location, scaling, **i );
      }
        
      return 0;
    }
      
    /* geometries */
      
    dGeomID makeGeom( dSpace & geomSpace, const Geometry & geometry )
    {
      // attempt downcasts to supported types
      if( const BasicGeometry * basicGeometry =
          dynamic_cast<const BasicGeometry *>( &geometry ) )
        return makeGeom( geomSpace, *basicGeometry );
        
      // no other types known, avoid no-op for now for debugging reasons
      assert(false); return 0;
    }
      
    dGeomID makeGeom( dSpace & geomSpace,
                      const BasicGeometry & basicGeometry )
    {
      return makeGeom( geomSpace, *basicGeometry.collisionMaterial, 0, 0,
                       *basicGeometry.shape );
    }
      
    /* object */
      
    dGeomID makeGeom( dSpace & geomSpace, const Object & object )
    {
      boost::shared_ptr<const ODELocator> odeLocator =
        boost::dynamic_pointer_cast<const ODELocator>( object.getLocator() );
      boost::shared_ptr<const Geometry> geometry = object.getGeometry();
        
      // check that we have a geometry, and in case on an ODELocator, check
      // that it is in active state.
      assert( geometry && ( !odeLocator || odeLocator->isActive() ) );
        
      objectBodyID = odeLocator ? odeLocator->getODEBodyId() : 0;
      return makeGeom( geomSpace, *geometry );
    }
      
      
  public:
      
    ObjectNode( Collider & collider_,
                dSpace & hostGeomSpace_,
                Object & object_ ) :
      collider( collider_ ),
      hostGeomSpace( hostGeomSpace_ ),
      geomSpace( new dSimpleSpace( hostGeomSpace_ ) ),
      object( object_ ),
      objectBodyID( 0 )
    {
      geomSpace->setCleanup( 1 );
      makeGeom( *geomSpace, object );
      object.events.addListener( this );
    }
      
    virtual ~ObjectNode()
    {
      deleteContacts();
      object.events.removeListener( this );
      delete geomSpace; geomSpace = 0;
    }
    
    
    virtual void processEvent( const Object::ObjectEvent * event )
    {
      assert_internal( event->data.source == &object );
      
      switch( event->id )
        {
        case Object::OE_OBJECT_DYING:
          delete this; return;
          break;
        case Object::OE_VISUAL_CHANGING:
          break;
        case Object::OE_GEOMETRY_CHANGING:
          if( !event->data.changingTarget.geometry ) {
            // geometry is being removed (and not replaced)
            delete this;
            return;
          }
          deleteContacts();
          delete geomSpace;
          geomSpace = new dSimpleSpace( hostGeomSpace );
          geomSpace->setCleanup( 1 );
          makeGeom( *geomSpace, *event->data.changingTarget.geometry );
          break;
        case Object::OE_LOCATOR_CHANGING:
        case Object::OE_HOSTSPACE_CHANGING:
          if( event->data.changingTarget.hostSpace == 0 ) {
            // target object is being disconnected from its host subspace, stop
            // tracking it
            delete this; return;
          }
          assert(false);   // dynamic tracking not yet implemented
          break;
        case Object::OE_VISUAL_MODIFIED:
          break;
        case Object::OE_LOCATOR_MODIFIED:
        case Object::OE_GEOMETRY_MODIFIED:
          assert(false);   // dynamic tracking not yet implemented
          break;
        }
    }
  };
  
  
  
  
}   /* namespace lifespace */




#endif   /* LS_R_OBJECTNODE_HPP */
