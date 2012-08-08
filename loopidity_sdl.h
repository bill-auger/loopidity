
#ifndef _LOOPIDITY_SDL_H_
#define _LOOPIDITY_SDL_H_


#ifdef __cplusplus
#include <cstdlib>
#include <vector>
#include <string>
#else
#include <stdlib.h>
#include <vector.h>
#include <string.h>
#endif

#ifdef __APPLE__
#include <SDL/SDL.h>
#else
#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_ttf.h>
#endif


#include "loopidity.h"
class Scene ;
#include "scene_sdl.h"
class SceneSdl ;


using namespace std ;


/*
// intervals
#define GUI_UPDATE_INTERVAL_SHORT 125
#define GUI_UPDATE_INTERVAL_LONG 1000

// GUI magnitudes
#define INIT_W 600
#define INIT_H 400
#define STATUS_W 300

// scope magnitudes
#define SCOPE_Y 200
#define SCOPE_H 100
#define SCOPE_LOUD 0.95
#define SCOPE_OPTIMAL 0.8
#define VU_SCALE 100

// colors
#define SCOPE_BG_COLOR Black
#define INSCOPE_QUIET_COLOR Green
#define INSCOPE_OPTIMAL_COLOR Yellow
#define INSCOPE_LOUD_COLOR Red
#define OUTSCOPE_QUIET_COLOR Green
#define OUTSCOPE_OPTIMAL_COLOR Yellow
#define OUTSCOPE_LOUD_COLOR Red
*/
#define STATUS_RECT {100 , 100 , 0 , 0}
#define STATUS_FONT "/usr/share/fonts/truetype/tlwg/Purisa.ttf" , 12
#define STATUS_TEXT_COLOR { 255 , 0 , 255 }


class LoopiditySdl
{
	public:

		// main GUI
		static SDL_Surface* Screen ;
		static TTF_Font* StatusFont ;
		static SDL_Rect StatusRect ;
		static SDL_Color StatusColor ;

		// scenes GUI
		static SceneSdl* SdlScenes[N_SCENES] ;

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
		static void SdlError(char* functionName) ;
		static void TtfError(char* functionName) ;
		static bool Init(bool isMonitorInputs) ;
		static void Cleanup() ;

		// drawing
		static void ResetGUI() ;
		static void DrawText(const char* text , SDL_Surface* surface , TTF_Font* font , SDL_Rect* rect , SDL_Color fgColor) ;
		static void TempStatusL(const char* msg) ;
		static void SetStatusL(const char* msg) ;
		static void SetStatusR(const char* msg) ;
		static void SetMode() ;
		static void Alert(const char* msg) ;

//		void drawScopes(Draw& w , Rect winRect) ;
//		void drawScene(Draw& d) ;

		// helpers
		static string makeTime(unsigned int seconds) ;
		static unsigned int getAvailableMemory() ;
/*
		void updateMemory() ;
		void updateLoopProgress() ;
		void updateVUMeters() ;
*/
} ;


#endif // #ifndef _LOOPIDITY_SDL_H_
