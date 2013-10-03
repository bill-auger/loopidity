
#ifndef _LOOPIDITY_H_
#define _LOOPIDITY_H_


// DEBUG begin
#define DEBUG 1
#if DEBUG
#define MEMORY_CHECK 1 // if 0 choose DEFAULT_BUFFER_SIZE wisely
#define INIT_LOOPIDITY 1
#define INIT_JACK 1
#define LOOP_COUNTER 1
#define DSP 1
// TODO: user defined buffer sizes
#define STATIC_BUFFER_SIZE 1

#define DRAW_STATUS 1
#define DRAW_SCENES 1
#define DRAW_SCOPES 1
#define DRAW_DEBUG_TEXT 1
#define DEBUG_TRACE 0 // TODO: implement Trace class
#endif // #if DEBUG

#if DRAW_STATUS
#define DRAW_MODE 0
/*
#define UPDATE_MEMORY 0
#define UPDATE_LOOP_PROGRESS 0
#define UPDATE_VU 0
#define VARDUMP DEBUG && 0
*/
#endif // #if DRAW_STATUS

#if DRAW_SCENES
#define DRAW_SCENE_SCOPE 1
#define DRAW_HISTOGRAMS 1
#define DRAW_MUTED_HISTOGRAMS 1
#define DRAW_PEAK_RINGS 1
#define DRAW_LOOPS 1
#define DRAW_MUTED_LOOPS 0
#define DRAW_RECORDING_LOOP DRAW_LOOPS && 1
#define DRAW_CURRENT_SCENE_INDICATOR 1
#endif // #if DRAW_SCENES

#define SCAN_LOOP_PEAKS_DATA 1
#define SCAN_TRANSIENT_PEAKS_DATA 1

#if DEBUG_TRACE
#define DEBUG_TRACE_EVS DEBUG_TRACE && 1
#define DEBUG_TRACE_IN DEBUG_TRACE && 1
#define DEBUG_TRACE_OUT DEBUG_TRACE && 1
#define DEBUG_TRACE_CLASS DEBUG_TRACE && 0
#define TRACE_EVS Loopidity::TraceEvs
#define TRACE_IN Loopidity::TraceIn
#define TRACE_OUT Loopidity::TraceOut
#define TRACE_SCENE Loopidity::TraceScene

