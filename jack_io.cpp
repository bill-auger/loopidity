
#include "jack_io.h"


/* JackIO class side private varables */

// JACK handles
jack_client_t* JackIO::Client      = 0 ;
jack_port_t*   JackIO::PortInput1  = 0 ;
jack_port_t*   JackIO::PortInput2  = 0 ;
jack_port_t*   JackIO::PortOutput1 = 0 ;
jack_port_t*   JackIO::PortOutput2 = 0 ;

// app state
Scene*       JackIO::CurrentScene     = 0 ;
Scene*       JackIO::NextScene        = 0 ;
unsigned int JackIO::CurrentSceneN    = 0 ;
unsigned int JackIO::NextSceneN       = 0 ;

// audio data
Sample*            JackIO::Buffer1                 = 0 ;
Sample*            JackIO::Buffer2                 = 0 ;
#if SCENE_NFRAMES_EDITABLE
Sample*            JackIO::EditBeginBuffer1        = 0 ;
Sample*            JackIO::EditBeginBuffer2        = 0 ;
Sample*            JackIO::EditEndBuffer1          = 0 ;
Sample*            JackIO::EditEndBuffer2          = 0 ;
#endif // #if SCENE_NFRAMES_EDITABLE
vector<Sample>     JackIO::PeaksIn ;
vector<Sample>     JackIO::PeaksOut ;
const unsigned int JackIO::N_TRANSIENT_PEAKS       = N_PEAKS_TRANSIENT ;
Sample             JackIO::TransientPeaks[N_PORTS] = {0} ;
Sample             JackIO::TransientPeakInMix      = 0 ;
//Sample             JackIO::TransientPeakOutMix     = 0 ;
unsigned int       JackIO::RecordBufferSize        = 0 ;
const unsigned int JackIO::GUI_UPDATE_IVL          = GUI_UPDATE_INTERVAL ;
unsigned int       JackIO::NFramesPerGuiInterval   = 0 ;

// event structs
SDL_Event    JackIO::EventLoopCreation ;
Loop*        JackIO::NewLoop                 = 0 ;
SDL_Event    JackIO::EventSceneChange ;
unsigned int JackIO::EventLoopCreationSceneN = 0 ;
unsigned int JackIO::EventSceneChangeSceneN  = 0 ;

// server state
jack_nframes_t     JackIO::NFramesPerPeriod = 0 ;
const unsigned int JackIO::FRAME_SIZE       = sizeof(Sample) ;
unsigned int       JackIO::PeriodSize       = 0 ;
jack_nframes_t     JackIO::SampleRate       = 0 ;
unsigned int       JackIO::NBytesPerSecond  = 0 ;

// misc flags
bool JackIO::ShouldMonitorInputs = true ;


/* JackIO class side public functions */

// setup

unsigned int JackIO::Init(Scene* currentScene , unsigned int currentSceneN ,
                          bool shouldMonitorInputs , unsigned int recordBufferSize)
{
if (!INIT_JACK) return JACK_INIT_SUCCESS ;

  // set initial state
  Reset(currentScene , currentSceneN) ; ShouldMonitorInputs = shouldMonitorInputs ;

  // initialize event structs
  EventLoopCreation.type       = SDL_USEREVENT ;
  EventLoopCreation.user.code  = EVT_NEW_LOOP ;
  EventLoopCreation.user.data1 = &EventLoopCreationSceneN ;
  EventLoopCreation.user.data2 = &NewLoop ;
  EventSceneChange.type        = SDL_USEREVENT ;
  EventSceneChange.user.code   = EVT_SCENE_CHANGED ;
  EventSceneChange.user.data1  = &EventSceneChangeSceneN ;
  EventSceneChange.user.data2  = 0 ; // unused

  // initialize JACK client
  const char*    server_name = NULL ;
  const char*    client_name = APP_NAME ;
  jack_options_t options     = JackNullOption ;
  jack_status_t  status ;

  // register client
  Client = jack_client_open(client_name , options , &status , server_name) ;
  if (!Client) return JACK_FAIL ;

  // initialize record buffers
  RecordBufferSize = (recordBufferSize)?
      (recordBufferSize / FRAME_SIZE) : DEFAULT_BUFFER_SIZE ;
  if (!(Buffer1 = new (nothrow) Sample[RecordBufferSize]()) ||
      !(Buffer2 = new (nothrow) Sample[RecordBufferSize]())) return JACK_MEM_FAIL ;

  // assign callbacks
  jack_set_process_callback(Client , ProcessCallback , 0) ;
  jack_set_buffer_size_callback(Client , SetBufferSizeCallback , 0) ;
  jack_on_shutdown(Client , ShutdownCallback , 0) ;

  // register ports and activate client
  PortInput1  = jack_port_register(Client , "input1"  , JACK_DEFAULT_AUDIO_TYPE , JackPortIsInput  , 0) ;
  PortInput2  = jack_port_register(Client , "input2"  , JACK_DEFAULT_AUDIO_TYPE , JackPortIsInput  , 0) ;
  PortOutput1 = jack_port_register(Client , "output1" , JACK_DEFAULT_AUDIO_TYPE , JackPortIsOutput , 0) ;
  PortOutput2 = jack_port_register(Client , "output2" , JACK_DEFAULT_AUDIO_TYPE , JackPortIsOutput , 0) ;
  if (PortInput1 == NULL || PortInput2 == NULL || PortOutput1 == NULL || PortOutput2 == NULL ||
      jack_activate(Client)) return JACK_FAIL ;

  return JACK_INIT_SUCCESS ;
}

