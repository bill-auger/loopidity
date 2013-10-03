
#include "loopidity.h"
#include "trace.h"


/* Loopidity Class private varables */

// recording state
unsigned int Loopidity::CurrentSceneN = 0 ;
unsigned int Loopidity::NextSceneN = 0 ;
//bool Loopidity::IsRolling = false ;
bool Loopidity::ShouldSceneAutoChange = false ;

// audio data
Scene* Loopidity::Scenes[N_SCENES] = {0} ;
SceneSdl* Loopidity::SdlScenes[N_SCENES] = {0} ;
unsigned int Loopidity::NFramesPerPeriod = 0 ;
unsigned int Loopidity::NFramesPerGuiInterval = 0 ;
SAMPLE* Loopidity::Buffer1 = 0 ;
SAMPLE* Loopidity::Buffer2 = 0 ;
vector<SAMPLE> Loopidity::PeaksIn ;
vector<SAMPLE> Loopidity::PeaksOut ;
SAMPLE Loopidity::TransientPeaks[N_PORTS] = {0} ;
SAMPLE Loopidity::TransientPeakInMix = 0 ;
SAMPLE Loopidity::TransientPeakOutMix = 0 ;


/* Loopidity Class public functions */

// setup

bool Loopidity::SetRecordBufferSize(unsigned int recordBufferSize)
{
	return (JackIO::SetRecordBufferSize(recordBufferSize)) ;
// TODO: caller can LoopiditySdl::Alert(ZERO_BUFFER_SIZE_MSG) ;
}

SceneSdl** Loopidity::Init(bool isMonitorInputs , bool isAutoSceneChange)
{
	// sanity check
	if (Scenes[0]) return NULL ;

	// disable AutoSceneChange if SCENE_CHANGE_ARG given
	if (!isAutoSceneChange) ToggleAutoSceneChange() ;

	// instantiate Scenes (models) and SdlScenes (views)
	for (unsigned int sceneN = 0 ; sceneN < N_SCENES ; ++sceneN)
	{
		Uint32 frameColor = (!sceneN)? STATE_PLAYING_COLOR : STATE_IDLE_COLOR ;
		Scenes[sceneN] = new Scene(sceneN , JackIO::GetRecordBufferSize()) ;
		SdlScenes[sceneN] = new SceneSdl(Scenes[sceneN] , sceneN , frameColor) ;
		UpdateView(sceneN) ;
	}

	// initialize JACK
	int jackInit = JackIO::Init(Scenes[0] , 0 , isMonitorInputs) ;
	if (jackInit == JACK_MEM_FAIL) { LoopiditySdl::Alert(INSUFFICIENT_MEMORY_MSG) ; return NULL ; }
	else if(jackInit == JACK_FAIL) { LoopiditySdl::Alert(JACK_FAIL_MSG) ; return NULL ; }

	// initialize scope/VU peaks cache
	for (unsigned int peakN = 0 ; peakN < N_PEAKS_TRANSIENT ; ++peakN)
		{ PeaksIn.push_back(0.0) ; PeaksOut.push_back(0.0) ; }

	// get handles on JACK buffers for scope/VU peaks
	Buffer1 = JackIO::GetBuffer1() ; Buffer2 = JackIO::GetBuffer2() ;

	return SdlScenes ;
}


// user actions

void Loopidity::ToggleAutoSceneChange() { ShouldSceneAutoChange = !ShouldSceneAutoChange ; }

void Loopidity::ToggleScene()
{
DEBUG_TRACE_LOOPIDITY_TOGGLESCENE_IN

	unsigned int prevSceneN = NextSceneN ; NextSceneN = (NextSceneN + 1) % N_SCENES ;
	UpdateView(prevSceneN) ; UpdateView(NextSceneN) ;
printf("xx\n");
	JackIO::SetNextScene(Scenes[NextSceneN] , NextSceneN) ;

	if (!Scenes[CurrentSceneN]->isRolling)
	{
		prevSceneN = CurrentSceneN ; CurrentSceneN = NextSceneN ;
		Scenes[prevSceneN]->reset() ; // SdlScenes[prevSceneN]->reset() ;
		UpdateView(prevSceneN) ; UpdateView(NextSceneN) ;
printf("xx\n");
		JackIO::SetCurrentScene(Scenes[NextSceneN] , NextSceneN) ;
	}

DEBUG_TRACE_LOOPIDITY_TOGGLESCENE_OUT
}