#define DEBUG_TRACE_LOOPIDITY_TOGGLESTATE_IN if (TRACE_EVS(CurrentSceneN)) printf("\nUSER: SDLK_SPACE --> Loopidity::ToggleState(%d)\n\n" , CurrentSceneN) ; if (TRACE_IN(CurrentSceneN) && !TRACE_SCENE("Loopidity::ToggleState(%d)  IN" , Scenes[CurrentSceneN])) return ;
#define DEBUG_TRACE_LOOPIDITY_TOGGLESTATE_OUT if (TRACE_OUT(CurrentSceneN)) TRACE_SCENE("Loopidity::ToggleState(%d) OUT" , Scenes[CurrentSceneN]) ;
#define DEBUG_TRACE_LOOPIDITY_TOGGLESCENE_IN if (TRACE_EVS(CurrentSceneN)) printf("\nUSER: SDLK_KP0 --> Loopidity::ToggleScene(%d)\n\n" , CurrentSceneN) ; if (TRACE_IN(CurrentSceneN) && !TRACE_SCENE("Loopidity::ToggleScene(%d)  IN" , Scenes[CurrentSceneN])) return ;
#define DEBUG_TRACE_LOOPIDITY_TOGGLESCENE_OUT if (TRACE_OUT(CurrentSceneN)) TRACE_SCENE("Loopidity::ToggleScene(%d) OUT" , Scenes[CurrentSceneN]) ;
#define DEBUG_TRACE_LOOPIDITY_DELETELASTLOOP_IN if (TRACE_EVS(CurrentSceneN)) printf("\nUSER: SDLK_ESCAPE --> Loopidity::DeleteLastLoop(%d)\n\n" , CurrentSceneN) ; if (TRACE_IN(CurrentSceneN) && !TRACE_SCENE("Loopidity::DeleteLastLoop(%d)  IN" , Scenes[CurrentSceneN])) return ;
#define DEBUG_TRACE_LOOPIDITY_DELETELASTLOOP_OUT if (TRACE_OUT(CurrentSceneN)) TRACE_SCENE("Loopidity::DeleteLoop(%d) OUT" , Scenes[CurrentSceneN]) ;
#define DEBUG_TRACE_LOOPIDITY_DELETELOOP_IN if (TRACE_EVS(sceneN)) printf("\nUSER: SDL_BUTTON_MIDDLE --> Loopidity::DeleteLoop(%d)\n\n" , sceneN) ; if (TRACE_IN(sceneN) && !TRACE_SCENE("Loopidity::DeleteLoop(%d)  IN" , Scenes[sceneN])) return ;
#define DEBUG_TRACE_LOOPIDITY_DELETELOOP_OUT if (TRACE_OUT(sceneN)) TRACE_SCENE("Loopidity::DeleteLoop(%d) OUT" , Scenes[sceneN]) ;
#define DEBUG_TRACE_LOOPIDITY_INCLOOPVOL_IN if (TRACE_EVS(sceneN)) printf("\nUSER: %s --> Loopidity::IncLoopVol(%d)  IN vol=%f\n\n" , (isInc)? "SDL_BUTTON_WHEELUP" : "SDL_BUTTON_WHEELDOWN" , sceneN , Scenes[sceneN]->getLoop(loopN)->vol) ;
#define DEBUG_TRACE_LOOPIDITY_INCLOOPVOL_OUT if (DEBUG_TRACE_OUT) { char event[128] ; sprintf(event , "%s OUT vol=%3.1f\n\n" , "Loopidity::IncLoopVol(%d)" , *vol) ; TRACE_SCENE(event , Scenes[sceneN]) ; }
#define DEBUG_TRACE_LOOPIDITY_TOGGLELOOPISMUTED_IN if (TRACE_EVS(sceneN)) { printf("\nUSER: SDLK_KP_ENTER --> Loopidity::ToggleLoopIsMuted(%d)\n\n" , sceneN) ; printf("\nUSER: SDL_BUTTON_LEFT --> Loopidity::ToggleLoopIsMuted(%d)\n\n" , sceneN) ; } if (TRACE_IN(sceneN) && !TRACE_SCENE("Loopidity::ToggleLoopIsMuted(%d)  IN" , Scenes[sceneN])) return ;
#define DEBUG_TRACE_LOOPIDITY_TOGGLELOOPISMUTED_OUT if (TRACE_OUT(sceneN)) TRACE_SCENE("Loopidity::ToggleLoopIsMuted(%d) OUT" , Scenes[sceneN]) ;
#define DEBUG_TRACE_LOOPIDITY_RESETSCENE_IN if (TRACE_EVS(CurrentSceneN)) printf("\nUSER: KMOD_RSHIFT+SDLK_ESCAPE --> Loopidity::ResetScene(%d)\n\n" , sceneN) ; if (TRACE_IN(CurrentSceneN) && !TRACE_SCENE("Loopidity::ResetCurrentScene(%d)  IN" , Scenes[CurrentSceneN])) return ;
#define DEBUG_TRACE_LOOPIDITY_RESETSCENE_OUT if (DEBUG_TRACE_OUT) TRACE_SCENE("Loopidity::ResetScene(%d) OUT" , Scenes[sceneN]) ;
#define DEBUG_TRACE_LOOPIDITY_RESET_IN if (TRACE_EVS(CurrentSceneN)) printf("\nUSER: KMOD_RCTRL+SDLK_ESCAPE --> Loopidity::Reset(%d)\n\n" , CurrentSceneN) ; if (TRACE_IN(CurrentSceneN) && !TRACE_SCENE("Loopidity::Reset(%d)  IN" , Scenes[CurrentSceneN])) return ;
#define DEBUG_TRACE_LOOPIDITY_RESET_OUT if (TRACE_OUT(CurrentSceneN)) TRACE_SCENE("Loopidity::Reset(%d) OUT" , Scenes[CurrentSceneN]) ;
#define DEBUG_TRACE_LOOPIDITY_ONLOOPCREATION_IN if (TRACE_EVS(sceneN)) printf("\nEVT_NEW_LOOP --> Loopidity::OnLoopCreation(%d)\n\n" , sceneN) ; if (TRACE_IN(sceneN) && !TRACE_SCENE("Loopidity::OnLoopCreation(%d)  IN" , Scenes[sceneN])) return ;
#define DEBUG_TRACE_LOOPIDITY_ONLOOPCREATION_OUT if (TRACE_OUT(sceneN)) TRACE_SCENE("Loopidity::OnLoopCreation(%d) OUT" , Scenes[sceneN]) ;
#define DEBUG_TRACE_LOOPIDITY_ONSCENECHANGE_IN if (TRACE_EVS(CurrentSceneN)) printf("\nEVT_SCENE_CHANGED --> Loopidity::OnSceneChange(%d)\n\n" , CurrentSceneN) ; if (TRACE_IN(CurrentSceneN) && !TRACE_SCENE("Loopidity::OnSceneChange(%d)  IN" , Scenes[CurrentSceneN])) return ;
#define DEBUG_TRACE_LOOPIDITY_ONSCENECHANGE_OUT if (TRACE_OUT(nextSceneN)) TRACE_SCENE("Loopidity::OnSceneChange(%d)  OUT" , Scenes[nextSceneN]) ;
#define DEBUG_TRACE_LOOPIDITY_OOM_IN TRACE_SCENE("Loopidity::OOM(%d)   IN" , Scenes[CurrentSceneN]) ;
#define DEBUG_TRACE_LOOPIDITYSDL_HANDLEKEYEVENT if (DEBUG_TRACE_EVS) switch (event->key.keysym.sym) { case SDLK_SPACE: printf("\nKEY: SDLK_SPACE\n") ; break ; case SDLK_KP0: printf("\nKEY: SDLK_KP0\n") ; break ; case SDLK_KP_ENTER: printf("\nKEY: SDLK_KP_ENTER\n") ; break ; case SDLK_ESCAPE: printf("\nKEY: SDLK_ESCAPE\n") ;  break ; default: break ; }
#define DEBUG_TRACE_SCENE_ADDLOOP_IN if (TRACE_IN(sceneN) && !TRACE_SCENE("Scene::addLoop(%d)  IN" , this)) return false ;
#define DEBUG_TRACE_SCENE_ADDLOOP_OUT if (TRACE_OUT(sceneN)) TRACE_SCENE("Scene::addLoop(%d) OUT" , this) ;
#define DEBUG_TRACE_SCENE_DELETELOOP_IN if (TRACE_IN(sceneN) && !TRACE_SCENE("Scene::deleteLoop(%d)  IN" , this)) return ;
#define DEBUG_TRACE_SCENE_DELETELOOP_OUT if (TRACE_OUT(sceneN)) TRACE_SCENE("Scene::deleteLoop(%d) OUT" , this) ;
#define DEBUG_TRACE_SCENE_RESET_IN if (TRACE_IN(sceneN) && !TRACE_SCENE("Scene::reset(%d)  IN" , this)) return ; ;
#define DEBUG_TRACE_SCENE_RESET_OUT if (TRACE_OUT(sceneN)) TRACE_SCENE("Scene::reset(%d) OUT" , this) ;
#define DEBUG_TRACE_SCENE_SCANPEAKS_IN if (TRACE_IN(sceneN) && !TRACE_SCENE("Scene::scanPeaks(%d)  IN" , this)) return ;
#define DEBUG_TRACE_SCENE_SCANPEAKS_OUT if (TRACE_OUT(sceneN)) TRACE_SCENE("Scene::scanPeaks(%d) OUT" , this) ;
#define DEBUG_TRACE_SCENE_RESCANPEAKS_IN if (TRACE_IN(sceneN) && !TRACE_SCENE("Scene::rescanPeaks(%d)  IN" , this)) return ;
#define DEBUG_TRACE_SCENE_RESCANPEAKS_OUT if (TRACE_OUT) TRACE_SCENE("Scene::rescanPeaks(%d) OUT" , this) ;
#define DEBUG_TRACE_SCENE_TOGGLESTATE_IN if (TRACE_IN(sceneN) && !TRACE_SCENE("Scene::toggleState(%d)      IN" , this)) return ;
#define DEBUG_TRACE_SCENE_TOGGLESTATE_OUT if (TRACE_OUT(sceneN)) TRACE_SCENE("Scene::toggleState(%d)     OUT" , this) ;
#define DEBUG_TRACE_SCENESDL_UPDATESTATUS_IN if (TRACE_IN(scene->sceneN)) TRACE_SCENE("SceneSdl::updateState(%d)   IN" , scene) ;
#define DEBUG_TRACE_SCENESDL_UPDATESTATUS_OUT if (TRACE_OUT(scene->sceneN)) TRACE_SCENE("SceneSdl::updateState(%d)  OUT" , scene) ;
#define DEBUG_TRACE_SCENESDL_ADDLOOP_IN if (TRACE_IN(scene->sceneN) && (loopImgs.size() != scene->loops.size() - 1) && !TRACE_SCENE("SceneSdl::addLoop(%d) ERR" , scene)) return ;
#define DEBUG_TRACE_SCENESDL_ADDLOOP_OUT if (TRACE_OUT(scene->sceneN)) TRACE_SCENE("SceneSdl::addLoop(%d) OUT" , scene) ;
#define DEBUG_TRACE_SCENESDL_DELETELOOP_IN if (TRACE_IN(scene->sceneN) && !TRACE_SCENE("SceneSdl::deleteLoop(%d)  IN" , scene)) return ;
#define DEBUG_TRACE_SCENESDL_DELETELOOP_OUT if (TRACE_OUT(scene->sceneN)) TRACE_SCENE("SceneSdl::deleteLoop(%d) OUT" , scene) ;
#else
#define DEBUG_TRACE_LOOPIDITY_TOGGLESTATE_IN ;
#define DEBUG_TRACE_LOOPIDITY_TOGGLESTATE_OUT ;
#define DEBUG_TRACE_LOOPIDITY_TOGGLESCENE_IN ;
#define DEBUG_TRACE_LOOPIDITY_TOGGLESCENE_OUT ;
#define DEBUG_TRACE_LOOPIDITY_DELETELASTLOOP_IN ;
#define DEBUG_TRACE_LOOPIDITY_DELETELASTLOOP_OUT ;
#define DEBUG_TRACE_LOOPIDITY_DELETELOOP_IN ;
#define DEBUG_TRACE_LOOPIDITY_DELETELOOP_OUT ;
#define DEBUG_TRACE_LOOPIDITY_INCLOOPVOL_IN ;
#define DEBUG_TRACE_LOOPIDITY_INCLOOPVOL_OUT ;
#define DEBUG_TRACE_LOOPIDITY_TOGGLELOOPISMUTED_IN ;
#define DEBUG_TRACE_LOOPIDITY_TOGGLELOOPISMUTED_OUT ;
#define DEBUG_TRACE_LOOPIDITY_RESETSCENE_IN ;
#define DEBUG_TRACE_LOOPIDITY_RESETSCENE_OUT ;
#define DEBUG_TRACE_LOOPIDITY_RESET_IN ;
#define DEBUG_TRACE_LOOPIDITY_RESET_OUT ;
#define DEBUG_TRACE_LOOPIDITY_ONLOOPCREATION_IN ;
#define DEBUG_TRACE_LOOPIDITY_ONLOOPCREATION_OUT ;
#define DEBUG_TRACE_LOOPIDITY_ONSCENECHANGE_IN ;
#define DEBUG_TRACE_LOOPIDITY_ONSCENECHANGE_OUT ;
#define DEBUG_TRACE_LOOPIDITY_OOM_IN ;
#define DEBUG_TRACE_LOOPIDITYSDL_HANDLEKEYEVENT ;
#define DEBUG_TRACE_SCENE_ADDLOOP_IN ;
#define DEBUG_TRACE_SCENE_ADDLOOP_OUT ;
#define DEBUG_TRACE_SCENE_DELETELOOP_IN ;
#define DEBUG_TRACE_SCENE_DELETELOOP_OUT ;
#define DEBUG_TRACE_SCENE_RESET_IN ;
#define DEBUG_TRACE_SCENE_RESET_OUT ;
#define DEBUG_TRACE_SCENE_SCANPEAKS_IN ;
#define DEBUG_TRACE_SCENE_SCANPEAKS_OUT ;
#define DEBUG_TRACE_SCENE_RESCANPEAKS_IN ;
#define DEBUG_TRACE_SCENE_RESCANPEAKS_OUT ;
#define DEBUG_TRACE_SCENE_TOGGLESTATE_IN ;
#define DEBUG_TRACE_SCENE_TOGGLESTATE_OUT ;
#define DEBUG_TRACE_SCENESDL_UPDATESTATUS_IN ;
#define DEBUG_TRACE_SCENESDL_UPDATESTATUS_OUT ;
#define DEBUG_TRACE_SCENESDL_ADDLOOP_IN ;
#define DEBUG_TRACE_SCENESDL_ADDLOOP_OUT ;
#define DEBUG_TRACE_SCENESDL_DELETELOOP_IN ;
#define DEBUG_TRACE_SCENESDL_DELETELOOP_OUT ;
#endif // #if DEBUG_TRACE
// DEBUG end


