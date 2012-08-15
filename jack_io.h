
#ifndef _JACK_IO_H_
#define _JACK_IO_H_


#include <jack/jack.h>

#include "loopidity.h"
class Loop ;
class Scene ;


class JackIO
{
	friend class Loopidity ;

  private:

		// setup
    static unsigned int Init(Scene* currentScene , bool isMonitorInputs) ;
		static void Reset(Scene* currentScene) ;

		// getters/setters
		static SAMPLE* GetRecordBuffer1() ;
		static SAMPLE* GetRecordBuffer2() ;
		static unsigned int GetRecordBufferSize() ;
		static bool SetRecordBufferSize(unsigned int recordBufferSize) ;
		static unsigned int GetNFramesPerPeriod() ;
		static const unsigned int GetFrameSize() ;
		static unsigned int GetSampleRate() ;
		static unsigned int GetBytesPerSecond() ;
		static void SetNextScene(Scene* nextScene) ;

  private:

		// JACK handles
    static jack_client_t* Client ;
    static jack_port_t* InputPort1 ;
    static jack_port_t* InputPort2 ;
    static jack_port_t* OutputPort1 ;
    static jack_port_t* OutputPort2 ;

		// audio data
		static Scene* CurrentScene ;
		static Scene* NextScene ;
		static SAMPLE* RecordBuffer1 ;
		static SAMPLE* RecordBuffer2 ;
		static unsigned int RecordBufferSize ;
		static Loop* NewLoop ;

		// server state
		static unsigned int NFramesPerPeriod ;
		static const unsigned int FRAME_SIZE ;
		static unsigned int PeriodSize ;
		static unsigned int SampleRate ;
		static unsigned int BytesPerSecond ;

		// misc flags
		static bool IsMonitorInputs ;

		// JACK callbacks
    static int ProcessCallback(jack_nframes_t nframes , void* arg) ;
    static int SetBufferSizeCallback(jack_nframes_t nframes , void* arg) ;
    static void ShutdownCallback(void* arg) ;

// DEBUG
public:
static unsigned int GetPeriodSize() { return PeriodSize ; }
// DEBUG
} ;


#endif // #ifndef _JACK_IO_H_