void JackIO::Reset(Scene* currentScene , unsigned int currentSceneN)
{
  CurrentScene  = NextScene        = currentScene ;
  CurrentSceneN = NextSceneN       = currentSceneN ;
  PeriodSize    = NFramesPerPeriod = 0 ;

  // initialize scope/VU peaks cache
  for (unsigned int peakN = 0 ; peakN < N_TRANSIENT_PEAKS ; ++peakN)
    { PeaksIn.push_back(0.0) ; PeaksOut.push_back(0.0) ; }
}


// getters/setters

unsigned int JackIO::GetRecordBufferSize()
  { return (RecordBufferSize)? RecordBufferSize : DEFAULT_BUFFER_SIZE ; }
/*
unsigned int JackIO::GetNFramesPerPeriod() { return NFramesPerPeriod ; }

unsigned int JackIO::GetFrameSize() { return FRAME_SIZE ; }

unsigned int JackIO::GetSampleRate() { return SampleRate ; }

unsigned int JackIO::GetNBytesPerSecond() { return NBytesPerSecond ; }
*/
void JackIO::SetCurrentScene(Scene* currentScene)
  { CurrentScene = currentScene ; CurrentSceneN = currentScene->sceneN ; }

void JackIO::SetNextScene(Scene* nextScene)
  { NextScene = nextScene ; NextSceneN = nextScene->sceneN ; }

vector<Sample>* JackIO::GetPeaksIn() { return &PeaksIn ; }

vector<Sample>* JackIO::GetPeaksOut() { return &PeaksOut ; }

Sample* JackIO::GetTransientPeaks() { return TransientPeaks ; }

Sample* JackIO::GetTransientPeakIn() { return &TransientPeakInMix ; }

//Sample* JackIO::GetTransientPeakOut() { return &TransientPeakOutMix ; }


// helpers

