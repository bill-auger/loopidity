
#ifndef _SCENE_H_
#define _SCENE_H_


#include "loopidity.h"


using namespace std ;


class Loop
{
  friend class JackIO ;
  friend class Loopidity ;
  friend class Scene ;
friend class SceneSdl ;

	public:

		SAMPLE getPeakFine(unsigned int peakN) ;
		SAMPLE getPeakCourse(unsigned int peakN) ;

	private:

		Loop(unsigned int nFrames) ;
		~Loop() ;

		// audio data
		SAMPLE* buffer1 ;
		SAMPLE* buffer2 ;

		// peaks cache
		SAMPLE peaksFine[N_PEAKS_FINE] ;
		SAMPLE peaksCourse[N_PEAKS_COURSE] ;

		// loop state
		float vol ;
		bool isMuted ;
} ;


class Scene
{
  friend class JackIO ;
  friend class Loopidity ;
  friend class LoopiditySdl ;
  friend class SceneSdl ;
  friend class Trace ;

	public:

		// scene progress
		unsigned int getCurrentPeakN() ;
		float getCurrentSeconds() ;
		float getTotalSeconds() ;

	private:

		typedef Scene CLASSNAME ;
		Scene(unsigned int scenen , unsigned int recordBufferSize) ;
		virtual ~Scene() {}

		// identity
		unsigned int sceneN ;

		// audio data
		list<Loop*> loops ;

		// peaks cache
		float hiScenePeaks[N_PEAKS_FINE] ; // the loudest of the currently playing samples in the current scene
		float hiLoopPeaks[N_LOOPS] ; // the loudest sample for each loop of the current scene
		float highestScenePeak ; // the loudest of all samples in all loops of the current scene (nyi)

		// sample metedata
		static unsigned int RecordBufferSize ;
		static unsigned int SampleRate ;
		static unsigned int FrameSize ;
		static unsigned int NFramesPerPeriod ;

		// buffer iteration
		unsigned int nFrames ;
		unsigned int nFramesPerPeak ;
		unsigned int frameN ;
		unsigned int nBytes ;

		// scene state
		bool isRolling ;
		bool shouldSaveLoop ;
		bool doesPulseExist ;
		bool isMuted ;

		// audio data
		bool addLoop(Loop* newLoop) ;
		void deleteLoop(unsigned int loopN) ;
		void startRolling() ;
		void reset() ;

		// peaks cache
		void scanPeaks(Loop* loop , unsigned int loopN) ;
		void rescanPeaks() ;

		// scene state
    void toggleState() ;

		// getters/setters
		static void SetMetaData(unsigned int sampleRate , unsigned int frameSize , unsigned int nFramesPerPeriod) ;
		Loop* getLoop(unsigned int loopN) ;
		bool getIsRolling() ;
    unsigned int getLoopPos() ;
} ;


#endif // #ifndef _SCENE_H_
