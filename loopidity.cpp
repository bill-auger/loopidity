
#include "loopidity.h"


// DEBUG
#include "debug.h"

void Scene::makeMainDbgText(char* dbg) { sprintf(dbg , "Paint() SceneN=%d nLoops=%d PeakN=%d" , sceneN , loops.size() , getCurrentPeakN()) ; }

void Loopidity::SetDbgLabels()
{
#if VARDUMP
app->dbgLabel100.SetText("Loopidity::") ;
app->dbgLabel11.SetText("CurrentSceneN") ;
app->dbgLabel12.SetText("NextSceneN") ;
app->dbgLabel13.SetText("NScenes") ;
app->dbgLabel14.SetText("IsRecording") ;
app->dbgLabel22.SetText("Progress %%") ;

app->dbgLabel102.SetText("Current Scene::") ;
app->dbgLabel0.SetText("LoopN") ;
app->dbgLabel1.SetText("FrameN") ;
app->dbgLabel2.SetText("NFrames") ;
app->dbgLabel3.SetText("IsAutoRecord") ;
app->dbgLabel4.SetText("IsPulseExist") ;
app->dbgLabel5.SetText("") ;
app->dbgLabel6.SetText("") ;

app->dbgLabel103.SetText("Next Scene::") ;
app->dbgLabel15.SetText("LoopN") ;
app->dbgLabel16.SetText("FrameN") ;
app->dbgLabel17.SetText("NFrames") ;
app->dbgLabel18.SetText("IsAutoRecord") ;
app->dbgLabel19.SetText("IsPulseExist") ;
app->dbgLabel20.SetText("") ;
app->dbgLabel21.SetText("") ;

app->dbgLabel101.SetText("JackIO::") ;
app->dbgLabel8.SetText("NFramesPerPeriod") ;
app->dbgLabel9.SetText("FrameSize") ;
app->dbgLabel10.SetText("PeriodSize") ;
#endif
}

void Loopidity::Vardump() {
#if VARDUMP
	char dbg[256] ;
	unsigned int sampleRate = JackIO::GetSampleRate() ;
	sprintf(dbg , "%u" , GetCurrentSceneN()) ; app->dbgText11 = dbg ; app->dbgText102.SetText(dbg) ;
	sprintf(dbg , "%u" , NextSceneN) ; app->dbgText12 = dbg ; app->dbgText103.SetText(dbg) ;
	sprintf(dbg , "%u" , Scenes.GetCount()) ; app->dbgText13 = dbg ;
	sprintf(dbg , "%d" , IsRecording) ; app->dbgText14 = dbg ;
	sprintf(dbg , "%u %%" , GetLoopPos() / 10) ; app->dbgText22 = dbg ;

	sprintf(dbg , "%u" , JackIO::GetCurrentScene()->loops.size()) ; app->dbgText0 = dbg ;
	sprintf(dbg , "%d (%ds)" , JackIO::GetCurrentScene()->frameN , JackIO::GetCurrentScene()->frameN / sampleRate) ; app->dbgText1 = dbg ;
	sprintf(dbg , "%u (%ds)" , JackIO::GetCurrentScene()->nFrames , JackIO::GetCurrentScene()->nFrames / sampleRate) ; app->dbgText2 = dbg ;
	sprintf(dbg , "%d" , JackIO::GetCurrentScene()->isSaveLoop) ; app->dbgText3 = dbg ;
	sprintf(dbg , "%d" , JackIO::GetCurrentScene()->isPulseExist) ; app->dbgText4 = dbg ;

	Scene* nextScene = Scenes[NextSceneN] ;
	sprintf(dbg , "%u" , nextScene->loops.size()) ; app->dbgText15 = dbg ;
	sprintf(dbg , "%d (%ds)" , nextScene->frameN , nextScene->frameN / sampleRate) ; app->dbgText16 = dbg ;
	sprintf(dbg , "%u (%ds)" , nextScene->nFrames , nextScene->nFrames / sampleRate) ; app->dbgText17 = dbg ;
	sprintf(dbg , "%d" , nextScene->isSaveLoop) ; app->dbgText18 = dbg ;
	sprintf(dbg , "%d" , nextScene->isPulseExist) ; app->dbgText19 = dbg ;

	sprintf(dbg , "%u" , JackIO::GetNFramesPerPeriod()) ; app->dbgText8 = dbg ;
	sprintf(dbg , "%u" , JackIO::GetFrameSize()) ; app->dbgText9 = dbg ;
	sprintf(dbg , "%u" , JackIO::GetPeriodSize()) ; app->dbgText10 = dbg ;
#endif
} // DEBUG end


