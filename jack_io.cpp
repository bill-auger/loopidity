
#include "jack_io.h"


/* JackIO class side private constants */

const unsigned int JackIO::N_PORTS              = N_AUDIO_PORTS ;
const unsigned int JackIO::N_INPUT_PORTS        = N_INPUT_CHANNELS ;
const unsigned int JackIO::N_OUTPUT_PORTS       = N_OUTPUT_CHANNELS ;
const unsigned int JackIO::N_TRANSIENT_PEAKS    = N_PEAKS_TRANSIENT ;
const unsigned int JackIO::DEFAULT_BUFFER_SIZE  = DEFAULT_AUDIO_BUFFER_SIZE ;
const unsigned int JackIO::N_BYTES_PER_FRAME    = sizeof(Sample) ;
const unsigned int JackIO::GUI_UPDATE_IVL       = GUI_UPDATE_INTERVAL ;


/* JackIO class side private varables */

// JACK handles
jack_client_t* JackIO::Client      = 0 ; // Init()
#if FIXED_N_AUDIO_PORTS
jack_port_t*   JackIO::InputPort1  = 0 ; // Init()
jack_port_t*   JackIO::InputPort2  = 0 ; // Init()
jack_port_t*   JackIO::OutputPort1 = 0 ; // Init()
jack_port_t*   JackIO::OutputPort2 = 0 ; // Init()
#else // TODO: much
#endif // #if FIXED_N_AUDIO_PORTS

// app state
Scene*       JackIO::CurrentScene  = 0 ; // Reset()
Scene*       JackIO::NextScene     = 0 ; // Reset()
//unsigned int JackIO::CurrentSceneN = 0 ;
//unsigned int JackIO::NextSceneN    = 0 ;

// audio data
unsigned int JackIO::RecordBufferSize = 0 ; // Init()
#if FIXED_N_AUDIO_PORTS
Sample*      JackIO::RecordBuffer1    = 0 ; // Init()
Sample*      JackIO::RecordBuffer2    = 0 ; // Init()
#  if SCENE_NFRAMES_EDITABLE
/*
Sample* JackIO::LeadInBuffer1  = 0 ; // SetMetadata()
Sample* JackIO::LeadInBuffer2  = 0 ; // SetMetadata()
Sample* JackIO::LeadOutBuffer1 = 0 ; // SetMetadata()
Sample* JackIO::LeadOutBuffer2 = 0 ; // SetMetadata()
*/
#  endif // #if SCENE_NFRAMES_EDITABLE
#else // TODO: much
#endif // #if FIXED_N_AUDIO_PORTS

// peaks data
vector<Sample> JackIO::PeaksIn ;                         // Reset()
vector<Sample> JackIO::PeaksOut ;                        // Reset()
Sample         JackIO::TransientPeaks[N_PORTS] = {0.0} ;
Sample         JackIO::TransientPeakInMix      = 0 ;
//Sample         JackIO::TransientPeakOutMix     = 0 ;

// event structs
SDL_Event    JackIO::NewLoopEvent ;               // Init()
unsigned int JackIO::NewLoopEventSceneN     = 0 ; // Init()
Loop*        JackIO::NewLoopEventLoop       = 0 ; // Init()
SDL_Event    JackIO::SceneChangeEvent ;           // Init()
unsigned int JackIO::SceneChangeEventSceneN = 0 ; // Init()

// metadata
jack_nframes_t JackIO::SampleRate           = 0 ; // SetMetadata()
//unsigned int   JackIO::NBytesPerSecond      = 0 ; // SetMetadata()
//jack_nframes_t JackIO::NFramesPerPeriod     = 0 ; // SetMetadata()
#if SCENE_NFRAMES_EDITABLE
unsigned int   JackIO::MinLoopSize          = 0 ; // SetMetadata()
unsigned int   JackIO::BufferMarginSize     = 0 ; // SetMetadata()
unsigned int   JackIO::TriggerLatencySize   = 0 ; // SetMetadata()
#  if INIT_JACK_BEFORE_SCENES
unsigned int   JackIO::EndFrameN            = 0 ; // SetMetaData()
#  endif // #if INIT_JACK_BEFORE_SCENES
unsigned int   JackIO::BeginFrameN          = 0 ; // SetMetadata()
unsigned int   JackIO::BufferMarginsSize    = 0 ; // SetMetadata()
unsigned int   JackIO::BytesPerPeriod       = 0 ; // SetMetadata()
unsigned int   JackIO::BufferMarginBytes    = 0 ; // SetMetadata()
unsigned int   JackIO::TriggerLatencyBytes  = 0 ; // SetMetadata()
#else
#  if INIT_JACK_BEFORE_SCENES
unsigned int   JackIO::EndFrameN            = 0 ; // SetMetaData()
#  endif // #if INIT_JACK_BEFORE_SCENES
unsigned int   JackIO::BytesPerPeriod       = 0 ; // SetMetadata()
#endif // #if SCENE_NFRAMES_EDITABLE
unsigned int   JackIO::FramesPerGuiInterval = 0 ; // SetMetadata()

