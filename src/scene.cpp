/*\ Loopidity - multitrack audio looper designed for live handsfree use
|*| https://github.com/bill-auger/loopidity/issues/
|*| Copyright 2013,2015 Bill Auger - https://bill-auger.github.io/
|*|
|*| This file is part of Loopidity.
|*|
|*| Loopidity is free software: you can redistribute it and/or modify
|*| it under the terms of the GNU General Public License version 3
|*| as published by the Free Software Foundation.
|*|
|*| Loopidity is distributed in the hope that it will be useful,
|*| but WITHOUT ANY WARRANTY; without even the implied warranty of
|*| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
|*| GNU General Public License for more details.
|*|
|*| You should have received a copy of the GNU General Public License
|*| along with Loopidity.  If not, see <http://www.gnu.org/licenses/>.
\*/


#include "jack_io.h"


/* Scene class side public constants */

const unsigned int Scene::N_FINE_PEAKS   = N_PEAKS_FINE ;
const unsigned int Scene::N_COURSE_PEAKS = N_PEAKS_COURSE ;


/* Scene class side private constants */

const InvalidMetadataException* Scene::INVALID_METADATA_EXCEPTION = new InvalidMetadataException() ;


/* Scene class side private varables */

// peaks cache
float        Scene::FinePeaksPerCoursePeak  = (float)N_FINE_PEAKS / (float)N_COURSE_PEAKS ;
unsigned int Scene::NFinePeaksPerCoursePeak = (unsigned int)FinePeaksPerCoursePeak ;

// sample metedata
unsigned int Scene::SampleRate         = 0 ; // SetMetaData()
unsigned int Scene::FramesPerPeriod    = 0 ; // SetMetaData()
unsigned int Scene::TriggerLatencySize = 0 ; // SetMetaData()
#if SCENE_NFRAMES_EDITABLE
unsigned int Scene::MinLoopSize        = 0 ; // SetMetaData()
#endif // #if SCENE_NFRAMES_EDITABLE
#if SCENE_NFRAMES_EDITABLE && INIT_JACK_BEFORE_SCENES
unsigned int Scene::BytesPerFrame      = 0 ; // SetMetaData()
unsigned int Scene::BeginFrameN        = 0 ; // SetMetaData()
unsigned int Scene::EndFrameN          = 0 ; // SetMetaData()
#endif // #if SCENE_NFRAMES_EDITABLE && INIT_JACK_BEFORE_SCENES
#if !SCENE_NFRAMES_EDITABLE || !INIT_JACK_BEFORE_SCENES
unsigned int Scene::RecordBufferSize   = 0 ; // SetMetaData()
#endif // #if !SCENE_NFRAMES_EDITABLE || !INIT_JACK_BEFORE_SCENES


/* Loop class side private functions */

Loop::Loop(unsigned int nFrames)
{
  // audio data
  buffer1 = new Sample[nFrames] ;
  buffer2 = new Sample[nFrames] ;

  // peaks cache
  peaksFine  [Scene::N_FINE_PEAKS  ] = {0.0} ;
  peaksCourse[Scene::N_COURSE_PEAKS] = {0.0} ;

  // loop state
  vol     = 1.0 ;
  isMuted = false ;
}

Loop::~Loop() { delete buffer1 ; delete buffer2 ; }


/* Loop instantce side public functions */

Sample Loop::getPeakFine(unsigned int peakN) { return peaksFine[peakN] ; }

Sample Loop::getPeakCourse(unsigned int peakN) { return peaksCourse[peakN] ; }
//Scene* Scene::DummyScene = new Scene(DUMMY_SCENEN) ;

