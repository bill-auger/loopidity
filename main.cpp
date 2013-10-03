
#include "loopidity_sdl.h"


/* LoopiditySdl class public variables */

// window
Uint16 LoopiditySdl::GuiLongCount = 0 ;
SDL_Surface* LoopiditySdl::Screen = 0 ;
SDL_Rect LoopiditySdl::WinRect = WIN_RECT ;
Uint32 LoopiditySdl::WinBgColor = 0 ;
const Uint16 LoopiditySdl::WinCenter = WIN_CENTER ;

// header
SDL_Rect LoopiditySdl::HeaderRectDim = HEADER_RECT_DIM ;
SDL_Rect LoopiditySdl::HeaderRectC = HEADER_RECT_C ;
TTF_Font* LoopiditySdl::HeaderFont = 0 ;
const SDL_Color LoopiditySdl::HeaderColor = HEADER_TEXT_COLOR ;

// status
SDL_Rect LoopiditySdl::StatusRectDim = STATUS_RECT_DIM ;
SDL_Rect LoopiditySdl::StatusRectL = STATUS_RECT_L ;
SDL_Rect LoopiditySdl::StatusRectC = STATUS_RECT_C ;
SDL_Rect LoopiditySdl::StatusRectR = STATUS_RECT_R ;
TTF_Font* LoopiditySdl::StatusFont = 0 ;
const SDL_Color LoopiditySdl::StatusColor = STATUS_TEXT_COLOR ;
string LoopiditySdl::StatusTextL = "" ;
string LoopiditySdl::StatusTextC = "" ;
string LoopiditySdl::StatusTextR = "" ;

// scenes
SceneSdl** LoopiditySdl::SdlScenes = 0 ;
SDL_Surface* LoopiditySdl::ScopeGradient = 0 ;
SDL_Surface* LoopiditySdl::HistogramGradient = 0 ;
SDL_Surface* LoopiditySdl::LoopGradient = 0 ;

// scopes
SDL_Rect LoopiditySdl::ScopeRect = SCOPE_RECT ;
SDL_Rect LoopiditySdl::MaskRect = {0 , 0 , 1 , 0} ;
SDL_Rect LoopiditySdl::GradientRect = {0 , 0 , 0 , 0} ;
const Sint16 LoopiditySdl::Scope0 = SCOPE_0 ;
const Uint16 LoopiditySdl::ScopeR = SCOPE_R ;
const float LoopiditySdl::ScopePeakH = SCOPE_PEAK_H ;
vector<SAMPLE>* LoopiditySdl::PeaksIn ;
vector<SAMPLE>* LoopiditySdl::PeaksOut ;
SAMPLE* LoopiditySdl::PeaksTransient = 0 ;

// DrawScenes() 'local' variables
Uint16 LoopiditySdl::CurrentSceneN = 0 ;
Uint16 LoopiditySdl::NextSceneN = 0 ;
Uint32 LoopiditySdl::CurrentPeakN = 0 ;
Uint16 LoopiditySdl::LoopProgress = 0 ;
Uint16 LoopiditySdl::SceneN = 0 ;
SceneSdl* LoopiditySdl::SdlScene = 0 ;
SDL_Surface* LoopiditySdl::ActiveSurface = 0 ;


/* LoopiditySdl class public functions */

// setup

