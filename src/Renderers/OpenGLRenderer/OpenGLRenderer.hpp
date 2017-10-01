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
 * @file OpenGLRenderer.hpp
 *
 * An OpenGL renderer for graphics Viewports.
 */

/**
 * @defgroup OpenGLRenderer OpenGLRenderer
 * @ingroup Renderers
 *
 * A basic OpenGL renderer for displaying a World in a Viewport from the
 * viewpoint of a Camera.
 */

/**
 * @class lifespace::OpenGLRenderer
 * @ingroup OpenGLRenderer
 *
 * @brief
 * An OpenGL renderer for graphics Viewports.
 *
 * @warning
 * Contextual render objects are not monitored for deletion, and no stored
 * context objects will be deleted until the renderer itself is deleted.
 *
 * @todo
 * Add an abort_if_unknown (abortIfUnknown) flag or parameter etc. to the
 * renderer (controls what happens if none of the downcasts succeed).
 *
 * @todo
 * Move the renderTarget and renderSource pointers to the Renderer base
 * class. (maybe?)
 *
 * @sa Viewport, Camera
 */
#ifndef LS_R_OPENGLRENDERER_HPP
#define LS_R_OPENGLRENDERER_HPP


#include "../../types.hpp"
#include "../Renderer.hpp"
#include "../RenderSource.hpp"
#include "../RenderTarget.hpp"
#include "FrameState.hpp"
#include "../../Graphics/Viewport.hpp"
#include "../../Graphics/Visual.hpp"
#include "../../Graphics/BasicVisual.hpp"
#include "../../Graphics/CustomVisual.hpp"
#include "../../Graphics/Material.hpp"
#include "../../Graphics/Environment.hpp"
#include "../../Graphics/Light.hpp"
#include "../../Structures/Camera.hpp"
#include "../../Structures/Object.hpp"
#include "../../Structures/Subspace.hpp"
#include "../../Utility/shapes.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <list>
#include <map>
#include <algorithm>
#include <functional>


