
#include "jack_io.h"


/* Scene class side private varables */

unsigned int Scene::RecordBufferSize = 0 ;
unsigned int Scene::SampleRate       = 0 ;
unsigned int Scene::FrameSize        = 0 ;
unsigned int Scene::NFramesPerPeriod = 0 ;


/* Loop instantce side public functions */

Sample Loop::getPeakFine(unsigned int peakN) { return peaksFine[peakN] ; }

Sample Loop::getPeakCourse(unsigned int peakN) { return peaksCourse[peakN] ; }


/* Loop class side private functions */

Loop::Loop(unsigned int nFrames)
{
  // audio data
  buffer1     = new Sample[nFrames] ;
  buffer2     = new Sample[nFrames] ;

  // peaks cache
  peaksFine[N_PEAKS_FINE]   = {0.0} ;
  peaksCourse[N_PEAKS_FINE] = {0.0} ;

  // loop state
  vol         = 1.0 ;
  isMuted     = false ;
}

Loop::~Loop() { delete buffer1 ; delete buffer2 ; }


/* Scene instance side public functions */

unsigned int Scene::getCurrentPeakN() { return ((float)frameN / nFrames) * N_PEAKS_FINE ; }

//float Scene::getCurrentSeconds() { return frameN / SampleRate ; }

//float Scene::getTotalSeconds() { return nFrames / SampleRate ; }


/* Scene class side private functions */

Scene::Scene(unsigned int sceneNum , unsigned int recordBufferSize)
{
  // identity
  sceneN = sceneNum ;

  // audio data
  list<Loop*> loops ;

  // peaks cache
  hiScenePeaks[N_PEAKS_FINE] = {0.0} ;
  hiLoopPeaks[N_PEAKS_FINE]  = {0.0} ;
  highestScenePeak           = 0.0 ;

  // buffer iteration
  frameN           = 0 ;
#if SCENE_NFRAMES_EDITABLE
beginFrameN      = 0 ;
endFrameN        = RecordBufferSize = recordBufferSize ;
#endif // #if SCENE_NFRAMES_EDITABLE
  nFrames          = RecordBufferSize = recordBufferSize ;
  nFramesPerPeak   = nFrames / N_PEAKS_FINE ;
  nBytes           = 0 ;
  nSeconds         = 0 ;

  // recording state
  isRolling      = false ;
  shouldSaveLoop = false ;
  doesPulseExist = false ;
  isMuted        = false ;
}

// getters/setters

void Scene::SetMetaData(unsigned int sampleRate , unsigned int frameSize , unsigned int nFramesPerPeriod)
	{ SampleRate = sampleRate ; FrameSize = frameSize ; NFramesPerPeriod = nFramesPerPeriod ; }


/* Scene instance side private functions */

// scene state

void Scene::startRolling()
{
DEBUG_TRACE_SCENE_RESET_IN
#if SCENE_NFRAMES_EDITABLE
beginFrameN = frameN ;
#else
frameN = 0 ;
#endif // #if SCENE_NFRAMES_EDITABLE
  isRolling = true ; Loopidity::UpdateView(sceneN) ;

DEBUG_TRACE_SCENE_RESET_OUT
}

void Scene::toggleState()
{
DEBUG_TRACE_SCENE_TOGGLESTATE_IN

  if (!doesPulseExist)
  {
#if SCENE_NFRAMES_EDITABLE
endFrameN = frameN + NFramesPerPeriod ; nFrames = endFrameN - beginFrameN ;
                                           nFramesPerPeak = nFrames / N_PEAKS_FINE ;
#else
    nFrames  = frameN + NFramesPerPeriod ; nFramesPerPeak = nFrames / N_PEAKS_FINE ;
#endif // #if SCENE_NFRAMES_EDITABLE
    nBytes   = FrameSize * nFrames ;       shouldSaveLoop = doesPulseExist = true ;
    nSeconds = nFrames / SampleRate ;
    Loopidity::UpdateView(sceneN) ;
  }
  else shouldSaveLoop = !shouldSaveLoop ;

DEBUG_TRACE_SCENE_TOGGLESTATE_OUT
}


// audio data

bool Scene::addLoop(Loop* newLoop)
{
DEBUG_TRACE_SCENE_ADDLOOP_IN

  unsigned int nLoops = loops.size() ; if (nLoops >= N_LOOPS) return false ;

  scanPeaks(newLoop , nLoops) ; loops.push_back(newLoop) ; return true ;

DEBUG_TRACE_SCENE_ADDLOOP_OUT
}