// misc flags
bool JackIO::ShouldMonitorInputs = true ;


/* JackIO class side public functions */

// setup
#if INIT_JACK_BEFORE_SCENES
unsigned int JackIO::Init(bool shouldMonitorInputs , unsigned int recordBufferSize)
#else
unsigned int JackIO::Init(Scene* currentScene , bool shouldMonitorInputs ,
                          unsigned int recordBufferSize)
#endif // #if INIT_JACK_BEFORE_SCENES
{
DEBUG_TRACE_JACK_INIT

  // set initial state
#if INIT_JACK_BEFORE_SCENES
  ShouldMonitorInputs = shouldMonitorInputs ;
#else
  Reset(currentScene) ; ShouldMonitorInputs = shouldMonitorInputs ;
#endif // #if INIT_JACK_BEFORE_SCENES

  // initialize record buffers
  if (!(RecordBufferSize = recordBufferSize / N_BYTES_PER_FRAME))
    RecordBufferSize = DEFAULT_BUFFER_SIZE ;
  if (!(RecordBuffer1 = new (nothrow) Sample[RecordBufferSize]()) ||
      !(RecordBuffer2 = new (nothrow) Sample[RecordBufferSize]()))
    return JACK_MEM_FAIL ;

  // initialize SDL event structs
  NewLoopEvent.type           = SDL_USEREVENT ;
  NewLoopEvent.user.code      = EVT_NEW_LOOP ;
  NewLoopEvent.user.data1     = &NewLoopEventSceneN ;
  NewLoopEvent.user.data2     = &NewLoopEventLoop ;
  SceneChangeEvent.type       = SDL_USEREVENT ;
  SceneChangeEvent.user.code  = EVT_SCENE_CHANGED ;
  SceneChangeEvent.user.data1 = &SceneChangeEventSceneN ;
  SceneChangeEvent.user.data2 = 0 ; // n/a

  // register JACK client
  if (!(Client = jack_client_open(APP_NAME , JackNoStartServer , NULL)))
    return JACK_FAIL ;

#if INIT_JACK_BEFORE_SCENES
#define DUMMY_SCENEN -1
  // instantiate dummy Scene
//  CurrentScene = Scene::DummyScene ;
#endif // #if !INIT_JACK_BEFORE_SCENES

  // assign server callbacks
  jack_set_process_callback(    Client , ProcessCallback    , 0) ;
  jack_set_sample_rate_callback(Client , SampleRateCallback , 0) ;
  jack_set_buffer_size_callback(Client , BufferSizeCallback , 0) ;
  jack_on_shutdown(             Client , ShutdownCallback   , 0) ;

  // register I/O ports
#if INIT_JACK_BEFORE_SCENES
  if (!(InputPort1  = RegisterPort(JACK_INPUT1_PORT_NAME  , JackPortIsInput))  ||
      !(InputPort2  = RegisterPort(JACK_INPUT2_PORT_NAME  , JackPortIsInput))  ||
      !(OutputPort1 = RegisterPort(JACK_OUTPUT1_PORT_NAME , JackPortIsOutput)) ||
      !(OutputPort2 = RegisterPort(JACK_OUTPUT2_PORT_NAME , JackPortIsOutput)))
    return JACK_FAIL ;
#else
  if (!(InputPort1  = RegisterPort(JACK_INPUT1_PORT_NAME  , JackPortIsInput))  ||
      !(InputPort2  = RegisterPort(JACK_INPUT2_PORT_NAME  , JackPortIsInput))  ||
      !(OutputPort1 = RegisterPort(JACK_OUTPUT1_PORT_NAME , JackPortIsOutput)) ||
      !(OutputPort2 = RegisterPort(JACK_OUTPUT2_PORT_NAME , JackPortIsOutput)) ||
      jack_activate(Client)) return JACK_FAIL ;
#endif // #if INIT_JACK_BEFORE_SCENES

  // propogate server state
  SetMetaData(jack_get_sample_rate(Client) , jack_get_buffer_size(Client)) ;

  return JACK_INIT_SUCCESS ;
}

