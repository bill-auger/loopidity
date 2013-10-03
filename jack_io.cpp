
#include "jack_io.h"


/* JackIO class private varables */

// JACK handles
jack_client_t* JackIO::Client = 0 ;
jack_port_t* JackIO::PortInput1 = 0 ;
jack_port_t* JackIO::PortInput2 = 0 ;
jack_port_t* JackIO::PortOutput1 = 0 ;
jack_port_t* JackIO::PortOutput2 = 0 ;

// audio data
Scene* JackIO::CurrentScene = 0 ;
Scene* JackIO::NextScene = 0 ;
unsigned int JackIO::CurrentSceneN = 0 ;
unsigned int JackIO::NextSceneN = 0 ;
SAMPLE* JackIO::Buffer1 = 0 ;
SAMPLE* JackIO::Buffer2 = 0 ;
unsigned int JackIO::RecordBufferSize = 0 ;

// event structs
SDL_Event JackIO::EventLoopCreation ;
Loop* JackIO::NewLoop = 0 ;
SDL_Event JackIO::EventSceneChange ;
unsigned int JackIO::EventLoopCreationSceneN = 0 ;
unsigned int JackIO::EventSceneChangeSceneN = 0 ;

// server state
unsigned int JackIO::NFramesPerPeriod = 0 ;
const unsigned int JackIO::FRAME_SIZE = sizeof(SAMPLE) ;
unsigned int JackIO::PeriodSize = 0 ;
unsigned int JackIO::SampleRate = 0 ;
unsigned int JackIO::BytesPerSecond = FRAME_SIZE * SampleRate ;

// misc flags
bool JackIO::ShouldMonitorInputs = true ;


/* JackIO class public functions */

// setup

unsigned int JackIO::Init(Scene* currentScene , unsigned int currentSceneN , bool shouldMonitorInputs)
{
if (! INIT_JACK) return JACK_INIT_SUCCESS ;

	// set initial state
	Reset(currentScene , currentSceneN) ; ShouldMonitorInputs = shouldMonitorInputs ;

	// initialize event structs
	EventLoopCreation.type = SDL_USEREVENT ;
	EventLoopCreation.user.code = EVT_NEW_LOOP ;
	EventLoopCreation.user.data1 = &EventLoopCreationSceneN ;
	EventLoopCreation.user.data2 = &NewLoop ;
	EventSceneChange.type = SDL_USEREVENT ;
	EventSceneChange.user.code = EVT_SCENE_CHANGED ;
	EventSceneChange.user.data1 = &EventSceneChangeSceneN ;
	EventSceneChange.user.data2 = 0 ; // unused

	// initialize record buffers
	if (!RecordBufferSize) RecordBufferSize = DEFAULT_BUFFER_SIZE ;
	if (!(Buffer1 = new (nothrow) SAMPLE[RecordBufferSize]())) return JACK_MEM_FAIL ;
	if (!(Buffer2 = new (nothrow) SAMPLE[RecordBufferSize]())) return JACK_MEM_FAIL ;

	// initialize JACK client
	const char* client_name = APP_NAME ;
	jack_options_t options = JackNullOption ;
	jack_status_t status ;
	const char* server_name = NULL ;

	// register client
	Client = jack_client_open(client_name , options , &status , server_name) ;
	if (!Client) return JACK_FAIL ;

	// assign callbacks
	jack_set_process_callback(Client , ProcessCallback , 0) ;
	jack_set_buffer_size_callback(Client , SetBufferSizeCallback , 0) ;
	jack_on_shutdown(Client , ShutdownCallback , 0) ;

	// register ports and activate client
	PortInput1 = jack_port_register(Client , "input1" , JACK_DEFAULT_AUDIO_TYPE , JackPortIsInput , 0) ;
	PortInput2 = jack_port_register(Client , "input2" , JACK_DEFAULT_AUDIO_TYPE , JackPortIsInput , 0) ;
	PortOutput1 = jack_port_register(Client , "output1" , JACK_DEFAULT_AUDIO_TYPE , JackPortIsOutput , 0) ;
	PortOutput2 = jack_port_register(Client , "output2" , JACK_DEFAULT_AUDIO_TYPE , JackPortIsOutput , 0) ;
	if (PortInput1 == NULL || PortInput2 == NULL || PortOutput1 == NULL || PortOutput2 == NULL ||
			jack_activate(Client)) return JACK_FAIL ;

	return JACK_INIT_SUCCESS ;
}

void JackIO::Reset(Scene* currentScene , unsigned int currentSceneN)
{
	CurrentScene = NextScene = currentScene ; CurrentSceneN = NextSceneN = currentSceneN ;
	PeriodSize = NFramesPerPeriod = 0 ;
}


// getters/setters

SAMPLE* JackIO::GetBuffer1() { return Buffer1 ; }

SAMPLE* JackIO::GetBuffer2() { return Buffer2 ; }

bool JackIO::SetRecordBufferSize(unsigned int recordBufferSize)
{
	if (RecordBufferSize || !recordBufferSize) return false ;
	else { RecordBufferSize = recordBufferSize  / JackIO::GetFrameSize() ; return true ; }
}

unsigned int JackIO::GetRecordBufferSize()
	{ return (RecordBufferSize)? RecordBufferSize : DEFAULT_BUFFER_SIZE ; }