void JackIO::ScanTransientPeaks()
{
#if SCAN_TRANSIENT_PEAKS_DATA
// TODO; first NFramesPerGuiInterval duplicated?
  unsigned int frameN = CurrentScene->frameN ;
  frameN = (frameN < NFramesPerGuiInterval)? 0 : frameN - NFramesPerGuiInterval ;

  // initialize with inputs
  Sample peakIn1 = GetPeak(&(Buffer1[frameN]) , NFramesPerGuiInterval) ;
  Sample peakIn2 = GetPeak(&(Buffer2[frameN]) , NFramesPerGuiInterval) ;

  // add in unmuted tracks
  Sample peakOut1     = 0.0 ; Sample peakOut2 = 0.0 ;
  unsigned int nLoops = CurrentScene->loops.size() ;
  for (unsigned int loopN = 0 ; loopN < nLoops ; ++loopN)
  {
    Loop* loop = CurrentScene->getLoop(loopN) ;
    if (CurrentScene->isMuted && loop->isMuted) continue ;

    peakOut1 += GetPeak(&(loop->buffer1[frameN]) , NFramesPerGuiInterval) * loop->vol ;
    peakOut2 += GetPeak(&(loop->buffer2[frameN]) , NFramesPerGuiInterval) * loop->vol ;
  }

  Sample peakIn  = (peakIn1 + peakIn2)   / N_INPUT_CHANNELS ;
  Sample peakOut = (peakOut1 + peakOut2) / N_OUTPUT_CHANNELS ;
  if (peakOut > 1.0) peakOut = 1.0 ;

  // load scope peaks (mono mix)
  PeaksIn.pop_back()  ; PeaksIn.insert(PeaksIn.begin()   , peakIn) ;
  PeaksOut.pop_back() ; PeaksOut.insert(PeaksOut.begin() , peakOut) ;

  // load VU peaks (per channel)
  TransientPeakInMix = peakIn ;   //TransientPeakOutMix = peakOut ;
  TransientPeaks[0]  = peakIn1 ;  TransientPeaks[1]   = peakIn2 ;
  TransientPeaks[2]  = peakOut1 ; TransientPeaks[3]   = peakOut2 ;
#endif // #if SCAN_TRANSIENT_PEAKS_DATA
}

Sample JackIO::GetPeak(Sample* buffer , unsigned int nFrames)
{
#if !SCAN_PEAKS
  return 0.0 ;
#endif // #if SCAN_PEAKS

  Sample peak = 0.0 ;
  try // TODO: this function is unsafe
  {
    for (unsigned int frameN = 0 ; frameN < nFrames ; ++frameN)
      { Sample sample = fabs(buffer[frameN]) ; if (peak < sample) peak = sample ; }
  }
  catch(int ex) { printf(GETPEAK_ERROR_MSG) ; }
  return peak ;
}


/* JackIO class side private functions */

// JACK callbacks