// quantities
#define DEFAULT_BUFFER_SIZE 33554432 // 2^25 (approx 3 min @ 48k)
//#define DEFAULT_BUFFER_SIZE 25165824 // 1024 * 1024 * 24 (approx 135 sec @ 48k)
//#define DEFAULT_BUFFER_SIZE 16777216 // 2^24 (approx 90 sec @ 48k)
//#define DEFAULT_BUFFER_SIZE 8388608 // 2^23 (approx 45 sec @ 48k)
//#define DEFAULT_BUFFER_SIZE 2097152 // 2^21 (approx 10 sec @ 48k)
//#define DEFAULT_BUFFER_SIZE 1048576 // 2^20 (approx 5 sec @ 48k)

// TODO: implement setting N_CHANNELS via cmd line arg - GetTransientPeaks and updateVUMeters are especially brittle now
#define N_INPUT_CHANNELS 2 // TODO: nyi - only used for memory check and scope cache
#define N_OUTPUT_CHANNELS 2 // TODO: nyi - only used for N_PORTS
#define N_PORTS N_INPUT_CHANNELS + N_OUTPUT_CHANNELS // TODO: nyi - only used for scope cache
#define N_SCENES 3
#define N_LOOPS 9 // N_LOOPS_PER_SCENE
#define LOOP_VOL_INC 0.1