bool LoopiditySdl::Init(int argc , char** argv)
{
#if DEBUG_TRACE
printf("\nINIT: LoopiditySdl::Init()\n\n") ;
#endif // #if DEBUG_TRACE

	if (Screen) return false ; // we only want to do this once

	// parse command line arguments
	bool isMonitorInputs = true , isAutoSceneChange = true ;
	for (int argN = 0 ; argN < argc ; ++argN)
		if (!strcmp(argv[argN] , MONITOR_ARG)) isMonitorInputs = false ;
		else if (!strcmp(argv[argN] , SCENE_CHANGE_ARG)) isAutoSceneChange = false ;

	// detect screen resolution
	Display* display = XOpenDisplay(NULL) ; XWindowAttributes winAttr ;
	Uint16 screenN = DefaultScreen(display) ;
	if (!XGetWindowAttributes(display, RootWindow(display , screenN) , &winAttr))
		{ printf("ERROR: LoopiditySdl::Init(): XGetWindowAttributes(): can't get root window geometry - quitting\n"); return false ; }
	if (winAttr.width < SCREEN_W || winAttr.height < SCREEN_H)
		{ printf("ERROR: screen resolution must be at least %dx%d - quitting\n" , SCREEN_W , SCREEN_H) ; return false ; }

	// initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) { SdlError("SDL_Init") ; return false ; }
	atexit(SDL_Quit) ;
	Screen = SDL_SetVideoMode(WinRect.w , WinRect.h , PIXEL_DEPTH , SDL_HWSURFACE | SDL_DOUBLEBUF) ;
	if (!Screen) { SdlError("SDL_SetVideoMode") ; return false ; }

	// set input params
	if (SDL_EnableKeyRepeat(0 , 0)) { SdlError("SDL_EnableKeyRepeat") ; return false ; }
	SDL_EventState(SDL_MOUSEMOTION , SDL_IGNORE) ;

	// set default window params
	WinBgColor = SDL_MapRGB(LoopiditySdl::Screen->format , 16 , 16 , 16) ;
	SDL_WM_SetCaption(APP_NAME , APP_NAME) ;
/*
	SDL_Surface* icon = SDL_LoadBMP("icon.bmp") ;
	SDL_SetColorKey(icon , SDL_SRCCOLORKEY , SDL_MapRGB(icon->format , 255 , 0 , 255)) ;
	SDL_WM_SetIcon(icon , NULL) ;
*/

	// load fonts
	if (TTF_Init()) { TtfError("TTF_Init") ; return false ; }
	if(!(StatusFont = TTF_OpenFont(STATUS_FONT))) { TtfError("TTF_OpenFont") ; return false ; }
	if(!(HeaderFont = TTF_OpenFont(HEADER_FONT))) { TtfError("TTF_OpenFont") ; return false ; }

	// load images
	if (!(ScopeGradient = SDL_LoadBMP(SCOPE_IMG))) { SdlError("SDL_LoadBMP") ; return false ; }
	if (!(HistogramGradient = SDL_LoadBMP(HISTOGRAM_IMG))) { SdlError("SDL_LoadBMP") ; return false ; }
	if (!(LoopGradient = SDL_LoadBMP(LOOP_IMG))) { SdlError("SDL_LoadBMP") ; return false ; }

	// initialize Loopidity class (controller) and instantiate SdlScenes (views)
	if (!(SdlScenes = Loopidity::Init((isMonitorInputs) , (isAutoSceneChange)))) return false ;

	// get handles to scope and VU peaks caches
	PeaksIn = Loopidity::GetPeaksInCache() ; PeaksOut = Loopidity::GetPeaksOutCache() ;
	PeaksTransient = Loopidity::GetTransientPeaksCache() ;

	return true ;
}

void LoopiditySdl::SdlError(const char* functionName) { printf("ERROR: %s(): %s\n" , functionName , SDL_GetError()) ; }

void LoopiditySdl::TtfError(const char* functionName) { printf("ERROR: %s(): %s\n" , functionName , TTF_GetError()) ; }

void LoopiditySdl::Cleanup()
{
	Loopidity::Cleanup() ;

	TTF_CloseFont(HeaderFont) ; TTF_CloseFont(StatusFont) ;
	SDL_FreeSurface(ScopeGradient) ; SDL_FreeSurface(HistogramGradient) ;
	SDL_FreeSurface(LoopGradient) ;
	SDL_FreeSurface(Screen) ;
}


// event handlers

void LoopiditySdl::HandleKeyEvent(SDL_Event* event)
{
DEBUG_TRACE_LOOPIDITYSDL_HANDLEKEYEVENT

	switch (event->key.keysym.sym)
	{
		case SDLK_SPACE: Loopidity::ToggleState() ; break ;
		case SDLK_KP0: Loopidity::ToggleScene() ; break ;
		case SDLK_KP_ENTER: Loopidity::ToggleSceneIsMuted() ; break ;
		case SDLK_ESCAPE:
			switch (event->key.keysym.mod)
			{
				case KMOD_RCTRL: Loopidity::Reset() ; break ;
				case KMOD_RSHIFT: Loopidity::ResetCurrentScene() ; break ;
				default: Loopidity::DeleteLastLoop() ; break ;
			}
		default: break ;
	}
}