void Loopidity::ToggleState()
{
DEBUG_TRACE_LOOPIDITY_TOGGLESTATE_IN

	if (Scenes[CurrentSceneN]->isRolling) Scenes[CurrentSceneN]->toggleState() ;
	else { Scenes[CurrentSceneN]->startRolling() ; SdlScenes[CurrentSceneN]->startRolling() ; }
	UpdateView(CurrentSceneN) ;

DEBUG_TRACE_LOOPIDITY_TOGGLESTATE_OUT
}

void Loopidity::DeleteLoop(unsigned int sceneN , unsigned int loopN)
{
DEBUG_TRACE_LOOPIDITY_DELETELOOP_IN

	SdlScenes[sceneN]->deleteLoop(loopN) ; Scenes[sceneN]->deleteLoop(loopN) ;

DEBUG_TRACE_LOOPIDITY_DELETELOOP_OUT
}

void Loopidity::DeleteLastLoop()
{
DEBUG_TRACE_LOOPIDITY_DELETELASTLOOP_IN

	DeleteLoop(CurrentSceneN , Scenes[CurrentSceneN]->loops.size() - 1) ;

DEBUG_TRACE_LOOPIDITY_DELETELASTLOOP_OUT
}

void Loopidity::IncLoopVol(unsigned int sceneN , unsigned int loopN , bool isInc)
{
DEBUG_TRACE_LOOPIDITY_INCLOOPVOL_IN

	Loop* loop = Scenes[sceneN]->getLoop(loopN) ; if (!loop) return ;

	float* vol = &loop->vol ;
	if (isInc) { *vol += LOOP_VOL_INC ; if (*vol > 1.0) *vol = 1.0 ; }
	else { *vol -= LOOP_VOL_INC ; if (*vol < 0.0) *vol = 0.0 ; }

DEBUG_TRACE_LOOPIDITY_INCLOOPVOL_OUT
}

void Loopidity::ToggleLoopIsMuted(unsigned int sceneN , unsigned int loopN)
{
DEBUG_TRACE_LOOPIDITY_TOGGLELOOPISMUTED_IN

	Loop* loop = Scenes[sceneN]->getLoop(loopN) ; loop->isMuted = !loop->isMuted ;

DEBUG_TRACE_LOOPIDITY_TOGGLELOOPISMUTED_OUT
}

void Loopidity::ToggleSceneIsMuted()
	{ Scene* scene = Scenes[CurrentSceneN] ; scene->isMuted = !scene->isMuted ; }

void Loopidity::ResetScene(unsigned int sceneN)
{
DEBUG_TRACE_LOOPIDITY_RESETSCENE_IN

	Scenes[sceneN]->reset() ; SdlScenes[sceneN]->reset() ;

DEBUG_TRACE_LOOPIDITY_RESETSCENE_OUT
}

void Loopidity::ResetCurrentScene() { ResetScene(CurrentSceneN) ; }

void Loopidity::Reset()
{
DEBUG_TRACE_LOOPIDITY_RESET_IN

	CurrentSceneN = NextSceneN = 0 ; JackIO::Reset(Scenes[0] , 0) ;
	for (unsigned int sceneN = 0 ; sceneN < N_SCENES ; ++sceneN) ResetScene(sceneN) ;

DEBUG_TRACE_LOOPIDITY_RESET_OUT
}

void Loopidity::Cleanup()
{
	for (unsigned int sceneN = 0 ; sceneN < N_SCENES ; ++sceneN)
		if (SdlScenes[sceneN]) SdlScenes[sceneN]->cleanup() ;
}


// getters/setters

void Loopidity::SetMetaData(unsigned int sampleRate , unsigned int frameSize , unsigned int nFramesPerPeriod)
{
	float interval = (float)GUI_UPDATE_INTERVAL_SHORT * 0.001 ;
	NFramesPerGuiInterval = (unsigned int)(sampleRate * interval) ;

	Scene::SetMetaData(sampleRate , frameSize , nFramesPerPeriod) ;
}

Scene** Loopidity::GetScenes() { return Scenes ; }

SceneSdl** Loopidity::GetSdlScenes() { return SdlScenes ; }

unsigned int Loopidity::GetCurrentSceneN() { return CurrentSceneN ; }

unsigned int Loopidity::GetNextSceneN() { return NextSceneN ; }

unsigned int Loopidity::GetLoopPos() { return Scenes[CurrentSceneN]->getLoopPos() ; }

