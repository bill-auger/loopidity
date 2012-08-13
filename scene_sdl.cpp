
#include "scene_sdl.h"


/* SceneSdl class private constants */

const Uint16 SceneSdl::XPadding = X_PADDING ;
const Uint16 SceneSdl::YPadding = Y_PADDING ;
const Uint16 SceneSdl::HistogramH = HISTOGRAM_H ;
const Sint16 SceneSdl::HistogramT = HISTOGRAM_T ;
const Sint16 SceneSdl::Histogram0 = HISTOGRAM_0 ;
const Sint16 SceneSdl::HistogramB = HISTOGRAM_B ;
const Sint16 SceneSdl::HistFrameT = HISTOGRAM_FRAME_T ;
const Sint16 SceneSdl::HistFrameB = HISTOGRAM_FRAME_B ;
const Uint16 SceneSdl::LoopD(LOOP_DIAMETER) ;
const Uint16 SceneSdl::LoopW = LOOP_W ;
const Uint16 SceneSdl::LoopT = LOOP_T ;
const Uint16 SceneSdl::Loop0 = LOOP_0 ;
const Uint16 SceneSdl::LoopB = LOOP_B ;
const Sint16 SceneSdl::LoopFrameT = LOOP_FRAME_T ;
const Sint16 SceneSdl::LoopFrameB = LOOP_FRAME_B ;
const Uint16 SceneSdl::SceneW = SCENE_W ;
const Uint16 SceneSdl::SceneH = SCENE_H ;
const Uint16 SceneSdl::SceneL = SCENE_L ;
const Uint16 SceneSdl::SceneR = SCENE_R ;
const Uint16 SceneSdl::SceneFrameL = SCENE_FRAME_L ;
const Uint16 SceneSdl::SceneFrameR = SCENE_FRAME_R ;
const float SceneSdl::PieSliceDegrees = PIE_SLICE_DEGREES ;


/* LoopImg class private functions */

LoopImg::LoopImg(SDL_Surface* loopImg , Uint16 loopX , Uint16 loopY)
	{ loopSurface = loopImg ; loopRect = {loopX , loopY , 0 , 0} ; }

LoopImg::~LoopImg() { SDL_FreeSurface(loopSurface) ; }


/* SceneSdl class private functions */

SceneSdl::SceneSdl(Uint16 sceneN) :
		// constants
		sceneT(SCENE_T) , sceneFrameT(SCENE_FRAME_T) , sceneFrameB(SCENE_FRAME_B) ,
		// audio data
		scene(0) , // owner Scene instance set by Scene instance
		// drawScene() instance variables
		histFrameColor(STATUS_PLAYING_COLOR) ,
		loopFrameColor(STATUS_IDLE_COLOR) ,
		sceneFrameColor(STATUS_IDLE_COLOR) ,
		// drawScene() 'local' variables
		currentPeakN(0) , hiScenePeak(0) , loopN(0) , histN(0) ,
		loopL(0) , histFrameL(0) , histFrameR(0) , peakH(0) , l(0) , t(0) , b(0) , r(0) ,
		peakColor(0) ,
		maskRect({0 , 0 , SceneW , 0}) , gradientRect({SceneL , 0 , 0 , 0}) , rotRect({0 , 0 , 0 , 0}) ,
		loopImg(0) , rotImg(0)
{
	// variables
	sceneRect = {0 , sceneT , LoopiditySdl::WinRect.w , SceneH} ;
	activeSceneSurface = SDL_CreateRGBSurface(SDL_HWSURFACE , sceneRect.w , SceneH , PIXEL_DEPTH , 0 , 0 , 0 , 0) ;
	inactiveSceneSurface = SDL_CreateRGBSurface(SDL_HWSURFACE , sceneRect.w , SceneH , PIXEL_DEPTH , 0 , 0 , 0 , 0) ;
	SDL_SetAlpha(inactiveSceneSurface , SDL_SRCALPHA | SDL_RLEACCEL , 128) ;
}