void JackIO::Reset(Scene* currentScene)
{
DEBUG_TRACE_JACK_RESET

  if (!(CurrentScene = NextScene = currentScene)) return ;

//  CurrentScene   = NextScene  = currentScene ;
//  CurrentSceneN  = NextSceneN = currentScene->sceneN ;
//  BytesPerPeriod = FramesPerPeriod = 0 ;
  BytesPerPeriod = 0 ;

  // initialize scope/VU peaks cache
  for (unsigned int peakN = 0 ; peakN < N_TRANSIENT_PEAKS ; ++peakN)
    { PeaksIn.push_back(0.0) ; PeaksOut.push_back(0.0) ; }

#if INIT_JACK_BEFORE_SCENES
  // destroy dummy Scene
//  if (DummyScene) { delete DummyScene ; DummyScene = 0 ; }

  // begin processing audio
  jack_activate(Client) ;
#endif // #if INIT_JACK_BEFORE_SCENES
}


// getters/setters
#if !INIT_JACK_BEFORE_SCENES
unsigned int JackIO::GetRecordBufferSize() { return RecordBufferSize ; }
#endif // #if !INIT_JACK_BEFORE_SCENES
/*
unsigned int JackIO::GetNFramesPerPeriod() { return NFramesPerPeriod ; }

unsigned int JackIO::GetFrameSize() { return N_BYTES_PER_FRAME ; }

unsigned int JackIO::GetSampleRate() { return SampleRate ; }

unsigned int JackIO::GetNBytesPerSecond() { return NBytesPerSecond ; }
*/
void JackIO::SetCurrentScene(Scene* currentScene) { CurrentScene = currentScene ; }

void JackIO::SetNextScene(Scene* nextScene) { NextScene = nextScene ; }

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
  unsigned int currentFrameN = CurrentScene->currentFrameN - FramesPerGuiInterval ;
  unsigned int offsetFrameN  = currentFrameN ;//+ TriggerLatencySize ;
//   unsigned int currentFrameN = CurrentScene->currentFrameN ;
//   currentFrameN = (currentFrameN < FramesPerGuiInterval)? 0 : currentFrameN - FramesPerGuiInterval ;
//  unsigned int offsetFrameN  = currentFrameN + BufferMarginSize ;//+ TriggerLatencySize ;
  unsigned int nFrames = FramesPerGuiInterval ;

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
  frameN = (frameN < FramesPerGuiInterval)? 0 : frameN - FramesPerGuiInterval ;

  // initialize with inputs
  Sample peakIn1 = GetPeak(&(RecordBuffer1[frameN]) , FramesPerGuiInterval) ;
  Sample peakIn2 = GetPeak(&(RecordBuffer2[frameN]) , FramesPerGuiInterval) ;

  // add in unmuted tracks
  Sample peakOut1     = 0.0 ; Sample peakOut2 = 0.0 ;
  unsigned int nLoops = CurrentScene->loops.size() ;
  for (unsigned int loopN = 0 ; loopN < nLoops ; ++loopN)
  {
    Loop* loop = CurrentScene->getLoop(loopN) ;
    if (CurrentScene->isMuted && loop->isMuted) continue ;

    peakOut1 += GetPeak(&(loop->buffer1[frameN]) , FramesPerGuiInterval) * loop->vol ;
    peakOut2 += GetPeak(&(loop->buffer2[frameN]) , FramesPerGuiInterval) * loop->vol ;
  }
#  endif // #if SCENE_NFRAMES_EDITABLE

#  if FIXED_N_AUDIO_PORTS
  Sample peakIn  = (peakIn1 + peakIn2)   / N_INPUT_PORTS ;
  Sample peakOut = (peakOut1 + peakOut2) / N_OUTPUT_PORTS ;
  if (peakOut > 1.0) peakOut = 1.0 ;
