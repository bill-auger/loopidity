
#include "loopidity_sdl.h"


/* LoopiditySdl class public variables */

// main GUI
SDL_Surface* LoopiditySdl::Screen = 0 ;
SDL_Rect LoopiditySdl::WinRect = WIN_RECT ;
SDL_Rect LoopiditySdl::StatusRectL = STATUS_RECT_L ;
SDL_Rect LoopiditySdl::StatusRectR = STATUS_RECT_R ;
TTF_Font* LoopiditySdl::StatusFont = 0 ;
SDL_Color LoopiditySdl::StatusColor = STATUS_TEXT_COLOR ;

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

	// load fonts
	if (TTF_Init()) { TtfError("TTF_Init") ; return false ; }
	if(!(StatusFont = TTF_OpenFont(STATUS_FONT))) { TtfError("TTF_OpenFont") ; return false ; }

	// load images
	if (!(SceneBgGradient = SDL_LoadBMP(SCENE_BG_IMG)))  { SdlError("SDL_LoadBMP") ; return false ; }
	if (!(LoopBgGradient = IMG_Load(LOOP_BG_IMG)))  { SdlError("SDL_LoadBMP") ; return false ; }

	// initialize Loopidity JackIO and SceneUpp classes
	if (!Loopidity::Init(DEFAULT_BUFFER_SIZE , isMonitorInputs)) return false ;

	Scene** scenes = Loopidity::GetScenes() ;
	for (unsigned int sceneN = 0 ; sceneN < N_SCENES ; ++sceneN)
		SdlScenes[sceneN] = new SceneSdl(scenes[sceneN] , sceneN , WinRect) ;

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

void LoopiditySdl::ResetGUI() { SetMode() ; }

void LoopiditySdl::DrawText(const char* text , SDL_Surface* surface , TTF_Font* font , SDL_Rect* rect , SDL_Color fgColor)
{
	SDL_Surface* textSurface = TTF_RenderText_Solid(font , text , fgColor) ;
	if (!textSurface) { TtfError("TTF_Init") ; return ; }

	SDL_BlitSurface(textSurface , NULL , surface , rect) ; SDL_FreeSurface(textSurface) ;
}

void LoopiditySdl::SetStatusL(const char* msg) { DrawText(msg , Screen , StatusFont , &StatusRectL , StatusColor) ; }

void LoopiditySdl::SetStatusR(const char* msg) { DrawText(msg , Screen , StatusFont , &StatusRectR , StatusColor) ; }

void LoopiditySdl::SetMode()
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
#endif
}

void LoopiditySdl::Alert(const char* msg) { printf("%s" , msg) ; }

void LoopiditySdl::DrawScenes()
{
#if DRAW_SCENES
	unsigned int currentSceneN = Loopidity::GetCurrentSceneN() ;
	SceneSdl* sdlScene = SdlScenes[currentSceneN] ;
#if ! DRAW_BG_FULL
if (!IsBetterWayToDoThis) { wd.DrawRect(winRect , WIN_BG_COLOR) ; IsBetterWayToDoThis = true ; }
#endif

		// draw current scene
		sdlScene->setDims(true) ; sdlScene->drawScene(Screen) ;
#if DRAW_INACTIVE_SCENES
		// draw inactive scenes
		for (unsigned int sceneN = 0 ; sceneN < N_SCENES ; ++sceneN)
		{
			if (sceneN == currentSceneN) continue ;

			sdlScene = SdlScenes[sceneN] ; sdlScene->setDims(false) ;
//			SDL_Surface* img = sdlScene->createSceneImgCached() ;
			SDL_Rect sceneRect ; sceneRect.x = sdlScene->sceneX ; sceneRect.y = sdlScene->sceneY ;
			SDL_BlitSurface(LoopiditySdl::SceneBgGradient , 0 , Screen , &sceneRect) ;
		}
#endif // #if DRAW_INACTIVE_SCENES
#endif // #if DRAW_SCENES

#if DRAW_SCENES && DRAW_DEBUG_TEXT
char dbg[255] ; SdlScenes[currentSceneN]->makeMainDbgText(dbg) ;
DrawText(MAIN_DEBUG_TEXT_POS , winRect.Width() , winRect.Height() , WIN_BG_COLOR) ; wd.DrawText(MAIN_DEBUG_TEXT_POS , dbg , Roman(18) , White) ;
#endif
}

//LoopiditySdl::DrawScopes() { #if DRAW_SCOPES}


/* entry point */

bool IsBetterWayToDoThis = false ;
int main(int argc , char** argv)
{
	// parse command line arguments
	bool isMonitorInputs = true ;
	for (int argN = 0 ; argN < argc ; ++argN)
		if (!strcmp(argv[argN] , MONITOR_ARG)) isMonitorInputs = false ;

	// initialize SDL
	if (!LoopiditySdl::Init(isMonitorInputs)) return 1 ;

	// main loop
	bool done = false ; SDL_Event event ;
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

#if DRAW_BG_FULL
		SDL_FillRect(LoopiditySdl::Screen , 0 , SDL_MapRGB(LoopiditySdl::Screen->format , 0 , 0 , 0)) ;
#endif

/* draw a pie
Sint16 x = 200 , y = 200 , radius = 100 , begin = 0 , end = 180 ;
Uint8 r = 255 , g = 255 , b = 255 , a = 255 ;
filledPieRGBA(LoopiditySdl::Screen , x , y , radius , begin , end , r , g , b , a) ;
*/
LoopiditySdl::SetStatusL("some text") ;

		LoopiditySdl::DrawScenes() ;
//		LoopiditySdl::DrawScopes(wd , winRect) ;

		SDL_Flip(LoopiditySdl::Screen) ;
	} // while (!done)

	LoopiditySdl::Cleanup() ; return 0 ;
}