//bool Loopidity::GetIsRolling() { return IsRolling ; }

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
	try // TODO: this function is unsafe
	{
		for (unsigned int frameN = 0 ; frameN < nFrames ; ++frameN)
			{ SAMPLE sample = fabs(buffer[frameN]) ; if (peak < sample) peak = sample ; }
	}
	catch(int ex) { printf("Loopidity::GetPeak(): subscript out of range\n") ; }
	return peak ;
}


// event handlers

void Loopidity::OnLoopCreation(unsigned int sceneN , Loop* newLoop)
{
DEBUG_TRACE_LOOPIDITY_ONLOOPCREATION_IN

	Scene* scene = Scenes[sceneN] ; SceneSdl* sdlScene = SdlScenes[sceneN] ;
	if (scene->addLoop(newLoop)) sdlScene->addLoop(newLoop , scene->loops.size() - 1) ;

	UpdateView(sceneN) ;

DEBUG_TRACE_LOOPIDITY_ONLOOPCREATION_OUT
}

void Loopidity::OnSceneChange(unsigned int nextSceneN)
{
DEBUG_TRACE_LOOPIDITY_ONSCENECHANGE_IN //else { Scenes[CurrentSceneN]->startRolling() ; SdlScenes[CurrentSceneN]->startRolling() ; }

//	if (!Scenes[CurrentSceneN]->isRolling) { Scenes[CurrentSceneN]->reset() ; }

	unsigned int prevSceneN = CurrentSceneN ; CurrentSceneN = NextSceneN = nextSceneN ;
	SdlScenes[prevSceneN]->drawSceneInactive() ; UpdateView(prevSceneN) ; UpdateView(nextSceneN) ;

	if (ShouldSceneAutoChange) do ToggleScene() ; while (!Scenes[NextSceneN]->loops.size()) ;

DEBUG_TRACE_LOOPIDITY_ONSCENECHANGE_OUT
}


// helpers

void Loopidity::ScanTransientPeaks()
{
#if SCAN_TRANSIENT_PEAKS_DATA
	Scene* currentScene = Scenes[CurrentSceneN] ; unsigned int frameN = currentScene->frameN ;
// TODO; first NFramesPerGuiInterval duplicated
	unsigned int currentFrameN = (frameN < NFramesPerGuiInterval)? 0 : frameN - NFramesPerGuiInterval ;

	// initialize with inputs
	SAMPLE peakIn1 = GetPeak(&(Buffer1[currentFrameN]) , NFramesPerGuiInterval) ;
	SAMPLE peakIn2 = GetPeak(&(Buffer2[currentFrameN]) , NFramesPerGuiInterval) ;

	// add in unmuted tracks
	SAMPLE peakOut1 = 0.0 , peakOut2 = 0.0 ; unsigned int nLoops = currentScene->loops.size() ;
	for (unsigned int loopN = 0 ; loopN < nLoops ; ++loopN)
	{
		Loop* loop = currentScene->getLoop(loopN) ; if (currentScene->isMuted && loop->isMuted) continue ;

		peakOut1 += GetPeak(&(loop->buffer1[currentFrameN]) , NFramesPerGuiInterval) * loop->vol ;
		peakOut2 += GetPeak(&(loop->buffer2[currentFrameN]) , NFramesPerGuiInterval) * loop->vol ;
	}

	SAMPLE peakIn = (peakIn1 + peakIn2) / N_INPUT_CHANNELS ;
	SAMPLE peakOut = (peakOut1 + peakOut2) / N_OUTPUT_CHANNELS ;
	if (peakOut > 1.0) peakOut = 1.0 ;

	// load scope peaks (mono mix)
	PeaksIn.pop_back() ; PeaksIn.insert(PeaksIn.begin() , peakIn) ;
	PeaksOut.pop_back() ; PeaksOut.insert(PeaksOut.begin() , peakOut) ;

	// load VU peaks (per channel)
	TransientPeakInMix = peakIn ; TransientPeakOutMix = peakOut ;
	TransientPeaks[0] = peakIn1 ; TransientPeaks[1] = peakIn2 ;
	TransientPeaks[2] = peakOut1 ; TransientPeaks[3] = peakOut2 ;
#endif // #if SCAN_TRANSIENT_PEAKS_DATA
}

void Loopidity::UpdateView(unsigned int sceneN) { SdlScenes[sceneN]->updateStatus() ; }
//{ if (Scenes[sceneN]->isRolling) SdlScenes[sceneN]->updateStatus() ; }

