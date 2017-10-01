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
 * @file Camera.hpp
 */

/**
 * @class lifespace::Camera
 * @ingroup Structures
 *
 * @brief
 * The camera class defines a point of view in a World (or a Subspace), so that
 * it can be rendered into a Viewport.
 *
 * The class contains a pointer to an Object (targetObject). The Object's
 * Locator defines the location and orientation of the camera's view, and the
 * Object's host World (the root of the subspace hierarchy where the object has
 * been is inserted) defines the world that will be rendered when the camera is
 * rendered by a renderer. Normally the Camera's locator inherited from the
 * Object base class should be left unused (null pointer). Note that the
 * targetObject should be inserted to a Subspace, otherwise the image generated
 * by the camera will be blank.
 *
 * \par Dual locators
 * This class is also inherited from Object, so it may contain also another
 * physical location within some other Subspace (via the Object's
 * Locator). This defines one more transformation which will be applied before
 * the transformation defined by the targetObject. This feature might be used
 * some day to implement portal rendering or mirrors.
 *
 * \par
 * The possible existence of two simultaneous locations can thought as the
 * camera being actually a projector, which creates a 3d projection of the
 * target object's hostspace viewed from the target object's location. This
 * projection is then placed either directly into a Viewport on the screen, or
 * in front of the Camera's own location if it has its own locator.
 *
 * @internal
 * <b>Design decision</b>: The camera follows \em another object, instead of
 * being itself the object defining the view position. \b Rationale: it is
 * desirable that the camera can be attached to \em any object on the fly,
 * instead of having to inherit every potential camera object from the Camera
 * class. Also with portal rendering or mirrors it is more natural that the
 * camera itself acts as the projector, instead of some random target object
 * becoming suddenly a projector.
 *
 * @todo
 * Consider implementing distance alpha fading (with 1-d textures, for example)
 * and user definable clipping planes to be used with projectors.
 *
 * @sa FloatingActor, Object, OpenGLRenderer
 */
#ifndef LS_S_CAMERA_HPP
#define LS_S_CAMERA_HPP


#include "../types.hpp"
#include "Object.hpp"
#include "Locator.hpp"
#include "BasicLocator.hpp"
#include "World.hpp"
#include "../Renderers/RenderSource.hpp"
#include <boost/shared_ptr.hpp>




namespace lifespace {
  
  
  
  
  class Camera :
    public virtual Object,
    public RenderSource
  {
    static const float DEFAULT_FOV;
    
    boost::shared_ptr<const Object> targetObject;
    Vector scaling;
    float fov;
    
    
  public:
    
    Camera( const Vector & scaling_ = makeVector3d( 1.0, 1.0, 1.0 ),
            float fov_ = DEFAULT_FOV ) :
      scaling( scaling_ ),
      fov( fov_ )
    {}
    
    boost::shared_ptr<const Object> getTargetObject() const
    { return targetObject; }
    
    void setTargetObject( boost::shared_ptr<const Object> newTargetObject )
    { targetObject = newTargetObject; }
    
    const Vector & getScaling() const
    { return scaling; }
    
    /**
     * Sets the scene scaling for this Camera. The scaling is done with the
     * projection matrix, so you don't need to use GL_NORMALIZE if you use
     * this feature.
     *
     * @warning
     * The scaling is currently applied directly to the projection matrix even
     * in the case of recursive cameras (mirrors, projectors, ..), so the
     * results might not be exactly what might be expected.
     */
    void setScaling( const Vector & newScaling )
    { scaling = newScaling; }
    
    float getFov() const
    { return fov; }
    
    void setFov( float newFov )
    { fov = newFov; }
    
  };
  
  
  
  
}   /* namespace lifespace */




#endif   /* LS_S_CAMERA_HPP */