void SceneSdl::drawScene(SDL_Surface* surface , unsigned int currentPeakN , Uint16 loopProgress)
{
	SDL_FillRect(surface , 0 , LoopiditySdl::WinBgColor) ;

#if DRAW_SCENE_PEAK_BAR
	// draw peak gradient mask
	hiScenePeak = (Uint16)(scene->hiScenePeaks[currentPeakN] * (float)PEAK_RADIUS) ;
	maskRect.y = PEAK_RADIUS - hiScenePeak + 1 ; maskRect.h = (hiScenePeak * 2) + 1 ;
	gradientRect.y = LoopT + maskRect.y ;
	SDL_BlitSurface(LoopiditySdl::SceneBgGradient , &maskRect , surface , &gradientRect) ;

	// draw scene max , zero , and , min peak lines
	hlineColor(surface , SceneL , SceneR , LoopT , SCENE_PEAK_MAX_COLOR) ;
	hlineColor(surface , SceneL , SceneR , Loop0 , SCENE_PEAK_ZERO_COLOR) ;
	hlineColor(surface , SceneL , SceneR , LoopB , SCENE_PEAK_MAX_COLOR) ;
#endif // #if DRAW_SCENE_PEAK_BAR

// TODO: formalize this? (with approxCurrentPeakN)
Uint16 nLoopPeaksPerHistogramSample = N_LOOP_PEAKS / LoopD ;

	// draw loops
	for (loopN = 0 ; loopN < scene->loops.size() ; ++loopN)
	{
// TODO: Loop class is private - draw histogram along with loop upon creation
SAMPLE* peaks = scene->loops[loopN]->peaks ;
		loopL = SceneL + (LoopW * loopN) ;

#if DRAW_HISTOGRAMS
// TODO: perhaps draw histogram mixing all loops in this sceneN
// perhaps better to make this a cached image (it only changes omce per loop)
// for histogram we need LOOP_D samples of N_LOOP_PEAKS
// for peak bar and loop peaks we need N_LOOP_PEAKS/guiInterval samples of N_LOOP_PEAKS
		// draw histogram border
		histFrameL = loopL - 1 , histFrameR = loopL + LoopD + 1 ;
		roundedRectangleColor(surface , histFrameL , HistFrameT , histFrameR , HistFrameB , 5 , histFrameColor) ;
		// draw histogram
		for (histN = 0 ; histN < LoopD ; ++histN)
		{
// TODO: formalize this? (with nLoopPeaksPerHistogramSample and loopProgress)
Uint16 approxPeakN = histN * nLoopPeaksPerHistogramSample ;
			l = loopL + histN ; peakH = HistogramH * peaks[approxPeakN] ;
			if (histN && histN == loopProgress) { t = HistogramT ; b = HistogramB ; peakColor = PEAK_CURRENT_COLOR ; }
			else { t = Histogram0 - (peakH / 2) ; b = t + peakH ; peakColor = HISTOGRAM_PEAK_COLOR ; }
			if (loopProgress) vlineColor(surface , l , t , b , peakColor) ;
		}
#endif // #if DRAW_HISTOGRAMS

#if DRAW_PEAK_RINGS
		// draw the current sample value and loudest sample value in this loop as rings
		l = loopL + PEAK_RADIUS ; r = (Sint16)(scene->hiLoopPeaks[loopN] * (float)PEAK_RADIUS) ;
		circleColor(surface , l , Loop0 , r, LOOP_PEAK_MAX_COLOR) ;
// TODO: Loop class is private - pass in peaks[currentPeakN]
r = (Sint16)(peaks[currentPeakN] * (float)PEAK_RADIUS) ;
		circleColor(surface , l , Loop0 , r, PEAK_CURRENT_COLOR) ;
#endif // #if DRAW_PEAK_RINGS

#if DRAW_LOOPS
		// draw loop peaks
		loopImg = loopImgs[loopN] ; // TODO: we could cache these rotImgs if need be
		rotImg = rotozoomSurface(loopImg->loopSurface , currentPeakN * PIE_SLICE_DEGREES , 1.0 , 0) ;
		rotRect = {l - (rotImg->w - loopImg->loopRect.w) / 2 , Loop0 - (rotImg->h - loopImg->loopRect.h) / 2 , 0 , 0} ;
		SDL_BlitSurface(rotImg , 0 , surface , &rotRect) ; SDL_FreeSurface(rotImg) ;
#endif // #if DRAW_LOOPS
	} // for (loopN)
}

