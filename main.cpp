
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
SDL_Rect LoopiditySdl::StatusRectR = STATUS_RECT_R ;
TTF_Font* LoopiditySdl::StatusFont = 0 ;
const SDL_Color LoopiditySdl::StatusColor = STATUS_TEXT_COLOR ;
string LoopiditySdl::StatusTextL = "" ;
string LoopiditySdl::StatusTextR = "" ;

// scenes
SceneSdl* LoopiditySdl::SdlScenes[N_SCENES] = {0} ;
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
SAMPLE* LoopiditySdl::TransientPeaks = 0 ;

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

void LoopiditySdl::SdlError(const char* functionName) { printf("%s(): %s\n" , functionName , SDL_GetError()) ; }

void LoopiditySdl::TtfError(const char* functionName) { printf("%s(): %s\n" , functionName , TTF_GetError()) ; }

bool LoopiditySdl::Init(bool isMonitorInputs)
{
	// initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) { SdlError("SDL_Init") ; return false ; }
	atexit(SDL_Quit) ;
	Screen = SDL_SetVideoMode(WinRect.w , WinRect.h , PIXEL_DEPTH , SDL_HWSURFACE | SDL_DOUBLEBUF) ;
	if (!Screen) { SdlError("SDL_SetVideoMode") ; return false ; }
	if (SDL_EnableKeyRepeat(0 , 0)) { LoopiditySdl::Cleanup() ; return false ; }

	// sett default window params
	WinBgColor = SDL_MapRGB(LoopiditySdl::Screen->format , 16 , 16 , 16) ;
	SDL_WM_SetCaption(APP_NAME , APP_NAME) ;

	// load fonts
	if (TTF_Init()) { TtfError("TTF_Init") ; return false ; }
	if(!(StatusFont = TTF_OpenFont(STATUS_FONT))) { TtfError("TTF_OpenFont") ; return false ; }
	if(!(HeaderFont = TTF_OpenFont(HEADER_FONT))) { TtfError("TTF_OpenFont") ; return false ; }

	// load images
	if (!(ScopeGradient = SDL_LoadBMP(SCOPE_IMG)))  { SdlError("SDL_LoadBMP") ; return false ; }
	if (!(HistogramGradient = SDL_LoadBMP(HISTOGRAM_IMG)))  { SdlError("SDL_LoadBMP") ; return false ; }
	if (!(LoopGradient = SDL_LoadBMP(LOOP_IMG)))  { SdlError("SDL_LoadBMP") ; return false ; }

	// initialize Loopidity and JackIO classes
	if (!Loopidity::Init(isMonitorInputs)) return false ;

	// instantiate SdlScenes
	for (Uint16 sceneN = 0 ; sceneN < N_SCENES ; ++sceneN)
	{
		SdlScenes[sceneN] = new SceneSdl(sceneN) ;
		Loopidity::SetSceneGui(SdlScenes[sceneN] , sceneN) ;
	}

	// get handles to scope and VU peaks caches
	PeaksIn = Loopidity::GetPeaksInCache() ; PeaksOut = Loopidity::GetPeaksOutCache() ;
	TransientPeaks = Loopidity::GetTransientPeaksCache() ;

	return true ;
}

void LoopiditySdl::Cleanup()
{
	TTF_CloseFont(HeaderFont) ; TTF_CloseFont(StatusFont) ;
	SDL_FreeSurface(ScopeGradient) ; SDL_FreeSurface(HistogramGradient) ;
	SDL_FreeSurface(LoopGradient) ;
	for (Uint16 sceneN = 0 ; sceneN < N_SCENES ; ++sceneN)
	{
		SceneSdl* sdlScene = SdlScenes[sceneN] ;
		SDL_FreeSurface(sdlScene->activeSceneSurface) ;
		SDL_FreeSurface(sdlScene->inactiveSceneSurface) ;
		sdlScene->loopImgs.clear() ;
	}
	SDL_FreeSurface(Screen) ;
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

#if DRAW_DEBUG_TEXT
char dbg[255] ; Loopidity::GetCurrentScene()->makeMainDbgText(dbg) ; LoopiditySdl::SetStatusL(dbg) ;
sprintf(dbg , "isPulseExist:%d isSaveLoop:%d" , Loopidity::GetCurrentScene()->isPulseExist , Loopidity::GetCurrentScene()->isSaveLoop) ; LoopiditySdl::SetStatusR(dbg) ;
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
	if (!text.size()) return ;

	SDL_Surface* textSurface = TTF_RenderText_Solid(font , text.c_str() , fgColor) ;
	if (!textSurface) { TtfError("TTF_Init") ; return ; }

	SDL_FillRect(surface , screenRect , WinBgColor) ;
	SDL_BlitSurface(textSurface , cropRect , surface , screenRect) ; SDL_FreeSurface(textSurface) ;
}

