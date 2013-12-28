
#ifndef _LOOPIDITY_H_
#define _LOOPIDITY_H_


// DEBUG begin
// setup features
//#define INIT_LOOPIDITY          1
#define INIT_JACK_BEFORE_SCENES 1
#define WAIT_FOR_JACK_INIT      0
#define FIXED_N_AUDIO_PORTS     1
//#define MEMORY_CHECK            1 // if 0 choose DEFAULT_AUDIO_BUFFER_SIZE wisely
#define FIXED_AUDIO_BUFFER_SIZE 1 // TODO: user defined/adjustable buffer sizes
#define SCENE_NFRAMES_EDITABLE  1

// runtime features
#define JACK_IO_READ_WRITE            1
#define JACK_IO_COPY                  1
#define HANDLE_KEYBOARD_EVENTS        1
#define HANDLE_MOUSE_EVENTS           0
#define HANDLE_USER_EVENTS            1
#define SCAN_LOOP_PEAKS_DATA          1
#define SCAN_TRANSIENT_PEAKS_DATA     1
#define SCAN_PEAKS                    1
#define DRAW_STATUS                   1
#define DRAW_SCENES                   1
#define DRAW_SCOPES                   1
#define DRAW_EDIT_HISTOGRAM           SCENE_NFRAMES_EDITABLE && 1
#define DRAW_DEBUG_TEXT               1
#define DEBUG_TRACE                   0
#define AUTO_UNMUTE_LOOPS_ON_ROLLOVER 1

#if DRAW_STATUS
#  define DRAW_MODE 0
/*
#  define UPDATE_MEMORY 0
#  define UPDATE_LOOP_PROGRESS 0
#  define UPDATE_VU 0
#  define VARDUMP 0
*/
#endif // #if DRAW_STATUS

#if DRAW_SCENES
#  define DRAW_SCENE_SCOPE             1
#  define DRAW_HISTOGRAMS              1
#  define DRAW_MUTED_HISTOGRAMS        1
#  define DRAW_PEAK_RINGS              1
#  define DRAW_LOOPS                   1
#  define DRAW_MUTED_LOOPS             0
#  define DRAW_RECORDING_LOOP          DRAW_LOOPS && 1
#  define DRAW_CURRENT_SCENE_INDICATOR 1
#endif // #if DRAW_SCENES

#if DRAW_DEBUG_TEXT
#  define DRAW_DEBUG_TEXT_L Trace::SetDbgTextC() ;
#  define DRAW_DEBUG_TEXT_R Trace::SetDbgTextR() ;
#else
#  define DRAW_DEBUG_TEXT_L ;
#  define DRAW_DEBUG_TEXT_R ;
#endif // #if DRAW_DEBUG_TEXT

// Trace class features
#define DEBUG_TRACE_JACK         DEBUG_TRACE || 1
#define DEBUG_TRACE_LOOPIDITY    DEBUG_TRACE && 0
#define DEBUG_TRACE_LOOPIDITYSDL DEBUG_TRACE && 0
#define DEBUG_TRACE_SCENE        DEBUG_TRACE && 0
#define DEBUG_TRACE_SCENESDL     DEBUG_TRACE && 0
#define DEBUG_TRACE_EVS          DEBUG_TRACE && 1
#define DEBUG_TRACE_IN           DEBUG_TRACE && 1
#define DEBUG_TRACE_OUT          DEBUG_TRACE && 1
#define DEBUG_TRACE_CLASS        DEBUG_TRACE && 0
// DEBUG end

