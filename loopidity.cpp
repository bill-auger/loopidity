
#include "loopidity.h"


// DEBUG
void Loopidity::SetDbgText()
{ char dbg[255] ; sprintf(dbg , "SceneN=%d nLoops=%d PeakN=%d" , CurrentSceneN , Scenes[CurrentSceneN]->loops.size() , Scenes[CurrentSceneN]->getCurrentPeakN()) ; LoopiditySdl::SetStatusL(dbg) ;
	sprintf(dbg , "doesPulseExist:%d shouldSaveLoop:%d" , Scenes[CurrentSceneN]->doesPulseExist , Scenes[CurrentSceneN]->shouldSaveLoop) ; LoopiditySdl::SetStatusR(dbg) ; }
// DEBUG end


/* Scene Class private varables */

unsigned int Scene::RecordBufferSize = 0 ;
unsigned int Scene::SampleRate = 0 ;
unsigned int Scene::FrameSize = 0 ;
unsigned int Scene::NFramesPerPeriod = 0 ;
bool Scene::IsRecording = false ;


/* Loopidity Class private varables */

// recording state
unsigned int Loopidity::CurrentSceneN = 0 ;
unsigned int Loopidity::NextSceneN = 0 ;
bool Loopidity::ShouldSceneAutoChange = true ;

// audio data
Scene* Loopidity::Scenes[N_SCENES] = {0} ;
unsigned int Loopidity::NFramesPerPeriod = 0 ;
unsigned int Loopidity::NFramesPerGuiInterval = 0 ;
SAMPLE* Loopidity::RecordBuffer1 = 0 ;
SAMPLE* Loopidity::RecordBuffer2 = 0 ;
vector<SAMPLE> Loopidity::PeaksIn ;
vector<SAMPLE> Loopidity::PeaksOut ;
SAMPLE Loopidity::TransientPeaks[N_PORTS] = {0} ;
SAMPLE Loopidity::TransientPeakInMix = 0 ;
SAMPLE Loopidity::TransientPeakOutMix = 0 ;


/* Loop Class public functions */

SAMPLE Loop::getPeakFine(unsigned int peakN) { return peaksFine[peakN] ; }

SAMPLE Loop::getPeakCourse(unsigned int peakN) { return peaksCourse[peakN] ; }


/* Loop Class private functions */

Loop::Loop(unsigned int nFrames) : peaksFine() , peaksCourse()
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
		shouldSaveLoop(false) , doesPulseExist(false)
{
	// buffer iteration
	nFrames = RecordBufferSize = recordBufferSize ; nFramesPerPeak = nFrames / N_PEAKS_FINE ;
}


// audio data

bool Scene::addLoop(Loop* newLoop)
{
	unsigned int nLoops = loops.size() ; if (nLoops >= N_LOOPS) return false ;

	scanPeaks(newLoop , nLoops) ; loops.push_back(newLoop) ;
	return true ;
}

void Scene::deleteLoop()
{
	if (!loops.empty()) { loops.pop_back() ; rescanPeaks() ; }
	if (loops.empty()) reset() ;
}

void Scene::reset()
{
	unsigned int loopN = loops.size() ; while (loopN--) deleteLoop() ;
	nFrames = RecordBufferSize ; IsRecording = doesPulseExist = shouldSaveLoop = false ;
}


// peaks cache

void Scene::scanPeaks(Loop* loop , unsigned int loopN)
{
#if SCAN_LOOP_PEAKS_DATA
	if (!loop || loopN >= N_LOOPS) return ;

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
}

void Scene::rescanPeaks()
{
	highestScenePeak = 0.0 ;
	unsigned int peakN = N_PEAKS_FINE ; while (peakN--) hiScenePeaks[peakN] = 0.0 ;
	unsigned int loopN = N_LOOPS ; while (loopN--)
		{ hiLoopPeaks[loopN] = 0.0 ; if (loopN < loops.size()) scanPeaks(loops[loopN] , loopN) ; }
}


// scene state

void Scene::setStatus()
{
	if (!IsRecording) { frameN = 0 ; IsRecording = true ; }
	else if (!doesPulseExist)
	{
		nFrames = frameN + NFramesPerPeriod ; nFramesPerPeak = nFrames / N_PEAKS_FINE ;
		nBytes = FrameSize * nFrames ; shouldSaveLoop = doesPulseExist = true ;
	}
	else shouldSaveLoop = !shouldSaveLoop ;
}