void SceneSdl::drawLoop(Uint16 loopN , SAMPLE* peaks)
{
	SDL_Surface* loopBgGradient = LoopiditySdl::LoopBgGradient ;
	SDL_SetColorKey(loopBgGradient , SDL_SRCCOLORKEY , LOOP_IMG_MASK_COLOR) ;
	SDL_Surface* loopSurface = SDL_DisplayFormat(loopBgGradient) ;
	SDL_SetAlpha(loopSurface , SDL_SRCALPHA , 255) ;

	// draw loop mask
	SDL_Surface* maskSurface = SDL_CreateRGBSurface(SDL_SWSURFACE , LoopD , LoopD , PIXEL_DEPTH , 0 , 0 , 0 , 0) ;
	for (Uint16 peakN = 0 ; peakN < N_LOOP_PEAKS ; ++peakN)
	{
		Uint16 radius = (Uint16)(peaks[peakN] * (float)PEAK_RADIUS) ;
		Sint16 begin = PIE_12_OCLOCK + (PieSliceDegrees * peakN) ;
		Sint16 end = begin + PIE_SLICE_DEGREES ;
		filledPieColor(maskSurface , PEAK_RADIUS , PEAK_RADIUS , radius , begin , end , LOOP_IMG_MASK_COLOR) ;
	}

	// mask loop gradient image
	Uint32 srcPixel , maskPixel ; Uint8* destPixel ; Uint16 xxxx ;
	SDL_LockSurface(loopSurface) ; SDL_LockSurface(maskSurface) ;
	for (Uint16 y = 0 ; y < LoopD ; ++y) for (Uint16 x = 0 ; x < LoopD ; ++x)
	{
		xxxx = x * LOOP_IMG_BYTES_PER_PIXEL ;
		destPixel = (Uint8*)loopSurface->pixels + (y * loopSurface->pitch) + xxxx ;
		if (x == PEAK_RADIUS && y < PEAK_RADIUS) srcPixel = PEAK_CURRENT_COLOR ;
		else
		{
			maskPixel = *(Uint32*)((Uint8*)maskSurface->pixels + (y * maskSurface->pitch) + xxxx) ;
			srcPixel = (maskPixel)?
					*(Uint32*)((Uint8*)loopBgGradient->pixels + (y * loopBgGradient->pitch) + xxxx) :
					loopSurface->format->colorkey ;
		}
		*(Uint32*)destPixel = srcPixel ;
	}
	SDL_UnlockSurface(maskSurface) ; SDL_FreeSurface(maskSurface) ;
	SDL_UnlockSurface(loopSurface) ;

	loopImgs.push_back(new LoopImg(loopSurface , SceneL + (LoopW * loopN) , LoopT)) ;
}

void SceneSdl::drawRecordingLoop(SDL_Surface* surface , Uint16 loopProgress)
{
#if DRAW_RECORDING_LOOP
	// simplified histogram and transient peak ring for currently recording loop
	loopL = SceneL + (LoopW * scene->loops.size()) ; histFrameL = loopL - 1 , histFrameR = loopL + LoopD + 1 ;
	roundedRectangleColor(surface , histFrameL , HistFrameT , histFrameR , HistFrameB , 5 , loopFrameColor) ;
	roundedRectangleColor(surface , histFrameL - BORDER_PAD , LoopFrameT , histFrameR + BORDER_PAD , LoopFrameB , 5 , loopFrameColor) ;
	l = loopL + loopProgress ; t = HistogramT ; b = HistogramB ;
	if (loopProgress && scene->getIsRecording())
		vlineColor(surface , l , t , b , LOOP_PEAK_MAX_COLOR) ;
	l = loopL + PEAK_RADIUS ; r = *Loopidity::GetTransientPeakIn() * (float)PEAK_RADIUS ;
	circleColor(surface , l , Loop0 , r , PEAK_CURRENT_COLOR) ;
#endif
}

void SceneSdl::drawSceneStateIndicator(SDL_Surface* surface)
	{ roundedRectangleColor(surface , SceneFrameL , sceneFrameT , SceneFrameR , sceneFrameB , 5 , sceneFrameColor) ; }
