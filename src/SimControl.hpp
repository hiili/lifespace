/**
 * @file SimControl.hpp
 *
 * <brief>
 */

/**
 * @class SimControl
 *
 * @brief
 * <brief>
 *
 * \par Rec/play -system
 *
 * This subsystem's function is to record simulation results step by step and,
 * in playback mode, to act as a surrogate for the actual simulator.
 */
#ifndef SIM_SIMCONTROL_HPP
#define SIM_SIMCONTROL_HPP


#include "RecPlaySystem.hpp"
#include <lifespace/lifespace.hpp>
#include <boost/shared_ptr.hpp>




class SimControl :
  public lifespace::EventListener<lifespace::GraphicsEvent>,
  public lifespace::Actor
{
  
  /** */
  enum Mode { MODE_SIMULATE, MODE_PAUSE, MODE_SEEK, MODE_PLAYBACK, ModeCount };
  
  
  /** Camera object will be manually timestepped in sync with visualization. */
  lifespace::Object * cameraObject;
  
  /** Camera timestep length. */
  float camDt;
  
  /** The recording and playback system to be used. */
  boost::shared_ptr<RecPlaySystem> recPlaySystem;
  
  
  /** Current mode. */
  Mode mode;
  
  /** Current simulation iteration index. */
  long long currentSimIteration;
  
  /** Current visualization position. */
  double currentVisualizationIteration;
  
  /** Default visualization timestep multiplier (framedrop ratio). */ 
  float defaultVisualizationDtMultiplier;
  
  /** Current visualization timestep multiplier (framedrop ratio). */ 
  float visualizationDtMultiplier;
  
  /** Current external controller timestep multiplier. */
  int controlDtMultiplier;
  
  /** Playback position and speed. Position in dimension x defines the
  currently displayed simulation iteration. Velocity in dimension x defines the
  playback speed and direction. */
  lifespace::InertiaLocator pauseSeekPosition;
  
  
public:
  
  /** */
  enum Controls {
    CTRL_PAUSE_SEEK,        /**< Switch from sim/playback to pause, or between
                                 pause and seek. */
    CTRL_SIM_PLAYBACK,      /**< Switch from pause/seek to playback, or between
                                 playback and sim. */
    CTRL_INC_DEC,           /**< Increase/decrease simulation/playback
                                 speed/position. */
    CTRL_SEEK,              /**< Increment/decrement current iteration in seek
                                 mode. */
    CTRL_SPEED_RESET,
    ControlCount
  };
  
  /** Sends an event when the render system should run. */
  lifespace::EventHost<lifespace::GraphicsEvent> doRenderEvent;
  
  /** Sends an event when the external control system should run. */
  lifespace::EventHost<lifespace::GraphicsEvent> doControlEvent;
  
  /** Sends an event when the simulation system should timestep the world. */
  lifespace::EventHost<lifespace::GraphicsEvent> doSimulateEvent;
  
  
  /* constructors/destructors/etc */
  
  /** . */
  SimControl( boost::shared_ptr<RecPlaySystem> recPlaySystem );
  
  
  /* accessors */
  
  void setCameraObjectAndDt( lifespace::Object * cameraObject_, float camDt_ )
  { cameraObject = cameraObject_; camDt = camDt_; }
  
  void setRecPlaySystem( boost::shared_ptr<RecPlaySystem> recPlaySystem_ )
  { recPlaySystem = recPlaySystem_; }
  
  void setDefaultVisualizationDtMultiplier( float m );
  
  void setControlDtMultiplier( int m )
  { controlDtMultiplier = m; }
  
  
  /* operations */
  
  /** */
  void processEvent( const lifespace::GraphicsEvent * event );
  
};




#endif   /* SIM_SIMCONTROL_HPP */