namespace lifespace {
  
  
  /* forwards */
  class FrameState;
  
  
  
  
  class OpenGLRenderer :
    public Renderer
  {
    
    struct PrivateContext {};
    
    struct PrecomputedContext : public PrivateContext {
      GLuint displaylistId;
      
      PrecomputedContext( GLuint displaylistId_ ) :
        displaylistId( displaylistId_ )
      {}
      
      ~PrecomputedContext()
      {
        if( displaylistId ) glDeleteLists( displaylistId, 1 );
      }
    };
    
    static const int SPHERE_SLICES, SPHERE_STACKS;
    static const int CAPPEDCYLINDER_SLICES, CAPPEDCYLINDER_STACKS;
    static const int DEFAULT_MAX_RECURSION_DEPTH;
    
    Viewport * renderTarget;
    boost::shared_ptr<const Camera> renderSource;
    
    bool autoDisplaylisting;
    bool displaylistCompileRunning;
    
    int maxRecursionDepth;
    int currentRecursionDepth;
    
    typedef std::map<const void *, PrivateContext *> privateContexts_t;
    privateContexts_t privateContexts;
    
    typedef std::map<const CustomVisual *, CustomVisual::Context *>
    customVisualContexts_t;
    customVisualContexts_t customVisualContexts;
    
    typedef std::map<const shapes::CustomOpenGLShape *,
                     shapes::CustomOpenGLShape::Context *>
    customOpenGLShapeContexts_t;
    customOpenGLShapeContexts_t customOpenGLShapeContexts;
    
    FrameState * frame;
    
    
    template<class TargetT>
    void compileDisplaylist( const TargetT & target )
    {
      privateContexts_t::iterator context_i =
        privateContexts.find( (const void *)&target );
      
      if( context_i == privateContexts.end() ) {
        
        // no context found, compile and render the target
        
        assert_internal( !displaylistCompileRunning );
        displaylistCompileRunning = true;
        
        // allocate a display list
        GLuint displaylistId = glGenLists(1);
        assert_user( displaylistId != 0,
                     "OpenGL error: cannot allocate a display list!" );
        
        // render the target into the list and to the display
        glNewList( displaylistId, GL_COMPILE_AND_EXECUTE );
        render( target );
        glEndList();
        
        // store the display list id
        PrecomputedContext * context = new PrecomputedContext( displaylistId );
        privateContexts.insert
          ( std::make_pair( (const void *)&target, context ) );
        
        displaylistCompileRunning = false;
        
      } else {
        
        // context found, just call the display list (is ok if zero)
        glCallList( ((PrecomputedContext *)context_i->second)->displaylistId );
        
      }
    }
    
    
  protected:
    
    /**
     * Prepare the renderTarget and renderSource for rendering.
     */
    void preRender()
    {
      // prepare
      assert( !frame );
      frame = new FrameState( *this, GL_MAX_LIGHTS );
      currentRecursionDepth = 0;
      glPushMatrix();
      
      // move to the Camera's location
      boost::shared_ptr<const Locator> worldLocator =
        renderSource->getTargetObject()->getWorldLocator();
      render( *worldLocator, Reverse );
      
      // apply the Camera's scaling
      const Vector & scaling = renderSource->getScaling();
      assert_user( scaling.size() == 3,
                   "The Camera's scaling vector must be 3-dimensional!" );
      glScalef( scaling(0), scaling(1), scaling(2) );
      if( scaling(0) * scaling(1) * scaling(2) < 0.0 ) {
        // is a mirror scaling
        GLint oldFrontFace;
        glGetIntegerv( GL_FRONT_FACE, &oldFrontFace );
        glFrontFace( oldFrontFace == GL_CW ? GL_CCW : GL_CW );
      }
    }
    
    /**
     * Finalize the rendering process.
     */
    void postRender()
    {
      glPopMatrix();
      
      assert_internal( currentRecursionDepth == 0 );
      delete frame; frame = 0;
    }
    
    void render( const Object & object )
    {
      // attempt downcasts to supported types
      if( const Camera * camera =
          dynamic_cast<const Camera *>( &object ) ) {
        // if is a Camera, then render it and continue
        render( *camera, true );
      }
      if( const Subspace * subspace =
          dynamic_cast<const Subspace *>( &object ) ) {
        // if is a Subspace, then delegate to it
        render( *subspace );
        return;
      }
      
      // render the Object's own Visual
      boost::shared_ptr<const Locator> locator = object.getLocator();
      boost::shared_ptr<const Visual> visual = object.getVisual();
      if( visual ) {
        glPushMatrix();
        if( locator ) render( *locator );
        render( *visual );
        glPopMatrix();
      }
    }
    
    void render( const Subspace & subspace )
    {
      boost::shared_ptr<const Environment> environment =
        subspace.getEnvironment();
      boost::shared_ptr<const Locator> locator = subspace.getLocator();
      boost::shared_ptr<const Visual> visual = subspace.getVisual();
      
      glPushMatrix();
      
      // move OGL if located
      if( locator ) render( *locator );
      // apply environment
      if( environment ) render( *environment, subspace );
      
      // draw contents
      const Subspace::objects_t & objects = subspace.getObjects();
      for( Subspace::objects_t::const_iterator i = objects.begin() ;
           i != objects.end() ; i++ ) {
        render( **i );
      }
      
      // draw own visual if supplied
      if( visual ) render( *visual );
      
      // undo environment
      if( environment ) render( *environment, subspace, Reverse );
      
      glPopMatrix();
    }
    
    /**
     * @param dispatched   For internal use only, do not use.
     *
     * @todo
     * Merge this with the top-level render() method.
     */
    void render( const Camera & camera, bool dispatched = false )
    {
      // push it though the Object dispatcher if not yet done
      if( !dispatched ) render( (Object &)camera );
      
      // no-op if currently at maximum recursion depth
      if( currentRecursionDepth == maxRecursionDepth ) return;

      // no-op if no targetobject, targetobject does not have a locator or
      // targetobject is not within a world
      if(!( camera.getTargetObject() &&
            camera.getTargetObject()->getLocator() &&
            camera.getTargetObject()->getHostWorld() )) return;

      // assert that the world locator is available
      assert_internal( renderSource->getTargetObject()->getWorldLocator() );
      
      ++currentRecursionDepth;
      glPushMatrix();
      
      // move to the Camera's location in the current world
      boost::shared_ptr<const Locator> locator = camera.getLocator();
      if( locator ) render( *locator );
      
      // move to the target Object's location in the target world
      boost::shared_ptr<const Locator> worldLocator =
        camera.getTargetObject()->getWorldLocator();
      render( *worldLocator, Reverse );
      
      // apply the Camera's scaling
      const Vector & scaling = camera.getScaling();
      assert_user( scaling.size() == 3,
                   "The Camera's scaling vector must be 3-dimensional!" );
      glScalef( scaling(0), scaling(1), scaling(2) );
      if( scaling(0) * scaling(1) * scaling(2) < 0.0 ) {
        // is a mirror scaling
        GLint oldFrontFace;
        glGetIntegerv( GL_FRONT_FACE, &oldFrontFace );
        glFrontFace( oldFrontFace == GL_CW ? GL_CCW : GL_CW );
      }
      
      // render the target subspace
      render( *camera.getTargetObject()->getHostSpace() );
      
      // undo the Camera's scaling
      if( scaling(0) * scaling(1) * scaling(2) < 0.0 ) {
        // is a mirror scaling
        GLint oldFrontFace;
        glGetIntegerv( GL_FRONT_FACE, &oldFrontFace );
        glFrontFace( oldFrontFace == GL_CW ? GL_CCW : GL_CW );
      }
      
      glPopMatrix();
      --currentRecursionDepth;
    }
    
    void render( const Locator & locator,
                 Direction direction = Normal )
    {
      const Vector & loc = locator.getLoc();
      
      switch( direction )
        {
        case Normal:
          glTranslatef( loc[0], loc[1], loc[2] );
          render( locator.getBasis() );
          break;
        case Reverse:
          render( locator.getBasis().inverted() );
          glTranslatef( -loc[0], -loc[1], -loc[2] );
          break;
        default:
          assert(false);   // unkown enum value
        }
    }
    
    void render( const BasisMatrix & basis )
    {
      GLfloat m[4][4];   // col major!
      int col, row, i;
      
      // create the rotation matrix
      for( col=0 ; col<3 ; col++ )
        for( row=0; row<3 ; row++ )
          m[col][row] = basis(row,col);
      for( i=0 ; i<3 ; i++ )
        m[i][3] = m[3][i] = 0.0;
      m[3][3] = 1.0;
      
      glMultMatrixf( (const GLfloat *)m );
    }
    
    void render( const Visual & visual )
    {
      if( autoDisplaylisting && !displaylistCompileRunning )
        { compileDisplaylist( visual ); return; }
      
      // attempt downcasts to supported types
      if( const BasicVisual * basicVisual =
          dynamic_cast<const BasicVisual *>( &visual ) )
        render( *basicVisual );
      else if( const CustomVisual * customVisual =
               dynamic_cast<const CustomVisual *>( &visual ) )
        render( *customVisual );
      else {
        // 'pure virtual' renderer, so no default implementation here
        assert(false);
      }
    }
    
    void render( const BasicVisual & basicVisual )
    {
      const Material * material = basicVisual.material;
      boost::shared_ptr<const Shape> shape = basicVisual.shape;
      
      if( material ) render( *material );
      if( shape ) render( *shape );
    }
    
    void render( const CustomVisual & customVisual )
    {
      customVisualContexts_t::iterator context_i =
        customVisualContexts.find( &customVisual );
      
      customVisualContexts[&customVisual] =
        customVisual.render( ( context_i != customVisualContexts.end() ?
                               (*context_i).second : 0 ) );
    }
    
    void render( const Material & material, int lightNum = -1 )
    {
      if( lightNum != -1 ) {
        glLightfv( GL_LIGHT0 + lightNum, GL_AMBIENT, material.ambient );
        glLightfv( GL_LIGHT0 + lightNum, GL_DIFFUSE, material.diffuse );
        glLightfv( GL_LIGHT0 + lightNum, GL_SPECULAR, material.specular );
      } else {
        glMaterialfv( material.face, GL_AMBIENT, material.ambient );
        glMaterialfv( material.face, GL_DIFFUSE, material.diffuse );
        glMaterialfv( material.face, GL_SPECULAR, material.specular );
        glMaterialfv( material.face, GL_EMISSION, material.emission );
        glMaterialfv( material.face, GL_SHININESS, material.shininess );
      }
    }
    
    void render( const Shape & shape )
    {
      // attempt downcasts to supported types (do not switch on typeid to allow
      // the shapes to be extended)
      if( const shapes::Sphere * sphere =
          dynamic_cast<const shapes::Sphere *>( &shape ) )
        render( *sphere );
      else if( const shapes::Cube * cube =
          dynamic_cast<const shapes::Cube *>( &shape ) )
        render( *cube );
      else if( const shapes::CappedCylinder * cappedCylinder =
          dynamic_cast<const shapes::CappedCylinder *>( &shape ) )
        render( *cappedCylinder );
      else if( const shapes::Scaled * scaled =
          dynamic_cast<const shapes::Scaled *>( &shape ) )
        render( *scaled );
      else if( const shapes::Located * located =
          dynamic_cast<const shapes::Located *>( &shape ) )
        render( *located );
      else if( const shapes::Precomputed * precomputed =
          dynamic_cast<const shapes::Precomputed *>( &shape ) )
        render( *precomputed );
      else if( const shapes::Union * shapeUnion =
          dynamic_cast<const shapes::Union *>( &shape ) )
        render( *shapeUnion );
      else if( const shapes::CustomOpenGLShape * customOpenGLShape =
          dynamic_cast<const shapes::CustomOpenGLShape *>( &shape ) )
        render( *customOpenGLShape );
      else {
        // 'pure virtual' renderer, so no default implementation here
        assert(false);
      }
    }
    
    void render( const shapes::Sphere & sphere )
    {
      glutSolidSphere( sphere.radius, SPHERE_SLICES, SPHERE_STACKS );
    }
    
    void render( const shapes::Cube & cube )
    {
      // size vector must be 3-dimensional
      assert( cube.size.size() == 3 );
      
      if( cube.size(0) == cube.size(1) &&
          cube.size(0) == cube.size(2) ) {
        glutSolidCube( cube.size(0) );
      } else {
        glPushMatrix();
        glScalef( cube.size(0), cube.size(1), cube.size(2) );
        glutSolidCube( 1.0 );
        glPopMatrix();
      }
    }
    
    void render( const shapes::CappedCylinder & cc )
    {
      GLUquadric * quad = gluNewQuadric();
      assert( quad );   // not even trying to survive in low-memory conditions
      
      glPushMatrix();
      glTranslatef( 0.0, 0.0, -(cc.length / 2.0) );
      glutSolidSphere( cc.radius, SPHERE_SLICES, SPHERE_STACKS );
      gluCylinder( quad, cc.radius, cc.radius, cc.length,
                   CAPPEDCYLINDER_SLICES, CAPPEDCYLINDER_STACKS );
      glTranslatef( 0.0, 0.0, cc.length );
      glutSolidSphere( cc.radius, SPHERE_SLICES, SPHERE_STACKS );
      glPopMatrix();
      
      gluDeleteQuadric( quad );
    }
    
    void render( const shapes::Scaled & scaled )
    {
      // scaling vector must be 3-dimensional
      assert( scaled.scale.size() == 3 );
      
      glPushMatrix();
      glScalef( scaled.scale(0), scaled.scale(1), scaled.scale(2) );
      render( *scaled.target );
      glPopMatrix();
    }

    void render( const shapes::Located & located )
    {
      glPushMatrix();
      render( located.location );
      render( *located.target );
      glPopMatrix();
    }
    
    void render( const shapes::Precomputed & precomputed )
    {
      // no-op if no target
      if( !precomputed.target ) return;
      
      // generate a displaylist (if not already generating)
      if( !displaylistCompileRunning )
        { compileDisplaylist( *precomputed.target ); return; }
      
      // render the target
      render( *precomputed.target );
    }
    
    void render( const shapes::Union & shapeUnion )
    {
      // for_each( shapeUnion.targets )
      for( shapes::Union::targets_t::const_iterator i =
             shapeUnion.targets.begin() ;
           i != shapeUnion.targets.end() ; i++ ) {
        // do
        render( **i );
      }
    }
    
    void render( const shapes::CustomOpenGLShape & customOpenGLShape )
    {
      customOpenGLShapeContexts_t::iterator context_i =
        customOpenGLShapeContexts.find( &customOpenGLShape );
      
      customOpenGLShapeContexts[&customOpenGLShape] =
        customOpenGLShape.render
        ( ( context_i != customOpenGLShapeContexts.end() ?
            (*context_i).second : 0 ) );
    }
    
    void render( const Environment & environment,
                 const Subspace & hostSpace,
                 Direction direction = Normal )
    {
      switch( direction )
        {
        case Normal:   // apply the environment
          
          // apply OpenGL state changes
          if( !environment.oglStates.empty() ) {
            glPushAttrib( environment.oglStateMask );
            for( std::list<const Environment::OGLState *>::const_iterator i =
                   environment.oglStates.begin() ;
                 i != environment.oglStates.end() ; i++ ) {
              render( **i );
            }
          }
          
          // add lights
          for( std::list<Light *>::const_iterator i =
                 environment.lights.begin() ;
               i != environment.lights.end() ; i++ ) {
            render( **i, hostSpace );
          }
          
          break;
          
        case Reverse:   // undo the environment
          
          // remove lights
          if( !environment.lights.empty() ) {
            frame->popLight( environment.lights.size() );
          }
          
          // revert OpenGL state changes
          if( !environment.oglStates.empty() ) glPopAttrib();
          
          break;
          
        }
    }
    
    void render( const Environment::OGLState & oglState )
    {
      switch( oglState.type ) {
      case GL_ENABLE_BIT:
        if( *(GLboolean *)oglState.value == GL_TRUE )
          glEnable( oglState.name );
        else
          glDisable( oglState.name );
        break;
      case GL_LIGHTING_BIT:
        if( oglState.name == GL_LIGHT_MODEL_LOCAL_VIEWER ||
            oglState.name == GL_LIGHT_MODEL_TWO_SIDE )
          glLightModeli( oglState.name, *(GLboolean *)oglState.value );
        break;
      case GL_COLOR_BUFFER_BIT:
        if( oglState.name == GL_ALPHA_TEST )
          glAlphaFunc( GL_GREATER, *(GLfloat *)oglState.value );
        break;
      }
    }
    
    void render( const Light & light, const Subspace & hostSpace )
    {
      frame->pushLight( light, hostSpace );
    }
    
    
    friend class FrameState;
    
    
  public:
    
    /* constructors/destructors/etc */
    
    /**
     * Creates a new OpenGL renderer.
     *
     * The renderTarget and renderSource can be left null and set later with
     * setRenderSource() and setRenderTarget().
     *
     * The maximum recursion depth when processing mirrors and projectors is
     * set to OpenGLRenderer::DEFAULT_MAX_RECURSION_DEPTH (see
     * setMaxRecursionDepth() for details).
     */
    OpenGLRenderer( Viewport * renderTarget_,
                    boost::shared_ptr<const Camera> renderSource_ ) :
      Renderer(),
      renderTarget( renderTarget_ ),
      renderSource( renderSource_ ),
      autoDisplaylisting( false ),
      displaylistCompileRunning( false ),
      maxRecursionDepth( DEFAULT_MAX_RECURSION_DEPTH ),
      frame( 0 )
    {}
    
    virtual ~OpenGLRenderer()
    {
      // for_each( customVisualContexts )
      for( customVisualContexts_t::iterator i = customVisualContexts.begin() ;
           i != customVisualContexts.end() ; i++ ) {
        // do
        delete i->second;
      }
      customVisualContexts.clear();
      
      // for_each( customOpenGLShapeContexts )
      for( customOpenGLShapeContexts_t::iterator i =
             customOpenGLShapeContexts.begin() ;
           i != customOpenGLShapeContexts.end() ; i++ ) {
        // do
        delete i->second;
      }
      customOpenGLShapeContexts.clear();
      
      // for_each( privateContexts )
      for( privateContexts_t::iterator i = privateContexts.begin() ;
           i != privateContexts.end() ; i++ ) {
        // do
        delete i->second;
      }
      privateContexts.clear();
    }
    
    
    /* accessors */
    
    virtual void setRenderTarget( RenderTarget * newRenderTarget )
    {
      assert( !renderTarget );   // rendertarget cannot be changed once set
      
      Viewport * viewport = dynamic_cast<Viewport *>( newRenderTarget );
      assert_user( viewport || !newRenderTarget,
                   "Only Viewport RenderTargets accepted here!" );
      
      renderTarget = viewport;
    }
    
    virtual void setRenderSource( boost::shared_ptr<const RenderSource>
                                  newRenderSource )
    {
      boost::shared_ptr<const Camera> camera
        ( boost::dynamic_pointer_cast<const Camera>( newRenderSource ) );
      assert_user( camera || !newRenderSource,
                   "Only Camera RenderSources accepted here!" );
      
      renderSource = camera;
    }
    
    int getMaxRecursionDepth() const
    { return maxRecursionDepth; }
    
    /**
     * Set the maximum recursion depth when processing mirrors and
     * projectors.
     *
     * @param depth   New maximum depth. Value of 0 disables these effects
     *                completely.
     */
    void setMaxRecursionDepth( int depth )
    { maxRecursionDepth = depth; }
    
    /**
     * This controls whether all encountered shapes should be automatically
     * wrapped into a display list. Note that this cannot be currently disabled
     * after once activated!
     *
     * @warning
     * If this is turned on, then the generated display lists cannot be
     * released or updated in any way! This implies also that dynamic
     * modifications to the shapes and visuals do not affect this renderer
     * anymore. Use the Precomputed -shape instead to manually control the
     * creation of display lists.
     */
    void setAutoDisplaylisting( bool newState )
    {
      assert_user( !( autoDisplaylisting && !newState ),
                   "Automatic display list generation cannot currently be "
                   "turned off after once activated!" );
      
      autoDisplaylisting = newState;
    }
    
    bool getAutoDisplaylisting() const
    { return autoDisplaylisting; }
    
    
    /* operations */
    
    void connect( boost::weak_ptr<const World> world ) {}
    
    void disconnect() {}
    
    /**
     * Renders the current renderSource (Camera) into the current renderTarget
     * (Viewport). The method is no-op if all needed data is not
     * available. Needed things:
     *   - renderTarget and renderSource must be non-null.
     *   - renderTarget must have a target object.
     *   - target object must have a locator.
     *   - target object must reside inside a world-rooted subspace hierarchy.
     */
    virtual void render()
    {
      // no-op if no rendertarget or rendersource
      if( !renderTarget || !renderSource ) return;
      
      // no-op if no targetobject, targetobject does not have a locator or
      // targetobject is not within a world
      if(!( renderSource->getTargetObject() &&
            renderSource->getTargetObject()->getLocator() &&
            renderSource->getTargetObject()->getHostWorld() )) return;
      
      // assert that the world locator is available
      assert_internal( renderSource->getTargetObject()->getWorldLocator() );
      
      // do the actual rendering
      preRender();
      render( *renderSource->getTargetObject()->getHostWorld() );
      postRender();
    }
    
  };
  
  
}   /* lifespace */




#endif   /* LS_R_OPENGLRENDERER_HPP */
