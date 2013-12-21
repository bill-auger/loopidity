
#include "jack_io.h"


/* JackIO class side private varables */

// JACK handles
jack_client_t* JackIO::Client      = 0 ;
#if FIXED_N_AUDIO_PORTS
jack_port_t*   JackIO::PortInput1  = 0 ;
jack_port_t*   JackIO::PortInput2  = 0 ;
jack_port_t*   JackIO::PortOutput1 = 0 ;
jack_port_t*   JackIO::PortOutput2 = 0 ;
#else // TODO: much
#endif // #if FIXED_N_AUDIO_PORTS

// app state
Scene*       JackIO::CurrentScene  = 0 ;
Scene*       JackIO::NextScene     = 0 ;
unsigned int JackIO::CurrentSceneN = 0 ;
unsigned int JackIO::NextSceneN    = 0 ;

// audio data
#if FIXED_N_AUDIO_PORTS
Sample* JackIO::RecordBuffer1  = 0 ; // Init()
Sample* JackIO::RecordBuffer2  = 0 ; // Init()
#  if SCENE_NFRAMES_EDITABLE
Sample* JackIO::LeadInBuffer1  = 0 ; // SetBufferSizeCallback()
Sample* JackIO::LeadInBuffer2  = 0 ; // SetBufferSizeCallback()
Sample* JackIO::LeadOutBuffer1 = 0 ; // SetBufferSizeCallback()
Sample* JackIO::LeadOutBuffer2 = 0 ; // SetBufferSizeCallback()
#  endif // #if SCENE_NFRAMES_EDITABLE
#else // TODO: much
#endif // #if FIXED_N_AUDIO_PORTS

// peaks data
const unsigned int JackIO::N_TRANSIENT_PEAKS             = N_PEAKS_TRANSIENT ;
vector<Sample>     JackIO::PeaksIn ;
vector<Sample>     JackIO::PeaksOut ;
Sample             JackIO::TransientPeaks[N_AUDIO_PORTS] = {0} ;
Sample             JackIO::TransientPeakInMix            = 0 ;
//Sample             JackIO::TransientPeakOutMix           = 0 ;

// event structs
SDL_Event    JackIO::EventLoopCreation ;
Loop*        JackIO::NewLoop                 = 0 ;
SDL_Event    JackIO::EventSceneChange ;
unsigned int JackIO::EventLoopCreationSceneN = 0 ;
unsigned int JackIO::EventSceneChangeSceneN  = 0 ;

// metadata
jack_nframes_t     JackIO::SampleRate             = 0 ; // SetBufferSizeCallback()
unsigned int       JackIO::NBytesPerSecond        = 0 ; // SetBufferSizeCallback()
jack_nframes_t     JackIO::NFramesPerPeriod       = 0 ; // SetBufferSizeCallback()
unsigned int       JackIO::PeriodSize             = 0 ; // SetBufferSizeCallback()
const unsigned int JackIO::N_BYTES_PER_FRAME      = sizeof(Sample) ;
unsigned int       JackIO::RecordBufferSize       = DEFAULT_AUDIO_BUFFER_SIZE ;
#if SCENE_NFRAMES_EDITABLE
unsigned int       JackIO::BufferMarginSize       = 0 ; // SetBufferSizeCallback()
unsigned int       JackIO::NMarginBytes           = 0 ; // SetBufferSizeCallback()
#endif // #if SCENE_NFRAMES_EDITABLE
#if WAIT_FOR_JACK_INIT
unsigned int       JackIO::RolloverFrameN         = 0 ; // SetBufferSizeCallback()
#endif // #if WAIT_FOR_JACK_INIT
const unsigned int JackIO::GUI_UPDATE_IVL         = GUI_UPDATE_INTERVAL ;
unsigned int       JackIO::NFramesPerGuiInterval  = 0 ; // SetBufferSizeCallback()
unsigned int       JackIO::TriggerLatencyNFrames  = TRIGGER_LATENCY_N_FRAMES ;

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
  EventSceneChange.user.data2  = 0 ; // n/a

  // initialize JACK client
  const char*    server_name = NULL ;
  const char*    client_name = APP_NAME ;
  jack_options_t options     = JackNullOption ;
  jack_status_t  status ;

  // register client
  Client = jack_client_open(client_name , options , &status , server_name) ;
  if (!Client) return JACK_FAIL ;

  // initialize record buffers
  if (recordBufferSize) RecordBufferSize = recordBufferSize / N_BYTES_PER_FRAME ;
  if (!(RecordBuffer1 = new (nothrow) Sample[RecordBufferSize]()) ||
      !(RecordBuffer2 = new (nothrow) Sample[RecordBufferSize]())) return JACK_MEM_FAIL ;

  // assign callbacks
  jack_set_process_callback(    Client , ProcessCallback       , 0) ;
  jack_set_buffer_size_callback(Client , SetBufferSizeCallback , 0) ;
  jack_on_shutdown(             Client , ShutdownCallback      , 0) ;

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

