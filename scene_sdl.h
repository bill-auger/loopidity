
#ifndef _SCENE_SDL_H_
#define _SCENE_SDL_H_


#ifdef __cplusplus
    #include <cstdlib>
#else
    #include <stdlib.h>
#endif

#ifdef __APPLE__
#include <SDL/SDL.h>
#else
#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#endif


#include "loopidity.h"
class Scene ;


// loop magnitudes
#define LOOP_PEAK_H 50 // lets fix this to N_PEAKS / 2 for sdl histogramWidth (loopD)
/*
#define LOOP_IMG_RES LOOP_D // TODO: fixed to LOOP_D for now (as long as PEAK_MAX is sensible for this)
#define LOOP_BG_IMG_RES 256 // must be 256
*/
/*
#define PIE_SLICE_DEGREES 3600 / N_PEAKS
#define PIE_12_OCLOCK -900
*/
/*
// colors
#define WIN_BG_COLOR Black
#define SCENE_BG_COLOR WIN_BG_COLOR
#define SCENE_PEAK_MAX_COLOR Red
#define SCENE_PEAK_ZERO_COLOR Color(128 , 128 , 128)
*/
#define HISTOGRAM_BORDER_ACTIVE_COLOR 0x00ff00ff
#define HISTOGRAM_BORDER_INACTIVE_COLOR 0x004000ff
#define HISTOGRAM_PEAK_CURRENT_ACTIVE_COLOR 0xffff00ff
#define HISTOGRAM_PEAK_CURRENT_INACTIVE_COLOR 0x808000ff
#define HISTOGRAM_PEAK_ACTIVE_COLOR 0x008000ff
#define HISTOGRAM_PEAK_INACTIVE_COLOR 0x002000ff
/*
#define LOOP_PEAK_CURRENT_COLOR White
#define LOOP_PEAK_MAX_COLOR Red
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

		SceneSdl(Scene* scene , unsigned int sceneNum , SDL_Rect winRect) :
				// constants
				scene(scene) , sceneN(sceneNum) , loopD(LOOP_PEAK_H * 2) , yOffset(loopD / 4) ,
				xPadding(loopD / 8) , yPadding(loopD / 3) , histogramH(yPadding / 2) ,
				loopW(xPadding + loopD) , sceneW(winRect.w - (xPadding * 2)) ,
				sceneH(yPadding + loopD) , sceneX(xPadding) , sceneY(yOffset + (sceneH * sceneN)) ,
				// variables
				sceneL(0) , sceneT(0) , histogramT(0) , histogram0(0) , histogramB(0) ,
				maxPeakY(0) , zeroPeakY(0) , minPeakY(0) {}

		// audio/peaks data
		const Scene* scene ;
		const unsigned int sceneN ;

		// drawing constants
		const unsigned int loopD ;
		const unsigned int yOffset ;
		const unsigned int xPadding ;
		const unsigned int yPadding ;
		const unsigned int histogramH ;
		const unsigned int loopW ;
		const unsigned int sceneW ;
		const unsigned int sceneH ;
		const unsigned int sceneX ;
		const unsigned int sceneY ;

		// drawing dimensions
		unsigned int sceneL ;
		unsigned int sceneT ;
		Sint16 histogramT ;
		Sint16 histogram0 ;
		Sint16 histogramB ;
		unsigned int maxPeakY ;
		unsigned int zeroPeakY ;
		unsigned int minPeakY ;

		// drawing helpers
		void setDims(bool isFullScreen) ;
/*
		Image createLoopPeaksMask(unsigned int loopN , unsigned int peakN) ;
		static Image CreateLoopGradientImg() ;
		static Image CreateLoopGradientImgCached() ;
		Image createLoopImg(unsigned int loopN , unsigned int peakN) ;
		Image createLoopImgCached(unsigned int loopN , unsigned int peakN) ;
		static Image CreatePeaksBgGradientImg() ;
		static Image CreatePeaksBgGradientImgCached() ;
		Image createSceneImg() ;
		Image createSceneImgCached(unsigned int w , unsigned int h) ;
*/

		// drawing
		void drawScene(SDL_Surface* screen) ;
// DEBUG
void makeMainDbgText(char* dbg) ;
// DEBUG end
} ;


#endif // #ifndef _SCENE_SDL_H_