/* Scene class side private functions */
#if INIT_JACK_BEFORE_SCENES
Scene::Scene(unsigned int sceneNum)
#else
#  if SCENE_NFRAMES_EDITABLE
Scene::Scene(unsigned int sceneNum , unsigned int endFrameN)
#  else
Scene::Scene(unsigned int sceneNum , unsigned int recordBufferSize)
#  endif // #if SCENE_NFRAMES_EDITABLE
#endif // #if INIT_JACK_BEFORE_SCENES
{
  if (sceneNum >= Loopidity::N_SCENES ||
      !EndFrameN || EndFrameN <= BeginFrameN || !FramesPerPeriod ||
      BeginFrameN % FramesPerPeriod || EndFrameN % FramesPerPeriod)
    throw INVALID_METADATA_EXCEPTION ;

  // identity
  sceneN = sceneNum ;

  // audio data
  list<Loop*> loops ;

  // peaks cache
  hiScenePeaks[N_FINE_PEAKS      ] = {0.0} ; // scanPeaks()
  hiLoopPeaks [Loopidity::N_LOOPS] = {0.0} ; // scanPeaks()
  highestScenePeak                 = 0.0 ;   // scanPeaks()
  nFramesPerPeak                   = 0 ;     // toggleRecordingState()

  // buffer iteration
#if SCENE_NFRAMES_EDITABLE
#  if INIT_JACK_BEFORE_SCENES
  currentFrameN         = BeginFrameN ; // JackIO::ProcessCallback()
  beginFrameN           = BeginFrameN ; // beginRecording()
  endFrameN             = EndFrameN ;   // toggleRecordingState()
  nFrames               = EndFrameN ;   // toggleRecordingState()
#  else
  currentFrameN         = BUFFER_MARGIN_SIZE ; // KLUDGE
  beginFrameN           = BUFFER_MARGIN_SIZE ; // KLUDGE
  endFrameN             = endFrameN ;
  nFrames               = endFrameN ;
#  endif // #if INIT_JACK_BEFORE_SCENES
#else
#  if INIT_JACK_BEFORE_SCENES
  currentFrameN         = 0 ;
  nFrames               = RecordBufferSize ;
#else
  currentFrameN         = 0 ;
  nFrames               = RecordBufferSize = recordBufferSize ;
#  endif // #if INIT_JACK_BEFORE_SCENES
#endif // #if SCENE_NFRAMES_EDITABLE
  nBytes                = 0 ; // toggleRecordingState()
  nSeconds              = 0 ; // toggleRecordingState()

  // recording state
  shouldSaveLoop = true ;  // toggleRecordingState()
  doesPulseExist = false ; // toggleRecordingState()
  isMuted        = false ;
}

// getters/setters
#if INIT_JACK_BEFORE_SCENES
#  if SCENE_NFRAMES_EDITABLE
void Scene::SetMetaData(unsigned int sampleRate , unsigned int nFramesPerPeriod ,
                        unsigned int bytesPerFrame , unsigned int minLoopSize ,
                        unsigned int triggerLatencySize ,
                        unsigned int beginFrameN , unsigned int endFrameN)
{
  SampleRate         = sampleRate ;
  FramesPerPeriod    = nFramesPerPeriod ;
  BytesPerFrame      = bytesPerFrame ;
  MinLoopSize        = minLoopSize ;
  TriggerLatencySize = triggerLatencySize ;
  BeginFrameN        = beginFrameN ;
  EndFrameN          = endFrameN ;
}
#  else
void Scene::SetMetaData(unsigned int sampleRate , unsigned int nFramesPerPeriod ,
                        unsigned int recordBufferSize)
{
  SampleRate             = sampleRate ;       FramesPerPeriod = nFramesPerPeriod ;
  RecordBufferSize       = recordBufferSize ;
}
#  endif // #if SCENE_NFRAMES_EDITABLE
#else
void Scene::SetMetaData(unsigned int sampleRate , unsigned int nFramesPerPeriod)
{
  SampleRate         = sampleRate ;
  FramesPerPeriod    = nFramesPerPeriod ;
#  if SCENE_NFRAMES_EDITABLE
  TriggerLatencySize = (TRIGGER_LATENCY_SIZE / nFramesPerPeriod) * nFramesPerPeriod ;
#  endif // #if SCENE_NFRAMES_EDITABLE
}
#endif // #if INIT_JACK_BEFORE_SCENES


/* Scene instance side private functions */

// scene state