#  else
  Sample peakIn  = (peakIn1 + peakIn2)   / N_INPUT_PORTS ;
  Sample peakOut = (peakOut1 + peakOut2) / N_OUTPUT_PORTS ;
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
#  else // TODO: magic #s ~= N_PORTS (see #def)
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

// JACK server callbacks

int JackIO::ProcessCallback(jack_nframes_t nFramesPerPeriod , void* unused)
#if SCENE_NFRAMES_EDITABLE
{
DEBUG_TRACE_JACK_PROCESS_CALLBACK_IN

#if JACK_IO_READ_WRITE
  // get JACK buffers
  Sample* in1  = (Sample*)jack_port_get_buffer(InputPort1  , nFramesPerPeriod) ;
  Sample* out1 = (Sample*)jack_port_get_buffer(OutputPort1 , nFramesPerPeriod) ;
  Sample* in2  = (Sample*)jack_port_get_buffer(InputPort2  , nFramesPerPeriod) ;
  Sample* out2 = (Sample*)jack_port_get_buffer(OutputPort2 , nFramesPerPeriod) ;

  // index into the record buffers and mix out
  list<Loop*>::iterator loopIter , loopsBeginIter , loopsEndIter ; Loop* aLoop ; float vol ;
  loopsBeginIter            = CurrentScene->loops.begin() ;
  loopsEndIter              = CurrentScene->loops.end() ;
  unsigned int sceneFrameN  = CurrentScene->currentFrameN , frameN ;
  unsigned int offsetFrameN = CurrentScene->currentFrameN ;//+ BufferMarginSize ;
  unsigned int mixFrameN    = offsetFrameN ;
  for (frameN = 0 ; frameN < nFramesPerPeriod ; ++frameN && ++mixFrameN && ++sceneFrameN)
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
  memcpy(out1      , mixBegin1 , BytesPerPeriod) ;
  memcpy(out2      , mixBegin2 , BytesPerPeriod) ;
  memcpy(mixBegin1 , in1       , BytesPerPeriod) ;
  memcpy(mixBegin2 , in2       , BytesPerPeriod) ;
#endif // #if JACK_IO_READ_WRITE

  // increment ring buffer index
  if (!((CurrentScene->currentFrameN += nFramesPerPeriod) % CurrentScene->endFrameN))
    CurrentScene->currentFrameN = BeginFrameN ;
  else return 0 ;

  unsigned int beginFrameN = CurrentScene->beginFrameN ;
  unsigned int endFrameN   = CurrentScene->endFrameN ;
  unsigned int nLoops      = CurrentScene->loops.size() ;
  unsigned int nFrames     = CurrentScene->nFrames ;
  bool isBaseLoop          = !nLoops ;

DEBUG_TRACE_JACK_PROCESS_CALLBACK_ROLLOVER

  // sanity checks
#  if ALLOW_BUFFER_ROLLOVER
#    if INIT_JACK_BEFORE_SCENES
  if (endFrameN > EndFrameN) endFrameN = EndFrameN ; // manual trigger (quite unlikely)
  else if (isBaseLoop &&
          (endFrameN == EndFrameN   || // rollover before base loop exists
           endFrameN <= beginFrameN || nFrames < MinLoopSize))
    { CurrentScene->endFrameN = EndFrameN ; return 0 ; }
#    else
  if (isBaseLoop &&
      (endFrameN == RecordBufferSize || // rollover while recording base loop
      endFrameN <= beginFrameN || nFrames < MinLoopSize))
    { CurrentScene->endFrameN = RecordBufferSize ; return 0 ; }
#    endif // #if INIT_JACK_BEFORE_SCENES
#  else
#    if INIT_JACK_BEFORE_SCENES
  // bail if currentFrameN rolls over implicitly (issue #11)
  if (isBaseLoop && endFrameN == EndFrameN)
    { Loopidity::ResetCurrentScene() ; return 0 ; }
  // bail if loop too short (issue #12)
  if (isBaseLoop && nFrames < MinLoopSize)
    { CurrentScene->endFrameN = EndFrameN ; return 0 ; }
#    else
  // bail if currentFrameN rolls over implicitly (issue #11)
  if (isBaseLoop && endFrameN == RecordBufferSize)
    { Loopidity::ResetCurrentScene() ; return 0 ; }
  // bail if loop too short (issue #12)
  if (isBaseLoop && nFrames < MinLoopSize)
    { CurrentScene->endFrameN = RecordBufferSize ; return 0 ; }
#    endif // #if INIT_JACK_BEFORE_SCENES
#  endif // #if ALLOW_BUFFER_ROLLOVER

  // unmute 'paused' loops
#  if AUTO_UNMUTE_LOOPS_ON_ROLLOVER
  CurrentScene->isMuted = false ;
#  endif // #if AUTO_UNMUTE_LOOPS_ON_ROLLOVER

  // create new Loop instance
  if (CurrentScene->shouldSaveLoop && nLoops < Loopidity::N_LOOPS)
  {
// TODO: adjustable loop seams (issue #14)

    // copy audio samples - (see note on RecordBuffer layout in jack_io.h)
    if ((NewLoopEventLoop = new (nothrow) Loop(nFrames + BufferMarginsSize)))
    {
DEBUG_TRACE_JACK_PROCESS_CALLBACK_NEW_LOOP

#if JACK_IO_COPY
      size_t thisLeadInFrameN  = beginFrameN       - BufferMarginSize ;
      size_t nextLeadInFrameN  = thisLeadInFrameN  + nFrames ;
      Sample* thisLeadInBegin1 = RecordBuffer1     + thisLeadInFrameN ;
      Sample* thisLeadInBegin2 = RecordBuffer2     + thisLeadInFrameN ;
      Sample* nextLeadInBegin1 = RecordBuffer1     + nextLeadInFrameN ;
      Sample* nextLeadInBegin2 = RecordBuffer2     + nextLeadInFrameN ;
//       Sample* loopBegin1       = RecordBuffer1 +  beginFrameN ;//+ BufferMarginSize ;
//       Sample* loopBegin2       = RecordBuffer2 +  beginFrameN ;//+ BufferMarginSize ;
//       Sample* leadOutBegin1    = RecordBuffer1 + endFrameN   + BufferMarginSize ;
//       Sample* leadOutBegin2    = RecordBuffer2 + endFrameN   + BufferMarginSize ;
//      unsigned int nLoopBytes  = CurrentScene->nFrames * N_BYTES_PER_FRAME ;
      size_t nThisLoopBytes    = BufferMarginBytes + CurrentScene->nBytes ;
      size_t nNextLeadInBytes  = BufferMarginBytes ;

      // copy record buffers to new Loop
//         memcpy(NewLoop->buffer1 , loopBegin1 , nLoopBytes) ;
//         memcpy(NewLoop->buffer2 , loopBegin2 , nLoopBytes) ;
//       memcpy(NewLoop->buffer1 , leadInBegin1 , nLoopBytes + BufferMarginBytes) ;
//       memcpy(NewLoop->buffer2 , leadInBegin1 , nLoopBytes + BufferMarginBytes) ;
      memcpy(NewLoopEventLoop->buffer1 , thisLeadInBegin1 , nThisLoopBytes) ;
      memcpy(NewLoopEventLoop->buffer2 , thisLeadInBegin2 , nThisLoopBytes) ;
#endif // #if JACK_IO_COPY

      if (isBaseLoop)
      {
        // copy leading and trailing seconds to auxiliary buffers for editing seam
//         memcpy(LeadInBuffer1  , leadInBegin1  , BufferMarginBytes) ;
//         memcpy(LeadInBuffer2  , leadInBegin2  , BufferMarginBytes) ;
//           memcpy(LeadOutBuffer1 , leadOutBegin1 , BufferMarginBytes) ;
//           memcpy(LeadOutBuffer2 , leadOutBegin2 , BufferMarginBytes) ;
#if JACK_IO_COPY
        // 'shift' last BufferMarginSize + TriggerLatencySize back for next loop leadIn
        nNextLeadInBytes += TriggerLatencyBytes ;
        memcpy(RecordBuffer1 , nextLeadInBegin1 , nNextLeadInBytes) ;
        memcpy(RecordBuffer2 , nextLeadInBegin2 , nNextLeadInBytes) ;
#endif // #if JACK_IO_COPY

        // align buffer indicies to base loop
        CurrentScene->beginFrameN   = BeginFrameN ;
        CurrentScene->currentFrameN = BeginFrameN + TriggerLatencySize ;
        CurrentScene->endFrameN     = BeginFrameN + nFrames ;
      }

      NewLoopEventSceneN = CurrentScene->sceneN ; SDL_PushEvent(&NewLoopEvent) ;
    }
    else Loopidity::OOM() ;
  }
/*
#if JACK_IO_COPY
  // copy trunctuated TriggerLatencySize to beginning of RecordBuffer for next loop
  unsigned int nLeadInBytes = TriggerLatencySize * N_BYTES_PER_FRAME ;
  memcpy(RecordBuffer1 + BufferMarginSize , leadOutBegin1 , nLeadInBytes) ;
  memcpy(RecordBuffer2 + BufferMarginSize , leadOutBegin2 , nLeadInBytes) ;
#endif // #if JACK_IO_COPY
*/
  // switch to NextScene if necessary
  if (CurrentScene != NextScene)
  {
    CurrentScene = NextScene ; SceneChangeEventSceneN = NextScene->sceneN ;
    SDL_PushEvent(&SceneChangeEvent) ;
  }

  return 0 ;
}
#else
{
  // get JACK buffers
Sample* in1  = (Sample*)jack_port_get_buffer(InputPort1  , nFrames) ;
Sample* out1 = (Sample*)jack_port_get_buffer(OutputPort1 , nFrames) ;
Sample* in2  = (Sample*)jack_port_get_buffer(InputPort2  , nFrames) ;
Sample* out2 = (Sample*)jack_port_get_buffer(OutputPort2 , nFrames) ;

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
  memcpy(out1 , buf1 , BytesPerPeriod) ; memcpy(out2 , buf2 , BytesPerPeriod) ;
  memcpy(buf1 , in1  , BytesPerPeriod) ; memcpy(buf2 , in2  , BytesPerPeriod) ;

  // increment sample rollover
  if (!(CurrentScene->frameN = (CurrentScene->frameN + nFrames) % CurrentScene->nFrames))
  {
#  if AUTO_UNMUTE_LOOPS_ON_ROLLOVER
    // unmute 'paused' loops
    CurrentScene->isMuted = false ;
#  endif // #if AUTO_UNMUTE_LOOPS_ON_ROLLOVER

    // create new Loop instance and copy record buffers to it
    if (CurrentScene->shouldSaveLoop && CurrentScene->loops.size() < Loopidity::N_LOOPS)
    {
      if ((NewLoopEventLoop = new (nothrow) Loop(CurrentScene->nFrames)))
      {
        memcpy(EventLoopCreationLoop->buffer1 , RecordBuffer1 , CurrentScene->nBytes) ;
        memcpy(EventLoopCreationLoop->buffer2 , RecordBuffer2 , CurrentScene->nBytes) ;
        NewLoopEventSceneN = CurrentScene->sceneN ; SDL_PushEvent(&NewLoopEvent) ;
      }
      else Loopidity::OOM() ;
    }

    // switch to NextScene if necessary
    if (CurrentScene != NextScene)
    {
      CurrentScene = NextScene ; SceneChangeEventSceneN = NextScene->sceneN ;
      SDL_PushEvent(&SceneChangeEvent) ;
    }
  }

  return 0 ;
}
#endif // #if SCENE_NFRAMES_EDITABLE