void Scene::deleteLoop(unsigned int loopN)
{
DEBUG_TRACE_SCENE_DELETELOOP_IN

  if (loops.empty()) { reset() ; return ; }
  if (loopN >= loops.size())     return ;

  list<Loop*>::iterator loopIter = loops.begin() ; while (loopN--) ++loopIter ;
  loops.erase(loopIter) ; rescanPeaks() ;

DEBUG_TRACE_SCENE_DELETELOOP_IN
}

void Scene::reset()
{
DEBUG_TRACE_SCENE_RESET_IN

  frameN    = 0 ; nFrames   = RecordBufferSize ;
  isRolling = shouldSaveLoop = doesPulseExist = false ;
  loops.clear() ; Loopidity::UpdateView(sceneN) ;

DEBUG_TRACE_SCENE_RESET_OUT
}


// peaks cache

void Scene::scanPeaks(Loop* loop , unsigned int loopN)
{
DEBUG_TRACE_SCENE_SCANPEAKS_IN

#if SCAN_LOOP_PEAKS_DATA
  if (!loop || loopN >= N_LOOPS) return ;

  // fill fine peaks arrays
  Sample* peaks = loop->peaksFine ; unsigned int peakN , frameNum ; Sample peak1 , peak2 ;
  for (peakN = 0 ; peakN < N_PEAKS_FINE ; ++peakN)
  {
    frameNum     = nFramesPerPeak * peakN ;
    peak1        = JackIO::GetPeak(&(loop->buffer1[frameNum]) , nFramesPerPeak) ;
    peak2        = JackIO::GetPeak(&(loop->buffer2[frameNum]) , nFramesPerPeak) ;
    peaks[peakN] = (peak1 + peak2) / N_INPUT_CHANNELS ;

    // find the loudest peak for this loop
    if (hiLoopPeaks[loopN] < peaks[peakN]) hiLoopPeaks[loopN] = peaks[peakN] ;

    // find the loudest of each peak from all loops of the current scene
    if (hiScenePeaks[peakN] < peaks[peakN]) hiScenePeaks[peakN] = peaks[peakN] ;
  }
  // find the loudest of all peaks of all loops of the current scene
  if (highestScenePeak < hiLoopPeaks[loopN]) highestScenePeak = hiLoopPeaks[loopN] ;

  // fill course peaks array
  Sample* peaksCourse      = loop->peaksCourse ;
  float nPeaksAsFloat      = (float)N_PEAKS_FINE / (float)N_PEAKS_COURSE ;
  unsigned int nPeaksAsInt = (unsigned int)nPeaksAsFloat ;
  for (unsigned int histPeakN = 0 ; histPeakN < N_PEAKS_COURSE ; ++histPeakN)
  {
    peakN                  = (unsigned int)(nPeaksAsFloat * (float)histPeakN) ;
    peaksCourse[histPeakN] = JackIO::GetPeak(&peaks[peakN] , nPeaksAsInt) ;
  }
#endif // #if SCAN_LOOP_PEAKS_DATA

DEBUG_TRACE_SCENE_SCANPEAKS_OUT
}

void Scene::rescanPeaks()
{
DEBUG_TRACE_SCENE_RESCANPEAKS_IN

  highestScenePeak   = 0.0 ;
  unsigned int peakN = N_PEAKS_FINE ; while (peakN--) hiScenePeaks[peakN] = 0.0 ;
//  unsigned int loopN = N_LOOPS ; while (loopN--)

  list<Loop*>::iterator loopIter ; unsigned int loopN = 0 ;
  for (loopIter = loops.begin() ; loopIter != loops.end() ; ++loopIter)
    { hiLoopPeaks[loopN] = 0.0 ; scanPeaks(*loopIter , loopN) ; ++loopN ; }

DEBUG_TRACE_SCENE_RESCANPEAKS_OUT
}


// getters/setters

Loop* Scene::getLoop(unsigned int loopN)
{
  if (loopN >= loops.size()) return NULL ;

  list<Loop*>::iterator aLoop = loops.begin() ; while (loopN--) ++aLoop ;
  return (*aLoop) ;
}

//bool Scene::getIsRolling() { return isRolling ; }

//unsigned int Scene::getLoopPos() { return (frameN * 1000) / nFrames ; }
