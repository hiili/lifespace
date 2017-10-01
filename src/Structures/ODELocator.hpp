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
 * @file ODELocator.hpp
 *
 * A locator that is managed by the Open Dynamics Engine.
 */

/**
 * @class lifespace::ODELocator
 * @ingroup Structures
 *
 * @brief
 * A locator that is managed by the Open Dynamics Engine.
 *
 * @par Active vs. inactive state
 * When created, the locator is in inactive state and most of its functionality
 * is not available for use (will assert fail): it has only the location and
 * orientation attributes from BasicLocator. It can be activated by inserting
 * the host object into a subspace hierarchy that is rooted to an ODEWorld
 * object and calling its ODEWorld::activate() method, or explicitly with the
 * static ODEWorld::Activate method. After this the locator is fully
 * functional.
 *
 * @warning
 * Removing an object or subspace with an active ODELocator from an ODEWorld
 * rooted subspace hierarchy is not allowed: the object or subspace must first
 * be inactivated! This is not currently checked anywhere and will result in
 * undefined behaviour.
 * 
 * @par Drag forces
 * The friction arguments control the linear and rotational drag force applied
 * to the locator. Drag is always applied relative to world coordinates (as if
 * the surrounding air were still relative to the nearest world in the world
 * hierarchy). Currently it is always applied to the mass center of the
 * locator. The different drag degrees are computed as the name suggests
 * (relative to the locator's velocity), but conceptually they represent the
 * following properties:
 *   - constant drag: This is not very realistic, but is useful to make objects
 *     stop completely eventually (to prevent continuous slow drifting).
 *   - linear drag: Represents non-turbulent drag.
 *   - quadratic drag: Represents turbulent drag. This is also useful to
 *     quickly damp extremely fast rotations to keep simulation errors under
 *     control.
 * Note that the drag forces are applied as normal external forces and computed
 * based on the previous timestep. This implies that they might actually
 * increase velocities in some cases (if the drag parameters are very high,
 * some joints or contacts are connected to the host object and the motion
 * state of the locator is changing extremely fast).
 * 
 * @todo
 * Consider moving the locator to be a permanent part of Object (templated
 * inheritance? different locator types must be supported still..): now all
 * that absolute-ODE vs. relative-locators -resolving has to bounce back and
 * forth between object hierarchy and the locators of the corresponding
 * objects. .. or not!
 *
 * @todo
 * Should the ODELocator::setVel() and setRotation() methods maybe removed?
 * Currently they do not work completely correctly: the \em real absolute
 * velocity and rotation should take host velocity and rotation into account..
 *
 * @todo
 * \b IMPORTANT: Reset the force accumulators if repeating the prepare pass!
 *
 * @todo
 * Add support to pass in a generic shape to be used in the inertia
 * matrix. Remember to fix the rotational drag computation after this!
 *
 * @todo
 * Check the "nod" prevention computation in the drag computations.
 *
 * @internal 
 * @warning
 * The Connector class relies heavily on the internal structure of the
 * ODELocator!
 *
 * @todo Why is this inherited from BasicLocator instead of MotionLocator or
 * InertiaLocator? The whole Locator hierarchy should be refactored..
 */
#ifndef LS_S_ODELOCATOR_HPP
#define LS_S_ODELOCATOR_HPP


#include "../types.hpp"
#include "Locator.hpp"
#include "BasicLocator.hpp"
#include "ODEWorld.hpp"
#include "../Utility/shapes.hpp"
#include <boost/utility.hpp>
#include <ode/ode.h>
#include <ode/odecpp.h>


