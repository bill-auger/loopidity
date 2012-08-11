// TODO: u++ (in DEBUG cfg) says jack_client_open and jack_client_close cause a heap leak

#include "jack_io.h"


/* JackIO class private varables */

// JACK handles
jack_client_t* JackIO::Client = 0 ;
jack_port_t* JackIO::InputPort1 = 0 ;
jack_port_t* JackIO::InputPort2 = 0 ;
jack_port_t* JackIO::OutputPort1 = 0 ;
jack_port_t* JackIO::OutputPort2 = 0 ;

// audio data
Scene* JackIO::CurrentScene = 0 ;
Scene* JackIO::NextScene = 0 ;
SAMPLE* JackIO::RecordBuffer1 = 0 ;
SAMPLE* JackIO::RecordBuffer2 = 0 ;

// server state
unsigned int JackIO::NFramesPerPeriod = 0 ;
const unsigned int JackIO::FRAME_SIZE = sizeof(SAMPLE) ;
unsigned int JackIO::PeriodSize = 0 ;
unsigned int JackIO::SampleRate = 48000 ;
unsigned int JackIO::BytesPerSecond = FRAME_SIZE * SampleRate ;

// misc flags
bool JackIO::IsMonitorInputs = true ;


/* JackIO class public functions */

// setup

bool JackIO::Init(unsigned int nFrames , Scene* currentScene , bool isMonitorInputs)
{
if (! INIT_JACK) return true ;

	// set initial state
	Reset(currentScene) ; IsMonitorInputs = isMonitorInputs ;

	// initialize JACK client
	const char* client_name = APP_NAME ;
	jack_options_t options = JackNullOption ;
	jack_status_t status ;
	const char* server_name = NULL ;

	// register client
	Client = jack_client_open(client_name , options , &status , server_name) ;
	if (!Client) return false ;

	// assign callbacks
	jack_set_process_callback(Client , ProcessCallback , 0) ;
	jack_set_buffer_size_callback(Client , SetBufferSizeCallback , 0) ;
	jack_on_shutdown(Client , ShutdownCallback , 0) ;

	// register ports and activate client
	InputPort1 = jack_port_register(Client , "input1" , JACK_DEFAULT_AUDIO_TYPE , JackPortIsInput , 0) ;
	InputPort2 = jack_port_register(Client , "input2" , JACK_DEFAULT_AUDIO_TYPE , JackPortIsInput , 0) ;
	OutputPort1 = jack_port_register(Client , "output1" , JACK_DEFAULT_AUDIO_TYPE , JackPortIsOutput , 0) ;
	OutputPort2 = jack_port_register(Client , "output2" , JACK_DEFAULT_AUDIO_TYPE , JackPortIsOutput , 0) ;
	if (InputPort1 == NULL || InputPort2 == NULL || OutputPort1 == NULL || OutputPort2 == NULL ||
			jack_activate(Client)) return false ;

	// initialize record buffers
	RecordBuffer1 = new SAMPLE[nFrames]() ; RecordBuffer2 = new SAMPLE[nFrames]() ;

	return true ;
}

void JackIO::Reset(Scene* currentScene)
	{ CurrentScene = NextScene = currentScene ; PeriodSize = NFramesPerPeriod = 0 ; }


// getters/setters

jack_port_t* JackIO::GetInPort1() { return InputPort1 ; }

jack_port_t* JackIO::GetInPort2() { return InputPort2 ; }

jack_port_t* JackIO::GetOutPort1() { return OutputPort1 ; }

jack_port_t* JackIO::GetOutPort2() { return OutputPort2 ; }

SAMPLE* JackIO::GetRecordBuffer1() { return RecordBuffer1 ; }

SAMPLE* JackIO::GetRecordBuffer2() { return RecordBuffer2 ; }

unsigned int JackIO::GetNFramesPerPeriod() { return NFramesPerPeriod ; }

const unsigned int JackIO::GetFrameSize() { return FRAME_SIZE ; }

unsigned int JackIO::GetSampleRate() { return SampleRate ; }

unsigned int JackIO::GetBytesPerSecond() { return BytesPerSecond ; }

Scene* JackIO::GetCurrentScene() { return CurrentScene ; }

void JackIO::SetNextScene(Scene* nextScene) { NextScene = nextScene ; }


/* JackIO class private functions */

// JACK callbacks

