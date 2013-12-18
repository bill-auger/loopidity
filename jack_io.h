
#ifndef _JACK_IO_H_
#define _JACK_IO_H_


#include <jack/jack.h>
typedef jack_default_audio_sample_t Sample ;
#include "loopidity.h"
class Loop ;
class Scene ;


using namespace std ;


class JackIO
{
//  friend class Loopidity ;

  private:

    /* class side private varables */

    // JACK handles
    static jack_client_t* Client ;
    static jack_port_t*   PortInput1 ;
    static jack_port_t*   PortInput2 ;
    static jack_port_t*   PortOutput1 ;
    static jack_port_t*   PortOutput2 ;

    // app state
    static Scene*       CurrentScene ;
    static Scene*       NextScene ;
    static unsigned int CurrentSceneN ;
    static unsigned int NextSceneN ;

    // audio data
    static Sample*            Buffer1 ;
    static Sample*            Buffer2 ;
    static vector<Sample>     PeaksIn ;
    static vector<Sample>     PeaksOut ;
    static const unsigned int N_TRANSIENT_PEAKS ;
    static Sample             TransientPeaks[N_PORTS] ;
    static Sample             TransientPeakInMix ;
//    static Sample             TransientPeakOutMix ;
    static unsigned int       RecordBufferSize ;
    static const unsigned int GUI_UPDATE_IVL ;
    static unsigned int       NFramesPerGuiInterval ;

    // event structs
    static SDL_Event    EventLoopCreation ;
    static Loop*        NewLoop ;
    static SDL_Event    EventSceneChange ;
    static unsigned int EventLoopCreationSceneN ;
    static unsigned int EventSceneChangeSceneN ;

    // server state
    static jack_nframes_t     NFramesPerPeriod ;
    static const unsigned int FRAME_SIZE ;
    static unsigned int       PeriodSize ;
    static jack_nframes_t     SampleRate ;
    static unsigned int       BytesPerSecond ;

    // misc flags
    static bool ShouldMonitorInputs ;


  public:

    /* class side public functions */

    // setup
    static unsigned int Init(Scene* currentScene , unsigned int currentSceneN ,
                             bool isMonitorInputs , unsigned int recordBufferSize) ;
    static void Reset(       Scene* currentScene , unsigned int currentSceneN) ;

    // getters/setters
    static unsigned int    GetRecordBufferSize(void) ;
/*
    static unsigned int    GetNFramesPerPeriod(void) ;
    static unsigned int    GetFrameSize(       void) ;
    static unsigned int    GetSampleRate(      void) ;
*/
    static unsigned int    GetBytesPerSecond(  void) ;
    static void            SetCurrentScene(   Scene* currentScene) ;
    static void            SetNextScene(      Scene* nextScene) ;
    static vector<Sample>* GetPeaksIn() ;
    static vector<Sample>* GetPeaksOut() ;
    static Sample*         GetTransientPeaks() ;
    static Sample*         GetTransientPeakIn(void) ;
//    static Sample*         GetTransientPeakOut(   void) ;

    // helpers
    static void   ScanTransientPeaks(void) ;
    static Sample GetPeak(           Sample* buffer , unsigned int nFrames) ;

private:

    /* class side private functions */

    // JACK callbacks
    static int  ProcessCallback(      jack_nframes_t nframes , void* arg) ;
    static int  SetBufferSizeCallback(jack_nframes_t nframes , void* arg) ;
    static void ShutdownCallback(     void* arg) ;

// DEBUG
public: static unsigned int GetPeriodSize() { return PeriodSize ; }
// DEBUG
} ;


#endif // #ifndef _JACK_IO_H_