void Loopidity::OOM() { DEBUG_TRACE_LOOPIDITY_OOM_IN LoopiditySdl::SetStatusC(OUT_OF_MEMORY_MSG) ; }


// DEBUG
//void Loopidity::DbgPad(unsigned int nTabs , string* str)
//	{ nTabs -= str->size() ; if (nTabs > 0) while (nTabs--) *str += " " ; }
/*
unsigned int Loopidity::GetNLoops(unsigned int sceneN)  { return Scenes[sceneN]->loops.size() ; }
unsigned int Loopidity::GetHistogramImgsSize(unsigned int sceneN) {	return SdlScenes[sceneN]->histogramImgs.size() ; }
unsigned int Loopidity::GetLoopImgsSize(unsigned int sceneN) {	return SdlScenes[sceneN]->loopImgs.size() ; }
*/
bool Loopidity::SanityCheck(unsigned int sceneN)
{
	Scene* scene = Scenes[sceneN] ; SceneSdl* sdlScene = SdlScenes[sceneN] ;
	unsigned int nLoops = scene->loops.size() ;
	return (nLoops == sdlScene->histogramImgs.size() && nLoops == sdlScene->loopImgs.size()) ;
}

//bool Scene::traceScene(const char* senderTemplate) { return Loopidity::TraceScene(senderTemplate , this) ; }