void LoopiditySdl::DrawHeader() { DrawText(HEADER_TEXT , Screen , HeaderFont , &HeaderRectC , &HeaderRectDim , HeaderColor) ; }

void LoopiditySdl::DrawStatusTextL() { DrawText(StatusTextL , Screen , StatusFont , &StatusRectL , &StatusRectDim , StatusColor) ; }

void LoopiditySdl::DrawStatusTextR() { DrawText(StatusTextR , Screen , StatusFont , &StatusRectR , &StatusRectDim , StatusColor) ; }

void LoopiditySdl::Alert(const char* msg) { printf("%s" , msg) ; }


// helpers

void LoopiditySdl::SetStatusL(string text) { StatusTextL = text ; }

void LoopiditySdl::SetStatusR(string text) { StatusTextR = text ; }


/* entry point */

int main(int argc , char** argv)
{
	// detect screen resolution
	Display* display = XOpenDisplay(NULL) ; XWindowAttributes winAttr ;
	Uint16 screenN = DefaultScreen(display);
	if (!XGetWindowAttributes(display, RootWindow(display , screenN) , &winAttr))
		{ printf("XGetWindowAttributes(): can't get root window geometry - quitting\n"); return 1 ; }
	if (winAttr.width < SCREEN_W || winAttr.height < SCREEN_H)
		{ printf("screen resolution must be at least 1024x760 - quitting\n"); return 1 ; }

	// parse command line arguments
	bool isMonitorInputs = true ;
	for (int argN = 0 ; argN < argc ; ++argN)
		if (!strcmp(argv[argN] , MONITOR_ARG)) isMonitorInputs = false ;

	// initialize SDL
	if (!LoopiditySdl::Init(isMonitorInputs)) return 1 ;

	// blank screen and draw header
	SDL_FillRect(LoopiditySdl::Screen , 0 , LoopiditySdl::WinBgColor) ;
	LoopiditySdl::DrawHeader() ;

	// main loop
	bool done = false ; SDL_Event event ; Uint16 timerStart = SDL_GetTicks() , elapsed ;
	while (!done)
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT: done = true ; break ;
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_SPACE: Loopidity::SetMode() ; break ;
						case SDLK_KP0: Loopidity::ToggleScene() ; break ;
						case SDLK_ESCAPE:
							switch (event.key.keysym.mod)
							{
								case KMOD_RCTRL: Loopidity::Reset() ; break ;
								case KMOD_RSHIFT: Loopidity::ResetCurrentScene() ; break ;
								default: Loopidity::DeleteLoop() ; break ;
							}
						default: break ;
					}
				break ;
				default: break ;
			} // switch (event.type)
		} // while (SDL_PollEvent(&event))

		elapsed = SDL_GetTicks() - timerStart ;
		if (elapsed >= GUI_UPDATE_INTERVAL_SHORT) timerStart = SDL_GetTicks() ;
		else { SDL_Delay(1) ; continue ; }

#if DRAW_STATUS
		if (!(LoopiditySdl::GuiLongCount = (LoopiditySdl::GuiLongCount + 1) % GUI_LONGCOUNT))
			{ /*updateMemory() ;*/ LoopiditySdl::DrawStatusTextL() ; LoopiditySdl::DrawStatusTextR() ; }
#endif // #if DRAW_STATUS

		Loopidity::ScanTransientPeaks() ; //updateVUMeters() ;
		LoopiditySdl::DrawScenes() ;
		LoopiditySdl::DrawScopes() ;

		SDL_Flip(LoopiditySdl::Screen) ;
	} // while (!done)

	LoopiditySdl::Cleanup() ; return 0 ;
}