// quantities
#define DEFAULT_AUDIO_BUFFER_SIZE 33554432 // 2^25 (approx 3 min @ 48k)
//#define DEFAULT_AUDIO_BUFFER_SIZE 25165824 // 1024 * 1024 * 24 (approx 135 sec @ 48k)
//#define DEFAULT_AUDIO_BUFFER_SIZE 16777216 // 2^24 (approx 90 sec @ 48k)
//#define DEFAULT_AUDIO_BUFFER_SIZE 8388608  // 2^23 (approx 45 sec @ 48k)
//#define DEFAULT_AUDIO_BUFFER_SIZE 2097152  // 2^21 (approx 10 sec @ 48k)
//#define DEFAULT_AUDIO_BUFFER_SIZE 1048576  // 2^20 (approx 5 sec @ 48k)
#define NUM_SCENES                 3
#define NUM_LOOPS                  9 // per scene
#define LOOP_VOL_INC               0.1
#if FIXED_N_AUDIO_PORTS
#  define N_INPUT_CHANNELS         2
#  define N_OUTPUT_CHANNELS        2
#  define N_AUDIO_PORTS            N_INPUT_CHANNELS + N_OUTPUT_CHANNELS
#else // TODO: implement setting N_CHANNELS via cmd line arg - GetTransientPeaks and updateVUMeters are especially brittle now
#  define N_INPUT_CHANNELS         2 // TODO: nyi - only used for memory check and scope cache
#  define N_OUTPUT_CHANNELS        2 // TODO: nyi - only used for N_AUDIO_PORTS
#  define N_AUDIO_PORTS            N_INPUT_CHANNELS + N_OUTPUT_CHANNELS // TODO: nyi - only used for scope cache
#endif // #if FIXED_N_AUDIO_PORTS
#if SCENE_NFRAMES_EDITABLE
#  define BUFFER_MARGIN_SIZE       SampleRate
#  define TRIGGER_LATENCY_SIZE     1280 // nFrames - kludge to compensate for keyboard delay - optimized for BufferSize <= 128
#  define MINIMUM_LOOP_DURATION    2    // nSeconds
#  if INIT_JACK_BEFORE_SCENES
#    if ALLOW_BUFFER_ROLLOVER
#      define INITIAL_END_FRAMEN     (RecordBufferSize - nFramesPerPeriod)
#    else
#      define INITIAL_END_FRAMEN     RecordBufferSize
#    endif // #if ALLOW_BUFFER_ROLLOVER
#  endif // #if INIT_JACK_BEFORE_SCENES
#endif // #if SCENE_NFRAMES_EDITABLE
#if DRAW_EDIT_HISTOGRAM
#  define N_PEAKS_FINE             720 // should be divisible into 360
#else
#  define N_PEAKS_FINE             360 // should be divisible into 360
#endif // #if DRAW_EDIT_HISTOGRAM
#define PEAK_RADIUS                50 // TODO: PEAK_RADIUS and LOOP_DIAMETER are GUI specific - should probably be defined elsewhere
#define LOOP_DIAMETER              ((PEAK_RADIUS * 2) + 1)
#define N_PEAKS_COURSE             LOOP_DIAMETER

// string constants
#define APP_NAME                "Loopidity"
#define INIT_MSG                "\nINIT:  Loopidity::Main(): init\n\n"
#define INIT_SUCCESS_MSG        "\nINIT:  Loopidity::Main(): init success - entering sdl loop\n\n"
#define INIT_FAIL_MSG           "\nERROR: Loopidity::Main(): init failed - quitting\n\n"
//#define CONNECT_ARG             "--connect"
#define MONITOR_ARG             "--nomon"
#define SCENE_CHANGE_ARG        "--noautoscenechange"
//#define FREEMEM_FAIL_MSG        "ERROR: Could not determine available memory - quitting"
#define JACK_FAIL_MSG           "ERROR: Could not register JACK client - quitting"
#define ZERO_BUFFER_SIZE_MSG    "ERROR: initBufferSize is zero - quitting"
#define INSUFFICIENT_MEMORY_MSG "ERROR: Insufficient memory - quitting"
#define OUT_OF_MEMORY_MSG       "ERROR: Out of Memory"
#define GETPEAK_ERROR_MSG       "Loopidity::GetPeak(): subscript out of range\n"
#define INVALID_METADATA_MSG    "ERROR: Scene metadata state insane"
#define JACK_INPUT1_PORT_NAME   "inL"
#define JACK_INPUT2_PORT_NAME   "inR"
#define JACK_OUTPUT1_PORT_NAME  "outL"
#define JACK_OUTPUT2_PORT_NAME  "outR"

// sdl user events
#define EVT_NEW_LOOP      1
#define EVT_SCENE_CHANGED 2

// error states
#define JACK_INIT_SUCCESS 0
#define JACK_FAIL         1
#define JACK_BUFF_FAIL    2
#define JACK_MEM_FAIL     3


// dependencies

#include <cstdlib>
#include <exception>           // Scene::Scene()
#include <iostream>
#include <limits>              // Loopidity::Init()
#include <list>
#include <sstream>
#include <string>
#include <vector>

