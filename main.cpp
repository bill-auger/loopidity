
#include "loopidity_sdl.h"
#include <X11/Xlib.h>

/* LoopiditySdl class public variables */

// main GUI
SDL_Surface* LoopiditySdl::Screen = 0 ;
SDL_Rect LoopiditySdl::WinRect = WIN_RECT ;
Uint32 LoopiditySdl::WinBgColor = 0 ;
SDL_Rect LoopiditySdl::HeaderRectDim = HEADER_RECT_DIM ;
SDL_Rect LoopiditySdl::HeaderRectC = HEADER_RECT_C ;
TTF_Font* LoopiditySdl::HeaderFont = 0 ;
SDL_Color LoopiditySdl::HeaderColor = HEADER_TEXT_COLOR ;
SDL_Rect LoopiditySdl::StatusRectDim = STATUS_RECT_DIM ;
SDL_Rect LoopiditySdl::StatusRectL = STATUS_RECT_L ;
SDL_Rect LoopiditySdl::StatusRectR = STATUS_RECT_R ;
TTF_Font* LoopiditySdl::StatusFont = 0 ;
SDL_Color LoopiditySdl::StatusColor = STATUS_TEXT_COLOR ;
string LoopiditySdl::StatusTextL = "StatusTextL" ;
string LoopiditySdl::StatusTextR = "StatusTextR" ;
Uint16 LoopiditySdl::GuiLongCount = 0 ;

// scenes GUI
SceneSdl* LoopiditySdl::SdlScenes[N_SCENES] = {0} ;
SDL_Surface* LoopiditySdl::SceneBgGradient = 0 ;
SDL_Surface* LoopiditySdl::LoopBgGradient = 0 ;

// scope peaks cache
vector<SAMPLE>* LoopiditySdl::InPeaks ;
vector<SAMPLE>* LoopiditySdl::OutPeaks ;
SAMPLE* LoopiditySdl::TransientPeaks = 0 ;


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
	WinBgColor = SDL_MapRGB(LoopiditySdl::Screen->format , 0 , 0 , 0) ;
	SDL_WM_SetCaption(APP_NAME , APP_NAME) ;

	// load fonts
	if (TTF_Init()) { TtfError("TTF_Init") ; return false ; }
	if(!(StatusFont = TTF_OpenFont(STATUS_FONT))) { TtfError("TTF_OpenFont") ; return false ; }
	if(!(HeaderFont = TTF_OpenFont(HEADER_FONT))) { TtfError("TTF_OpenFont") ; return false ; }

	// load images
	if (!(SceneBgGradient = SDL_LoadBMP(SCENE_BG_IMG)))  { SdlError("SDL_LoadBMP") ; return false ; }
	if (!(LoopBgGradient = IMG_Load(LOOP_BG_IMG)))  { SdlError("SDL_LoadBMP") ; return false ; }

	// initialize Loopidity and JackIO classes
	if (!Loopidity::Init(DEFAULT_BUFFER_SIZE , isMonitorInputs)) return false ;

	// instantiate SdlScenes
	Scene** scenes = Loopidity::GetScenes() ;
	for (unsigned int sceneN = 0 ; sceneN < N_SCENES ; ++sceneN)
		SdlScenes[sceneN] = new SceneSdl(scenes[sceneN] , sceneN) ;

	// get handles to scope and VU peaks caches
	InPeaks = Loopidity::GetInPeaksCache() ; OutPeaks = Loopidity::GetOutPeaksCache() ;
	TransientPeaks = Loopidity::GetTransientPeaksCache() ;

	return true ;
}

void LoopiditySdl::Cleanup()
{
	TTF_CloseFont(StatusFont) ; SDL_FreeSurface(SceneBgGradient) ;
	SDL_FreeSurface(LoopBgGradient) ; SDL_FreeSurface(Screen) ;
}


// drawing

void LoopiditySdl::DrawScenes()
{
#if DRAW_SCENES
	unsigned int currentSceneN = Loopidity::GetCurrentSceneN() ; SceneSdl* sdlScene ;
	for (unsigned int sceneN = 0 ; sceneN < N_SCENES ; ++sceneN)
	{
		sdlScene = SdlScenes[sceneN] ;
		SDL_FillRect(LoopiditySdl::Screen , &sdlScene->sceneRect , WinBgColor) ;
		if (sceneN == currentSceneN)
		{
			sdlScene->drawScene(sdlScene->activeSceneSurface) ;
			SDL_BlitSurface(sdlScene->activeSceneSurface , 0 , Screen , &sdlScene->sceneRect) ;
		}
#if DRAW_INACTIVE_SCENES
		else SDL_BlitSurface(sdlScene->inactiveSceneSurface , 0 , Screen , &sdlScene->sceneRect) ;
#endif // #if DRAW_INACTIVE_SCENES
	}

#if DRAW_DEBUG_TEXT
char dbg[255] ; SdlScenes[Loopidity::GetCurrentSceneN()]->makeMainDbgText(dbg) ; LoopiditySdl::SetStatusL(dbg) ;
#endif
#endif // #if DRAW_SCENES
} // void LoopiditySdl::DrawScenes()