/* Loopidity Class private varables */

// recording state
bool Loopidity::IsRecording = false ;
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


/* Loop Class private functions */

Loop::Loop(unsigned int nFrames) : peaks()
	{ buffer1 = new SAMPLE[nFrames] ; buffer2 = new SAMPLE[nFrames] ; }

Loop::~Loop() { delete buffer1 ; delete buffer2 ; }


/* Scene Class public functions */

Uint16 Scene::getCurrentPeakN() { return ((float)frameN / (float)nFrames) * (float)N_LOOP_PEAKS ; }

float Scene::getCurrentSeconds() { return frameN / JackIO::GetSampleRate() ; }

float Scene::getTotalSeconds() { return nFrames / JackIO::GetSampleRate() ; }


/* Scene Class private functions */

Scene::Scene(unsigned int scenen , unsigned int nframes) :
		// identity
		sceneN(scenen) ,
		// GUI
		sceneGui(0) ,
		// peaks cache
		hiScenePeaks() , hiLoopPeaks(), highestScenePeak(0.0) ,
		// buffer iteration
		nFrames(nframes) , nFramesPerPeak(nframes / N_LOOP_PEAKS) , frameN(0) ,
		// recording state
		isSaveLoop(true) , isPulseExist(false) {}

// audio data
void Scene::addLoop(Loop* newLoop)
{
	unsigned int nLoops = loops.size() ;
	if (nLoops < N_LOOPS)
	{
		scanPeaks(newLoop , nLoops) ; sceneGui->drawLoop(nLoops , newLoop->peaks) ;
		loops.push_back(newLoop) ;
	}
}

void Scene::deleteLoop()
	{ if (loops.size()) { loops.pop_back() ; sceneGui->loopImgs.pop_back() ; rescanPeaks() ; } }


void Scene::reset()
{
	for (unsigned int loopN = 0 ; loopN < loops.size() ; ++loopN) deleteLoop() ;
	frameN = 0 ; isSaveLoop = true ; isPulseExist = false ;
}


// peaks cache

void Scene::scanPeaks(Loop* loop , unsigned int loopN)
{
#if SCAN_LOOP_PEAKS_DATA
	if (!loop || loopN >= N_LOOPS) return ;

	SAMPLE* peaks = loop->peaks ; unsigned int framen ; SAMPLE peak1 , peak2 ;
	for (unsigned int peakN = 0 ; peakN < N_LOOP_PEAKS ; ++peakN)
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
#endif
}

void Scene::rescanPeaks()
{
	highestScenePeak = 0.0 ;
	for (unsigned int peakN = 0 ; peakN < N_LOOP_PEAKS ; ++peakN) hiScenePeaks[peakN] = 0.0 ;
	for (unsigned int loopN = 0 ; loopN < N_LOOPS ; ++loopN)
		{ hiLoopPeaks[loopN] = 0.0 ; if (loopN < loops.size()) scanPeaks(loops[loopN] , loopN) ; }
}


// recording state

void Scene::setMode()
{
	if (isPulseExist) { isSaveLoop = !isSaveLoop ; return ; }

	nFramesPerPeak = (nFrames = frameN + JackIO::GetNFramesPerPeriod()) / N_LOOP_PEAKS ;
	isPulseExist = true ;
}


// getters/setters

unsigned int Scene::getLoopPos() { return (frameN * 1000) / nFrames ; }

// helpers