#include <jack/jack.h>
#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_rotozoom.h>
#include <SDL_ttf.h>
#include <unistd.h>            // Loopidity::Init()
#include <X11/Xlib.h>          // LoopiditySdl::Init()

typedef jack_default_audio_sample_t Sample ;

// local includes
#include "jack_io.h"
#include "loopidity_sdl.h"
#include "scene.h"
#include "scene_sdl.h"
#include "trace.h"


using namespace std ;


class Loopidity
{
  friend class JackIO ;
  friend class Trace ;

  public:

    /* Loopidity class side public constants */

    static const unsigned int N_SCENES ;
    static const unsigned int N_LOOPS ;


  private:

    /* Loopidity class side private variables */
#if WAIT_FOR_JACK_INIT
    // setup
    static int InitJackTimeout ;
#endif // #if WAIT_FOR_JACK_INIT

    // models and views
    static Scene*       Scenes[NUM_SCENES] ;
    static SceneSdl*    SdlScenes[NUM_SCENES] ;

    // runtime state
    static unsigned int CurrentSceneN ;
    static unsigned int NextSceneN ;

    // runtime flags
#if WAIT_FOR_JACK_INIT
    static bool IsJackReady ;
#endif // #if WAIT_FOR_JACK_INIT
    static bool IsRolling ;
    static bool ShouldSceneAutoChange ;
    static bool IsEditMode ;

  public:

    /* Loopidity class side public functions */

    // main
    static int Main(int argc , char** argv) ;

    // getters/setters
//    static void         SetNFramesPerPeriod(   unsigned int nFrames) ;
    static unsigned int GetCurrentSceneN(void) ;
    static unsigned int GetNextSceneN(   void) ;
//    static unsigned int GetLoopPos(    void) ;
    static bool         GetIsRolling(    void) ;
//    static bool         GetShouldSaveLoop(     void) ;
//    static bool         GetDoesPulseExist(     void) ;
//    static bool         GetIsEditMode(         void) ;


    /* Loopidity class side private functions */

    // setup
    static bool IsInitialized(void) ; // TODO: make singleton
    static bool Init(         bool shouldMonitorInputs , bool shouldAutoSceneChange ,
                              unsigned int recordBufferSize) ;
#if INIT_JACK_BEFORE_SCENES
#  if SCENE_NFRAMES_EDITABLE
    static void SetMetaData(  unsigned int sampleRate , unsigned int nFramesPerPeriod ,
                              unsigned int bytesPerFrame , unsigned int minLoopSize ,
                              unsigned int triggerLatencySize ,
                              unsigned int beginFrameN , unsigned int endFrameN) ;
#  else
    static void SetMetaData(  unsigned int sampleRate , unsigned int nFramesPerPeriod ,
                              unsigned int recordBufferSize) ;
#  endif // #if SCENE_NFRAMES_EDITABLE
#else
    static void SetMetaData(  unsigned int sampleRate , unsigned int nFramesPerPeriod) ;
#endif // #if INIT_JACK_BEFORE_SCENES
    static void Cleanup(      void) ;

    // event handlers
    static void HandleKeyEvent(  SDL_Event* event) ;
    static void HandleMouseEvent(SDL_Event* event) ;
    static void HandleUserEvent( SDL_Event* event) ;
    static void OnLoopCreation(  unsigned int* sceneNum , Loop** newLoop) ;
    static void OnSceneChange(   unsigned int* sceneNum) ;

    // user actions
    static void ToggleAutoSceneChange(void) ;
    static void ToggleRecordingState( void) ;
    static void ToggleNextScene(      void) ;
    static void DeleteLoop(           unsigned int sceneN , unsigned int loopN) ;
    static void DeleteLastLoop(       void) ;
    static void IncLoopVol(           unsigned int sceneN , unsigned int loopN , bool IsInc) ;
    static void ToggleLoopIsMuted(    unsigned int sceneN , unsigned int loopN) ;
    static void ToggleSceneIsMuted(   void) ;
    static void ToggleEditMode(       void) ;
    static void ResetScene(           unsigned int sceneN) ;
    static void ResetCurrentScene(    void) ;
    static void Reset(                void) ;

    // helpers
    static void UpdateView(unsigned int sceneN) ;
    static void OOM(       void) ;
} ;

#endif // #ifndef _LOOPIDITY_H_