void LoopiditySdl::HandleMouseEvent(SDL_Event* event)
{
	Sint16 x = event->button.x , y = event->button.y ;
	if (x < MOUSE_SCENES_L || x > MOUSE_SCENES_R ||
			y < MOUSE_SCENES_T || y > MOUSE_SCENES_B) return ;

	Uint16 sceneN = (y - MOUSE_SCENES_T) / SCENE_H , loopN = (x - MOUSE_SCENES_L) / LOOP_W ;
	switch (event->button.button)
	{
		case SDL_BUTTON_LEFT: Loopidity::ToggleLoopIsMuted(sceneN , loopN) ; break ;
		case SDL_BUTTON_MIDDLE: Loopidity::DeleteLoop(sceneN , loopN) ; break ;
		case SDL_BUTTON_RIGHT: break ;
		case SDL_BUTTON_WHEELUP: Loopidity::IncLoopVol(sceneN , loopN , true) ; break ;
		case SDL_BUTTON_WHEELDOWN: Loopidity::IncLoopVol(sceneN , loopN , false) ; break ;
		default: break ;
	}
}

void LoopiditySdl::HandleUserEvent(SDL_Event* event)
{
	unsigned int sceneN = *((unsigned int*)event->user.data1) ;
	switch (event->user.code)
	{
		case EVT_NEW_LOOP: Loopidity::OnLoopCreation(sceneN , *((Loop**)event->user.data2)) ; break ;
		case EVT_SCENE_CHANGED: Loopidity::OnSceneChange(sceneN) ; break ;
		default: break ;
	}
}


// drawing

void LoopiditySdl::DrawScenes()
{
#if DRAW_SCENES
	CurrentSceneN = Loopidity::GetCurrentSceneN() ; NextSceneN = Loopidity::GetNextSceneN() ;
	for (SceneN = 0 ; SceneN < N_SCENES ; ++SceneN)
	{
		SdlScene = SdlScenes[SceneN] ; ActiveSurface = SdlScene->activeSceneSurface ;
		SDL_FillRect(LoopiditySdl::Screen , &SdlScene->sceneRect , WinBgColor) ;
		if (SceneN == CurrentSceneN)
		{
			CurrentPeakN = SdlScene->scene->getCurrentPeakN() ;
			LoopProgress = ((float)CurrentPeakN / (float)N_PEAKS_FINE) * 100 ;
			SdlScene->drawScene(ActiveSurface , CurrentPeakN , LoopProgress) ;

#if DRAW_RECORDING_LOOP
			if (SdlScene->scene->loops.size() < N_LOOPS)
				SdlScene->drawRecordingLoop(ActiveSurface , LoopProgress) ;
#endif

			SDL_BlitSurface(ActiveSurface , 0 , Screen , &SdlScene->sceneRect) ;
		}
		else SDL_BlitSurface(SdlScene->inactiveSceneSurface , 0 , Screen , &SdlScene->sceneRect) ;
		SdlScene->drawSceneStateIndicator(Screen) ;
	}

//SDL_FillRect((LoopiditySdl::Screen , &(SdlScenes[SceneN]->sceneRect) , WinBgColor) ;

#if DRAW_DEBUG_TEXT
Loopidity::SetDbgTextL() ;
#endif
#endif // #if DRAW_SCENES
} // void LoopiditySdl::DrawScenes()

void LoopiditySdl::DrawScopes()
{
#if DRAW_SCOPES
	SDL_FillRect(Screen , &ScopeRect , WinBgColor) ;
	for (Uint16 peakN = 0 ; peakN < N_PEAKS_TRANSIENT ; ++peakN)
	{
		Sint16 inX = ScopeR - peakN , outX = WinCenter - peakN ;
		Sint16 inH = (Uint16)((*PeaksIn)[peakN] * ScopePeakH) ;
		Sint16 outH = (Uint16)((*PeaksOut)[peakN] * ScopePeakH) ;
		MaskRect.y = (Sint16)ScopePeakH - inH ; MaskRect.h = (inH * 2) + 1 ;
		GradientRect.x = inX ; GradientRect.y = Scope0 - inH ;
		SDL_BlitSurface(LoopiditySdl::ScopeGradient , &MaskRect , Screen , &GradientRect) ;
		MaskRect.y = (Sint16)ScopePeakH - outH ; MaskRect.h = (outH * 2) + 1 ;
		GradientRect.x = outX ; GradientRect.y = Scope0 - outH ;
		SDL_BlitSurface(LoopiditySdl::ScopeGradient , &MaskRect , Screen , &GradientRect) ;
	}
#endif
}