#define SDL_PEAK_RADIUS 50
#define SDL_LOOP_DIAMETER ((SDL_PEAK_RADIUS * 2) + 1)
#define N_PEAKS_COURSE SDL_LOOP_DIAMETER
#define N_PEAKS_FINE 360 // should be divisible into 360

// string constants
#define APP_NAME "Loopidity"
//#define CONNECT_ARG "--connect"
#define MONITOR_ARG "--nomon"
#define SCENE_CHANGE_ARG "--noautoscenechange"
#define JACK_FAIL_MSG "ERROR: Could not register JACK client - quitting"
#define FREEMEM_FAIL_MSG "ERROR: Could not determine available memory - quitting"
#define ZERO_BUFFER_SIZE_MSG "ERROR: initBufferSize is zero - quitting"
#define INSUFFICIENT_MEMORY_MSG "ERROR: Insufficient memory - quitting"
#define OUT_OF_MEMORY_MSG "ERROR: Out of Memory"
#define INIT_SUCCESS_MSG "\nINIT: main(): LoopiditySdl::Init()         OUT success - entering sdl loop\n\n"
#define INIT_FAIL_MSG "\nERROR: main(): LoopiditySdl::Init()         OUT failed - quitting\n\n"

// sdl user events
#define EVT_NEW_LOOP 1
#define EVT_SCENE_CHANGED 2