int JackIO::ProcessCallback(jack_nframes_t nFrames , void* arg)
{
  // get JACK buffers
Sample* in1  = (Sample*)jack_port_get_buffer(PortInput1  , nFrames) ;
Sample* out1 = (Sample*)jack_port_get_buffer(PortOutput1 , nFrames) ;
Sample* in2  = (Sample*)jack_port_get_buffer(PortInput2  , nFrames) ;
Sample* out2 = (Sample*)jack_port_get_buffer(PortOutput2 , nFrames) ;

  // index into the record buffers and mix out
unsigned int currFrameN = CurrentScene->frameN , frameN , frameIdx ;
list<Loop*>::iterator loopsBeginIter = CurrentScene->loops.begin() ;
list<Loop*>::iterator loopsEndIter   = CurrentScene->loops.end() ;
list<Loop*>::iterator loopIter ; Loop* aLoop ; float vol ;
  for (frameN = 0 ; frameN < nFrames ; ++frameN)
  {
    frameIdx = currFrameN + frameN ;

    // write input to outputs mix buffers
    if (!ShouldMonitorInputs) Buffer1[frameIdx] = Buffer2[frameIdx] = 0 ;
    else { Buffer1[frameIdx] = in1[frameN] ; Buffer2[frameIdx] = in2[frameN] ; }

    // mix unmuted tracks into outputs mix buffers
    for (loopIter = loopsBeginIter ; loopIter != loopsEndIter ; ++loopIter)
    {
      if (CurrentScene->isMuted && (*loopIter)->isMuted) continue ;

      aLoop = *loopIter ; vol = aLoop->vol ;
      Buffer1[frameIdx] += aLoop->buffer1[frameIdx] * vol ;
      Buffer2[frameIdx] += aLoop->buffer2[frameIdx] * vol ;
    }
  }

  // write output mix buffers to outputs and write input to record buffers
  Sample* buf1 = Buffer1 + currFrameN ; Sample* buf2 = Buffer2 + currFrameN ;
  memcpy(out1 , buf1 , PeriodSize)    ; memcpy(out2 , buf2 , PeriodSize) ;
  memcpy(buf1 , in1  , PeriodSize)    ; memcpy(buf2 , in2  , PeriodSize) ;

  // increment sample rollover
#if SCENE_NFRAMES_EDITABLE
  if (!(CurrentScene->frameN = (CurrentScene->frameN + nFrames) % CurrentScene->endFrameN))// && CurrentScene->nFrames == RecordBufferSize ;
  {
    // unmute 'paused' loops
    CurrentScene->isMuted = false ;

    // create new Loop instance and copy record buffers to it
    unsigned int nLoops = CurrentScene->loops.size() ;
    if (CurrentScene->shouldSaveLoop && nLoops < N_LOOPS)
    {
      if ((NewLoop = new (nothrow) Loop(CurrentScene->nFrames + (SampleRate * 2))))
      {
        unsigned int nBytes      = CurrentScene->nBytes ;
        if (!nLoops)
        {
          unsigned int beginFrameN = CurrentScene->beginFrameN - KLUDGE_OFFSET_N_FRAMES ;
          CurrentScene->endFrameN  = CurrentScene->nFrames ;
          Sample* buffer1Begin     = Buffer1 + beginFrameN ;
          Sample* buffer2Begin     = Buffer2 + beginFrameN ;
          memcpy(NewLoop->buffer1 , buffer1Begin , nBytes) ;
          memcpy(NewLoop->buffer2 , buffer2Begin , nBytes) ;
          // copy leading and trailing seconds to auxiliary buffers for editing seam
          memcpy(EditBeginBuffer1 , buffer1Begin - SampleRate , NBytesPerSecond) ;
          memcpy(EditBeginBuffer2 , buffer2Begin - SampleRate , NBytesPerSecond) ;
          memcpy(EditEndBuffer1   , buffer1Begin + SampleRate , NBytesPerSecond) ;
          memcpy(EditEndBuffer2   , buffer2Begin + SampleRate , NBytesPerSecond) ;
        }
        else
        {
          memcpy(NewLoop->buffer1 , Buffer1 , nBytes) ;
          memcpy(NewLoop->buffer2 , Buffer2 , nBytes) ;
        }
        EventLoopCreationSceneN = CurrentSceneN ; SDL_PushEvent(&EventLoopCreation) ;
      }
      else Loopidity::OOM() ;
    }
//CurrentScene->beginFrameN = 0 ; CurrentScene->endFrameN -= CurrentScene->beginFrameN ;
//    CurrentScene->frameN = CurrentScene->beginFrameN ;
#else
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
#endif // #if SCENE_NFRAMES_EDITABLE

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
  PeriodSize            = FRAME_SIZE * (NFramesPerPeriod = nFrames) ;
  NBytesPerSecond       = FRAME_SIZE * (SampleRate = jack_get_sample_rate(Client)) ;
  NFramesPerGuiInterval = (unsigned int)(SampleRate * (float)GUI_UPDATE_IVL * 0.001) ;

#if SCENE_NFRAMES_EDITABLE
  if ((EditBeginBuffer1 = new (nothrow) Sample[NBytesPerSecond]()) &&
      (EditBeginBuffer2 = new (nothrow) Sample[NBytesPerSecond]()) &&
      (EditEndBuffer1   = new (nothrow) Sample[NBytesPerSecond]()) &&
      (EditEndBuffer2   = new (nothrow) Sample[NBytesPerSecond]()))
    Loopidity::SetMetaData(SampleRate , FRAME_SIZE , nFrames) ;
  else printf("ERROR: failed to create edit buffers") ;//TODO: something better than this
#else
  Loopidity::SetMetaData(SampleRate , FRAME_SIZE , nFrames) ;
#endif // #if SCENE_NFRAMES_EDITABLE

  return 0 ;
}

void JackIO::ShutdownCallback(void* arg)
{
  // close client and free resouces
  if (Client)      { jack_client_close(Client) ; free(Client) ; Client = 0 ; }
  if (PortInput1)  { free(PortInput1)  ; PortInput1  = 0 ; }
  if (PortInput2)  { free(PortInput2)  ; PortInput2  = 0 ; }
  if (PortOutput1) { free(PortOutput1) ; PortOutput1 = 0 ; }
  if (PortOutput2) { free(PortOutput2) ; PortOutput2 = 0 ; }
  if (Buffer1)     { delete Buffer1    ; Buffer1     = 0 ; }
  if (Buffer2)     { delete Buffer2    ; Buffer2     = 0 ; }
  exit(1) ;
}
