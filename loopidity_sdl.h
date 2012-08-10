
#ifndef _LOOPIDITY_SDL_H_
#define _LOOPIDITY_SDL_H_


#include "loopidity.h"
class Scene ;
#include "scene_sdl.h"
class SceneSdl ;


using namespace std ;


// intervals
#define GUI_UPDATE_INTERVAL_SHORT 125
#define GUI_LONGCOUNT 8

// GUI magnitudes
#define SCREEN_W 1024 // minimum screen resolution
#define SCREEN_H 768 // minimum screen resolution
#define WIN_TITLE_H 20 // approximate window decoration size
#define WIN_BORDER_W 2 // approximate window decoration size
#define WIN_BORDER_H 2 // approximate window decoration size
#define PIXEL_DEPTH 16
#define HEADER_FONT_SIZE 36
//#define HEADER_W 200 // approx 10 chars @ STATUS_FONT_SIZE 36
#define HEADER_W 360 // approx 18 chars @ STATUS_FONT_SIZE 36
#define HEADER_H 48 // HEADER_FONT_SIZE 36
#define STATUS_FONT_SIZE 12
#define STATUS_H 20 // STATUS_FONT_SIZE 12
#define STATUS_W 256 // approx 36 chars @ STATUS_FONT_SIZE 12
#define WIN_RECT {0 , 0 , SCREEN_W - (WIN_BORDER_W * 2) , SCREEN_H - WIN_TITLE_H - WIN_BORDER_H}
#define HEADER_RECT_DIM {0 , 0 , HEADER_W , HEADER_H}
#define HEADER_RECT_C {(WinRect.w / 2) - (HEADER_W / 2) , 0 , 0 , 0}
#define STATUS_RECT_DIM {0 , 0 , STATUS_W , STATUS_H}
#define STATUS_RECT_L {0 , WinRect.h - STATUS_H , 0 , 0}
#define STATUS_RECT_R {WinRect.w - STATUS_W , WinRect.h - STATUS_H , 0 , 0}
/*
// scope magnitudes
#define SCOPE_Y 200
#define SCOPE_H 100
#define SCOPE_LOUD 0.95
#define SCOPE_OPTIMAL 0.8
#define VU_SCALE 100

// fonts and colors
#define SCOPE_BG_COLOR Black
#define INSCOPE_QUIET_COLOR Green
#define INSCOPE_OPTIMAL_COLOR Yellow
#define INSCOPE_LOUD_COLOR Red
#define OUTSCOPE_QUIET_COLOR Green
#define OUTSCOPE_OPTIMAL_COLOR Yellow
#define OUTSCOPE_LOUD_COLOR Red
*/
#define HEADER_FONT "/usr/share/fonts/truetype/tlwg/Purisa.ttf" , HEADER_FONT_SIZE
#define HEADER_TEXT_COLOR {255 , 0 , 255}
#define STATUS_FONT "/usr/share/fonts/truetype/tlwg/Purisa.ttf" , STATUS_FONT_SIZE
#define STATUS_TEXT_COLOR {255 , 0 , 255}

#define SCENE_BG_IMG "scene_bg_gradient.bmp"
#define LOOP_BG_IMG "loop_bg_gradient.alpha.bmp"

// string constants
#define HEADER_TEXT "This is "APP_NAME


class LoopiditySdl
{
	public:

		// main GUI
		static SDL_Surface* Screen ;
		static SDL_Rect WinRect ;
		static Uint32 WinBgColor ;
		static SDL_Rect HeaderRectDim ;
		static SDL_Rect HeaderRectC ;
		static TTF_Font* HeaderFont ;
		static SDL_Color HeaderColor ;
		static SDL_Rect StatusRectDim ;
		static SDL_Rect StatusRectL ;
		static SDL_Rect StatusRectR ;
		static TTF_Font* StatusFont ;
		static SDL_Color StatusColor ;
		static string StatusTextL ;
		static string StatusTextR ;
		static Uint16 GuiLongCount ;

		// scenes GUI
		static SceneSdl* SdlScenes[N_SCENES] ;
		static SDL_Surface* SceneBgGradient ;
		static SDL_Surface* LoopBgGradient ;

		// scope peaks cache
		static vector<SAMPLE>* InPeaks ;
		static vector<SAMPLE>* OutPeaks ;
		static SAMPLE* TransientPeaks ;
/*
		// constants
		static const Color STATUS_COLOR_RECORDING ;
		static const Color STATUS_COLOR_PLAYING ;
		static const Color STATUS_COLOR_IDLE ;
*/
		// setup
		static void SdlError(const char* functionName) ;
		static void TtfError(const char* functionName) ;
		static bool Init(bool isMonitorInputs) ;
		static void Cleanup() ;

		// drawing
		static void DrawScenes() ;
//		static void drawScopes(Draw& w , Rect winRect) ;
		static void DrawMode() ;
		static void DrawText(string text , SDL_Surface* surface , TTF_Font* font , SDL_Rect* screenRect , SDL_Rect* cropRect , SDL_Color fgColor) ;
		static void DrawHeader() ;
		static void DrawStatusTextL() ;
		static void DrawStatusTextR() ;
		static void Alert(const char* msg) ;

		// helpers
		static void SceneChanged(Uint16 sceneN) ;
		static void SetStatusL(string msg) ;
		static void SetStatusR(string msg) ;
/*
		static string makeTime(unsigned int seconds) ;
		static unsigned int getAvailableMemory() ;
*/
		static void ResetGUI() ;
/*
		void updateMemory() ;
		void updateLoopProgress() ;
		void updateVUMeters() ;
*/
} ;


#endif // #ifndef _LOOPIDITY_SDL_H_