void LoopiditySdl::DrawText(string text , SDL_Surface* surface , TTF_Font* font , SDL_Rect* screenRect , SDL_Rect* cropRect , SDL_Color fgColor)
{
#if DRAW_STATUS
	if (!text.size()) return ;

	SDL_Surface* textSurface = TTF_RenderText_Solid(font , text.c_str() , fgColor) ;
	if (!textSurface) { TtfError("TTF_Init") ; return ; }

	SDL_FillRect(surface , screenRect , WinBgColor) ;
	SDL_BlitSurface(textSurface , cropRect , surface , screenRect) ; SDL_FreeSurface(textSurface) ;
#endif // #if DRAW_STATUS
}

void LoopiditySdl::DrawHeader() { DrawText(HEADER_TEXT , Screen , HeaderFont , &HeaderRectC , &HeaderRectDim , HeaderColor) ; }

void LoopiditySdl::DrawStatusTextL() { DrawText(StatusTextL , Screen , StatusFont , &StatusRectL , &StatusRectDim , StatusColor) ; }

void LoopiditySdl::DrawStatusTextC() { DrawText(StatusTextC , Screen , StatusFont , &StatusRectC , &StatusRectDim , StatusColor) ; }

void LoopiditySdl::DrawStatusTextR() { DrawText(StatusTextR , Screen , StatusFont , &StatusRectR , &StatusRectDim , StatusColor) ; }

void LoopiditySdl::Alert(const char* msg) { printf("%s" , msg) ; }


// getters/settters

void LoopiditySdl::SetStatusL(string text) { StatusTextL = text ; }

void LoopiditySdl::SetStatusC(string text) { StatusTextC = text ; }

void LoopiditySdl::SetStatusR(string text) { StatusTextR = text ; }


/* entry point */

int main(int argc , char** argv)
{
	// initialize SDL and Loopidity
#if DEBUG_TRACE
		if (LoopiditySdl::Init(argc , argv)) printf(INIT_SUCCESS_MSG) ;
		else { printf(INIT_FAIL_MSG) ; LoopiditySdl::Cleanup() ; return 1 ; }
#else
	if (!LoopiditySdl::Init(argc , argv)) { LoopiditySdl::Cleanup() ; return 1 ; }
#endif // #if DEBUG_TRACE

	// blank screen and draw header
	SDL_FillRect(LoopiditySdl::Screen , 0 , LoopiditySdl::WinBgColor) ;
	LoopiditySdl::DrawHeader() ;

	// main loop
	bool done = false ; SDL_Event event ; Uint16 timerStart = SDL_GetTicks() , elapsed ;
	while (!done)
	{
		// poll events and pass them off to our controller
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT: done = true ; break ;
				case SDL_KEYDOWN: LoopiditySdl::HandleKeyEvent(&event) ; break ;
//				case SDL_MOUSEBUTTONDOWN: LoopiditySdl::HandleMouseEvent(&event) ; break ;
				case SDL_USEREVENT: LoopiditySdl::HandleUserEvent(&event) ; break ;
				default: break ;
			}
		}

		// throttle framerate
		elapsed = SDL_GetTicks() - timerStart ;
		if (elapsed >= GUI_UPDATE_INTERVAL_SHORT) timerStart = SDL_GetTicks() ;
		else { SDL_Delay(1) ; continue ; }

		// draw low priority
		if (!(LoopiditySdl::GuiLongCount = (LoopiditySdl::GuiLongCount + 1) % GUI_LONGCOUNT))
			{ /*updateMemory() ;*/ LoopiditySdl::DrawStatusTextL() ; LoopiditySdl::DrawStatusTextR() ; }

		// draw high priority
		Loopidity::ScanTransientPeaks() ; //updateVUMeters() ;
		LoopiditySdl::DrawScenes() ;
		LoopiditySdl::DrawScopes() ;

		SDL_Flip(LoopiditySdl::Screen) ;
	} // while (!done)

	LoopiditySdl::Cleanup() ; return 0 ;
}
