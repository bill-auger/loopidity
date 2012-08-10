
#ifndef _SCENE_SDL_H_
#define _SCENE_SDL_H_


#include "loopidity.h"
class Scene ;


// loop magnitudes
#define LOOP_PEAK_R 50 // lets fix this to N_PEAKS / 2 for sdl histogramWidth (loopD)
/*
#define LOOP_IMG_RES LOOP_D // TODO: fixed to LOOP_D for now (as long as PEAK_MAX is sensible for this)
#define LOOP_BG_IMG_RES 256 // must be 256
*/
/*
#define PIE_SLICE_DEGREES 3600 / N_PEAKS
#define PIE_12_OCLOCK -900
*/

// colors
// TODO: perhaps it maybe more efficient to SDL_MapRGB(LoopiditySdl::Screen->format , 0 , 0 , 0)
#define SCENE_BG_COLOR WIN_BG_COLOR
#define SCENE_PEAK_MAX_COLOR 0xff0000ff
#define SCENE_PEAK_ZERO_COLOR 0x808080ff
#define HISTOGRAM_BORDER_ACTIVE_COLOR 0x00ff00ff
#define HISTOGRAM_BORDER_INACTIVE_COLOR 0x008000ff
#define HISTOGRAM_PEAK_CURRENT_ACTIVE_COLOR 0xffff00ff
#define HISTOGRAM_PEAK_CURRENT_INACTIVE_COLOR 0x808000ff
#define HISTOGRAM_PEAK_ACTIVE_COLOR 0x008000ff
#define HISTOGRAM_PEAK_INACTIVE_COLOR 0x004000ff
#define LOOP_PEAK_CURRENT_COLOR 0xffffffff
#define LOOP_PEAK_MAX_COLOR 0xff0000ff
/*
#define LOOP_IMG_MASK_COLOR Color(128 , 128 , 128)

// string constants
#define LOOP_GRADIENT_IMAGE_KEY "LOOP_GRADIENT_IMAGE_KEY"
#define LOOP_BG_GRADIENT_IMAGE_KEY "LOOP_BG_GRADIENT_IMAGE_KEY"
#define SCENE_FADE_IMAGE_KEY "SCENE_FADE_IMAGE_KEY"


// testing
//#define N_PEAKS 1000 / GUI_UPDATE_INTERVAL_SHORT * 10
#define HISTOGRAM_Y_SCALE (HISTOGRAM_H) / (float)(LOOP_D)
#define HISTOGRAM_SAMPLE_W 1
*/


class SceneSdl
{
	friend class LoopiditySdl ;

	private:

		SceneSdl(Scene* scene , Uint16 sceneNum) ;

		// audio/peaks data
		const Scene* scene ;
		const Uint16 sceneN ;

		// drawing constants
		static const Uint16 XPadding ;
		static const Uint16 YPadding ;
		static const Uint16 HistogramH ;
		static const Sint16 HistogramT ;
		static const Sint16 HistogramY ;
		static const Sint16 HistogramB ;
		static const Sint16 HistogramBorderL ;
		static const Sint16 HistogramBorderR ;
		static const Uint16 LoopD ;
		static const Uint16 LoopW ;
		static const Uint16 LoopT ;
		static const Uint16 LoopY ;
		static const Uint16 LoopB ;
		static const Uint16 SceneH ;
		static const Uint16 SceneL ;
		const Uint16 sceneY ;
		const Uint16 sceneW ;
		const Uint16 sceneR ;
		SDL_Rect sceneRect ; // will not change but cannot be const

		// scene drawing backbuffers
		SDL_Surface* activeSceneSurface ;
		SDL_Surface* inactiveSceneSurface ;

		// drawing helpers
/*
		Image createLoopPeaksMask(Uint16 loopN , Uint16 peakN) ;
		static Image CreateLoopGradientImg() ;
		static Image CreateLoopGradientImgCached() ;
		Image createLoopImg(Uint16 loopN , Uint16 peakN) ;
		Image createLoopImgCached(Uint16 loopN , Uint16 peakN) ;
		static Image CreatePeaksBgGradientImg() ;
		static Image CreatePeaksBgGradientImgCached() ;
*/

		// drawing
		void drawScene(SDL_Surface* screen) ;

// DEBUG
void makeMainDbgText(char* dbg) ;
// DEBUG end
} ;


#endif // #ifndef _SCENE_SDL_H_