void Scene::sceneChanged()
	{ sceneGui->drawScene(sceneGui->inactiveSceneSurface , this) ; LoopiditySdl::DrawMode() ; }


/* Loopidity Class public functions */

// user actions

bool Loopidity::Init(unsigned int recordBufferSize , bool isMonitorInputs)
{
	if (!recordBufferSize) { LoopiditySdl::Alert(ZERO_BUFFER_SIZE_MSG) ; return false ; }

	unsigned int nFrames = recordBufferSize / JackIO::GetFrameSize() ;
	for (unsigned int sceneN = 0 ; sceneN < N_SCENES ; ++sceneN)
		Scenes[sceneN] = new Scene(sceneN , nFrames) ;
	if (!JackIO::Init(nFrames , Scenes[0] , isMonitorInputs))
		{ LoopiditySdl::Alert(JACK_FAIL_MSG) ; return false ; }

	// initialize scope/VU peaks cache
	float interval = (float)GUI_UPDATE_INTERVAL_SHORT * 0.001 ;
	float sampleRate = (float)JackIO::GetSampleRate() ;
	NFramesPerGuiInterval = (unsigned int)(sampleRate * interval) ;
	for (unsigned int peakN = 0 ; peakN < N_TRANSIENT_PEAKS ; ++peakN)
		{ PeaksIn.push_back(0.0) ; PeaksOut.push_back(0.0) ; }

	// get handles on JACK buffers for scope/VU peaks
	RecordBuffer1 = JackIO::GetRecordBuffer1() ; RecordBuffer2 = JackIO::GetRecordBuffer2() ;

	return true ;
}

void Loopidity::ToggleScene()
{
	JackIO::SetNextScene(Scenes[NextSceneN = (NextSceneN + 1) % N_SCENES]) ;
	LoopiditySdl::DrawMode() ;
}

void Loopidity::SetMode()
{
	if (!IsRecording) IsRecording = true ;
	else JackIO::GetCurrentScene()->setMode() ;
	LoopiditySdl::DrawMode() ;

if (DEBUG) { char dbg[256] ; sprintf(dbg , "IsRecording:%d isPulseExist:%d isSaveLoop:%d" , IsRecording , GetIsPulseExist() , GetIsSaveLoop()) ; LoopiditySdl::SetStatusR(dbg) ; }
}

// TODO: cancel initial recording if not committed if (!nLoops) IsRecording = false ; frameN = 0 ;
void Loopidity::DeleteLoop() { JackIO::GetCurrentScene()->deleteLoop() ; }

void Loopidity::ResetCurrentScene() { JackIO::GetCurrentScene()->reset() ; }

void Loopidity::Reset()
{
	NextSceneN = 0 ; unsigned int n = N_SCENES ; while (n--) Scenes[n]->reset() ;
	LoopiditySdl::ResetGUI() ; JackIO::Reset(Scenes[0]) ;
}


// getters/setters

Scene** Loopidity::GetScenes() { return Scenes ; }

unsigned int Loopidity::GetCurrentSceneN()
{
	unsigned int sceneN = N_SCENES ; Scene* currentScene = JackIO::GetCurrentScene() ;
	while (sceneN-- && Scenes[sceneN] != currentScene) ; return sceneN ;
}

unsigned int Loopidity::GetNextSceneN() { return NextSceneN ; }

unsigned int Loopidity::GetLoopPos() { return JackIO::GetCurrentScene()->getLoopPos() ; }

bool Loopidity::GetIsSaveLoop() { return JackIO::GetCurrentScene()->isSaveLoop ; }

bool Loopidity::GetIsRecording() { return IsRecording ; }

bool Loopidity::GetIsPulseExist() { return JackIO::GetCurrentScene()->isPulseExist ; }

void Loopidity::SetNFramesPerPeriod(unsigned int nFrames) { NFramesPerPeriod = nFrames ; }

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

void Loopidity::ScanTransientPeaks()
{
#if SCAN_TRANSIENT_PEAKS_DATA
	Scene* currentScene = JackIO::GetCurrentScene() ; unsigned int frameN = currentScene->frameN ;
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
#endif
}
