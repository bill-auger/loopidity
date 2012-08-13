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
unsigned int JackIO::RecordBufferSize = 0 ;

// server state
unsigned int JackIO::NFramesPerPeriod = 0 ;
const unsigned int JackIO::FRAME_SIZE = sizeof(SAMPLE) ;
unsigned int JackIO::PeriodSize = 0 ;
unsigned int JackIO::SampleRate = 0 ;
unsigned int JackIO::BytesPerSecond = FRAME_SIZE * SampleRate ;

// misc flags
bool JackIO::IsMonitorInputs = true ;


/* JackIO class public functions */

// setup

unsigned int JackIO::Init(Scene* currentScene , bool isMonitorInputs)
{
if (! INIT_JACK) return JACK_INIT_SUCCESS ;

	// set initial state and initialize record buffers
	Reset(currentScene) ; IsMonitorInputs = isMonitorInputs ;
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

void JackIO::Reset(Scene* currentScene)
	{ CurrentScene = NextScene = currentScene ; PeriodSize = NFramesPerPeriod = 0 ; }


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

void JackIO::SetNextScene(Scene* nextScene) { NextScene = nextScene ; }


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
		if (!IsMonitorInputs) RecordBuffer1[frameIdx] = RecordBuffer2[frameIdx] = 0 ;
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
		if (CurrentScene->isSaveLoop && CurrentScene->loops.size() < N_LOOPS)
		{
			Loop* newLoop = new Loop(CurrentScene->nFrames) ;
			memcpy(newLoop->buffer1 , RecordBuffer1 , CurrentScene->nBytes) ;
			memcpy(newLoop->buffer2 , RecordBuffer2 , CurrentScene->nBytes) ;
			CurrentScene->addLoop(newLoop) ;
		}

		// switch to NextScene if necessary
		if (CurrentScene != NextScene) { Loopidity::SceneChanged(CurrentScene = NextScene) ; }
	}

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
