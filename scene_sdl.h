
#ifndef _SCENE_SDL_H_
#define _SCENE_SDL_H_


#include "loopidity.h"
class Scene ;


// magnitudes
#define PEAK_RADIUS 50
#define LOOP_DIAMETER PEAK_RADIUS * 2
#define X_PADDING PEAK_RADIUS / 4
#define Y_PADDING PEAK_RADIUS / 3
#define BORDER_PAD 5
#define HISTOGRAM_H Y_PADDING
#define HISTOGRAM_T HISTOGRAM_H - 2
#define HISTOGRAM_0 HistogramT + (HistogramH / 2)
#define HISTOGRAM_B HistogramT + HistogramH
#define HISTOGRAM_FRAME_T HistogramT - 1
#define HISTOGRAM_FRAME_B HistogramB + 1
#define LOOP_W XPadding + LoopD
#define LOOP_T YPadding + HistogramH + BORDER_PAD
#define LOOP_0 LoopT + PEAK_RADIUS
#define LOOP_B LoopT + LoopD
#define LOOP_FRAME_T HistFrameT - BORDER_PAD
#define LOOP_FRAME_B LoopB + BORDER_PAD + 1
#define SCENE_W WIN_W - (X_PADDING * 2)
#define SCENE_H (Y_PADDING * 3) + HISTOGRAM_H + LoopD
#define SCENE_L X_PADDING
#define SCENE_R SceneL + SceneW - 1
#define PIE_SLICE_DEGREES 360.0 / (float)N_LOOP_PEAKS
#define PIE_12_OCLOCK -90

// colors
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
		static const Sint16 Histogram0 ;
		static const Sint16 HistogramB ;
		static const Sint16 HistFrameT ;
		static const Sint16 HistFrameB ;
		static const Uint16 LoopD ;
		static const Uint16 LoopW ;
		static const Uint16 LoopT ;
		static const Uint16 Loop0 ;
		static const Uint16 LoopB ;
		static const Sint16 LoopFrameT ;
		static const Sint16 LoopFrameB ;
		static const Uint16 SceneW ;
		static const Uint16 SceneH ;
		static const Uint16 SceneL ;
		static const Uint16 SceneR ;
		static const float PieSliceDegrees ;
		const Uint16 sceneY ;
		SDL_Rect sceneRect ; // will not change but cannot be const

		// drawing backbuffers
		SDL_Surface* activeSceneSurface ;
		SDL_Surface* inactiveSceneSurface ;

		// audio data
		Scene* scene ;

		// loop images
		vector<LoopImg*> loopImgs ;

		// drawScene variables
		Uint16 currentPeakN , hiScenePeak , loopN , histN ;
		Sint16 loopX , frameL , frameR , peakH , x , t , b , r ;
		Uint32 peakColor , loopFrameColor ;
		SDL_Rect maskRect , gradientRect , rotRect ;
		LoopImg* loopImg ;
		SDL_Surface* rotImg ;

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