unsigned int JackIO::GetRecordBufferSize() { return RecordBufferSize ; }
/*
unsigned int JackIO::GetNFramesPerPeriod() { return NFramesPerPeriod ; }

unsigned int JackIO::GetFrameSize() { return N_BYTES_PER_FRAME ; }

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
#  if SCENE_NFRAMES_EDITABLE
// TODO: next version currentFrameN will never be < BufferMarginSize (assert BufferMarginSize >> NFramesPerGuiInterval)
//  unsigned int currentFrameN = CurrentScene->currentFrameN - NFramesPerGuiInterval ;
//  unsigned int offsetFrameN  = currentFrameN + TriggerLatencyNFrames ;
  unsigned int currentFrameN = CurrentScene->currentFrameN ;
  currentFrameN = (currentFrameN < NFramesPerGuiInterval)? 0 : currentFrameN - NFramesPerGuiInterval ;
  unsigned int offsetFrameN  = currentFrameN + BufferMarginSize + TriggerLatencyNFrames ;
  unsigned int nFrames = NFramesPerGuiInterval ;

  // initialize with inputs
  Sample peakIn1 = GetPeak(&(RecordBuffer1[offsetFrameN]) , nFrames) ;
  Sample peakIn2 = GetPeak(&(RecordBuffer2[offsetFrameN]) , nFrames) ;

  // add in unmuted tracks
  Sample peakOut1     = 0.0 ; Sample peakOut2 = 0.0 ;
  unsigned int nLoops = CurrentScene->loops.size() ;
  for (unsigned int loopN = 0 ; loopN < nLoops ; ++loopN)
  {
    Loop* loop = CurrentScene->getLoop(loopN) ;
    if (CurrentScene->isMuted && loop->isMuted) continue ;

    peakOut1 += GetPeak(&(loop->buffer1[currentFrameN]) , nFrames) * loop->vol ;
    peakOut2 += GetPeak(&(loop->buffer2[currentFrameN]) , nFrames) * loop->vol ;
  }
#  else
  unsigned int frameN = CurrentScene->currentFrameN ;
  frameN = (frameN < NFramesPerGuiInterval)? 0 : frameN - NFramesPerGuiInterval ;

  // initialize with inputs
  Sample peakIn1 = GetPeak(&(RecordBuffer1[frameN]) , NFramesPerGuiInterval) ;
  Sample peakIn2 = GetPeak(&(RecordBuffer2[frameN]) , NFramesPerGuiInterval) ;

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
#  endif // #if SCENE_NFRAMES_EDITABLE

#  if FIXED_N_AUDIO_PORTS
  Sample peakIn  = (peakIn1 + peakIn2)   / N_INPUT_CHANNELS ;
  Sample peakOut = (peakOut1 + peakOut2) / N_OUTPUT_CHANNELS ;
  if (peakOut > 1.0) peakOut = 1.0 ;
#  else
  Sample peakIn  = (peakIn1 + peakIn2)   / N_INPUT_CHANNELS ;
  Sample peakOut = (peakOut1 + peakOut2) / N_OUTPUT_CHANNELS ;
  if (peakOut > 1.0) peakOut = 1.0 ;
#  endif // #if FIXED_N_AUDIO_PORTS

  // load scope peaks (mono mix)
  PeaksIn.pop_back()  ; PeaksIn.insert(PeaksIn.begin()   , peakIn) ;
  PeaksOut.pop_back() ; PeaksOut.insert(PeaksOut.begin() , peakOut) ;

  // load VU peaks (per channel)
#  if FIXED_N_AUDIO_PORTS
  TransientPeakInMix = peakIn ;   //TransientPeakOutMix = peakOut ;
  TransientPeaks[0]  = peakIn1 ;  TransientPeaks[1]   = peakIn2 ;
  TransientPeaks[2]  = peakOut1 ; TransientPeaks[3]   = peakOut2 ;
#  else // TODO: magic #s ~= N_AUDIO_PORTS (see #def)
  TransientPeakInMix = peakIn ;   //TransientPeakOutMix = peakOut ;
  TransientPeaks[0]  = peakIn1 ;  TransientPeaks[1]   = peakIn2 ;
  TransientPeaks[2]  = peakOut1 ; TransientPeaks[3]   = peakOut2 ;
#  endif // #if FIXED_N_AUDIO_PORTS
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
#if SCENE_NFRAMES_EDITABLE
{
  // get JACK buffers
  Sample* in1  = (Sample*)jack_port_get_buffer(PortInput1  , nFrames) ;
  Sample* out1 = (Sample*)jack_port_get_buffer(PortOutput1 , nFrames) ;
  Sample* in2  = (Sample*)jack_port_get_buffer(PortInput2  , nFrames) ;
  Sample* out2 = (Sample*)jack_port_get_buffer(PortOutput2 , nFrames) ;

  // index into the record buffers and mix out
  list<Loop*>::iterator loopIter , loopsBeginIter , loopsEndIter ; Loop* aLoop ; float vol ;
  loopsBeginIter             = CurrentScene->loops.begin() ;
  loopsEndIter               = CurrentScene->loops.end() ;
  unsigned int currentFrameN = CurrentScene->currentFrameN ;
  unsigned int offsetFrameN  = currentFrameN + BufferMarginSize + TriggerLatencyNFrames ;
  unsigned int mixFrameN     = offsetFrameN ;
  unsigned int sceneFrameN   = currentFrameN , frameN ;
  for (frameN = 0 ; frameN < nFrames ; ++frameN && ++mixFrameN && ++sceneFrameN)
  {
    // write input to outputs mix buffers
    if (!ShouldMonitorInputs) RecordBuffer1[mixFrameN] = RecordBuffer2[mixFrameN] = 0 ;
    else { RecordBuffer1[mixFrameN] = in1[frameN] ; RecordBuffer2[mixFrameN] = in2[frameN] ; }

    // mix unmuted tracks into outputs mix buffers
    for (loopIter = loopsBeginIter ; loopIter != loopsEndIter ; ++loopIter)
    {
      if (CurrentScene->isMuted && (*loopIter)->isMuted) continue ;

      aLoop = *loopIter ; vol = aLoop->vol ;
      RecordBuffer1[mixFrameN] += aLoop->buffer1[sceneFrameN] * vol ;
      RecordBuffer2[mixFrameN] += aLoop->buffer2[sceneFrameN] * vol ;
    }
  }

  // write output mix buffers to outputs and write input to record buffers
  Sample* mixBegin1 = RecordBuffer1 + offsetFrameN ;
  Sample* mixBegin2 = RecordBuffer2 + offsetFrameN ;
  memcpy(out1      , mixBegin1 , PeriodSize) ; memcpy(out2      , mixBegin2 , PeriodSize) ;
  memcpy(mixBegin1 , in1       , PeriodSize) ; memcpy(mixBegin2 , in2       , PeriodSize) ;

  // increment sample rollover
  if (!(CurrentScene->currentFrameN = (currentFrameN + nFrames) % CurrentScene->endFrameN))
  {
// TODO: bugginess here if currentFrameN rolls over implicitly at RecordBufferSize
//          especially for initial base loop (issue #11)
    unsigned int beginFrameN = CurrentScene->beginFrameN ;
    unsigned int endFrameN   = CurrentScene->endFrameN ;
    unsigned int nLoops      = CurrentScene->loops.size() ;
    unsigned int nLoopBytes  = CurrentScene->nBytes ;
    Sample* leadInBegin1     = RecordBuffer1 + beginFrameN ;
    Sample* leadInBegin2     = RecordBuffer2 + beginFrameN ;
    Sample* loopBegin1       = RecordBuffer1 + beginFrameN + BufferMarginSize ;
    Sample* loopBegin2       = RecordBuffer2 + beginFrameN + BufferMarginSize ;
    Sample* leadOutBegin1    = RecordBuffer1 + endFrameN   + BufferMarginSize ;
    Sample* leadOutBegin2    = RecordBuffer2 + endFrameN   + BufferMarginSize ;
#  if WAIT_FOR_JACK_INIT
    if (endFrameN > RolloverFrameN) endFrameN -= NFramesPerPeriod ; // manual trigger
    else if (endFrameN == RolloverFrameN ||                         // any loop
            (endFrameN <= beginFrameN && !nLoops)) return 0 ;       // base loops
#  else
    if (endFrameN == RecordBufferSize ||                 // any loop
       (endFrameN <= beginFrameN && !nLoops)) return 0 ; // base loops
#  endif // #if WAIT_FOR_JACK_INIT

    // unmute 'paused' loops
#if AUTO_UNMUTE_LOOPS_ON_ROLLOVER
    CurrentScene->isMuted = false ;
#endif // #if AUTO_UNMUTE_LOOPS_ON_ROLLOVER

    // create new Loop instance
    if (CurrentScene->shouldSaveLoop && nLoops < N_LOOPS)
    {
// TODO: adjustable loop seams (issue #14)
/* NOTE: on RecordBuffer layout

    input data is written to RecordBuffer at currentFrameN + BufferMarginSize + TriggerLatencyNFrames
        to allow for trigger delay compensation and dynamic adjustment of seams
    for the initial base loop beginFrameN may be any (< JackIO::RolloverFrameN)
    for all other loops beginFrameN will be BufferMarginSize


// MarginSize + TriggerLatencyNFrames implicitly added to offsets in mixdown stage above
    but the LeadIn and LeadOut buffers are currently unused

                          INITIAL STATE FOR ALL LOOPS
currentFrameN                                          RolloverFrameN         // offset
beginFrameN                                              endFrameN            // offsets
 |                                                           |
 |<----------------------RolloverRange---------------------->|<-MarginSize->| // sizes
 |<------------------------------RecordBuffer------------------------------>| // buffer


                        FINAL STATE FOR INITIAL BASE LOOP
                         (note: <-?-> is >= MarginSize)
   beginFrameN                      endFrameN                                 // offsets
     leadIn       loopBegin             |           leadOut                   // pointers
       |              |                 |              |
       |              |                 |<-MarginSize->|                      // size
 |<-?->|<-MarginSize->|<-----------nFrames------------>|<-MarginSize->|<-?->| // sizes
 |     |<---LeadIn--->|<-----------NewLoop------------>|<--LeadOut--->|     | // buffers
 |<------------------------------RecordBuffer------------------------------>| // buffer


                        FINAL STATE FOR SUBSEQUENT LOOPS
                         (note: <-?-> is >= MarginSize)
                                           currentFrameN                      // offset
           beginFrameN                       endFrameN                        // offsets
leadIn      loopBegin                         leadOut                         // pointers
 |              |                                |
 |              |                                |
 |<-MarginSize->|<-----------nFrames------------>|<-MarginSize->|<----?---->| // sizes
 |<---LeadIn--->|<-----------NewLoop------------>|<--LeadOut--->|           | // buffer
 |<------------------------------RecordBuffer------------------------------>| // buffer



// TODO next version perhaps beginFrameN should always include MarginSize and only
    TriggerLatencyNFrames implicitly added in JackIO
                          INITIAL STATE FOR ALL LOOPS
          currentFrameN                                RolloverFrameN         // offset
            beginFrameN                                  endFrameN            // offsets
                |                                            |
 |<-MarginSize->|<--------------RolloverRange--------------->|<-MarginSize->| // sizes
 |<------------------------------RecordBuffer------------------------------>| // buffer


                        FINAL STATE FOR INITIAL BASE LOOP
                         (note: <-?-> is >= MarginSize)
                                                 currentFrameN                // offset
                 beginFrameN                       endFrameN                  // offsets
     leadIn       loopBegin                         leadOut                   // pointers
       |              |                                |
       |              |                                |
 |<-?->|<-MarginSize->|<-----------nFrames------------>|<-MarginSize->|<-?->| // sizes
 |     |<---LeadIn--->|<--------RolloverRange--------->|<--LeadOut--->|     | // zones
 |     |<--------------------------NewLoop--------------------------->|     | // buffer
 |<------------------------------RecordBuffer------------------------------>| // buffer


                        FINAL STATE FOR SUBSEQUENT LOOPS
                         (note: <-?-> is >= MarginSize)
                                           currentFrameN                      // offset
           beginFrameN                       endFrameN                        // offsets
leadIn      loopBegin                         leadOut                         // pointers
 |              |                                |
 |              |                                |
 |<-MarginSize->|<-----------nFrames------------>|<-MarginSize->|<----?---->| // sizes
 |<---LeadIn--->|<--------RolloverRange--------->|<--LeadOut--->|           | // zones
 |<--------------------------NewLoop--------------------------->|           | // buffer
 |<------------------------------RecordBuffer------------------------------>| // buffer


on each rollover
    set frameN = beginFrameN
    copy tail end of RecordBuffer back to the beginning of RecordBuffer
    this will be the leadIn of the next loop (may or may not be part of a previous loop)
        RecordBuffer[endFrameN] upto RecordBuffer[leadOut + TriggerLatencyNFrames]
            --> RecordBuffer[0]

on creation of the initial base loop
    set frameN = beginFrameN = BufferMarginSize and endFrameN = BufferMarginSize + nFrames
    all subsequent loops will use beginFrameN and endFrameN (dynamically) as seams

on creation of the subsequent loops
    copy the entire buffer to include per loop leadIn and leadOut
        RecordBuffer[0] upto RecordBuffer[leadOut + TriggerLatencyNFrames]
            --> NewLoop[0]

after creation of the subsequent loops (first pass only)
    in the mixing stage copy the first BufferMarginSize - TriggerLatencyNFrames samples
    also to the end of the previous NewLoop
    this will be the leadOut of the previous loop (may or may not be part of a later loop)
        input[frameN == 0] upto input[frameN == BufferMarginSize - TriggerLatencyNFrames - 1]
            --> RecordBuffer[BufferMarginSize + TriggerLatencyNFrames] upto RecordBuffer[BufferMarginSize * 2]
            --> NewLoop[BufferMarginSize + nFrames + TriggerLatencyNFrames]
*/

      // copy audio samples - (see note on RecordBuffer layout in jack_o.h)
      if ((NewLoop = new (nothrow) Loop(CurrentScene->nFrames)))
      {
        // copy record buffers to new Loop
        memcpy(NewLoop->buffer1 , loopBegin1 , nLoopBytes) ;
        memcpy(NewLoop->buffer2 , loopBegin2 , nLoopBytes) ;
        if (!nLoops)
        {
          // copy leading and trailing seconds to auxiliary buffers for editing seam
          memcpy(LeadInBuffer1  , leadInBegin1  , NMarginBytes) ;
          memcpy(LeadInBuffer2  , leadInBegin2  , NMarginBytes) ;
          memcpy(LeadOutBuffer1 , leadOutBegin1 , NMarginBytes) ;
          memcpy(LeadOutBuffer2 , leadOutBegin2 , NMarginBytes) ;

          CurrentScene->beginFrameN = 0 ;
          CurrentScene->endFrameN   = CurrentScene->nFrames ;
        }

        EventLoopCreationSceneN = CurrentSceneN ; SDL_PushEvent(&EventLoopCreation) ;
      }
      else Loopidity::OOM() ;
    }

    // copy trunctuated TriggerLatencyNFrames to beginning of RecordBuffer for next loop
    unsigned int nLeadInBytes = TriggerLatencyNFrames * N_BYTES_PER_FRAME ;
    memcpy(RecordBuffer1 + BufferMarginSize , leadOutBegin1 , nLeadInBytes) ;
    memcpy(RecordBuffer2 + BufferMarginSize , leadOutBegin2 , nLeadInBytes) ;

    // switch to NextScene if necessary
    if (CurrentScene != NextScene)
    {
      CurrentScene = NextScene ; CurrentSceneN = EventSceneChangeSceneN = NextSceneN ;
      SDL_PushEvent(&EventSceneChange) ;
    }
  }

  return 0 ;
}
#else
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
    if (!ShouldMonitorInputs) RecordBuffer1[frameIdx] = RecordBuffer2[frameIdx] = 0 ;
    else { RecordBuffer1[frameIdx] = in1[frameN] ; RecordBuffer2[frameIdx] = in2[frameN] ; }

    // mix unmuted tracks into outputs mix buffers
    for (loopIter = loopsBeginIter ; loopIter != loopsEndIter ; ++loopIter)
    {
      if (CurrentScene->isMuted && (*loopIter)->isMuted) continue ;

      aLoop = *loopIter ; vol = aLoop->vol ;
      RecordBuffer1[frameIdx] += aLoop->buffer1[frameIdx] * vol ;
      RecordBuffer2[frameIdx] += aLoop->buffer2[frameIdx] * vol ;
    }
  }

  // write output mix buffers to outputs and write input to record buffers
  Sample* buf1 = RecordBuffer1 + currFrameN ; Sample* buf2 = RecordBuffer2 + currFrameN ;
  memcpy(out1 , buf1 , PeriodSize)          ; memcpy(out2 , buf2 , PeriodSize) ;
  memcpy(buf1 , in1  , PeriodSize)          ; memcpy(buf2 , in2  , PeriodSize) ;

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
        memcpy(NewLoop->buffer1 , RecordBuffer1 , CurrentScene->nBytes) ;
        memcpy(NewLoop->buffer2 , RecordBuffer2 , CurrentScene->nBytes) ;
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
#endif // #if SCENE_NFRAMES_EDITABLE