#if SCENE_NFRAMES_EDITABLE
int JackIO::SampleRateCallback(jack_nframes_t sampleRate , void* unused)
  { SetMetaData(sampleRate , jack_get_buffer_size(Client)) ; return 0 ; }
#endif // #if SCENE_NFRAMES_EDITABLE

int JackIO::BufferSizeCallback(jack_nframes_t nFramesPerPeriod , void* unused)
#if SCENE_NFRAMES_EDITABLE
  { SetMetaData(jack_get_sample_rate(Client) , nFramesPerPeriod) ; return 0 ; }
#else
{
  BytesPerPeriod       = N_BYTES_PER_FRAME * nFramesPerPeriod ;
//  NBytesPerSecond       = N_BYTES_PER_FRAME * (SampleRate = jack_get_sample_rate(Client)) ;
  FramesPerGuiInterval = (unsigned int)(SampleRate * (float)GUI_UPDATE_IVL * 0.001) ;
#  if INIT_JACK_BEFORE_SCENES
  Loopidity::SetMetaData(SampleRate , nFramesPerPeriod , RecordBufferSize) ;
#  else
  Loopidity::SetMetaData(SampleRate , nFramesPerPeriod) ;
#  endif // #if INIT_JACK_BEFORE_SCENES

  return 0 ;
}
#endif // #if SCENE_NFRAMES_EDITABLE