// error states
#define JACK_INIT_SUCCESS 0
#define JACK_FAIL 1
#define JACK_MEM_FAIL 2

// aliases
#define SAMPLE jack_default_audio_sample_t


// dependencies
#ifdef __cplusplus
#include <cstdlib>
#include <list>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>
#else
#include <list.h>
#include <sstream.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector.h>

#endif

#ifdef __APPLE__
#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_rotozoom.h>
#include <SDL/SDL_ttf.h>
#else
#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_rotozoom.h>
#include <SDL_ttf.h>
#endif

#include <X11/Xlib.h>


//#include "trace.h"
#include "jack_io.h"
#include "loopidity_sdl.h"
#include "scene.h"
#include "scene_sdl.h"


using namespace std ;


class Loopidity
{
	public:

		// user actions
static bool SetRecordBufferSize(unsigned int recordBufferSize) ;
static SceneSdl** Init(bool isMonitorInputs , bool isAutoSceneChange) ;
    static void ToggleAutoSceneChange() ;
		static void ToggleScene() ;
    static void ToggleState() ;
		static void DeleteLoop(unsigned int sceneN , unsigned int loopN) ;
    static void DeleteLastLoop() ;
    static void IncLoopVol(unsigned int sceneN , unsigned int loopN , bool IsInc) ;
		static void ToggleLoopIsMuted(unsigned int sceneN , unsigned int loopN) ;
		static void ToggleSceneIsMuted() ;
		static void SetStatus(unsigned int sceneN) ;
    static void ResetScene(unsigned int sceneN) ;
    static void ResetCurrentScene() ;
		static void Reset() ;
static void Cleanup() ;

