
#ifndef _SCENE_SDL_H_
#define _SCENE_SDL_H_


// magnitudes
#define PEAK_RADIUS             SDL_PEAK_RADIUS
#define LOOP_DIAMETER           SDL_LOOP_DIAMETER
#define X_PADDING               10
#define Y_PADDING               18
#define BORDER_PAD              5
#define HISTOGRAMS_H            17 // should be odd
#define HISTOGRAMS_T            (Y_PADDING - 2)                   // drawRecordingLoop() (offset from SCENE_T)
#define HISTOGRAMS_B            (HISTOGRAMS_T + HISTOGRAMS_H - 1) // ""
#define HISTOGRAM_FRAMES_T      (HISTOGRAMS_T - 1)                // ""
#define HISTOGRAM_FRAMES_B      (HISTOGRAMS_B + 1)                // ""
#define HISTOGRAM_IMG_W         103 // (N_PEAKS_COURSE + 2)       // drawHistogram() (offset from HISTOGRAMS_T)
#define HISTOGRAM_IMG_H         19  // (HISTOGRAMS_H + 2)         // ""
#define HISTOGRAM_FRAME_R       (N_PEAKS_COURSE + 1)              // ""
#define HISTOGRAM_FRAME_B       (HISTOGRAMS_H + 1)                // ""
#define HISTOGRAM_PEAK_H        (HISTOGRAMS_H / 2)                // ""
#define HISTOGRAM_0             HISTOGRAM_PEAK_H + 1              // ""
#define LOOP_0                  PEAK_RADIUS
#define LOOP_W                  (X_PADDING + LOOP_DIAMETER)
#define LOOPS_L                 16
#define LOOPS_T                 (HISTOGRAM_FRAMES_B + BORDER_PAD + 1)
#define LOOPS_0                 (LOOPS_T + PEAK_RADIUS + 1)
#define LOOPS_B                 (LOOPS_T + LOOP_DIAMETER + 1)
#define LOOP_FRAMES_T           (HISTOGRAM_FRAMES_T - BORDER_PAD - 1)
#define LOOP_FRAMES_B           (LOOPS_B + BORDER_PAD + 1)
#define SCENE_W                 1000 // SCOPE_IMG is 1000x101
#define SCENE_H                 ((Y_PADDING * 3) + HISTOGRAMS_H + LOOP_DIAMETER)
#define SCENE_L                 (LOOPS_L - BORDER_PAD - 1)
#define SCENE_R                 (SCENE_L + 999) // SCOPE_IMG is 1000x101 , LOOP_DIAMETER is 101 when PEAK_RADIUS is 50
#define SCENE_T                 (HEADER_H + (BORDER_PAD * 2) + (SCENE_H * sceneN))
#define SCENE_B                 (SCENE_T + SCENE_H)
#define SCENE_FRAME_L           (SCENE_L - BORDER_PAD - 1)
#define SCENE_FRAME_R           (SCENE_R + BORDER_PAD + 1)
#define SCENE_FRAME_T           (SCENE_T + LOOP_FRAMES_T - BORDER_PAD - 1)
#define SCENE_FRAME_B           (SCENE_T + LOOP_FRAMES_B + BORDER_PAD + 1)
#define SCOPE_MASK_RECT         { 0 , 0 , SCENE_W , 0 }
#define SCOPE_GRADIENT_RECT     { SCENE_L , 0 , 0 , 0 }
#define HISTOGRAM_RECT          { 0 , HISTOGRAM_FRAMES_T , 0 , 0 }
#define HISTOGRAM_MASK_RECT     { 0 , 0 , 1 , 0 }
#define HISTOGRAM_GRADIENT_RECT { 0 , 0 , 0 , 0 }
#define ROT_LOOP_IMG_RECT       { 0 , 0 , 0 , 0 }
#define PIE_SLICE_DEGREES       (360.0 / (float)N_PEAKS_FINE)
#define PIE_12_OCLOCK           -90

// colors
#define SCOPE_PEAK_MAX_COLOR  0x800000ff
#define SCOPE_PEAK_ZERO_COLOR 0x808080ff
#define STATE_RECORDING_COLOR 0xff0000ff
#define STATE_PENDING_COLOR   0xffff00ff
#define STATE_PLAYING_COLOR   0x00ff00ff
#define STATE_IDLE_COLOR      0x808080ff
#define PEAK_CURRENT_COLOR    0xffff00ff
#define HISTOGRAM_PEAK_COLOR  0x008000ff
#define LOOP_PEAK_MAX_COLOR   0x800000ff
#define LOOP_IMG_MASK_COLOR   0xffffffff

// loop states
#define STATE_LOOP_PLAYING 1
#define STATE_LOOP_PENDING 2
#define STATE_LOOP_MUTED   3

#include "loopidity.h"
class Loop ;
class Scene ;


using namespace std ;


class LoopSdl
{
	friend class SceneSdl ;

	private:

