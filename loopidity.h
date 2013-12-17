
#ifndef _LOOPIDITY_H_
#define _LOOPIDITY_H_


//#define MEMORY_CHECK              1 // if 0 choose DEFAULT_BUFFER_SIZE wisely
//#define INIT_LOOPIDITY            1
#define INIT_JACK                 1
//#define LOOP_COUNTER              1
//#define DSP                       1
#define USER_DEFINED_BUFFER       1 // TODO: user defined buffer sizes
#define SCENE_NFRAMES_EDITABLE    1
#define DRAW_STATUS               1
#define DRAW_SCENES               1
#define DRAW_SCOPES               1
#define DRAW_EDIT_HISTOGRAM       SCENE_NFRAMES_EDITABLE && 1
#define DRAW_DEBUG_TEXT           1
#define SCAN_LOOP_PEAKS_DATA      1
#define SCAN_TRANSIENT_PEAKS_DATA 1
#define DEBUG_TRACE               1

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
#  define DRAW_DEBUG_TEXT_L Trace::SetDbgTextL() ;
#  define DRAW_DEBUG_TEXT_R Trace::SetDbgTextR() ;
#else
#  define DRAW_DEBUG_TEXT_L ;
#  define DRAW_DEBUG_TEXT_R ;
#endif // #if DRAW_DEBUG_TEXT

// Trace class features
#if DEBUG_TRACE
#  define DEBUG_TRACE_EVS   1
#  define DEBUG_TRACE_IN    1
#  define DEBUG_TRACE_OUT   1
#  define DEBUG_TRACE_CLASS 0
#endif // #if DEBUG_TRACE


// quantities
#define DEFAULT_BUFFER_SIZE 33554432 // 2^25 (approx 3 min @ 48k)
//#define DEFAULT_BUFFER_SIZE 25165824 // 1024 * 1024 * 24 (approx 135 sec @ 48k)
//#define DEFAULT_BUFFER_SIZE 16777216 // 2^24 (approx 90 sec @ 48k)
//#define DEFAULT_BUFFER_SIZE 8388608  // 2^23 (approx 45 sec @ 48k)
//#define DEFAULT_BUFFER_SIZE 2097152  // 2^21 (approx 10 sec @ 48k)
//#define DEFAULT_BUFFER_SIZE 1048576  // 2^20 (approx 5 sec @ 48k)

// TODO: implement setting N_CHANNELS via cmd line arg - GetTransientPeaks and updateVUMeters are especially brittle now
#define N_INPUT_CHANNELS  2 // TODO: nyi - only used for memory check and scope cache
#define N_OUTPUT_CHANNELS 2 // TODO: nyi - only used for N_PORTS
#define N_PORTS           N_INPUT_CHANNELS + N_OUTPUT_CHANNELS // TODO: nyi - only used for scope cache
#define N_SCENES          3
#define N_LOOPS           9 // per scene
#define LOOP_VOL_INC      0.1

#define PEAK_RADIUS         50
#define LOOP_DIAMETER       ((PEAK_RADIUS * 2) + 1)
#define N_PEAKS_COURSE      LOOP_DIAMETER
#if DRAW_EDIT_HISTOGRAM
#  define N_PEAKS_FINE      720 // should be divisible into 360
#else
#  define N_PEAKS_FINE      360 // should be divisible into 360
#endif // #if DRAW_EDIT_HISTOGRAM

// string constants
#define APP_NAME                "Loopidity"
#define INIT_MSG                "\nINIT:  Loopidity::Main(): init\n\n"
#define INIT_SUCCESS_MSG        "\nINIT:  Loopidity::Main(): init success - entering sdl loop\n\n"
#define INIT_FAIL_MSG           "\nERROR: Loopidity::Main(): init failed - quitting\n\n"
//#define CONNECT_ARG             "--connect"
#define MONITOR_ARG             "--nomon"
#define SCENE_CHANGE_ARG        "--noautoscenechange"
#define FREEMEM_FAIL_MSG        "ERROR: Could not determine available memory - quitting"
#define JACK_FAIL_MSG           "ERROR: Could not register JACK client - quitting"
#define ZERO_BUFFER_SIZE_MSG    "ERROR: initBufferSize is zero - quitting"
#define INSUFFICIENT_MEMORY_MSG "ERROR: Insufficient memory - quitting"
#define OUT_OF_MEMORY_MSG       "ERROR: Out of Memory"
#define GETPEAK_ERROR_MSG       "Loopidity::GetPeak(): subscript out of range\n"

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
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <vector>

#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_rotozoom.h>
#include <SDL_ttf.h>
#include <X11/Xlib.h>

#include "jack_io.h"       // must be first Sample typedef declared here
#include "loopidity_sdl.h"
#include "scene.h"
#include "scene_sdl.h"
#include "trace.h"


using namespace std ;


class Loopidity
{
  friend class JackIO ;
  friend class Trace ;

    private:

    /* class side private variables */

    // recording state
    static unsigned int CurrentSceneN ;
    static unsigned int NextSceneN ;
//    static bool         IsRolling ;
    static bool         ShouldSceneAutoChange ;

static bool IsEditMode ;

    // audio data
    static Scene*       Scenes[N_SCENES] ;
    static SceneSdl*    SdlScenes[N_SCENES] ;
    static unsigned int NFramesPerPeriod ;
    static unsigned int NFramesPerGuiInterval ;

    // transient sample data
    static Sample*        Buffer1 ;
    static Sample*        Buffer2 ;
    static vector<Sample> PeaksIn ;
    static vector<Sample> PeaksOut ;
    static Sample         TransientPeaks[N_PORTS] ;
    static Sample         TransientPeakInMix ;
    static Sample         TransientPeakOutMix ;

  public:

    /* class side public functions */

    // setup
    static bool Init(   bool shouldMonitorInputs , bool shouldAutoSceneChange ,
                        unsigned int recordBufferSize) ;
    static void Cleanup(void) ;

    // getters/setters
//    static void            SetNFramesPerPeriod(   unsigned int nFrames) ;
    static unsigned int    GetCurrentSceneN(      void) ;
    static unsigned int    GetNextSceneN(         void) ;
//    static unsigned int    GetLoopPos(            void) ;
//    static bool            GetIsRolling(          void) ;
//    static bool            GetShouldSaveLoop(     void) ;
//    static bool            GetDoesPulseExist(     void) ;
    static bool            GetIsEditMode(         void) ;
    static Sample*         GetTransientPeakIn(    void) ;
//    static Sample*         GetTransientPeakOut(   void) ;
    static Sample          GetPeak(               Sample* buffer , unsigned int nFrames) ;

    // event handlers
    static void HandleKeyEvent(  SDL_Event* event) ;
    static void HandleMouseEvent(SDL_Event* event) ;
    static void HandleUserEvent( SDL_Event* event) ;
    static void OnLoopCreation(  unsigned int sceneN , Loop* newLoop) ;
    static void OnSceneChange(   unsigned int sceneN) ;

    // user actions
    static void ToggleAutoSceneChange(void) ;
    static void ToggleState(          void) ;
    static void ToggleScene(          void) ;
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
    static void ScanTransientPeaks(void) ;
    static void UpdateView(        unsigned int sceneN) ;
    static void OOM(               void) ;

    // main
    static int Main(int argc , char** argv) ;


    /* class side private functions */

    // getters/setters
    static void SetMetaData(unsigned int sampleRate , unsigned int frameSize ,
                            unsigned int nFramesPerPeriod) ;
} ;


#endif // #ifndef _LOOPIDITY_H_
