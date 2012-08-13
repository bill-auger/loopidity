
#ifndef _SCENE_SDL_H_
#define _SCENE_SDL_H_


#include "loopidity.h"
class Scene ;


// magnitudes
#define PEAK_RADIUS 50
#define LOOP_DIAMETER (PEAK_RADIUS * 2)
#define X_PADDING (PEAK_RADIUS / 4)
#define Y_PADDING (PEAK_RADIUS / 3)
#define BORDER_PAD 5
#define HISTOGRAM_H Y_PADDING
#define HISTOGRAM_T (HISTOGRAM_H - 2)
#define HISTOGRAM_0 (HISTOGRAM_T + (HISTOGRAM_H / 2))
#define HISTOGRAM_B (HISTOGRAM_T + HISTOGRAM_H)
#define HISTOGRAM_FRAME_T (HISTOGRAM_T - 1)
#define HISTOGRAM_FRAME_B (HISTOGRAM_B + 1)
#define LOOP_W (X_PADDING + LOOP_DIAMETER)
#define LOOP_T (Y_PADDING + HISTOGRAM_H + BORDER_PAD)
#define LOOP_0 (LOOP_T + PEAK_RADIUS)
#define LOOP_B (LOOP_T + LOOP_DIAMETER)
#define LOOP_FRAME_T (HISTOGRAM_FRAME_T - BORDER_PAD)
#define LOOP_FRAME_B (LOOP_B + BORDER_PAD + 1)
#define SCENE_W (WIN_W - (X_PADDING * 2))
#define SCENE_H ((Y_PADDING * 3) + HISTOGRAM_H + LOOP_DIAMETER)
#define SCENE_L X_PADDING
#define SCENE_R (SCENE_L + SCENE_W - 1)
#define SCENE_T (HEADER_H + (SCENE_H * sceneN))
#define SCENE_B (SCENE_T + SCENE_H)
#define SCENE_FRAME_L (SCENE_L - BORDER_PAD)
#define SCENE_FRAME_R (SCENE_R + BORDER_PAD)
#define SCENE_FRAME_T (SCENE_T + HISTOGRAM_FRAME_T - BORDER_PAD)
#define SCENE_FRAME_B (SCENE_T + LOOP_FRAME_B + BORDER_PAD)
#define PIE_SLICE_DEGREES (360.0 / (float)N_LOOP_PEAKS)
#define PIE_12_OCLOCK -90

// colors
#define SCENE_PEAK_MAX_COLOR 0xff0000ff
#define SCENE_PEAK_ZERO_COLOR 0x808080ff
#define STATUS_RECORDING_COLOR 0xff0000ff
#define STATUS_PENDING_COLOR 0xffff00ff
#define STATUS_PLAYING_COLOR 0x00ff00ff
#define STATUS_IDLE_COLOR 0x808080ff
#define PEAK_CURRENT_COLOR 0xffff00ff
#define HISTOGRAM_PEAK_COLOR 0x008000ff
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
		static const Uint16 SceneFrameL ;
		static const Uint16 SceneFrameR ;
		static const float PieSliceDegrees ;
		const Uint16 sceneT ;
		const Uint16 sceneFrameT ;
		const Uint16 sceneFrameB ;
		SDL_Rect sceneRect ; // will not change but cannot be const

		// drawing backbuffers
		SDL_Surface* activeSceneSurface ;
		SDL_Surface* inactiveSceneSurface ;

		// audio data
		Scene* scene ;

		// loop images
		vector<LoopImg*> loopImgs ;

		// drawScene() instance variables
		Uint32 histFrameColor ;
		Uint32 loopFrameColor ;
		Uint32 sceneFrameColor ;

		// drawScene() 'local' variables
		Uint16 currentPeakN , hiScenePeak , loopN , histN ;
		Sint16 loopL , histFrameL , histFrameR , peakH , l , t , b , r ;
		Uint32 peakColor ;
		SDL_Rect maskRect , gradientRect , rotRect ;
		LoopImg* loopImg ;
		SDL_Surface* rotImg ;

		// drawing
		void drawScene(SDL_Surface* screen , unsigned int currentPeakN , Uint16 loopProgress) ;
		void drawLoop(Uint16 loopN , SAMPLE* peaks) ;
		void drawRecordingLoop(SDL_Surface* surface , Uint16 loopProgress) ;
		void drawSceneStateIndicator(SDL_Surface* surface) ;
} ;


#endif // #ifndef _SCENE_SDL_H_