// getters/setters

void Scene::SetMetaData(unsigned int sampleRate , unsigned int frameSize , unsigned int nFramesPerPeriod)
	{ SampleRate = sampleRate ; FrameSize = frameSize ; NFramesPerPeriod = nFramesPerPeriod ; }

bool Scene::getIsRecording() { return IsRecording ; }

unsigned int Scene::getLoopPos() { return (frameN * 1000) / nFrames ; }


/* Loopidity Class public functions */

// setup

bool Loopidity::SetRecordBufferSize(unsigned int recordBufferSize)
{
	return (JackIO::SetRecordBufferSize(recordBufferSize)) ;
// TODO: caller can LoopiditySdl::Alert(ZERO_BUFFER_SIZE_MSG) ;
}

bool Loopidity::Init(bool isMonitorInputs)
{
	if (Scenes[0]) return false ; // we only want to do this once

	// instantiate Scenes (models) and SdlScenes (views)
	unsigned int sceneN = N_SCENES ; while (sceneN--)
	{
		Scene* scene = new Scene(sceneN , JackIO::GetRecordBufferSize()) ;
		Scenes[sceneN] = scene ;
		LoopiditySdl::SdlScenes[sceneN] = new SceneSdl(scene) ;
	}

	// initialize JACK
	int jackInit = JackIO::Init(Scenes[0] , 0 , isMonitorInputs) ;
	if (jackInit == JACK_MEM_FAIL) { LoopiditySdl::Alert(INSUFFICIENT_MEMORY_MSG) ; return false ; }
	else if(jackInit == JACK_FAIL) { LoopiditySdl::Alert(JACK_FAIL_MSG) ; return false ; }

	// initialize scope/VU peaks cache
	for (unsigned int peakN = 0 ; peakN < N_PEAKS_TRANSIENT ; ++peakN)
		{ PeaksIn.push_back(0.0) ; PeaksOut.push_back(0.0) ; }

	// get handles on JACK buffers for scope/VU peaks
	RecordBuffer1 = JackIO::GetRecordBuffer1() ; RecordBuffer2 = JackIO::GetRecordBuffer2() ;

	return true ;
}


// user actions

void Loopidity::ToggleAutoSceneChange() { ShouldSceneAutoChange = !ShouldSceneAutoChange ; }

void Loopidity::ToggleScene()
{
	NextSceneN = (NextSceneN + 1) % N_SCENES ; JackIO::SetNextScene(Scenes[NextSceneN] , NextSceneN) ;
	if (!Scene::IsRecording) JackIO::SetCurrentScene(Scenes[CurrentSceneN = NextSceneN] , NextSceneN) ;
	unsigned int sceneN = N_SCENES ; while (sceneN--) LoopiditySdl::SdlScenes[sceneN]->setStatus() ;
}

void Loopidity::SetStatus() { Scenes[CurrentSceneN]->setStatus() ; LoopiditySdl::SdlScenes[CurrentSceneN]->setStatus() ; }

void Loopidity::DeleteLoop()
	{ LoopiditySdl::SdlScenes[CurrentSceneN]->deleteLoop() ; Scenes[CurrentSceneN]->deleteLoop() ; }

void Loopidity::ResetCurrentScene() { Scenes[CurrentSceneN]->reset() ; }

void Loopidity::Reset()
{
	CurrentSceneN = NextSceneN = 0 ; JackIO::Reset(Scenes[0] , 0) ;
	unsigned int sceneN = N_SCENES ; while (sceneN--) Scenes[sceneN]->reset() ;
}


// getters/setters

void Loopidity::SetMetaData(unsigned int sampleRate , unsigned int frameSize , unsigned int nFramesPerPeriod)
{
	float interval = (float)GUI_UPDATE_INTERVAL_SHORT * 0.001 ;
	NFramesPerGuiInterval = (unsigned int)(sampleRate * interval) ;

	Scene::SetMetaData(sampleRate , frameSize , nFramesPerPeriod) ;
}

unsigned int Loopidity::GetCurrentSceneN() { return CurrentSceneN ; }

unsigned int Loopidity::GetNextSceneN() { return NextSceneN ; }

unsigned int Loopidity::GetLoopPos() { return Scenes[CurrentSceneN]->getLoopPos() ; }