int JackIO::ProcessCallback(jack_nframes_t nFrames , void* arg)
{
#if DSP
	// get JACK buffers
	SAMPLE* in1 = (SAMPLE*)jack_port_get_buffer(InputPort1 , nFrames) ;
	SAMPLE* out1 = (SAMPLE*)jack_port_get_buffer(OutputPort1 , nFrames) ;
	SAMPLE* in2 = (SAMPLE*)jack_port_get_buffer(InputPort2 , nFrames) ;
	SAMPLE* out2 = (SAMPLE*)jack_port_get_buffer(OutputPort2 , nFrames) ;
#if ! PASSTHRU
	if (!Loopidity::GetIsRecording()) // TODO: we could avoid this call if all CurrentScene->nFrames were initially 1
#endif
	{
		if (IsMonitorInputs) { memcpy(out1 , in1 , PeriodSize) ; memcpy(out2 , in2 , PeriodSize) ; }
		return 0 ;
	}

	// index into the record buffers and mix out
	SAMPLE* currBuff1 = &(RecordBuffer1[CurrentScene->frameN]) ;
	SAMPLE* currBuff2 = &(RecordBuffer2[CurrentScene->frameN]) ;
	for (unsigned int frameNin = 0 ; frameNin < nFrames ; ++frameNin)
	{
		// write input to mix buffers
		if (!IsMonitorInputs) currBuff1[frameNin] = currBuff2[frameNin] = 0 ;
		else { currBuff1[frameNin] = in1[frameNin] ; currBuff2[frameNin] = in2[frameNin] ; }

		// mix unmuted tracks into mix buffers
		unsigned int frameNout = CurrentScene->frameN + frameNin ;
		for (unsigned int loopN = 0 ; loopN < CurrentScene->nLoops ; ++loopN)
		{
			currBuff1[frameNin] += CurrentScene->loops[loopN]->buffer1[frameNout] ;
			currBuff2[frameNin] += CurrentScene->loops[loopN]->buffer2[frameNout] ;
		}
	}
	// write mix buffers to outputs and write input to record buffers
	memcpy(out1 , currBuff1 , PeriodSize) ; memcpy(out2 , currBuff2 , PeriodSize) ;
	memcpy(currBuff1 , in1 , PeriodSize) ; memcpy(currBuff2 , in2 , PeriodSize) ;
#endif

#if LOOP_COUNTER
	// increment sample rollover - ASSERT: ((nFrames == NFramesPerPeriod) && !(frameN % CurrentScene->nFrames))
	if (!(CurrentScene->frameN = (CurrentScene->frameN + nFrames) % CurrentScene->nFrames))
	{
		// copy record buffers , create peaks cache , and increment nLoops
		if (CurrentScene->isSaveLoop && CurrentScene->nLoops < N_LOOPS)
		{
#if DSP
			Loop* newLoop = new Loop(CurrentScene->nFrames) ;
			unsigned int nBytes = FRAME_SIZE * CurrentScene->nFrames ;
			memcpy(newLoop->buffer1 , RecordBuffer1 , nBytes) ;
			memcpy(newLoop->buffer2 , RecordBuffer2 , nBytes) ;
			CurrentScene->addLoop(newLoop) ;
#endif

if (DEBUG) { char dbg[255] ; sprintf(dbg , "NEW LOOP scene:%d loopN:%d" , Loopidity::GetCurrentSceneN() , CurrentScene->nLoops) ; LoopiditySdl::SetStatusR(dbg) ; }
		}

		// switch to NextScene if necessary
		if (CurrentScene != NextScene) { CurrentScene->sceneChanged() ; CurrentScene = NextScene ; }
	}
#endif

	return 0 ;
}

int JackIO::SetBufferSizeCallback(jack_nframes_t nFrames , void* arg)
{
	NFramesPerPeriod = nFrames ; PeriodSize = FRAME_SIZE * (nFrames) ;
	BytesPerSecond = FRAME_SIZE * (SampleRate = (unsigned int)jack_get_sample_rate(Client)) ;
	Loopidity::SetNFramesPerPeriod(nFrames) ; return 0 ;
}

void JackIO::ShutdownCallback(void* arg)
{
	// close client and free resouces
	if (Client) { jack_client_close(Client) ; free(Client) ; Client = 0 ; }
	if (InputPort1) { free(InputPort1) ; InputPort1 = 0 ; }
	if (InputPort2) { free(InputPort2) ; InputPort2 = 0 ; }
	if (OutputPort1) { free(OutputPort1) ; OutputPort1 = 0 ; }
	if (OutputPort2) { free(OutputPort2) ; OutputPort2 = 0 ; }
	if (RecordBuffer1) { delete RecordBuffer1 ; RecordBuffer1 = 0 ; }
	if (RecordBuffer2) { delete RecordBuffer2 ; RecordBuffer2 = 0 ; }
	exit(1) ;
}