void JackIO::ShutdownCallback(void* unused)
{
  // close client and free resouces
  if (Client)        { jack_client_close(Client) ; }
  if (Client)        { free(Client) ;         Client        = 0 ; }
  if (InputPort1)    { free(InputPort1) ;     InputPort1    = 0 ; }
  if (InputPort2)    { free(InputPort2) ;     InputPort2    = 0 ; }
  if (OutputPort1)   { free(OutputPort1) ;    OutputPort1   = 0 ; }
  if (OutputPort2)   { free(OutputPort2) ;    OutputPort2   = 0 ; }
  if (RecordBuffer1) { delete RecordBuffer1 ; RecordBuffer1 = 0 ; }
  if (RecordBuffer2) { delete RecordBuffer2 ; RecordBuffer2 = 0 ; }
  exit(1) ;
}


// helpers

jack_port_t* JackIO::RegisterPort(const char* portName , unsigned long portFlags)
  { return jack_port_register(Client , portName , JACK_DEFAULT_AUDIO_TYPE , portFlags , 0) ; }

#if SCENE_NFRAMES_EDITABLE
void JackIO::SetMetaData(jack_nframes_t sampleRate , jack_nframes_t nFramesPerPeriod)
{
  SampleRate           = sampleRate ;
//  NFramesPerPeriod     = nFramesPerPeriod ;
//  NBytesPerSecond      = SampleRate * N_BYTES_PER_FRAME ;
  MinLoopSize          = SampleRate         *  MINIMUM_LOOP_DURATION ;
  BufferMarginSize     = nFramesPerPeriod   * (BUFFER_MARGIN_SIZE   / nFramesPerPeriod) ;
  TriggerLatencySize   = nFramesPerPeriod   * (TRIGGER_LATENCY_SIZE / nFramesPerPeriod) ;
#  if INIT_JACK_BEFORE_SCENES
  EndFrameN            = nFramesPerPeriod   * (INITIAL_END_FRAMEN   / nFramesPerPeriod) ;
#  endif // #if INIT_JACK_BEFORE_SCENES
  BeginFrameN          = BufferMarginSize ;
  BufferMarginsSize    = BufferMarginSize * 2 ;
  BytesPerPeriod       = nFramesPerPeriod   *  N_BYTES_PER_FRAME ;
  BufferMarginBytes    = BufferMarginSize   *  N_BYTES_PER_FRAME ;
  TriggerLatencyBytes  = TriggerLatencySize *  N_BYTES_PER_FRAME ;
  FramesPerGuiInterval = (unsigned int)((float)SampleRate * (float)GUI_UPDATE_IVL * 0.001) ;

#  if INIT_JACK_BEFORE_SCENES
DEBUG_TRACE_JACK_SETMETADATA
#  endif // #if INIT_JACK_BEFORE_SCENES
/*
  if ((LeadInBuffer1    = new (nothrow) Sample[NBytesPerSecond]()) &&
      (LeadInBuffer2    = new (nothrow) Sample[NBytesPerSecond]()) &&
      (LeadOutBuffer1   = new (nothrow) Sample[NBytesPerSecond]()) &&
      (LeadOutBuffer2   = new (nothrow) Sample[NBytesPerSecond]()))
#  if INIT_JACK_BEFORE_SCENES
    Loopidity::SetMetaData(SampleRate , NFramesPerPeriod , BeginFrameN , EndFrameN) ;
#  else
    Loopidity::SetMetaData(SampleRate , nFramesPerPeriod) ;
#  endif // #if INIT_JACK_BEFORE_SCENES
  else printf(INSUFFICIENT_MEMORY_MSG) ;
*/
  Loopidity::SetMetaData(SampleRate , nFramesPerPeriod , N_BYTES_PER_FRAME , MinLoopSize ,
                         TriggerLatencySize , BeginFrameN , EndFrameN) ;
}
#endif // #if SCENE_NFRAMES_EDITABLE
