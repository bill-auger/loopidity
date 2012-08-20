
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
unsigned int JackIO::CurrentSceneN = 0 ;
unsigned int JackIO::NextSceneN = 0 ;
SAMPLE* JackIO::RecordBuffer1 = 0 ;
SAMPLE* JackIO::RecordBuffer2 = 0 ;
unsigned int JackIO::RecordBufferSize = 0 ;

// event structs
SDL_Event JackIO::eventNewLoop ;
Loop* JackIO::NewLoop = 0 ;
SDL_Event JackIO::eventSceneChanged ;
unsigned int JackIO::AddLoopSceneN = 0 ;
unsigned int JackIO::ChangedSceneN = 0 ;

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
	eventNewLoop.type = SDL_USEREVENT ;
	eventNewLoop.user.code = EVT_NEW_LOOP ;
	eventNewLoop.user.data1 = &AddLoopSceneN ;
	eventNewLoop.user.data2 = &NewLoop ;
	eventSceneChanged.type = SDL_USEREVENT ;
	eventSceneChanged.user.code = EVT_SCENE_CHANGED ;
	eventSceneChanged.user.data1 = &ChangedSceneN ;
	eventSceneChanged.user.data2 = 0 ; // unused

	// initialize record buffers
	if (!RecordBufferSize) RecordBufferSize = DEFAULT_BUFFER_SIZE ;
	if (!(RecordBuffer1 = new (nothrow) SAMPLE[RecordBufferSize]())) return JACK_MEM_FAIL ;
	if (!(RecordBuffer2 = new (nothrow) SAMPLE[RecordBufferSize]())) return JACK_MEM_FAIL ;

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
	InputPort1 = jack_port_register(Client , "input1" , JACK_DEFAULT_AUDIO_TYPE , JackPortIsInput , 0) ;
	InputPort2 = jack_port_register(Client , "input2" , JACK_DEFAULT_AUDIO_TYPE , JackPortIsInput , 0) ;
	OutputPort1 = jack_port_register(Client , "output1" , JACK_DEFAULT_AUDIO_TYPE , JackPortIsOutput , 0) ;
	OutputPort2 = jack_port_register(Client , "output2" , JACK_DEFAULT_AUDIO_TYPE , JackPortIsOutput , 0) ;
	if (InputPort1 == NULL || InputPort2 == NULL || OutputPort1 == NULL || OutputPort2 == NULL ||
			jack_activate(Client)) return JACK_FAIL ;

	return JACK_INIT_SUCCESS ;
}

void JackIO::Reset(Scene* currentScene , unsigned int currentSceneN)
{
	CurrentScene = NextScene = currentScene ; CurrentSceneN = NextSceneN = currentSceneN ;
	PeriodSize = NFramesPerPeriod = 0 ;
}


// getters/setters

SAMPLE* JackIO::GetRecordBuffer1() { return RecordBuffer1 ; }

SAMPLE* JackIO::GetRecordBuffer2() { return RecordBuffer2 ; }

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
	{ if (Scene::IsRecording) { CurrentScene = currentScene ; CurrentSceneN = currentSceneN ; } }

void JackIO::SetNextScene(Scene* nextScene , unsigned int nextSceneN)
	{ NextScene = nextScene ; NextSceneN = nextSceneN ; }


/* JackIO class private functions */

// JACK callbacks

int JackIO::ProcessCallback(jack_nframes_t nFrames , void* arg)
{
	// get JACK buffers
	SAMPLE* in1 = (SAMPLE*)jack_port_get_buffer(InputPort1 , nFrames) ;
	SAMPLE* out1 = (SAMPLE*)jack_port_get_buffer(OutputPort1 , nFrames) ;
	SAMPLE* in2 = (SAMPLE*)jack_port_get_buffer(InputPort2 , nFrames) ;
	SAMPLE* out2 = (SAMPLE*)jack_port_get_buffer(OutputPort2 , nFrames) ;

	// index into the record buffers and mix out
	unsigned int currFrameN = CurrentScene->frameN , frameN , frameIdx , loopN ;
	for (frameN = 0 ; frameN < nFrames ; ++frameN)
	{
		frameIdx = CurrentScene->frameN + frameN ;
		// write input to outputs mix buffers
		if (!ShouldMonitorInputs) RecordBuffer1[frameIdx] = RecordBuffer2[frameIdx] = 0 ;
		else { RecordBuffer1[frameIdx] = in1[frameN] ; RecordBuffer2[frameIdx] = in2[frameN] ; }

		// mix unmuted tracks into outputs mix buffers
		for (loopN = 0 ; loopN < CurrentScene->loops.size() ; ++loopN)
		{
			RecordBuffer1[frameIdx] += CurrentScene->loops[loopN]->buffer1[frameIdx] ;
			RecordBuffer2[frameIdx] += CurrentScene->loops[loopN]->buffer2[frameIdx] ;
		}
	}

	// write output mix buffers to outputs and write input to record buffers
	memcpy(out1 , &RecordBuffer1[currFrameN] , PeriodSize) ; memcpy(out2 , &RecordBuffer2[currFrameN] , PeriodSize) ;
	memcpy(&RecordBuffer1[currFrameN] , in1 , PeriodSize) ; memcpy(&RecordBuffer2[currFrameN] , in2 , PeriodSize) ;

	// increment sample rollover - ASSERT: ((nFrames == NFramesPerPeriod) && !(frameN % CurrentScene->nFrames))
	if (!(CurrentScene->frameN = (CurrentScene->frameN + nFrames) % CurrentScene->nFrames))
	{
		// create new Loop instance and copy record buffers to it
		if (CurrentScene->shouldSaveLoop)
		{
			if (!(NewLoop = new (nothrow) Loop(CurrentScene->nFrames))) { Loopidity::OOM() ; return 0 ; }

			memcpy(NewLoop->buffer1 , RecordBuffer1 , CurrentScene->nBytes) ;
			memcpy(NewLoop->buffer2 , RecordBuffer2 , CurrentScene->nBytes) ;
			AddLoopSceneN = CurrentSceneN ; SDL_PushEvent(&eventNewLoop) ;
		}

		// switch to NextScene if necessary
		if (CurrentScene != NextScene)
		{
			CurrentScene = NextScene ; CurrentSceneN = ChangedSceneN = NextSceneN ;
			SDL_PushEvent(&eventSceneChanged) ;
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
	if (InputPort1) { free(InputPort1) ; InputPort1 = 0 ; }
	if (InputPort2) { free(InputPort2) ; InputPort2 = 0 ; }
	if (OutputPort1) { free(OutputPort1) ; OutputPort1 = 0 ; }
	if (OutputPort2) { free(OutputPort2) ; OutputPort2 = 0 ; }
	if (RecordBuffer1) { delete RecordBuffer1 ; RecordBuffer1 = 0 ; }
	if (RecordBuffer2) { delete RecordBuffer2 ; RecordBuffer2 = 0 ; }
	exit(1) ;
}
