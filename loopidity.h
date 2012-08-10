
#ifndef _LOOPIDITY_H_
#define _LOOPIDITY_H_


// DEBUG
#define DEBUG 1
#define MEMORY_CHECK 1 // if 0 choose DEFAULT_BUFFER_SIZE wisely
#define INIT_LOOPIDITY 1
#define INIT_JACK 1
#define LOOP_COUNTER 1
#define DSP 1
#define PASSTHRU 0
// TODO: user defined buffer sizes
#define STATIC_BUFFER_SIZE 1
// DEBUG end

// GUI DEBUG
#define DRAW_STATUS 0
#define DRAW_SCENES 1
#define DRAW_SCOPES 0

#if DRAW_STATUS
#define UPDATE_MEMORY 0
#define UPDATE_LOOP_PROGRESS 0
#define UPDATE_VU 0
#define VARDUMP DEBUG && 0
#endif

#if DRAW_SCENES
#define DRAW_PEAK_BARS 1
#define DRAW_HISTOGRAMS 1
#define DRAW_LOOP_PEAKS 1
#define DRAW_LOOPS 0
#define DRAW_LOOP_MASKS DRAW_LOOPS && 1
#define DRAW_LOOP_GRADIENTS DRAW_LOOPS && 1
#define DRAW_INACTIVE_SCENES 1
#define DRAW_SCENE_FADE DRAW_INACTIVE_SCENES && 1
#define DRAW_DEBUG_TEXT 1
#endif

#define SCAN_PEAKS_DATA 1
#define SCAN_TRANSIENT_PEAKS_DATA 0
// GUI DEBUG end


#ifdef __cplusplus
#include <cstdlib>
#include <vector>
#include <string>
#else
#include <stdlib.h>
#include <vector.h>
#include <string.h>
#endif

#ifdef __APPLE__
#include <SDL/SDL.h>
#else
#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#endif


// quantities
//#define DEFAULT_BUFFER_SIZE 33554432 // 2^25 (approx 3 min @ 48k)
//#define DEFAULT_BUFFER_SIZE 25165824 // 1024 * 1024 * 24 (approx 135 sec @ 48k)
//#define DEFAULT_BUFFER_SIZE 16777216 // 2^24 (approx 90 sec @ 48k)
#define DEFAULT_BUFFER_SIZE 8388608 // 2^23 (approx 45 sec @ 48k)
//#define DEFAULT_BUFFER_SIZE 2097152 // 2^21 (approx 10 sec @ 48k)
//#define DEFAULT_BUFFER_SIZE 1048576 // 2^20 (approx 5 sec @ 48k)

// TODO: implement setting N_CHANNELS via cmd line arg - GetTransientPeaks and updateVUMeters are especially brittle now
#define N_INPUT_CHANNELS 2 // TODO: nyi - only used for memory check and scope cache
#define N_OUTPUT_CHANNELS 2 // TODO: nyi - only used for N_PORTS
#define N_PORTS N_INPUT_CHANNELS + N_OUTPUT_CHANNELS // TODO: nyi - only used for scope cache
#define N_SCENES 3
#define N_LOOPS 9 // N_LOOPS_PER_SCENE
#define N_PEAKS 100 // should be divisible into 3600

// string constants
//#define CONNECT_ARG "--connect"
#define APP_NAME "Loopidity"
#define MONITOR_ARG "--nomon"
#define JACK_FAIL_MSG "ERROR: Could not register JACK client - quitting"
#define FREEMEM_FAIL_MSG "ERROR: Could not determine available memory - quitting"
#define ZERO_BUFFER_SIZE_MSG "ERROR: initBufferSize is zero - quitting"
//#define INSUFFICIENT_MEMORY_MSG "ERROR: Insufficient memory - quitting"

// aliases
#define SAMPLE jack_default_audio_sample_t


#include "jack_io.h"
#include "loopidity_sdl.h"


using namespace std ;


class Loop
{
	public:

		Loop(unsigned int nFrames) ;
		~Loop() ;

		SAMPLE* buffer1 ;
		SAMPLE* buffer2 ;
		float peaks[N_PEAKS] ;
} ;


class Scene
{
	friend class Loopidity ;
	friend class SceneSdl ;
	friend class JackIO ;

	public:

		// scene time
		float getCurrentSeconds() ;
		float getTotalSeconds() ;

	private:

		typedef Scene CLASSNAME ;
		Scene(unsigned int sceneN , unsigned int initBufferSize) ;
		virtual ~Scene() {}

		// identity
		unsigned int sceneN ;

		// audio data
		vector<Loop*> loops ;

		// peaks cache
// hiCurrentSample = the loudest of the currently playing samples in the current scene
// hiLoopSamples[] = the loudest of all samples for each loop of the current scene (nyi)
// highestLoopSample = the loudest of all samples in all loops of the current scene (nyi)
		float hiScenePeaks[N_PEAKS] ;
		float hiLoopPeaks[N_LOOPS] ;
		float highestScenePeak ;

		// buffer iteration
		unsigned int nFrames ;
		unsigned int nFramesPerPeak ;
		unsigned int frameN ;
		unsigned int nLoops ;

		// recording state
		bool isSaveLoop ;
		bool isPulseExist ;

		// audio data
		void addLoop(Loop* newLoop) ;
		void deleteLoop() ;
		void reset() ;

		// peaks cache
		void scanPeaks(Loop* loop , unsigned int loopN) ;
		void rescanPeaks() ;

		// recording state
    void setMode() ;

		// getters/setters
    unsigned int getLoopPos() ;
} ;


class Loopidity
{
	public:

		// user actions
    static bool Init(unsigned int recordBufferSize , bool isMonitorInputs) ;
		static void ToggleScene() ;
    static void SetMode() ;
    static void DeleteLoop() ;
    static void ResetCurrentScene() ;
		static void Reset() ;

		// getters/setters
		static Scene** GetScenes() ;
		static unsigned int GetCurrentSceneN() ;
		static unsigned int GetNextSceneN() ;
    static unsigned int GetLoopPos() ;
		static bool GetIsSaveLoop() ;
    static bool GetIsRecording() ;
		static bool GetIsPulseExist() ;
		static void SetNFramesPerPeriod(unsigned int nFrames) ;
		static vector<SAMPLE>* GetInPeaksCache() ;
		static vector<SAMPLE>* GetOutPeaksCache() ;
		static SAMPLE* GetTransientPeaksCache() ;

		// helpers
		static SAMPLE GetPeak(SAMPLE* buffer , unsigned int nFrames) ;
		static void ScanTransientPeaks() ;

	private:

		// recording state
		static bool IsRecording ;
		static unsigned int NextSceneN ;

		// audio data
		static Scene* Scenes[N_SCENES] ;
		static unsigned int NFramesPerPeriod ;

		// transient sample data
		static jack_port_t* InPort1 ;
		static jack_port_t* InPort2 ;
		static jack_port_t* OutPort1 ;
		static jack_port_t* OutPort2 ;
		static SAMPLE* RecordBuffer1 ;
		static SAMPLE* RecordBuffer2 ;
		static vector<SAMPLE> InPeaks ;
		static vector<SAMPLE> OutPeaks ;
		static SAMPLE TransientPeaks[N_PORTS] ;

// DEBUG
public: static void SetDbgLabels() ; static void Vardump() ;
//static void dbgAddScene() { Scenes.push_back(new Scene(DEFAULT_BUFFER_SIZE)) ; }
// DEBUG end
} ;


#endif // #ifndef _LOOPIDITY_H_
