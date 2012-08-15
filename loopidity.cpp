
#include "loopidity.h"


// DEBUG
void Scene::makeMainDbgText(char* dbg) { sprintf(dbg , "Paint() SceneN=%d nLoops=%d PeakN=%d" , sceneN , loops.size() , getCurrentPeakN()) ; }
// DEBUG end


/* Scene Class private varables */

unsigned int Scene::RecordBufferSize = 0 ;
unsigned int Scene::SampleRate = 0 ;
unsigned int Scene::FrameSize = 0 ;
unsigned int Scene::NFramesPerPeriod = 0 ;
bool Scene::IsRecording = false ;


/* Loopidity Class private varables */

// recording state
Scene* Loopidity::CurrentScene = 0 ;
unsigned int Loopidity::NextSceneN = 0 ;

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

Scene::Scene(unsigned int scenen , unsigned int recordBufferSize) :
		// identity
		sceneN(scenen) ,
		// GUI
		sceneGui(0) ,
		// peaks cache
		hiScenePeaks() , hiLoopPeaks(), highestScenePeak(0.0) ,
		// buffer iteration
		frameN(0) , nBytes(0) ,
		// recording state
		isSaveLoop(false) , isPulseExist(false)
{
	// buffer iteration
	nFrames = RecordBufferSize = recordBufferSize ; nFramesPerPeak = nFrames / N_PEAKS_FINE ;
}


// audio data

void Scene::addLoop(Loop* newLoop)
{
	unsigned int nLoops = loops.size() ; if (nLoops >= N_LOOPS) return ;

	scanPeaks(newLoop , nLoops) ;
	sceneGui->drawHistogram(newLoop) ; sceneGui->drawLoop(newLoop , nLoops) ;
	loops.push_back(newLoop) ;
}

void Scene::deleteLoop()
{
	if (loops.size()) { loops.pop_back() ; sceneGui->loopImgs.pop_back() ; rescanPeaks() ; }
	if (!loops.size()) reset() ;
}

