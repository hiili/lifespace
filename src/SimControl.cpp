/**
 * @file SimControl.cpp
 *
 * Implementations for the SimControl class.
 */
#include "SimControl.hpp"
#include "RecPlaySystem.hpp"

#include <lifespace/lifespace.hpp>
using namespace lifespace;

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include <cmath>
using std::pow;




SimControl::SimControl( shared_ptr<RecPlaySystem> recPlaySystem_ ) :
  cameraObject( 0 ),
  camDt( 0.0 ),
  recPlaySystem( recPlaySystem_ ),
  mode( MODE_SIMULATE ),
  currentSimIteration( 0 ),
  currentVisualizationIteration( 0.0 ),
  defaultVisualizationDtMultiplier( 1.0 ),
  visualizationDtMultiplier( defaultVisualizationDtMultiplier ),
  controlDtMultiplier( 1 ),
  pauseSeekPosition( makeVector3d(), BasisMatrix(),
                     1.0, 1.0,           // mass, mInertia
                     500.0, 0.5 )        // normal drag (constant, linear)
{
  controls.resize( ControlCount, Control( Control::ContinuousMode ) );
  
  controls[CTRL_PAUSE_SEEK]     = Control( Control::PulseMode );
  controls[CTRL_SIM_PLAYBACK]   = Control( Control::PulseMode );
  controls[CTRL_INC_DEC]        = Control( Control::PulseMode );
  
  // record the state during timestep t=0 (initial configuration of the world)
  recPlaySystem->serializeState( currentSimIteration );
}




void SimControl::setDefaultVisualizationDtMultiplier( float m )
{
  defaultVisualizationDtMultiplier = m;
  visualizationDtMultiplier = defaultVisualizationDtMultiplier;
}




void SimControl::processEvent( const GraphicsEvent * event )
{
  if( event->id != GE_TICK ) return;
  
  
  // handle mode switching
  Mode newMode = mode;
  switch( mode )
    {
    case MODE_PAUSE:
      if( readControl(CTRL_PAUSE_SEEK) > 0.5 ) newMode = MODE_SEEK;
      if( readControl(CTRL_SIM_PLAYBACK) > 0.5 ) newMode = MODE_PLAYBACK;
      break;
    case MODE_SEEK:
      if( readControl(CTRL_PAUSE_SEEK) > 0.5 ) newMode = MODE_PAUSE;
      if( readControl(CTRL_SIM_PLAYBACK) > 0.5 ) newMode = MODE_PLAYBACK;
      break;
    case MODE_SIMULATE:
      if( readControl(CTRL_SIM_PLAYBACK) > 0.5 ) newMode = MODE_PLAYBACK;
      if( readControl(CTRL_PAUSE_SEEK) > 0.5 ) newMode = MODE_SEEK;
      break;
    case MODE_PLAYBACK:
      if( readControl(CTRL_SIM_PLAYBACK) > 0.5 ) newMode = MODE_SIMULATE;
      if( readControl(CTRL_PAUSE_SEEK) > 0.5 ) newMode = MODE_SEEK;
      break;
    default:
      assert(false);
    }
  if( newMode != mode ) {
    mode = newMode;
    if( mode == MODE_PAUSE || mode == MODE_SEEK ) {
      // entering pause/seek mode, copy position from
      // currentVisualizationIteration to pauseSeekPosition
      pauseSeekPosition.stopMoving();
      pauseSeekPosition.setLoc
        ( makeVector3d( currentVisualizationIteration, 0.0, 0.0 ) );
    }
    if( mode == MODE_SIMULATE ) {
      // entering simulation mode, jump to end and deserialize newest content
      currentVisualizationIteration = currentSimIteration;
      recPlaySystem->deserializeState( currentSimIteration );
    }
  }
  
  // handle mode-specific user input
  switch( mode )
    {
    case MODE_SIMULATE:
    case MODE_PLAYBACK:
      
      // adjust visualization dt multiplier
      visualizationDtMultiplier *= 
        pow( (float)2.0, readControl(CTRL_INC_DEC) );
      break;
      
    case MODE_SEEK:
      
      // adjust pause/seek position speed
      pauseSeekPosition.addForceAbs
        ( makeVector3d( readControl(CTRL_SEEK), 0.0, 0.0 ) );
      break;
      
    case MODE_PAUSE:
      
      // adjust pause/seek position
      pauseSeekPosition.stopMoving();
      pauseSeekPosition.setLoc
        ( makeVector3d( pauseSeekPosition.getLoc()(DIM_X) + 
                        readControl(CTRL_INC_DEC), 0.0, 0.0 ) );
      break;
      
    default:
      assert(false);
    }
  
  // speed reset
  if( readControl(CTRL_SPEED_RESET) > 0.5 ) {
    visualizationDtMultiplier = defaultVisualizationDtMultiplier;
  }
  
  
  // timestep all non-simulation stuff
  Actor::prepare( 0.0 );
  Actor::step();
  pauseSeekPosition.prepare( camDt );
  pauseSeekPosition.step();
  if( cameraObject ) {
    cameraObject->prepare( camDt );
    cameraObject->step();
  }
  
  // set new visualization position
  if( mode == MODE_SIMULATE || mode == MODE_PLAYBACK ) {
    currentVisualizationIteration += visualizationDtMultiplier;
  } else {
    currentVisualizationIteration = pauseSeekPosition.getLoc()(DIM_X);
  }
  
  
  // update world
  switch( mode )
    {
    case MODE_SIMULATE:
      
      // simulate until at visualization point
      while( currentSimIteration < (long long)currentVisualizationIteration ) {
        
        // begin computing s_t+1 from s_t
        
        // consider calling the external controller
        if( currentSimIteration % controlDtMultiplier == 0 ) {
          doControlEvent.sendEvent( event );
        }
        
        // advance simulation:   t := t+1
        doSimulateEvent.sendEvent( event );
        currentSimIteration++;
        
        // record the new world state s_t
        recPlaySystem->serializeState( currentSimIteration );
        
      }
      
      break;
      
    case MODE_PLAYBACK:
    case MODE_SEEK:
    case MODE_PAUSE:
      
      // past beginning/end?
      if( currentVisualizationIteration < 0.0 ||
          currentSimIteration < (long long)currentVisualizationIteration ) {
        // go to start/end
        currentVisualizationIteration =
          currentVisualizationIteration < 0.0 ?
          0.0 : (double)currentSimIteration;
        pauseSeekPosition.stopMoving();
        pauseSeekPosition.setLoc
          ( makeVector3d( currentVisualizationIteration, 0.0, 0.0 ));
      }
      
      // deserialize world content
      recPlaySystem->deserializeState
        ( (long long)currentVisualizationIteration );
      
      break;
      
    default:
      assert(false);
    }
  
  
  // schedule visualization to be updated
  doRenderEvent.sendEvent( event );
  
}