namespace lifespace {
  
  
  
  
  class ODELocator :
    public BasicLocator,
    private boost::noncopyable
  {
    
    
    /**
     * Interface to the ODE dBody object. Also represents the locator's state
     * relative to the host World.
     */
    class ODEWorldLocator :
      public Locator,
      private dBody,
      private boost::noncopyable
    {
      
      ODELocator & hostLocator;
      ODEWorld & hostODEWorld;
      
      mutable struct {
        Vector loc;
        BasisMatrix basis;
        Vector vel;
        Vector rotation;
        bool validLoc:1, validBasis:1, validVel:1, validRotation:1;
      } cache;
      
    public:
      
      ODEWorldLocator( ODELocator & hostLocator_,
                       ODEWorld & hostODEWorld_,
                       boost::shared_ptr<const Locator> location,
                       real mass, real density,
                       boost::shared_ptr<Shape> mInertiaShape ) :
        dBody( hostODEWorld_.id() ),
        hostLocator( hostLocator_ ),
        hostODEWorld( hostODEWorld_ )
      {
        // apply information from the location locator
        setLoc( location->getLoc() );
        setBasis( location->getBasis() );
        
        // set mass
        dMass odeMass;
        boost::shared_ptr<shapes::Sphere> sphere =
          boost::dynamic_pointer_cast<shapes::Sphere>( mInertiaShape );
        boost::shared_ptr<shapes::Cube> cube =
          boost::dynamic_pointer_cast<shapes::Cube>( mInertiaShape );
        boost::shared_ptr<shapes::CappedCylinder> cyl =
          boost::dynamic_pointer_cast<shapes::CappedCylinder>( mInertiaShape );
        if( sphere ) {
          // sphere shape
          if( density != 0.0 ) {
            dMassSetSphere( &odeMass, density, sphere->radius );
          } else {
            dMassSetSphereTotal( &odeMass, mass, sphere->radius );
          }
        } else if( cube ) {
          // box shape
          if( density != 0.0 ) {
            dMassSetBox( &odeMass, density,
                         cube->size(0), cube->size(1), cube->size(2) );
          } else {
            dMassSetBoxTotal( &odeMass, mass,
                              cube->size(0), cube->size(1), cube->size(2) );
          }
        } else if( cyl ) {
          // capped cylinder shape
          if( density != 0.0 ) {
            dMassSetCappedCylinder( &odeMass, density, 3,
                                    cyl->radius, cyl->length );
          } else {
            dMassSetCappedCylinderTotal( &odeMass, mass, 3,
                                         cyl->radius, cyl->length );
          }
        } else {
          // a nonsupported shape (should not have been accepted in the setter)
          assert_internal( false );
        }
        dBody::setMass( &odeMass );
        
        // set other params
        dBody::setFiniteRotationMode( 0 );
        
        // init cache
        cache.loc.resize(3);
        cache.basis.resize(3,3);
        cache.vel.resize(3);
        cache.rotation.resize(3);
        invalidateCache();
      }
      
      virtual ~ODEWorldLocator() {}
      
      virtual Locator * clone() const
      { assert( false ); return 0; }   // not copyable
      
      void invalidateCache() const
      {
        cache.validLoc = cache.validBasis =
          cache.validVel = cache.validRotation = false;
      }
      
      dBodyID getODEBodyId() const
      { return dBody::id(); }
      
      ODEWorld & getHostODEWorld()
      { return hostODEWorld; }
      
      void setGravityEnabled( bool state )
      { setGravityMode( state ? 1 : 0 ); }
      
      virtual const Vector & getLoc() const
      {
        if( cache.validLoc ) return cache.loc;
        
        const dReal * odeVec = dBody::getPosition();
        cache.loc[0] = odeVec[0];
        cache.loc[1] = odeVec[1];
        cache.loc[2] = odeVec[2];
        cache.validLoc = true;
        
        return cache.loc;
      }
      
      virtual const BasisMatrix & getBasis() const
      {
        if( cache.validBasis ) return cache.basis;
        
        const dReal * odeBasis = dBody::getRotation();
        cache.basis.getBasisVec(DIM_X)[0] = dACCESS33(odeBasis,0,0);;
        cache.basis.getBasisVec(DIM_X)[1] = dACCESS33(odeBasis,1,0);;
        cache.basis.getBasisVec(DIM_X)[2] = dACCESS33(odeBasis,2,0);;
        cache.basis.getBasisVec(DIM_Y)[0] = dACCESS33(odeBasis,0,1);;
        cache.basis.getBasisVec(DIM_Y)[1] = dACCESS33(odeBasis,1,1);;
        cache.basis.getBasisVec(DIM_Y)[2] = dACCESS33(odeBasis,2,1);;
        cache.basis.getBasisVec(DIM_Z)[0] = dACCESS33(odeBasis,0,2);;
        cache.basis.getBasisVec(DIM_Z)[1] = dACCESS33(odeBasis,1,2);;
        cache.basis.getBasisVec(DIM_Z)[2] = dACCESS33(odeBasis,2,2);;
        cache.validBasis = true;
        
        return cache.basis;
      }
      
      virtual const Vector & getVel() const
      {
        if( cache.validVel ) return cache.vel;
        
        const dReal * odeVec = dBody::getLinearVel();
        cache.vel[0] = odeVec[0];
        cache.vel[1] = odeVec[1];
        cache.vel[2] = odeVec[2];
        cache.validVel = true;
        
        return cache.vel;
      }
      
      virtual const Vector & getRotation() const
      {
        if( cache.validRotation ) return cache.rotation;
        
        const dReal * odeVec = dBody::getAngularVel();
        cache.rotation[0] = odeVec[0];
        cache.rotation[1] = odeVec[1];
        cache.rotation[2] = odeVec[2];
        cache.validRotation = true;
        
        return cache.rotation;
      }

      virtual bool isMoving() const
      { return dBody::isEnabled(); }
    
      virtual bool isRotating() const
      { return dBody::isEnabled(); }
    
      virtual void setLoc( const Vector & newLoc )
      {
        dBody::setPosition( newLoc[0], newLoc[1], newLoc[2] );
        cache.validLoc = false;
        hostLocator.invalidateCache();
      }
      
      virtual void setBasis( const BasisMatrix & newBasis )
      {
        dMatrix3 odeBasis;
        dACCESS33(odeBasis,0,0) = newBasis.getBasisVec(DIM_X)[0];
        dACCESS33(odeBasis,1,0) = newBasis.getBasisVec(DIM_X)[1];
        dACCESS33(odeBasis,2,0) = newBasis.getBasisVec(DIM_X)[2];
        dACCESS33(odeBasis,0,1) = newBasis.getBasisVec(DIM_Y)[0];
        dACCESS33(odeBasis,1,1) = newBasis.getBasisVec(DIM_Y)[1];
        dACCESS33(odeBasis,2,1) = newBasis.getBasisVec(DIM_Y)[2];
        dACCESS33(odeBasis,0,2) = newBasis.getBasisVec(DIM_Z)[0];
        dACCESS33(odeBasis,1,2) = newBasis.getBasisVec(DIM_Z)[1];
        dACCESS33(odeBasis,2,2) = newBasis.getBasisVec(DIM_Z)[2];
        dBody::setRotation( odeBasis );
        cache.validBasis = false;
        hostLocator.invalidateCache();
      }
      
      virtual void setVel( const Vector & newVel )
      {
        dBody::setLinearVel( newVel[0], newVel[1], newVel[2] );
        cache.validVel = false;
      }
      
      virtual void setRotation( const Vector & newRotation )
      {
        dBody::setAngularVel( newRotation[0], newRotation[1], newRotation[2] );
        cache.validRotation = false;
      }
      
      virtual void addForceAbs( const Vector & force )
      { dBody::addForce( force[0], force[1], force[2] ); }
      
      virtual void addForceRel( const Vector & force )
      { dBody::addRelForce( force[0], force[1], force[2] ); }
      
      virtual void addTorqueAbs( const Vector & torque )
      { dBody::addTorque( torque[0], torque[1], torque[2] ); }
      
      virtual void addTorqueRel( const Vector & torque )
      { dBody::addRelTorque( torque[0], torque[1], torque[2] ); }
      
      
      /** not supported, should be used only through ODELocator. */
      virtual void prepare( real dt )
      { assert(false); }   // not supported
      
      /** not supported, should be used only through ODELocator. */
      virtual void step()
      { assert(false); }   // not supported
      
    };
    
    
    /* ODEWorldLocator ends here */
    
    
    
    
    /* constants */
    static const real DEFAULT_VEL_CONSTANT_DRAG;
    static const real DEFAULT_VEL_LINEAR_DRAG;
    static const real DEFAULT_VEL_QUADRATIC_DRAG;
    static const real DEFAULT_ROT_CONSTANT_DRAG;
    static const real DEFAULT_ROT_LINEAR_DRAG;
    static const real DEFAULT_ROT_QUADRATIC_DRAG;
    
    /* mass and inertia properties. density overrides mass if defined (>
       0.0) */
    real mass, density;
    boost::shared_ptr<Shape> mInertiaShape;
    
    /* static drag properties */
    real velConstantDrag, velLinearDrag, velQuadraticDrag;
    real rotConstantDrag, rotLinearDrag, rotQuadraticDrag;
    
    
    /** This will be set when the locator gets rooted in a world. */
    boost::shared_ptr<ODEWorldLocator> worldLocator;
    
    /** Does gravity affect this Locator? */
    bool gravityEnabled;
    
    /** Is this locator up to date? This is always true if the locator is in
        inactive (static) state. */
    mutable bool thisLocatorValid;
    
    
    /** An active ODELocator cannot be moved to another Object or removed from
        the current. */
    virtual void setHostObject( Object * newHostObject )
    {
      assert_user( !isActive(),
                   "An active ODELocator cannot be moved to another Object "
                   "or removed from the current!" );
      BasicLocator::setHostObject( newHostObject );
    }
    
    /**
     * Either activates or deactivates the locator in an ODEWorld.
     * 
     * When activating, the current static location and position are used to
     * compute the location for the ODEWorldLocator. When deactivating, the
     * current relative position and orientation are updated and kept as the
     * static position and orientation of the inactive state.
     *
     * It is an error to (de)activate an ODELocator twice.
     * 
     * @if done_todos
     * @todo
     * done (Object::lockToHostSpace()): Think what happens if the subspace
     * hierarchy is broken after it once has been complete and all ODELocators
     * have been activated.
     * @endif
     *
     * @bug
     * IMPORTANT: if an ODE-located object with connected connectors gets
     * deactivated, the connection is lost permanently! Probably the
     * connector's status should be maintained somehow and reconnected when
     * activating again?
     */
    void activate( ODEWorld * hostODEWorld )
    {
      if( hostODEWorld ) {
        // activate
        assert( !isActive() );
        assert( getHostObject() && getHostObject()->getWorldLocator() );
        worldLocator = boost::shared_ptr<ODEWorldLocator>
          ( new ODEWorldLocator( *this, *hostODEWorld,
                                 getHostObject()->getWorldLocator(),
                                 mass, density, mInertiaShape ) );
        worldLocator->setGravityEnabled( gravityEnabled );
        thisLocatorValid = false;
      } else {
        // deactivate
        assert( isActive() );
        updateThisLocator();
        delete_shared( worldLocator );
        thisLocatorValid = true;
      }
    }
    
    /**
     * Returns the host locator, which defines this locator's reference
     * frame. The host locator is the locator of the nearest host Subspace
     * which contains a locator, or null if no such locator exists. The search
     * will end (and return its locator if it has one) if a World is
     * encountered while traversing the hierarchy.
     *
     * A null result means that this locator is in absolute world coordinates.
     *
     * @todo
     * Check that the dynamic_cast test really works as expected.
     */
    boost::shared_ptr<const Locator> findHostLocator() const
    {
      if( !getHostObject() ) return boost::shared_ptr<Locator>();
      
      const Subspace * subspace = getHostObject()->getHostSpace();
      while( subspace && !dynamic_cast<const World *>( subspace ) &&
             !subspace->getLocator() ) {
        subspace = subspace->getHostSpace();
      }
      
      return subspace ?
        subspace->getWorldLocator() : boost::shared_ptr<const Locator>();
    }
    
    void invalidateCache() const
    { thisLocatorValid = false; }
    
    /**
     * @warning
     * Discards constness with a cast! (the cached values need to be updated
     * from const get functions)
     */
    void updateThisLocator() const
    {
      assert( isActive() );
      ODELocator * nonconst_this = const_cast<ODELocator *>( this );
      
      // compute the new relative location from the absolute world location
      BasicLocator newLocation( *worldLocator );
      boost::shared_ptr<const Locator> hostLocator = findHostLocator();
      if( hostLocator ) {
        // the locator is not in absolute world coordinates and needs to be
        // transformed
        hostLocator->transform( newLocation, Reverse );
      }
      
      // assign it directly to the BasicLocator base class
      nonconst_this->BasicLocator::setLoc( newLocation.getLoc() );
      nonconst_this->BasicLocator::setBasis( newLocation.getBasis() );
      
      // mark this locator valid
      thisLocatorValid = true;
    }
    
    friend class ODEWorld;
    
    
  public:
    
    /**
     * Constructs a new ODE-managed locator.
     *
     * The locator is created in inactive state, see ODEWorld for details about
     * activating ODEWorld and ODELocator objects.
     *
     * The loc and basis arguments define the initial location, relative to the
     * host space. Drag parameters control the drag force applied to the
     * locator. See ODELocator main reference for details about drag.
     *
     * Mass and mInertia set the mass and magnitude of the moment of inertia.
     * Currently this is simplified to an inertia matrix representing a filled
     * sphere of radius \c mInertia_ and mass \c mass_. Other inertia shapes
     * can be set with the setInertiaShape() method. Mass can be computed from
     * a given density with the setDensity() method.
     *
     * Gravity is enabled by default (the gravity vector can be set in
     * ODEWorld).
     *
     * @sa ODEWorld
     */
    ODELocator( const Vector & loc = ZeroVector(3),
                const BasisMatrix & basis = BasisMatrix(3),
                real mass_ = 1.0, real mInertia_ = 1.0,
                real velConstantDrag_ = DEFAULT_VEL_CONSTANT_DRAG,
                real velLinearDrag_ = DEFAULT_VEL_LINEAR_DRAG,
                real velQuadraticDrag_ = DEFAULT_VEL_QUADRATIC_DRAG,
                real rotConstantDrag_ = DEFAULT_ROT_CONSTANT_DRAG,
                real rotLinearDrag_ = DEFAULT_ROT_LINEAR_DRAG,
                real rotQuadraticDrag_ = DEFAULT_ROT_QUADRATIC_DRAG ) :
      BasicLocator( loc, basis ),
      mass( mass_ ), density( 0.0 ),
      mInertiaShape( shapes::Sphere::create( mInertia_ ) ),
      velConstantDrag( velConstantDrag_ ),
      velLinearDrag( velLinearDrag_ ),
      velQuadraticDrag( velQuadraticDrag_ ),
      rotConstantDrag( rotConstantDrag_ ),
      rotLinearDrag( rotLinearDrag_ ),
      rotQuadraticDrag( rotQuadraticDrag_ ),
      worldLocator( boost::shared_ptr<ODEWorldLocator>() ),
      gravityEnabled( true ),
      thisLocatorValid( true )
    {}
    
    /**
     * Destroys the ODELocator.
     *
     * @warning
     * The locator must be inactive (is asserted) and in such state will also
     * not contain an internal ODEWorldLocator, or if it is active then no
     * other shared pointers should exist that point to the internal
     * ODEWorldLocator (is asserted) to make sure that the internal
     * ODEWorldLocator is really deleted!
     */
    virtual ~ODELocator()
    {
      // assert that the locator is either inactive or no external references
      // to the internal ODEWorldLocator exist
      assert( !isActive() || worldLocator.unique() );
    }
    
    /** The class is noncopyable (this method will assert fail). */
    virtual Locator * clone() const
    { assert( false ); return 0; }   // not copyable
    
    
    bool isActive() const
    { return worldLocator ? true : false; }
    
    dBodyID getODEBodyId() const
    {
      assert( isActive() );
      return worldLocator->getODEBodyId();
    }
    
    ODEWorld & getHostODEWorld()
    {
      assert( isActive() );
      return worldLocator->getHostODEWorld();
    }
    
    bool isGravityEnabled() const
    { return gravityEnabled; }
    
    void setGravityEnabled( bool state )
    {
      gravityEnabled = state;
      if( isActive() ) worldLocator->setGravityEnabled( state );
    }
    
    /**
     * Sets the moment of inertia shape. Supported shapes: Sphere, Cube,
     * CappedCylinder. The target shape is read and applied upon activation of
     * the locator.
     */
    void setInertiaShape( boost::shared_ptr<Shape> shape )
    {
      assert_user
        ( boost::dynamic_pointer_cast<shapes::Sphere>( shape ) ||
          boost::dynamic_pointer_cast<shapes::Cube>( shape ) ||
          boost::dynamic_pointer_cast<shapes::CappedCylinder>( shape ),
          "Only Sphere, Cube and CappedCylinder shapes are supported here!" );
      
      mInertiaShape = shape;
    }
    
    /** Sets the density of the object. The total mass will be computed upon
        activation of the locator according to the associated inertia shape at
        that moment. */
    void setDensity( real density_ )
    { density = density_; }
    
    
    virtual boost::shared_ptr<const Locator> getDirectWorldLocator() const
    { return worldLocator; }

    virtual boost::shared_ptr<Locator> getDirectWorldLocator()
    { return worldLocator; }
    
    
    /**
     * Returns either the stored static location if inactive, or the true
     * relative location if active (which will be computed from the
     * ODEWorldLocator).
     */
    virtual const Vector & getLoc() const
    {
      if( !thisLocatorValid ) {
        assert( isActive() );
        updateThisLocator();
      }
      return BasicLocator::getLoc();
    }
      
    /**
     * Returns either the stored static orientation if inactive, or the true
     * relative orientation if active (which will be computed from the
     * ODEWorldLocator).
     */
    virtual const BasisMatrix & getBasis() const
    {
      if( !thisLocatorValid ) {
        assert( isActive() );
        updateThisLocator();
      }
      return BasicLocator::getBasis();
    }
    
    
    /**
     * Sets either the stored static location if inactive, or the true relative
     * location if active (which will be applied through the ODEWorldLocator).
     */
    virtual void setLoc( const Vector & newLoc )
    {
      if( isActive() ) {
        assert( getHostObject() && getHostObject()->getHostSpace() );
        Vector absLoc( newLoc );
        getHostObject()->
          getHostSpace()->transformToWorldCoordinates( absLoc );
        worldLocator->setLoc( absLoc );
      }
      
      if( thisLocatorValid ) BasicLocator::setLoc( newLoc );
    }
    
    /**
     * Sets either the stored static orientation if inactive, or the true
     * relative orientation if active (which will be applied through the
     * ODEWorldLocator).
     */
    virtual void setBasis( const BasisMatrix & newBasis )
    {
      if( isActive() ) {
        assert( getHostObject() && getHostObject()->getHostSpace() );
        BasisMatrix absBasis( newBasis );
        getHostObject()->
          getHostSpace()->transformToWorldCoordinates( absBasis );
        worldLocator->setBasis( absBasis );
      }
      
      if( thisLocatorValid ) BasicLocator::setBasis( newBasis );
    }
    
    virtual void setVel( const Vector & newVel )
    {
      assert( isActive() &&
              getHostObject() && getHostObject()->getHostSpace() );
      
      Vector absVel( newVel );
      getHostObject()->getHostSpace()->transformToWorldCoordinates( absVel );
      worldLocator->setVel( absVel );
    }
    
    virtual void setRotation( const Vector & newRotation )
    {
      assert( isActive() &&
              getHostObject() && getHostObject()->getHostSpace() );
      
      Vector absRot( newRotation );
      getHostObject()->getHostSpace()->transformToWorldCoordinates( absRot );
      worldLocator->setRotation( absRot );
    }
    
    /** @bug The force is currently applied in \em world, not host Subspace
        coordinates! */
    virtual void addForceAbs( const Vector & force )
    {
      assert( isActive() &&
              getHostObject() && getHostObject()->getHostSpace() );
      
      Vector absForce( force );
      getHostObject()->getHostSpace()->transformToWorldCoordinates( absForce );
      worldLocator->addForceAbs( absForce );
    }
    
    virtual void addForceRel( const Vector & force )
    {
      assert( isActive() );
      worldLocator->addForceRel( force );
    }
    
    /** @bug The torque is currently applied in \em world, not host Subspace
        coordinates! */
    virtual void addTorqueAbs( const Vector & torque )
    {
      assert( isActive() &&
              getHostObject() && getHostObject()->getHostSpace() );
      
      Vector absTorque( torque );
      getHostObject()->getHostSpace()->transformToWorldCoordinates(absTorque);
      worldLocator->addTorqueAbs( absTorque );
    }
    
    virtual void addTorqueRel( const Vector & torque )
    {
      assert( isActive() );
      worldLocator->addTorqueRel( torque );
    }
    
    
    virtual void prepare( real dt );
    
    
    virtual void step();
    
  };
  
  
  
  
}   /* namespace lifespace */


#endif   /* LS_S_ODELOCATOR_HPP */