unsigned int JackIO::GetNFramesPerPeriod() { return NFramesPerPeriod ; }

const unsigned int JackIO::GetFrameSize() { return FRAME_SIZE ; }

unsigned int JackIO::GetSampleRate() { return SampleRate ; }

unsigned int JackIO::GetBytesPerSecond() { return BytesPerSecond ; }

void JackIO::SetCurrentScene(Scene* currentScene , unsigned int currentSceneN)
	{ CurrentScene = currentScene ; CurrentSceneN = currentSceneN ; }

void JackIO::SetNextScene(Scene* nextScene , unsigned int nextSceneN)
	{ NextScene = nextScene ; NextSceneN = nextSceneN ; }


/* JackIO class private functions */

// JACK callbacks

int JackIO::ProcessCallback(jack_nframes_t nFrames , void* arg)
{
	// get JACK buffers
SAMPLE* in1 = (SAMPLE*)jack_port_get_buffer(PortInput1 , nFrames) ;
SAMPLE* out1 = (SAMPLE*)jack_port_get_buffer(PortOutput1 , nFrames) ;
SAMPLE* in2 = (SAMPLE*)jack_port_get_buffer(PortInput2 , nFrames) ;
SAMPLE* out2 = (SAMPLE*)jack_port_get_buffer(PortOutput2 , nFrames) ;

	// index into the record buffers and mix out
unsigned int currFrameN = CurrentScene->frameN , frameN , frameIdx ;
	for (frameN = 0 ; frameN < nFrames ; ++frameN)
	{
		frameIdx = CurrentScene->frameN + frameN ;
		// write input to outputs mix buffers
		if (!ShouldMonitorInputs) Buffer1[frameIdx] = Buffer2[frameIdx] = 0 ;
		else { Buffer1[frameIdx] = in1[frameN] ; Buffer2[frameIdx] = in2[frameN] ; }

		// mix unmuted tracks into outputs mix buffers
list<Loop*>::iterator aLoop = CurrentScene->loops.begin() ; Loop* loop ; float vol ;
		for ( ; aLoop != CurrentScene->loops.end() ; ++aLoop)
		{
			if (CurrentScene->isMuted && (*aLoop)->isMuted) continue ;

			loop = *aLoop ; vol = loop->vol ;
			Buffer1[frameIdx] += loop->buffer1[frameIdx] * vol ;
			Buffer2[frameIdx] += loop->buffer2[frameIdx] * vol ;
		}
	}

	// write output mix buffers to outputs and write input to record buffers
	memcpy(out1 , &Buffer1[currFrameN] , PeriodSize) ; memcpy(out2 , &Buffer2[currFrameN] , PeriodSize) ;
	memcpy(&Buffer1[currFrameN] , in1 , PeriodSize) ; memcpy(&Buffer2[currFrameN] , in2 , PeriodSize) ;

	// increment sample rollover
	if (!(CurrentScene->frameN = (CurrentScene->frameN + nFrames) % CurrentScene->nFrames))
	{
		// unmute 'paused' loops
		CurrentScene->isMuted = false ;

		// create new Loop instance and copy record buffers to it
		if (CurrentScene->shouldSaveLoop && CurrentScene->loops.size() < N_LOOPS)
		{
			if ((NewLoop = new (nothrow) Loop(CurrentScene->nFrames)))
			{
				memcpy(NewLoop->buffer1 , Buffer1 , CurrentScene->nBytes) ;
				memcpy(NewLoop->buffer2 , Buffer2 , CurrentScene->nBytes) ;
				EventLoopCreationSceneN = CurrentSceneN ; SDL_PushEvent(&EventLoopCreation) ;
			}
			else Loopidity::OOM() ;
		}

		// switch to NextScene if necessary
		if (CurrentScene != NextScene)
		{
			CurrentScene = NextScene ; CurrentSceneN = EventSceneChangeSceneN = NextSceneN ;
			SDL_PushEvent(&EventSceneChange) ;
		}
	}

	return 0 ;
}

int JackIO::SetBufferSizeCallback(jack_nframes_t nFrames , void* arg)
{
	NFramesPerPeriod = nFrames ; PeriodSize = FRAME_SIZE * (nFrames) ;
	BytesPerSecond = FRAME_SIZE * (SampleRate = (unsigned int)jack_get_sample_rate(Client)) ;
	Loopidity::SetMetaData(SampleRate , FRAME_SIZE , nFrames) ; return 0 ;
}

void JackIO::ShutdownCallback(void* arg)
{
	// close client and free resouces
	if (Client) { jack_client_close(Client) ; free(Client) ; Client = 0 ; }
	if (PortInput1) { free(PortInput1) ; PortInput1 = 0 ; }
	if (PortInput2) { free(PortInput2) ; PortInput2 = 0 ; }
	if (PortOutput1) { free(PortOutput1) ; PortOutput1 = 0 ; }
	if (PortOutput2) { free(PortOutput2) ; PortOutput2 = 0 ; }
	if (Buffer1) { delete Buffer1 ; Buffer1 = 0 ; }
	if (Buffer2) { delete Buffer2 ; Buffer2 = 0 ; }
	exit(1) ;
}