void Scene::beginRecording()
{
DEBUG_TRACE_SCENE_BEGINRECORDING_IN
#if SCENE_NFRAMES_EDITABLE
  if (endFrameN - currentFrameN <= MinLoopSize) return ;

  beginFrameN = currentFrameN - TriggerLatencySize ;
#if INIT_JACK_BEFORE_SCENES
  if (beginFrameN - BeginFrameN < 0) beginFrameN = BeginFrameN ;
#else
  if (beginFrameN - BUFFER_MARGIN_SIZE < 0) beginFrameN = BUFFER_MARGIN_SIZE ;
#  endif // #if INIT_JACK_BEFORE_SCENES
  shouldSaveLoop = true ;
#else
  currentFrameN = 0 ; Loopidity::UpdateView(sceneN) ;
#endif // #if SCENE_NFRAMES_EDITABLE
}

void Scene::toggleRecordingState()
{
DEBUG_TRACE_SCENE_TOGGLERECORDINGSTATE_IN

  if (!doesPulseExist)
  {
#if SCENE_NFRAMES_EDITABLE
    // disallow segmented base loop (issue #11) and very short scenes (issue #12)
    unsigned int endFrameNum = currentFrameN + FramesPerPeriod ;
    if (endFrameNum <= beginFrameN + MinLoopSize) return ;

    endFrameN      =  endFrameNum ;
    nFrames        = (endFrameN - TriggerLatencySize) - beginFrameN ;
    nBytes         =  nFrames * BytesPerFrame ;
    nSeconds       =  nFrames / SampleRate ;
    nFramesPerPeak =  nFrames / N_FINE_PEAKS ;
    doesPulseExist =  true ;

#  if INIT_JACK_BEFORE_SCENES && ALLOW_BUFFER_ROLLOVER
    if (endFrameN == EndFrameN) endFrameN += FramesPerPeriod ; // avert auto-rollover guard
#  endif // #if INIT_JACK_BEFORE_SCENES && ALLOW_BUFFER_ROLLOVER
#else
    nFrames  = currentFrameN + FramesPerPeriod  ; nFramesPerPeak = nFrames / N_FINE_PEAKS ;
    nSeconds = nFrames       / SampleRate ;       shouldSaveLoop = doesPulseExist = true ;
#endif // #if SCENE_NFRAMES_EDITABLE

    Loopidity::UpdateView(sceneN) ;
  }
  else shouldSaveLoop = !shouldSaveLoop ;

DEBUG_TRACE_SCENE_TOGGLERECORDINGSTATE_OUT
}


// audio data

bool Scene::addLoop(Loop* newLoop)
{
DEBUG_TRACE_SCENE_ADDLOOP_IN

  unsigned int nLoops = loops.size() ; if (nLoops >= Loopidity::N_LOOPS) return false ;

  scanPeaks(newLoop , nLoops) ; loops.push_back(newLoop) ; return true ;

DEBUG_TRACE_SCENE_ADDLOOP_OUT
}

void Scene::deleteLoop(unsigned int loopN)
{
DEBUG_TRACE_SCENE_DELETELOOP_IN

  if (loopN >= loops.size()) return ;

  list<Loop*>::iterator loopIter = loops.begin() ; while (loopN--) ++loopIter ;
  loops.erase(loopIter) ; rescanPeaks() ;

DEBUG_TRACE_SCENE_DELETELOOP_IN
}

void Scene::reset()
{
DEBUG_TRACE_SCENE_RESET_IN

#if SCENE_NFRAMES_EDITABLE
#  if INIT_JACK_BEFORE_SCENES
  currentFrameN  = beginFrameN    = BeginFrameN ;
  nFrames        = endFrameN      = EndFrameN ;
  nBytes         = nSeconds       = 0 ;
#  else
  currentFrameN  = beginFrameN = BUFFER_MARGIN_SIZE /* KLUDGE */ ; nFrames        = EndFrameN ;
#  endif // #if INIT_JACK_BEFORE_SCENES
#else
  currentFrameN  = 0 ;     nFrames        = RecordBufferSize ;
#endif // #if SCENE_NFRAMES_EDITABLE

  shouldSaveLoop = doesPulseExist = false ; loops.clear() ;

DEBUG_TRACE_SCENE_RESET_OUT
}


// peaks cache