void Scene::reset()
{
	unsigned int loopN = loops.size() ; while (loopN--) deleteLoop() ;
	nFrames = RecordBufferSize ; IsRecording = isPulseExist = isSaveLoop = false ;
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

void Scene::setStateIndicators()
{
	if (IsRecording)
	{
		sceneGui->loopFrameColor = (isSaveLoop)? STATUS_RECORDING_COLOR : STATUS_PENDING_COLOR ;
		sceneGui->sceneFrameColor = (sceneN != Loopidity::GetNextSceneN())? STATUS_IDLE_COLOR :
			(sceneN == Loopidity::GetCurrentSceneN())? STATUS_PLAYING_COLOR : STATUS_PENDING_COLOR ;
	}
	else sceneGui->loopFrameColor = sceneGui->sceneFrameColor = STATUS_IDLE_COLOR ;

#if DRAW_MODE
	LoopiditySdl::DrawMode() ;
#endif
}

void Scene::setMode()
{
	if (!IsRecording) { frameN = 0 ; IsRecording = true ; }
	else if (!isPulseExist)
	{
		nFrames = frameN + NFramesPerPeriod ; nFramesPerPeak = nFrames / N_PEAKS_FINE ;
		nBytes = FrameSize * nFrames ; isSaveLoop = isPulseExist = true ;
	}
	else isSaveLoop = !isSaveLoop ;

	setStateIndicators() ;
}

void Scene::sceneChanged()
	{ setStateIndicators() ; sceneGui->drawScene(sceneGui->inactiveSceneSurface , 0 , 0) ; }


// getters/setters

void Scene::SetMetaData(unsigned int sampleRate , unsigned int frameSize , unsigned int nFramesPerPeriod)
	{ SampleRate = sampleRate ; FrameSize = frameSize ; NFramesPerPeriod = nFramesPerPeriod ; }

void Scene::setSceneGui(SceneSdl* aSceneGui)
	{ sceneGui = aSceneGui ; sceneGui->scene = this ; sceneGui->drawScene(sceneGui->inactiveSceneSurface , 0 , 0) ; }

bool Scene::getIsRecording() { return IsRecording ; }

unsigned int Scene::getLoopPos() { return (frameN * 1000) / nFrames ; }


/* Loopidity Class public functions */

// user actions

bool Loopidity::SetRecordBufferSize(unsigned int recordBufferSize)
{
	return (JackIO::SetRecordBufferSize(recordBufferSize)) ;
// TODO: caller can LoopiditySdl::Alert(ZERO_BUFFER_SIZE_MSG) ;
}

bool Loopidity::Init(bool isMonitorInputs)
{
	unsigned int sceneN = N_SCENES ; while (sceneN--)
		Scenes[sceneN] = new Scene(sceneN , JackIO::GetRecordBufferSize()) ;
	int jackInit = JackIO::Init((CurrentScene = Scenes[0]) , isMonitorInputs) ;
	if (jackInit == JACK_MEM_FAIL) { LoopiditySdl::Alert(INSUFFICIENT_MEMORY_MSG) ; return false ; }
	else if(jackInit == JACK_FAIL) { LoopiditySdl::Alert(JACK_FAIL_MSG) ; return false ; }

	// initialize scope/VU peaks cache
	for (unsigned int peakN = 0 ; peakN < N_PEAKS_TRANSIENT ; ++peakN)
		{ PeaksIn.push_back(0.0) ; PeaksOut.push_back(0.0) ; }

	// get handles on JACK buffers for scope/VU peaks
	RecordBuffer1 = JackIO::GetRecordBuffer1() ; RecordBuffer2 = JackIO::GetRecordBuffer2() ;

	return true ;
}

void Loopidity::ToggleScene()
{
	if (!Scene::IsRecording) return ;

	NextSceneN = (NextSceneN + 1) % N_SCENES ;
	JackIO::SetNextScene(Scenes[NextSceneN]) ;
	unsigned int sceneN = N_SCENES ; while (sceneN--) Scenes[sceneN]->setStateIndicators() ;
}

void Loopidity::SetMode() { CurrentScene->setMode() ; }

void Loopidity::DeleteLoop() { CurrentScene->deleteLoop() ; }

void Loopidity::ResetCurrentScene() { CurrentScene->reset() ; }

void Loopidity::Reset()
{
	CurrentScene = Scenes[NextSceneN = 0] ; JackIO::Reset(Scenes[0]) ;
	unsigned int sceneN = N_SCENES ; while (sceneN--) Scenes[sceneN]->reset() ;
}


// getters/setters

void Loopidity::SetMetaData(unsigned int sampleRate , unsigned int frameSize , unsigned int nFramesPerPeriod)
{
	float interval = (float)GUI_UPDATE_INTERVAL_SHORT * 0.001 ;
	NFramesPerGuiInterval = (unsigned int)(sampleRate * interval) ;

	Scene::SetMetaData(sampleRate , frameSize , nFramesPerPeriod) ;
}

void Loopidity::SetSceneGui(SceneSdl* sceneGui , unsigned int sceneN) { Scenes[sceneN]->setSceneGui(sceneGui) ; }

Scene* Loopidity::GetCurrentScene() { return CurrentScene ; }

unsigned int Loopidity::GetCurrentSceneN()
{
	unsigned int sceneN = N_SCENES ; Scene* currentScene = CurrentScene ;
	while (sceneN-- && Scenes[sceneN] != currentScene) ; return sceneN ;
}

unsigned int Loopidity::GetNextSceneN() { return NextSceneN ; }

unsigned int Loopidity::GetLoopPos() { return CurrentScene->getLoopPos() ; }

bool Loopidity::GetIsSaveLoop() { return CurrentScene->isSaveLoop ; }

bool Loopidity::GetIsPulseExist() { return CurrentScene->isPulseExist ; }

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

void Loopidity::SceneChanged(Scene* nextScene) { CurrentScene = nextScene ; CurrentScene->sceneChanged() ; }

void Loopidity::ScanTransientPeaks()
{
#if SCAN_TRANSIENT_PEAKS_DATA
	unsigned int frameN = CurrentScene->frameN ;
// TODO; first NFramesPerGuiInterval duplicated
	unsigned int currentFrameN = (frameN < NFramesPerGuiInterval)? 0 : frameN - NFramesPerGuiInterval ;
	SAMPLE peakIn1 = GetPeak(&(RecordBuffer1[currentFrameN]) , NFramesPerGuiInterval) ;
	SAMPLE peakIn2 = GetPeak(&(RecordBuffer2[currentFrameN]) , NFramesPerGuiInterval) ;
	SAMPLE peakOut1 = 0.0 , peakOut2 = 0.0 ; unsigned int nLoops = CurrentScene->loops.size() ;
	for (unsigned int loopN = 0 ; loopN < nLoops ; ++loopN)
	{
		Loop* loop = CurrentScene->loops[loopN] ;
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