int JackIO::SetBufferSizeCallback(jack_nframes_t nFrames , void* arg)
#if SCENE_NFRAMES_EDITABLE
{
  SampleRate            = jack_get_sample_rate(Client) ;
  NBytesPerSecond       = N_BYTES_PER_FRAME * SampleRate ;
  NFramesPerPeriod      = nFrames ;
  PeriodSize            = N_BYTES_PER_FRAME * NFramesPerPeriod ;
  BufferMarginSize   = BUFFER_MARGIN_SIZE ;
  NMarginBytes       = N_BYTES_PER_FRAME * BufferMarginSize ;
#if WAIT_FOR_JACK_INIT
  RolloverFrameN        = (RecordBufferSize - SampleRate) - NFramesPerPeriod ;
#endif // #if WAIT_FOR_JACK_INIT
  NFramesPerGuiInterval = (unsigned int)(SampleRate * (float)GUI_UPDATE_IVL * 0.001) ;
  if ((LeadInBuffer1  = new (nothrow) Sample[NBytesPerSecond]()) &&
      (LeadInBuffer2  = new (nothrow) Sample[NBytesPerSecond]()) &&
      (LeadOutBuffer1 = new (nothrow) Sample[NBytesPerSecond]()) &&
      (LeadOutBuffer2 = new (nothrow) Sample[NBytesPerSecond]()))
#if WAIT_FOR_JACK_INIT
    Loopidity::SetMetaData(SampleRate , nFrames , N_BYTES_PER_FRAME ,
                           RecordBufferSize , RolloverFrameN) ;
#else
    Loopidity::SetMetaData(SampleRate , N_BYTES_PER_FRAME , nFrames) ;
#endif // #if WAIT_FOR_JACK_INIT
  else printf(INSUFFICIENT_MEMORY_MSG) ;

  return 0 ;
}
#else
{
  PeriodSize            = N_BYTES_PER_FRAME * (NFramesPerPeriod = nFrames) ;
  NBytesPerSecond       = N_BYTES_PER_FRAME * (SampleRate = jack_get_sample_rate(Client)) ;
  NFramesPerGuiInterval = (unsigned int)(SampleRate * (float)GUI_UPDATE_IVL * 0.001) ;
  Loopidity::SetMetaData(SampleRate , N_BYTES_PER_FRAME , nFrames) ;

  return 0 ;
}
#endif // #if SCENE_NFRAMES_EDITABLE

void JackIO::ShutdownCallback(void* arg)
{
  // close client and free resouces
  if (Client)      { jack_client_close(Client) ; free(Client) ; Client = 0 ; }
  if (PortInput1)  { free(PortInput1)  ; PortInput1  = 0 ; }
  if (PortInput2)  { free(PortInput2)  ; PortInput2  = 0 ; }
  if (PortOutput1) { free(PortOutput1) ; PortOutput1 = 0 ; }
  if (PortOutput2) { free(PortOutput2) ; PortOutput2 = 0 ; }
  if (RecordBuffer1)     { delete RecordBuffer1    ; RecordBuffer1     = 0 ; }
  if (RecordBuffer2)     { delete RecordBuffer2    ; RecordBuffer2     = 0 ; }
  exit(1) ;
}