bool Loopidity::GetShouldSaveLoop() { return Scenes[CurrentSceneN]->shouldSaveLoop ; }

bool Loopidity::GetDoesPulseExist() { return Scenes[CurrentSceneN]->doesPulseExist ; }

vector<SAMPLE>* Loopidity::GetPeaksInCache() { return &PeaksIn ; }

vector<SAMPLE>* Loopidity::GetPeaksOutCache() { return &PeaksOut ; }

SAMPLE* Loopidity::GetTransientPeaksCache() { return TransientPeaks ; }

SAMPLE* Loopidity::GetTransientPeakIn() { return &TransientPeakInMix ; }

SAMPLE* Loopidity::GetTransientPeakOut() { return &TransientPeakOutMix ; }

SAMPLE Loopidity::GetPeak(SAMPLE* buffer , unsigned int nFrames)
{
	SAMPLE peak = 0.0 ;
	try
	{
		for (unsigned int frameN = 0 ; frameN < nFrames ; ++frameN)
			{ SAMPLE sample = fabs(buffer[frameN]) ; if (peak < sample) peak = sample ; }
	}
	catch(int ex) { printf("Loopidity::GetPeak(): subscript out of range\n") ; }
	return peak ;
}


// helpers

void Loopidity::AddLoop(unsigned int sceneN , Loop* newLoop)
{
	if (Scenes[sceneN]->addLoop(newLoop))
		LoopiditySdl::SdlScenes[sceneN]->addLoop(newLoop , Scenes[sceneN]->loops.size() - 1) ;
}

void Loopidity::SceneChanged(unsigned int sceneN)
{
	LoopiditySdl::SdlScenes[CurrentSceneN]->drawSceneInactive() ;
	CurrentSceneN = NextSceneN = sceneN ; LoopiditySdl::SdlScenes[NextSceneN]->setStatus() ;

	if (ShouldSceneAutoChange) do ToggleScene() ; while (!Scenes[NextSceneN]->loops.size()) ;
}

void Loopidity::ScanTransientPeaks()
{
#if SCAN_TRANSIENT_PEAKS_DATA
	Scene* currentScene = Scenes[CurrentSceneN] ; unsigned int frameN = currentScene->frameN ;
// TODO; first NFramesPerGuiInterval duplicated
	unsigned int currentFrameN = (frameN < NFramesPerGuiInterval)? 0 : frameN - NFramesPerGuiInterval ;
	SAMPLE peakIn1 = GetPeak(&(RecordBuffer1[currentFrameN]) , NFramesPerGuiInterval) ;
	SAMPLE peakIn2 = GetPeak(&(RecordBuffer2[currentFrameN]) , NFramesPerGuiInterval) ;
	SAMPLE peakOut1 = 0.0 , peakOut2 = 0.0 ; unsigned int nLoops = currentScene->loops.size() ;
	for (unsigned int loopN = 0 ; loopN < nLoops ; ++loopN)
	{
		Loop* loop = currentScene->loops[loopN] ;
		peakOut1 += GetPeak(&(loop->buffer1[currentFrameN]) , NFramesPerGuiInterval) ;
		peakOut2 += GetPeak(&(loop->buffer2[currentFrameN]) , NFramesPerGuiInterval) ;
	}
	// load scope peaks (mono mix)
	SAMPLE peakIn = (peakIn1 + peakIn2) / N_INPUT_CHANNELS ;
	SAMPLE peakOut = (peakOut1 + peakOut2) / N_OUTPUT_CHANNELS ;
	if (peakOut > 1.0) peakOut = 1.0 ;
	PeaksIn.pop_back() ; PeaksIn.insert(PeaksIn.begin() , peakIn) ;
	PeaksOut.pop_back() ; PeaksOut.insert(PeaksOut.begin() , peakOut) ;
	// load VU peaks (per channel)
	TransientPeakInMix = peakIn ; TransientPeakOutMix = peakOut ;
	TransientPeaks[0] = peakIn1 ; TransientPeaks[1] = peakIn2 ;
	TransientPeaks[2] = peakOut1 ; TransientPeaks[3] = peakOut2 ;
#endif // #if SCAN_TRANSIENT_PEAKS_DATA
}

void Loopidity::OOM() { LoopiditySdl::SetStatusL(OUT_OF_MEMORY_MSG) ; }