bool Loopidity::TraceScene(const char* senderTemplate , Scene* scene)
{
/*
	char* normalEvent , errorEvent ;
	bool isModel = (!strncmp(sender , "Scene::" , 0 , 7)) ;
	bool isView = (!strncmp(sender , "SceneSdl::" , 0 , 10) || !strncmp(sender "LoopiditySdl::" , 0 , 14)) ;
	bool isController = (!strncmp(sender "Loopidity::" , 0 , 11)) ;
printf("Loopidity::TraceScenes() isModel=%d isView=%d isController=%d\n" , isModel , isView , isController) ;
*/
/*
	if (isModel) { normalEvent = DEBUG_TRACE_MODEL ; errorEvent = DEBUG_TRACE_MODEL_ERR ; }
	else if (isView) { normalEvent = DEBUG_TRACE_VIEW ; errorEvent = DEBUG_TRACE_VIEW_ERR ; }
	else if (isController) { normalEvent = DEBUG_TRACE_CONTROLLER ; errorEvent = DEBUG_TRACE_CONTROLLER_ERR ; }
	else { printf ("Loopidity::TraceScenes() unknown sender='%s'\n" , sender) ; return false ; }
*/
#if DEBUG_TRACE
	// mvc sanity checks
	bool isEq = SanityCheck(scene->sceneN) ;
	char *modelEvent , *modelDescFormat , *viewEvent , *viewDescFormat ;
	if (isEq)
	{
		modelEvent = DEBUG_TRACE_MODEL ; modelDescFormat = DEBUG_TRACE_MODEL_DESC_FORMAT ;
		viewEvent = DEBUG_TRACE_VIEW ; viewDescFormat = DEBUG_TRACE_VIEW_DESC_FORMAT ;
	}
	else
	{
		modelEvent = DEBUG_TRACE_MODEL_ERROR ; modelDescFormat = DEBUG_TRACE_MODEL_ERROR_FORMAT ;
		viewEvent = DEBUG_TRACE_VIEW_ERROR ; viewDescFormat = DEBUG_TRACE_VIEW_ERROR_FORMAT ;
	}
/*
	switch (senderClass)
	{
		case LOOPIDITY_CLASS: sender = "Loopidity::" ; break ;
		case LOOPIDITYSDL_CLASS: sender = "LoopiditySdl::" ; break ;
		case SCENE_CLASS: SenderClass = "Scene::" ; break ;
		case SCENESDL_CLASS: SenderClass = "SceneSdl::" ; break ;
		default: printf ("Trace::TraceScene() unknown sender='%s'\n" , senderFunction) ; return ; break ;
	}
*/
	unsigned int sceneN = scene->sceneN ;
	char sender[DEBUG_TRACE_EVENT_LEN] ; sprintf(sender , senderTemplate , sceneN) ;
	SceneSdl* sdlScene = SdlScenes[sceneN] ;

	// model state dump
	Trace::TraceScene(modelEvent , sender ,
			DEBUG_TRACE_MODEL_STATE_FORMAT , modelDescFormat , scene->isRolling , scene->shouldSaveLoop , scene->doesPulseExist , isEq) ;
	// view state dump
	Trace::TraceScene(viewEvent , sender ,
			DEBUG_TRACE_VIEW_STATE_FORMAT , viewDescFormat , scene->loops.size() , sdlScene->histogramImgs.size() , sdlScene->loopImgs.size() , isEq) ;

//	Loopidity::SdlScenes[sceneN]->traceSceneSdl(sender) ;
/*
	char recordingStatus[2] = ' \0' , saveLoopStatus[2] = ' \0' , pulseExistStatus[2] = ' \0' ;
	if (isRecording < 0 || isRecording > 1) recordingStatus[0] = 'X' ; else snprintf(recordingStatus , "%d" , isRecording , 1) ;
	if (shouldSaveLoop < 0 || shouldSaveLoop > 1) saveLoopStatus[0] = 'X' ; else snprintf(saveLoopStatus , "%d" , shouldSaveLoop , 1) ;
	if (doesPulseExist < 0 || doesPulseExist > 1) pulseExistStatus[0] = 'X' ; else snprintf(pulseExistStatus , "%d" , doesPulseExist , 1) ;
*/
//	if (!IsEq) { event = DEBUG_TRACE_MODEL_ERR + sender ; Loopidity::DbgPad(0 , &event) ; } printf("%sScene::mvc inconsistent\n" , event.c_str()) ; }
/*
	event = "TRACE: " + sender ; Loopidity::DbgPad(0 , &event) ;
	stringstream recordingSs , saveLoopSs , pulseExistSs , descSs ;
	recordingSs << "isRecording=" << isRecording ;
	saveLoopSs << " shouldSaveLoop=" << shouldSaveLoop ;
	pulseExistSs << " doesPulseExist=" << doesPulseExist ;
	descSs << recordingSs << saveLoopSs << pulseExistSs ;
	string desc = descSs.str() ; Loopidity::DbgPad(1 , &desc) ;
	printf("%s%s%d%d%d\n" , event.c_str() , descStr.c_str() , isRecording , shouldSaveLoop , doesPulseExist) ;
*/
/*
	event = "TRACE: " + sender ; Loopidity::DbgPad(0 , &event) ; char desc[256] ;
	sprintf(desc , "isRecording=%d shouldSaveLoop=%d doesPulseExist=%d" , isRecording , shouldSaveLoop , doesPulseExist) ;
	string descStr = desc ; Loopidity::DbgPad(1 , &descStr) ;
	PrintScene(DEBUG_TRACE_TAB_MODEL , event.c_str() , isRecording , shouldSaveLoop , doesPulseExist , desc.c_str()) ;
*/
/*
	event = DEBUG_TRACE_VIEW + sender ; Loopidity::DbgPad(DEBUG_TRACE_EVENT , &event) ;
	char desc[256] ; sprintf(desc , DEBUG_TRACE_VIEW_DESC_FORMAT , scene->loops.size() , histogramImgs.size() , loopImgs.size()) ;
*/
/*
	stringstream descSs ; descSs << "nLoops=" << scene->loops.size() << " nHistogramImgs=" << histogramImgs.size() << " nLoopImgs=" << loopImgs.size() ;
	string desc = descSs.str() ; Loopidity::DbgPad(DEBUG_TRACE_DESC , &desc) ;
*/

	return (isEq) ;
#else // #if DEBUG_TRACE
	return (Loopidity::SanityCheck(scene->sceneN)) ;
#endif // #if DEBUG_TRACE
}
// DEBUG end

#if DRAW_DEBUG_TEXT
void Loopidity::SetDbgTextL() { char dbg[255] ; sprintf(dbg , "NextSceneN=%d SceneN=%d nLoops=%d PeakN=%d" , NextSceneN , CurrentSceneN , Scenes[CurrentSceneN]->loops.size() , Scenes[CurrentSceneN]->getCurrentPeakN()) ; LoopiditySdl::SetStatusL(dbg) ; }
void Loopidity::SetDbgTextR() { char dbg[255] ; sprintf(dbg , "%d%d%d %d%d%d" , Scenes[CurrentSceneN]->isRolling , Scenes[CurrentSceneN]->shouldSaveLoop , Scenes[CurrentSceneN]->doesPulseExist , Scenes[CurrentSceneN]->loops.size() , SdlScenes[CurrentSceneN]->histogramImgs.size() , SdlScenes[CurrentSceneN]->loopImgs.size()) ; LoopiditySdl::SetStatusR(dbg) ; }
#endif // #if DRAW_DEBUG_TEXT
