
#include "jack_io.h"


/* Scene Class private varables */

unsigned int Scene::RecordBufferSize = 0 ;
unsigned int Scene::SampleRate = 0 ;
unsigned int Scene::FrameSize = 0 ;
unsigned int Scene::NFramesPerPeriod = 0 ;


/* Loop Class public functions */

SAMPLE Loop::getPeakFine(unsigned int peakN) { return peaksFine[peakN] ; }

SAMPLE Loop::getPeakCourse(unsigned int peakN) { return peaksCourse[peakN] ; }


/* Loop Class private functions */

Loop::Loop(unsigned int nFrames) : peaksFine() , peaksCourse() , vol(1.0) , isMuted(false)
	{ buffer1 = new SAMPLE[nFrames] ; buffer2 = new SAMPLE[nFrames] ; }

Loop::~Loop() { delete buffer1 ; delete buffer2 ; }


/* Scene Class public functions */

unsigned int Scene::getCurrentPeakN() { return ((float)frameN / (float)nFrames) * (float)N_PEAKS_FINE ; }

float Scene::getCurrentSeconds() { return frameN / SampleRate ; }

float Scene::getTotalSeconds() { return nFrames / SampleRate ; }


/* Scene Class private functions */

Scene::Scene(unsigned int sceneNum , unsigned int recordBufferSize) :
		// identity
		sceneN(sceneNum) ,
		// peaks cache
		hiScenePeaks() , hiLoopPeaks(), highestScenePeak(0.0) ,
		// buffer iteration
		frameN(0) , nBytes(0) ,
		// recording state
		isRolling(false) , shouldSaveLoop(false) , doesPulseExist(false) , isMuted(false)
{
	// buffer iteration
	nFrames = RecordBufferSize = recordBufferSize ; nFramesPerPeak = nFrames / N_PEAKS_FINE ;
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
	if (loopN >= loops.size()) return ;

	list<Loop*>::iterator loop = loops.begin() ; while (loopN--) ++loop ;
	loops.erase(loop) ; rescanPeaks() ;

DEBUG_TRACE_SCENE_DELETELOOP_IN
}

void Scene::startRolling()
{
DEBUG_TRACE_SCENE_RESET_IN

	isRolling = true ; Loopidity::UpdateView(sceneN) ;

DEBUG_TRACE_SCENE_RESET_OUT
}

void Scene::reset()
{
DEBUG_TRACE_SCENE_RESET_IN

	frameN = 0 ; nFrames = RecordBufferSize ;
	isRolling = shouldSaveLoop = doesPulseExist = false ;
	loops.clear() ; Loopidity::UpdateView(sceneN) ;

DEBUG_TRACE_SCENE_RESET_OUT
}


// peaks cache

void Scene::scanPeaks(Loop* loop , unsigned int loopN)
{
DEBUG_TRACE_SCENE_SCANPEAKS_IN

#if SCAN_LOOP_PEAKS_DATA
	if (!loop || loopN >= N_LOOPS)
{printf("Scene::scanPeaks wtf\n");
		return ;}

	// fill fine peaks arrays
	SAMPLE* peaks = loop->peaksFine ; unsigned int peakN , framen ; SAMPLE peak1 , peak2 ;
	for (peakN = 0 ; peakN < N_PEAKS_FINE ; ++peakN)
	{
		framen = nFramesPerPeak * peakN ;
		peak1 = Loopidity::GetPeak(&(loop->buffer1[framen]) , nFramesPerPeak) ;
		peak2 = Loopidity::GetPeak(&(loop->buffer2[framen]) , nFramesPerPeak) ;
		peaks[peakN] = (peak1 + peak2) / N_INPUT_CHANNELS ;

		// find the loudest peak for this loop
		if (hiLoopPeaks[loopN] < peaks[peakN]) hiLoopPeaks[loopN] = peaks[peakN] ;

		// find the loudest of each peak from all loops of the current scene
		if (hiScenePeaks[peakN] < peaks[peakN]) hiScenePeaks[peakN] = peaks[peakN] ;
	}
	// find the loudest of all peaks of all loops of the current scene
	if (highestScenePeak < hiLoopPeaks[loopN]) highestScenePeak = hiLoopPeaks[loopN] ;

	// fill course peaks array
	SAMPLE* peaksCourse = loop->peaksCourse ;
	float nPeaksAsFloat = (float)N_PEAKS_FINE / (float)N_PEAKS_COURSE ;
	unsigned int nPeaksAsInt = (unsigned int)nPeaksAsFloat ;
	for (unsigned int histPeakN = 0 ; histPeakN < N_PEAKS_COURSE ; ++histPeakN)
	{
		peakN = (unsigned int)(nPeaksAsFloat * (float)histPeakN) ;
		peaksCourse[histPeakN] = Loopidity::GetPeak(&peaks[peakN] , nPeaksAsInt) ;
	}
#endif // #if SCAN_LOOP_PEAKS_DATA

DEBUG_TRACE_SCENE_SCANPEAKS_OUT
}

void Scene::rescanPeaks()
{
DEBUG_TRACE_SCENE_RESCANPEAKS_IN

	highestScenePeak = 0.0 ;
	unsigned int peakN = N_PEAKS_FINE ; while (peakN--) hiScenePeaks[peakN] = 0.0 ;
//	unsigned int loopN = N_LOOPS ; while (loopN--)

	list<Loop*>::iterator aLoopBegin = loops.begin() , aLoop ; unsigned int loopN = 0 ;
	for (aLoop = aLoopBegin ; aLoop != loops.end() ; ++aLoop)
		{ hiLoopPeaks[loopN] = 0.0 ; scanPeaks(*aLoop , loopN) ; ++loopN ; }

DEBUG_TRACE_SCENE_RESCANPEAKS_OUT
}


// scene state

void Scene::toggleState()
{
DEBUG_TRACE_SCENE_TOGGLESTATE_IN

	if (!doesPulseExist)
	{
		nFrames = frameN + NFramesPerPeriod ; nFramesPerPeak = nFrames / N_PEAKS_FINE ;
		nBytes = FrameSize * nFrames ; shouldSaveLoop = doesPulseExist = true ;
	}
	else shouldSaveLoop = !shouldSaveLoop ;

DEBUG_TRACE_SCENE_TOGGLESTATE_OUT
}


// getters/setters

void Scene::SetMetaData(unsigned int sampleRate , unsigned int frameSize , unsigned int nFramesPerPeriod)
	{ SampleRate = sampleRate ; FrameSize = frameSize ; NFramesPerPeriod = nFramesPerPeriod ; }

Loop* Scene::getLoop(unsigned int loopN)
{
	if (loopN >= loops.size()) return NULL ;

	list<Loop*>::iterator aLoop = loops.begin() ; while (loopN--) ++aLoop ;
	return (*aLoop) ;
}

bool Scene::getIsRolling() { return isRolling ; }

unsigned int Scene::getLoopPos() { return (frameN * 1000) / nFrames ; }

