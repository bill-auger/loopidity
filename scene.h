
#ifndef _SCENE_H_
#define _SCENE_H_


#include "loopidity.h"


using namespace std ;


class InvalidMetadataException: public exception
  { virtual const char* what() const throw() { return INVALID_METADATA_MSG ; } } ;


class Loop
{
  friend class JackIO ;
  friend class Loopidity ;
  friend class Scene ;
friend class SceneSdl ;


  private:

    /* Loop class side private funcrtions  */

    Loop(unsigned int nFrames) ;
    ~Loop() ;


    /* Loop instance side private varables */

    // audio data
    Sample* buffer1 ;
    Sample* buffer2 ;

    // peaks cache
    Sample peaksFine[N_PEAKS_FINE] ;
    Sample peaksCourse[N_PEAKS_COURSE] ;

    // loop state
    float vol ;
    bool  isMuted ;


  public:

    /* Loop instance side public functions */

    Sample getPeakFine(  unsigned int peakN) ;
    Sample getPeakCourse(unsigned int peakN) ;
} ;


class Scene
{
  friend class JackIO ;
  friend class Loopidity ;
  friend class LoopiditySdl ;
  friend class SceneSdl ;
  friend class Trace ;


  public:

    /* Scene class side public constants */

    static const unsigned int N_FINE_PEAKS ;
    static const unsigned int N_COURSE_PEAKS ;


  private:

    /* Scene class side private constants */
    static const InvalidMetadataException* INVALID_METADATA_EXCEPTION ;


    /* Scene class side private varables */

    // peaks cache
    static float        FinePeaksPerCoursePeak ;
    static unsigned int NFinePeaksPerCoursePeak ;

    // sample metedata
    static unsigned int SampleRate ;
    static unsigned int FramesPerPeriod ;
    static unsigned int TriggerLatencySize ;
#if SCENE_NFRAMES_EDITABLE
    static unsigned int MinLoopSize ;
#endif // #if SCENE_NFRAMES_EDITABLE
#if SCENE_NFRAMES_EDITABLE && INIT_JACK_BEFORE_SCENES
    static unsigned int BytesPerFrame ;
    static unsigned int BeginFrameN ;
    static unsigned int EndFrameN ;
#endif // #if SCENE_NFRAMES_EDITABLE && INIT_JACK_BEFORE_SCENES
#if !SCENE_NFRAMES_EDITABLE || !INIT_JACK_BEFORE_SCENES
    static unsigned int RecordBufferSize ;
#endif // #if !SCENE_NFRAMES_EDITABLE || !WAIT_FOR_JACK_INITWAIT_FOR_JACK_INIT


    /* Scene class side private functions */

    // setup
#if INIT_JACK_BEFORE_SCENES
    Scene(unsigned int sceneNum) ;
#else
#  if SCENE_NFRAMES_EDITABLE
    Scene(unsigned int sceneNum , unsigned int endFrameN) ;
#  else
    Scene(unsigned int sceneNum , unsigned int recordBufferSize) ;
#  endif // #if SCENE_NFRAMES_EDITABLE
#endif // #if INIT_JACK_BEFORE_SCENES

    // getters/setters
#if INIT_JACK_BEFORE_SCENES
#  if SCENE_NFRAMES_EDITABLE
    static void SetMetaData(unsigned int sampleRate , unsigned int nFramesPerPeriod ,
                            unsigned int bytesPerFrame , unsigned int minLoopSize ,
                            unsigned int triggerLatencySize ,
                            unsigned int beginFrameN , unsigned int endFrameN) ;
#  else
    static void SetMetaData(unsigned int sampleRate , unsigned int nFramesPerPeriod ,
                            unsigned int recordBufferSize) ;
#  endif // #if SCENE_NFRAMES_EDITABLE
#else
    static void SetMetaData(unsigned int sampleRate , unsigned int nFramesPerPeriod) ;
#endif // #if INIT_JACK_BEFORE_SCENES


    /* Scene instance side private varables */

    // identity
    unsigned int sceneN ;

    // audio data
    list<Loop*> loops ;

    // peaks cache
    float        hiScenePeaks[N_PEAKS_FINE] ; // the loudest of the currently playing samples in the current scene
    float        hiLoopPeaks[NUM_LOOPS] ;     // the loudest sample for each loop of the current scene
    float        highestScenePeak ;           // the loudest of all samples in all loops of the current scene (nyi)
    unsigned int nFramesPerPeak ;             // # of samples per fine peak (hiScenePeaks , hiLoopPeaks , peaksFine)

    // buffer iteration
    unsigned int currentFrameN ;
#if SCENE_NFRAMES_EDITABLE
    unsigned int beginFrameN ;
    unsigned int endFrameN ;
#endif // #if SCENE_NFRAMES_EDITABLE
    unsigned int nFrames ;
    unsigned int nBytes ;
    unsigned int nSeconds ;

    // scene state
    bool shouldSaveLoop ;
    bool doesPulseExist ;
    bool isMuted ;


    /* Scene instance side private functions */

    // scene state
    void beginRecording(      void) ;
    void toggleRecordingState(void) ;

    // audio data
    bool addLoop(   Loop* newLoop) ;
    void deleteLoop(unsigned int loopN) ;
    void reset(     void) ;

    // peaks cache
    void scanPeaks(  Loop* loop , unsigned int loopN) ;
    void rescanPeaks(void) ;

    // getters/setters
    Loop* getLoop(unsigned int loopN) ;
//    unsigned int getLoopPos(  void) ;


  public:

    /* Scene instance side public functions */

    // getters/setters
    unsigned int getSceneN() ;
    unsigned int getDoesPulseExist() ;
    unsigned int getNLoops() ;
    unsigned int getCurrentPeakN() ;
    float        getCurrentSeconds() ;
    float        getTotalSeconds() ;
} ;


#endif // #ifndef _SCENE_H_