void Scene::scanPeaks(Loop* loop , unsigned int loopN)
{
DEBUG_TRACE_SCENE_SCANPEAKS_IN

#if SCAN_LOOP_PEAKS_DATA
  if (!loop || loopN >= Loopidity::N_LOOPS) return ;

  // fill fine peaks arrays
  Sample* peaks = loop->peaksFine ; unsigned int peakN , frameN ; Sample peak1 , peak2 ;
  for (peakN = 0 ; peakN < N_FINE_PEAKS ; ++peakN)
  {
#if SCENE_NFRAMES_EDITABLE
#  if INIT_JACK_BEFORE_SCENES
    frameN       = BeginFrameN + (nFramesPerPeak * peakN) ;
#  else
    frameN       = BUFFER_MARGIN_SIZE + (nFramesPerPeak * peakN) ;
#  endif // #if INIT_JACK_BEFORE_SCENES
#else
    frameN       = nFramesPerPeak * peakN ;
#endif // #if SCENE_NFRAMES_EDITABLE
    peak1        = JackIO::GetPeak(&(loop->buffer1[frameN]) , nFramesPerPeak) ;
    peak2        = JackIO::GetPeak(&(loop->buffer2[frameN]) , nFramesPerPeak) ;
#  if FIXED_N_AUDIO_PORTS
    peaks[peakN] = (peak1 + peak2) / N_INPUT_CHANNELS ;
#  else
    peaks[peakN] = (peak1 + peak2) / N_INPUT_CHANNELS ;
#  endif // #if FIXED_N_AUDIO_PORTS

    // find the loudest peak for this loop
    if (hiLoopPeaks[loopN] < peaks[peakN]) hiLoopPeaks[loopN] = peaks[peakN] ;

    // find the loudest of each peak from all loops of the current scene
    if (hiScenePeaks[peakN] < peaks[peakN]) hiScenePeaks[peakN] = peaks[peakN] ;
  }
  // find the loudest of all peaks of all loops of the current scene
  if (highestScenePeak < hiLoopPeaks[loopN]) highestScenePeak = hiLoopPeaks[loopN] ;

  // fill course peaks array
  Sample* peaksCourse                  = loop->peaksCourse ;
  for (unsigned int histPeakN = 0 ; histPeakN < N_COURSE_PEAKS ; ++histPeakN)
  {
    peakN                  = (unsigned int)(FinePeaksPerCoursePeak * (float)histPeakN) ;
    peaksCourse[histPeakN] = JackIO::GetPeak(&peaks[peakN] , NFinePeaksPerCoursePeak) ;
  }
#endif // #if SCAN_LOOP_PEAKS_DATA

DEBUG_TRACE_SCENE_SCANPEAKS_OUT
}

void Scene::rescanPeaks()
{
DEBUG_TRACE_SCENE_RESCANPEAKS_IN

  highestScenePeak   = 0.0 ;
  unsigned int peakN = N_FINE_PEAKS ; while (peakN--) hiScenePeaks[peakN] = 0.0 ;
//  unsigned int loopN = Loopidity::N_LOOPS ; while (loopN--)
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

//unsigned int Scene::getLoopPos() { return (currentFrameN * 1000) / nFrames ; }


/* Scene instance side public functions */

unsigned int Scene::getSceneN() { return sceneN ; }

unsigned int Scene::getDoesPulseExist() { return doesPulseExist ; }

unsigned int Scene::getNLoops() { return loops.size() ; }

unsigned int Scene::getCurrentPeakN()
#if SCENE_NFRAMES_EDITABLE
#  if INIT_JACK_BEFORE_SCENES
  { return ((float)(currentFrameN - beginFrameN) / (float)nFrames) * N_FINE_PEAKS ; }
#  else
  { return ((float)(currentFrameN - BUFFER_MARGIN_SIZE) / nFrames) * N_FINE_PEAKS ; } // KLUDGE
#  endif // #if INIT_JACK_BEFORE_SCENES
#else
  { return ((float)currentFrameN / nFrames) * N_FINE_PEAKS ; }
#endif // #if SCENE_NFRAMES_EDITABLE

float Scene::getCurrentSeconds() { return (currentFrameN - beginFrameN) / SampleRate ; }

float Scene::getTotalSeconds()
{
  return (doesPulseExist || !Loopidity::GetIsRolling())? nSeconds : getCurrentSeconds() ;
}
