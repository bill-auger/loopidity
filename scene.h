
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

    /* instance side public functions */

  public:

    Sample getPeakFine(  unsigned int peakN) ;
    Sample getPeakCourse(unsigned int peakN) ;

  private:

    /* class side private funcrtions  */

    Loop(unsigned int nFrames) ;
    ~Loop() ;


    /* instance side private varables */

    // audio data
    Sample* buffer1 ;
    Sample* buffer2 ;

    // peaks cache
    Sample peaksFine[N_PEAKS_FINE] ;
    Sample peaksCourse[N_PEAKS_COURSE] ;

    // loop state
    float vol ;
    bool  isMuted ;
} ;


class Scene
{
  friend class JackIO ;
  friend class Loopidity ;
  friend class LoopiditySdl ;
  friend class SceneSdl ;
  friend class Trace ;

  public:

    /* instance side public functions */

    // getters/setters
    unsigned int getCurrentPeakN() ;
//    float        getCurrentSeconds() ;
//    float        getTotalSeconds() ;


  private:

    /* class side private varables */

    // sample metedata
    static unsigned int RecordBufferSize ;
    static unsigned int SampleRate ;
    static unsigned int FrameSize ;
    static unsigned int NFramesPerPeriod ;


    /* class side private functions */

    // setup
    Scene(unsigned int sceneNum , unsigned int recordBufferSize) ;

    // getters/setters
    static void SetMetaData(unsigned int sampleRate , unsigned int frameSize ,
                            unsigned int nFramesPerPeriod) ;


    /* instance side private varables */

    // identity
    unsigned int sceneN ;

    // audio data
    list<Loop*> loops ;

    // peaks cache
    float hiScenePeaks[N_PEAKS_FINE] ; // the loudest of the currently playing samples in the current scene
    float hiLoopPeaks[N_LOOPS] ;       // the loudest sample for each loop of the current scene
    float highestScenePeak ;           // the loudest of all samples in all loops of the current scene (nyi)

    // buffer iteration
    unsigned int frameN ;
#if SCENE_NFRAMES_EDITABLE
unsigned int beginFrameN ;
unsigned int endFrameN ;
#endif // #if SCENE_NFRAMES_EDITABLE
    unsigned int nFrames ;
    unsigned int nFramesPerPeak ;
    unsigned int nBytes ;
    unsigned int nSeconds ;

    // scene state
    bool isRolling ;
    bool shouldSaveLoop ;
    bool doesPulseExist ;
    bool isMuted ;


    /* instance side private functions */

    // scene state
    void startRolling(void) ;
    void toggleState(void) ;

    // audio data
    bool addLoop(     Loop* newLoop) ;
    void deleteLoop(  unsigned int loopN) ;
    void reset(       void) ;

    // peaks cache
    void scanPeaks(  Loop* loop , unsigned int loopN) ;
    void rescanPeaks(void) ;

    // getters/setters
    Loop*        getLoop(     unsigned int loopN) ;
//    bool         getIsRolling(void) ;
//    unsigned int getLoopPos(  void) ;
} ;


#endif // #ifndef _SCENE_H_