//LoopiditySdl::DrawScopes() { #if DRAW_SCOPES}

void LoopiditySdl::DrawMode()
{
#if DRAW_STATUS
	unsigned int currentSceneN = Loopidity::GetCurrentSceneN() ;
	unsigned int nextSceneN = Loopidity::GetNextSceneN() ;
	bool isSaveLoop = Loopidity::GetIsSaveLoop() ;
	bool isRecording = Loopidity::GetIsRecording() ;
	bool isPulseExist = Loopidity::GetIsPulseExist() ;

	// current scene indicator
	Color activeColor = (isSaveLoop)? STATUS_COLOR_RECORDING : STATUS_COLOR_PLAYING ;
	verseLabel.SetInk((currentSceneN == 0)? activeColor : STATUS_COLOR_IDLE) ;
	chorusLabel.SetInk((currentSceneN == 1)? activeColor : STATUS_COLOR_IDLE) ;
	bridgeLabel.SetInk((currentSceneN == 2)? activeColor : STATUS_COLOR_IDLE) ;

	// next scene indicator
	verseLabel.SetFrame((nextSceneN == 0)? BlackFrame() : NullFrame()) ;
	chorusLabel.SetFrame((nextSceneN == 1)? BlackFrame() : NullFrame()) ;
	bridgeLabel.SetFrame((nextSceneN == 2)? BlackFrame() : NullFrame()) ;

	// progress bar
	if (isRecording && !isPulseExist) loopProgress.SetColor(STATUS_COLOR_RECORDING) ;
	else loopProgress.SetColor(STATUS_COLOR_IDLE) ;
#endif // void LoopiditySdl::SetMode()
}

void LoopiditySdl::DrawText(string text , SDL_Surface* surface , TTF_Font* font , SDL_Rect* screenRect , SDL_Rect* cropRect , SDL_Color fgColor)
{
	if (!text.size()) return ;

	SDL_Surface* textSurface = TTF_RenderText_Solid(font , text.c_str() , fgColor) ;
	if (!textSurface) { TtfError("TTF_Init") ; return ; }

	SDL_BlitSurface(textSurface , cropRect , surface , screenRect) ; SDL_FreeSurface(textSurface) ;
}

void LoopiditySdl::DrawHeader() { DrawText(HEADER_TEXT , Screen , HeaderFont , &HeaderRectC , &HeaderRectDim , HeaderColor) ; }

void LoopiditySdl::DrawStatusTextL() { DrawText(StatusTextL , Screen , StatusFont , &StatusRectL , &StatusRectDim , StatusColor) ; }

void LoopiditySdl::DrawStatusTextR() { DrawText(StatusTextR , Screen , StatusFont , &StatusRectR , &StatusRectDim , StatusColor) ; }

void LoopiditySdl::Alert(const char* msg) { printf("%s" , msg) ; }


// helpers

void LoopiditySdl::SceneChanged(Uint16 sceneN)
{
	SceneSdl* sdlScene = SdlScenes[sceneN] ;
	sdlScene->drawScene(sdlScene->inactiveSceneSurface) ; DrawMode() ;
}

void LoopiditySdl::SetStatusL(string text) { StatusTextL = text ; }

void LoopiditySdl::SetStatusR(string text) { StatusTextR = text ; }

void LoopiditySdl::ResetGUI() { DrawMode() ; }


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
//		else { SDL_Delay(5) ; continue ; }
		else { SDL_Delay(GUI_UPDATE_INTERVAL_SHORT - elapsed) ; continue ; }

		if (!(LoopiditySdl::GuiLongCount = (LoopiditySdl::GuiLongCount + 1) % GUI_LONGCOUNT))
			{ LoopiditySdl::DrawStatusTextL() ; LoopiditySdl::DrawStatusTextR() ; }

/* draw a pie
Sint16 x = 200 , y = 200 , radius = 100 , begin = 0 , end = 180 ;
Uint8 r = 255 , g = 255 , b = 255 , a = 255 ;
filledPieRGBA(LoopiditySdl::Screen , x , y , radius , begin , end , r , g , b , a) ;
*/

		LoopiditySdl::DrawScenes() ;
//		LoopiditySdl::DrawScopes(wd , winRect) ;

		SDL_Flip(LoopiditySdl::Screen) ;
	} // while (!done)

	LoopiditySdl::Cleanup() ; return 0 ;
}
