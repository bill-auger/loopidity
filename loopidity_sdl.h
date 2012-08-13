
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

// window magnitudes
#define SCREEN_W 1024 // minimum screen resolution
#define SCREEN_H 768 // minimum screen resolution
#define WIN_TITLE_H 20 // approximate window decoration size
#define WIN_BORDER_W 2 // approximate window decoration size
#define WIN_BORDER_H 2 // approximate window decoration size
#define PIXEL_DEPTH 32
#define LOOP_IMG_BYTES_PER_PIXEL 4 // PIXEL_DEPTH / 8
#define WIN_W SCREEN_W - (WIN_BORDER_W * 2)
#define WIN_H SCREEN_H - WIN_TITLE_H - WIN_BORDER_H
#define WIN_RECT {0 , 0 , WIN_W , WIN_H}
#define WIN_CENTER (WinRect.w / 2)

// header magnitudes
#define HEADER_FONT_SIZE 36
//#define HEADER_W 200 // approx 10 chars @ STATUS_FONT_SIZE 36
#define HEADER_W 360 // approx 18 chars @ STATUS_FONT_SIZE 36
#define HEADER_H 48 // HEADER_FONT_SIZE 36
#define HEADER_X WIN_CENTER - (HEADER_W / 2)
#define HEADER_RECT_DIM {0 , 0 , HEADER_W , HEADER_H}
#define HEADER_RECT_C {HEADER_X , 0 , 0 , 0}

// status magnitudes
#define STATUS_FONT_SIZE 12
#define STATUS_H 20 // STATUS_FONT_SIZE 12
#define STATUS_W 256 // approx 36 chars @ STATUS_FONT_SIZE 12
#define STATUS_Y WinRect.h - STATUS_H
#define STATUS_X WinRect.w - STATUS_W
#define STATUS_RECT_DIM {0 , 0 , STATUS_W , STATUS_H}
#define STATUS_RECT_L {0 , STATUS_Y , 0 , 0}
#define STATUS_RECT_R {STATUS_X , STATUS_Y , 0 , 0}

// scope magnitudes
#define N_TRANSIENT_PEAKS 100 // TODO: maybe set this to screen width/2
#define SCOPE_W N_TRANSIENT_PEAKS * 2
#define SCOPE_H 100
#define SCOPE_X WIN_CENTER - N_TRANSIENT_PEAKS
#define SCOPE_R WIN_CENTER + N_TRANSIENT_PEAKS
#define SCOPE_Y WinRect.h - (STATUS_H * 2) - SCOPE_H
#define SCOPE_RECT {SCOPE_X , SCOPE_Y , SCOPE_W + 1 , SCOPE_H + 1}
#define SCOPE_LOUD 0.95
#define SCOPE_OPTIMAL 0.8

// fonts and colors
#define INSCOPE_QUIET_COLOR 0x00ff00ff
#define INSCOPE_OPTIMAL_COLOR 0xffff00ff
#define INSCOPE_LOUD_COLOR 0xff0000ff
#define OUTSCOPE_QUIET_COLOR 0x00ff00ff
#define OUTSCOPE_OPTIMAL_COLOR 0xffff00ff
#define OUTSCOPE_LOUD_COLOR 0xff0000ff
#define HEADER_FONT "/usr/share/fonts/truetype/tlwg/Purisa.ttf" , HEADER_FONT_SIZE
#define HEADER_TEXT_COLOR {255 , 0 , 255}
#define STATUS_FONT "/usr/share/fonts/truetype/tlwg/Purisa.ttf" , STATUS_FONT_SIZE
#define STATUS_TEXT_COLOR {255 , 0 , 255}

// external images
#define SCENE_BG_IMG "scene_bg_gradient.bmp"
#define LOOP_BG_IMG "loop_bg_gradient.alpha.bmp"

// string constants
#define HEADER_TEXT "This is "APP_NAME


class LoopiditySdl
{
	public:

		// window
		static Uint16 GuiLongCount ;
		static SDL_Surface* Screen ;
		static SDL_Rect WinRect ;
		static Uint32 WinBgColor ;
		static const Uint16 WinCenter ;

		// header
		static SDL_Rect HeaderRectDim ;
		static SDL_Rect HeaderRectC ;
		static TTF_Font* HeaderFont ;
		static const SDL_Color HeaderColor ;

		// status
		static SDL_Rect StatusRectDim ;
		static SDL_Rect StatusRectL ;
		static SDL_Rect StatusRectR ;
		static TTF_Font* StatusFont ;
		static const SDL_Color StatusColor ;
		static string StatusTextL ;
		static string StatusTextR ;

		// scenes
		static SceneSdl* SdlScenes[N_SCENES] ;
		static SDL_Surface* SceneBgGradient ;
		static SDL_Surface* LoopBgGradient ;

		// scopes
		static SDL_Rect ScopeRect ;
		static const Sint16 ScopeY ;
		static const Uint16 ScopeR ;
		static const float ScopePeakH ;
		static vector<SAMPLE>* PeaksIn ;
		static vector<SAMPLE>* PeaksOut ;
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
		static void DrawScopes() ;
		static void DrawMode() ;
		static void DrawText(string text , SDL_Surface* surface , TTF_Font* font , SDL_Rect* screenRect , SDL_Rect* cropRect , SDL_Color fgColor) ;
		static void DrawHeader() ;
		static void DrawStatusTextL() ;
		static void DrawStatusTextR() ;
		static void Alert(const char* msg) ;

		// helpers
		static void SetStatusL(string msg) ;
		static void SetStatusR(string msg) ;
/*
		static string makeTime(Uint16 seconds) ;
		static Uint32 getAvailableMemory() ;
*/
		static void ResetGUI() ;
/*
		void updateMemory() ;
		void updateLoopProgress() ;
		void updateVUMeters() ;
*/
} ;


#endif // #ifndef _LOOPIDITY_SDL_H_