		LoopSdl(SDL_Surface* playingImg , SDL_Surface* mutedImg , Sint16 x , Sint16 y) ;
		~LoopSdl() ;

		// drawing backbuffers
		SDL_Surface* playingSurface ;
		SDL_Surface* mutedSurface ;
		SDL_Surface* currentSurface ;

		// drawing coordinates
		Sint16   loopL ;
		Sint16   loopC ;
		SDL_Rect rect ;

		// loop state
		void setStatus(Uint16 loopStatus) ;
} ;


class SceneSdl
{
  friend class Loopidity ;
  friend class LoopiditySdl ;
  friend class Trace ;

  private:

    SceneSdl(Scene* aScene , Uint16 sceneN , Uint32 frameColor) ;

    // drawing constants
    // TODO: some of these Uint16 have caused conversion warnings (perhaps all should be Sint16)
    static Sint16       HistogramsT ;  // drawRecordingLoop()
    static Sint16       HistogramsB ;  // drawRecordingLoop()
    static const Sint16 HistFramesT ;  // drawRecordingLoop()
    static const Sint16 HistFramesB ;  // drawRecordingLoop()
    static const Uint16 HistSurfaceW ; // drawHistogram()
    static const Uint16 HistSurfaceH ; // drawHistogram()
    static const Sint16 HistFrameR ;   // drawHistogram()
    static const Sint16 HistFrameB ;   // drawHistogram()
    static const float  HistPeakH ;    // drawHistogram()
    static const Sint16 Histogram0 ;   // drawHistogram()
    static const Uint16 LoopD ;
    static const Sint16 LoopW ;
    static const Uint16 Loop0 ;
    static const Sint16 LoopsL ;
    static const Sint16 LoopsT ;
    static const Uint16 Loops0 ;
    static const Uint16 LoopsB ;
    static const Sint16 LoopFrameT ;
    static const Sint16 LoopFrameB ;
    static const Uint16 SceneH ;
    static const Uint16 SceneL ;
    static const Uint16 SceneR ;
    static const Uint16 SceneFrameL ;
    static const Uint16 SceneFrameR ;
    static const float  PieSliceDegrees ;
    static const Uint8  BytesPerPixel ;
    const Sint16        sceneT ;
    const Uint16        sceneFrameT ;
    const Uint16        sceneFrameB ;
    const SDL_Rect      sceneRect ; // will not change but cannot be const

    // model
    Scene* scene ;

    // loop image caches
    list<LoopSdl*> histogramImgs ;
    list<LoopSdl*> loopImgs ;

    // drawScene() instance variables
    Uint32 loopFrameColor ;
    Uint32 sceneFrameColor ;

    // drawScene() , drawHistogram() , and drawRecordingLoop() 'temp' variables
    Uint16       currentPeakN ;
    Uint16       hiScenePeak ;
    Uint16       loopN ;
    Uint16       histPeakN ;
    Uint16       peakH ;
    Sint16       loopL ;
    Sint16       loopC ;
    Sint16       histFrameL ;
    Sint16       histFrameR ;
    Sint16       ringR ;
    Sint16       loopFrameL ;
    Sint16       loopFrameR ;
    SDL_Rect     scopeMaskRect ;
    SDL_Rect     scopeGradRect ;
    SDL_Rect     histogramRect ;
    SDL_Rect     rotRect ;
    SDL_Rect     histMaskRect ;
    SDL_Rect     histGradRect ;
    LoopSdl*     histogramImg ;
    LoopSdl*     loopImg ;
    Loop*        loop ;
    SDL_Surface* rotImg ;

    // drawing backbuffers
    SDL_Surface* activeSceneSurface ;
    SDL_Surface* inactiveSceneSurface ;


    // getters/setters
    LoopSdl* getLoop( list<LoopSdl*>* imgs , unsigned int loopN) ;
    void updateStatus(void) ;
    void startRolling(void) ;
    void reset(       void) ;
    void cleanup(     void) ;

    // drawing
    void     drawScene(              SDL_Surface* screen , unsigned int currentPeakN ,
                                     Uint16 loopProgress) ;
    LoopSdl* drawHistogram(          Loop* loop) ;
    LoopSdl* drawLoop(               Loop* loop , Uint16 loopN) ;
    void     drawRecordingLoop(      SDL_Surface* surface , Uint16 loopProgress) ;
    void     drawSceneStateIndicator(SDL_Surface* surface) ;

    // helpers
    SDL_Surface*  createHwSurface(   Sint16 w , Sint16 h) ;
    SDL_Surface*  createSwSurface(   Sint16 w , Sint16 h) ;
    void          addLoop(           Loop* newLoop , Uint16 nLoops) ;
    void          deleteLoop(        unsigned int loopN) ;
    static void   PixelRgb2Greyscale(SDL_PixelFormat* fmt , Uint32* pixel) ;
    static Sint16 GetLoopL(          Uint16 loopN) ;
} ;


#endif // #ifndef _SCENE_SDL_H_
