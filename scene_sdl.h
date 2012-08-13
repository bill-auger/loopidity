
#ifndef _SCENE_SDL_H_
#define _SCENE_SDL_H_


#include "loopidity.h"
class Scene ;


// loop magnitudes
#define LOOP_PEAK_R 50
// lets fix this to N_LOOP_PEAKS / 2 for sdl histogramWidth (loopD)
#define PIE_SLICE_DEGREES 360.0 / (float)N_LOOP_PEAKS
#define PIE_12_OCLOCK -90
#define BORDER_PAD 5

// colors
// TODO: perhaps it maybe more efficient to SDL_MapRGB(LoopiditySdl::Screen->format , 0 , 0 , 0)
#define SCENE_PEAK_MAX_COLOR 0xff0000ff
#define SCENE_PEAK_ZERO_COLOR 0x808080ff
#define HIST_FRAME_ACTIVE_COLOR 0x00ff00ff
#define HIST_FRAME_INACTIVE_COLOR 0x008000ff
#define HISTOGRAM_PEAK_CURRENT_ACTIVE_COLOR 0xffff00ff
#define HISTOGRAM_PEAK_CURRENT_INACTIVE_COLOR 0x808000ff
#define HISTOGRAM_PEAK_ACTIVE_COLOR 0x008000ff
#define HISTOGRAM_PEAK_INACTIVE_COLOR 0x004000ff
#define LOOP_PEAK_CURRENT_COLOR 0xffff00ff
#define LOOP_PEAK_MAX_COLOR 0x800000ff
#define LOOP_IMG_MASK_COLOR 0xffffffff


using namespace std ;


class LoopImg
{
	friend class SceneSdl ;

	private:

		LoopImg(SDL_Surface* loopImg , Uint16 loopX , Uint16 loopY) ;
		~LoopImg() ;

		SDL_Surface* loopSurface ;
		SDL_Rect loopRect ;
} ;


class SceneSdl
{
	friend class Scene ;
	friend class LoopiditySdl ;

	private:

		SceneSdl(Uint16 sceneN) ;

		// drawing constants
		static const Uint16 XPadding ;
		static const Uint16 YPadding ;
		static const Uint16 HistogramH ;
		static const Sint16 HistogramT ;
		static const Sint16 HistogramY ;
		static const Sint16 HistogramB ;
		static const Sint16 HistFrameT ;
		static const Sint16 HistFrameB ;
		static const Uint16 LoopD ;
		static const Uint16 LoopW ;
		static const Uint16 LoopT ;
		static const Uint16 LoopY ;
		static const Uint16 LoopB ;
		static const Sint16 LoopFrameT ;
		static const Sint16 LoopFrameB ;
		static const Uint16 SceneH ;
		static const Uint16 SceneL ;
		static const float PieSliceDegrees ;
		const Uint16 sceneY ;
		const Uint16 sceneW ;
		const Uint16 sceneR ;
		SDL_Rect sceneRect ; // will not change but cannot be const

		// drawing backbuffers
		SDL_Surface* activeSceneSurface ;
		SDL_Surface* inactiveSceneSurface ;

		// audio data
		Scene* scene ;

		// loop images
		vector<LoopImg*> loopImgs ;

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
		void drawLoop(Uint16 loopN , SAMPLE* peaks) ;
} ;


#endif // #ifndef _SCENE_SDL_H_
