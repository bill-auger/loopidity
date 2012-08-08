
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
#define LOOP_D PEAK_H * 2 // TODO: make these elements scalable
/*
#define LOOP_IMG_RES LOOP_D // TODO: fixed to LOOP_D for now (as long as PEAK_MAX is sensible for this)
#define LOOP_BG_IMG_RES 256 // must be 256
*/
#define Y_OFFSET (float)(LOOP_D) * 0.25
#define X_PADDING (float)(LOOP_D) * 0.125
#define Y_PADDING (float)(LOOP_D) * 0.333
#define HISTOGRAM_H Y_PADDING * 0.5
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
#define HISTOGRAM_PEAK_CURRENT_COLOR Yellow
#define LOOP_ACTIVE_COLOR Green // TODO: for histogram may not need
#define LOOP_INACTIVE_COLOR Color(0 , 64 , 0) // TODO: for histogram may not need
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
//	friend class LoopiditySdl ;

//	private:
public:

		SceneSdl(Scene* scene , unsigned int sceneNum) :
				scene(scene) , sceneN(sceneNum) , xPadding(0) , loopW(0) ,
				sceneW(0) , sceneH(0) , sceneX(0) , sceneY(0) , sceneL(0) , sceneT(0) ,
				histogramT(0) , histogramB(0) , maxPeakY(0) ,
				zeroPeakY(0) , minPeakY(0) {}

		// audio/peaks data
		Scene* scene ;
		unsigned int sceneN ;

		// drawing dimensions
		unsigned int xPadding ;
		unsigned int loopW ;
		unsigned int sceneW ;
		unsigned int sceneH ;
		unsigned int sceneX ;
		unsigned int sceneY ;
		unsigned int sceneL ;
		unsigned int sceneT ;
		unsigned int histogramT ;
		unsigned int histogramB ;
		unsigned int maxPeakY ;
		unsigned int zeroPeakY ;
		unsigned int minPeakY ;

		// drawing
		void drawScene(SDL_Surface* screen) ;

		// drawing helpers
		void setDims(SDL_Rect winRect , bool isFullScreen) ;
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
// DEBUG
void makeMainDbgText(char* dbg) ;
// DEBUG end
} ;


#endif // #ifndef _SCENE_SDL_H_