		// getters/setters
static void SetMetaData(unsigned int sampleRate , unsigned int frameSize , unsigned int nFramesPerPeriod) ;
static Scene** GetScenes() ;
static SceneSdl** GetSdlScenes() ;
		static unsigned int GetCurrentSceneN() ;
		static unsigned int GetNextSceneN() ;
    static unsigned int GetLoopPos() ;
//    static bool GetIsRolling() ;
		static bool GetShouldSaveLoop() ;
		static bool GetDoesPulseExist() ;
		static void SetNFramesPerPeriod(unsigned int nFrames) ;
		static vector<SAMPLE>* GetPeaksInCache() ;
		static vector<SAMPLE>* GetPeaksOutCache() ;
		static SAMPLE* GetTransientPeaksCache() ;
		static SAMPLE* GetTransientPeakIn() ;
		static SAMPLE* GetTransientPeakOut() ;
		static SAMPLE GetPeak(SAMPLE* buffer , unsigned int nFrames) ;

		// event handlers
		static void OnLoopCreation(unsigned int sceneN , Loop* newLoop) ;
		static void OnSceneChange(unsigned int sceneN) ;

		// helpers
		static void ScanTransientPeaks() ;
		static void UpdateView(unsigned int sceneN) ;
		static void OOM() ;

	private:

		// recording state
		static unsigned int CurrentSceneN ;
		static unsigned int NextSceneN ;
//		static bool IsRolling ;
		static bool ShouldSceneAutoChange ;

		// audio data
		static Scene* Scenes[N_SCENES] ;
		static SceneSdl* SdlScenes[N_SCENES] ;
		static unsigned int NFramesPerPeriod ;
		static unsigned int NFramesPerGuiInterval ;

		// transient sample data
		static SAMPLE* Buffer1 ;
		static SAMPLE* Buffer2 ;
		static vector<SAMPLE> PeaksIn ;
		static vector<SAMPLE> PeaksOut ;
		static SAMPLE TransientPeaks[N_PORTS] ;
		static SAMPLE TransientPeakInMix ;
		static SAMPLE TransientPeakOutMix ;


// DEBUG
//public: static void DbgPad(unsigned int colN , string* str) ;
/*
public: static unsigned int GetNLoops(unsigned int sceneN) ;
public: static unsigned int GetHistogramImgsSize(unsigned int sceneN) ;
public: static unsigned int GetLoopImgsSize(unsigned int sceneN) ;
*/
public: static bool SanityCheck(unsigned int sceneN) ;
public: static bool TraceScene(const char* senderTemplate , Scene* scene) ;

#if DEBUG_TRACE
public: static bool TraceEvs(unsigned int sceneN) { return (DEBUG_TRACE_EVS || !Loopidity::SanityCheck(sceneN)) ; }
public: static bool TraceIn(unsigned int sceneN) { return (DEBUG_TRACE_IN || !Loopidity::SanityCheck(sceneN)) ; }
public: static bool TraceOut(unsigned int sceneN) { return (DEBUG_TRACE_OUT || !Loopidity::SanityCheck(sceneN)) ; }
#endif // #if DEBUG_TRACE

#if DRAW_DEBUG_TEXT
public: static void SetDbgTextL() ; public: static void SetDbgTextR() ;
#endif // #if DRAW_DEBUG_TEXT
// DEBUG end
} ;


#endif // #ifndef _LOOPIDITY_H_
