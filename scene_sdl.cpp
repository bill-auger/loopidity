
#include "scene_sdl.h"


/* SceneSdl class private constants */

const Uint16 SceneSdl::XPadding(LOOP_PEAK_R / 4) ;
const Uint16 SceneSdl::YPadding(LOOP_PEAK_R / 3) ;
const Uint16 SceneSdl::HistogramH(YPadding) ;
const Sint16 SceneSdl::HistogramT(HistogramH - 2) ;
const Sint16 SceneSdl::HistogramY(HistogramT + (HistogramH / 2)) ;
const Sint16 SceneSdl::HistogramB(HistogramT + HistogramH) ;
const Sint16 SceneSdl::HistFrameT = HistogramT - 1 ;
const Sint16 SceneSdl::HistFrameB = HistogramB + 1 ;
const Uint16 SceneSdl::LoopD(LOOP_PEAK_R * 2) ;
const Uint16 SceneSdl::LoopW(XPadding + LoopD) ;
const Uint16 SceneSdl::LoopT(YPadding + HistogramH + BORDER_PAD) ;
const Uint16 SceneSdl::LoopY(LoopT + LOOP_PEAK_R) ;
const Uint16 SceneSdl::LoopB(LoopT + LoopD) ;
const Sint16 SceneSdl::LoopFrameT = HistFrameT - BORDER_PAD ;
const Sint16 SceneSdl::LoopFrameB = LoopB + BORDER_PAD + 1 ;
const Uint16 SceneSdl::SceneH((YPadding * 3) + HistogramH + LoopD) ;
const Uint16 SceneSdl::SceneL(XPadding) ;
const float SceneSdl::PieSliceDegrees = PIE_SLICE_DEGREES ;


/* LoopImg class private functions */

LoopImg::LoopImg(SDL_Surface* loopImg , Uint16 loopX , Uint16 loopY)
	{ loopSurface = loopImg ; loopRect = {loopX , loopY , 0 , 0} ; }

LoopImg::~LoopImg() { SDL_FreeSurface(loopSurface) ; }


/* SceneSdl class private functions */

SceneSdl::SceneSdl(Scene* scene) :
		// constants
		sceneY(HEADER_H + (SceneH * scene->sceneN)) ,
		sceneW(LoopiditySdl::WinRect.w - (XPadding * 2)) ,
		sceneR(SceneL + sceneW - 1)
{
	// variables
	sceneRect = {0 , sceneY , LoopiditySdl::WinRect.w , SceneH} ;
	activeSceneSurface = SDL_CreateRGBSurface(SDL_HWSURFACE , sceneRect.w , SceneH , PIXEL_DEPTH , 0 , 0 , 0 , 0) ;
	inactiveSceneSurface = SDL_CreateRGBSurface(SDL_HWSURFACE , sceneRect.w , SceneH , PIXEL_DEPTH , 0 , 0 , 0 , 0) ;
	SDL_SetAlpha(inactiveSceneSurface , SDL_SRCALPHA | SDL_RLEACCEL , 128) ;
	drawScene(inactiveSceneSurface , scene) ;
}

void SceneSdl::drawScene(SDL_Surface* surface , Scene* scene)
{
	Uint16 currentPeakN = scene->getCurrentPeakN() , hiScenePeak , loopN , histN ;
	Sint16 loopX , frameL , frameR , peakH , x , t , b , r ;
	Uint32 peakColor , loopFrameColor ;
	SDL_Rect maskRect , gradientRect , rotRect ;
	LoopImg* loopImg ;
	SDL_Surface* rotImg ;

	SDL_FillRect(surface , 0 , LoopiditySdl::WinBgColor) ;

#if DRAW_SCENE_PEAK_BAR
	// draw peak gradient mask
	hiScenePeak = (Uint16)(scene->hiScenePeaks[currentPeakN] * (float)LOOP_PEAK_R) ;
	maskRect.x = 0 ; maskRect.y = LOOP_PEAK_R - hiScenePeak + 1;
	maskRect.w = sceneW ; maskRect.h = (hiScenePeak * 2) + 1 ;
	gradientRect.x = SceneL ; gradientRect.y = LoopT + maskRect.y ;
	SDL_BlitSurface(LoopiditySdl::SceneBgGradient , &maskRect , surface , &gradientRect) ;

	// draw scene max , zero , and , min peak lines
	hlineColor(surface , SceneL , sceneR , LoopT , SCENE_PEAK_MAX_COLOR) ;
	hlineColor(surface , SceneL , sceneR , LoopY , SCENE_PEAK_ZERO_COLOR) ;
	hlineColor(surface , SceneL , sceneR , LoopB , SCENE_PEAK_MAX_COLOR) ;
#endif // #if DRAW_SCENE_PEAK_BAR

// TODO: formalize this? (with approxCurrentPeakN)
// decoupling N_TRANSIENT_PEAKS from N_LOOP_PEAKS made the histogram draw poorly
Uint16 nLoopPeaksPerHistogramSample = N_LOOP_PEAKS / LoopD ;
Uint16 loopProgress = (((float)currentPeakN / (float)N_LOOP_PEAKS) * 100) ; //o/c this only works cause LoopD == 100

	// draw loops
	for (loopN = 0 ; loopN < scene->loops.size() ; ++loopN)
	{
// TODO: Loop class is private - draw histogram along with loop upon creation
SAMPLE* peaks = scene->loops[loopN]->peaks ;
		loopX = SceneL + (LoopW * loopN) ;

#if DRAW_HISTOGRAMS
// TODO: perhaps draw histogram mixing all loops in this sceneN
// perhaps better to make this a cached image (it only changes omce per loop)
// for histogram we need LOOP_D samples of N_LOOP_PEAKS
// for peak bar and loop peaks we need N_LOOP_PEAKS/guiInterval samples of N_LOOP_PEAKS
		// draw histogram border
		frameL = loopX - 1 , frameR = loopX + LoopD + 1 ;
		roundedRectangleColor(surface , frameL , HistFrameT , frameR , HistFrameB , 5 , HIST_FRAME_INACTIVE_COLOR) ;
		// draw histogram
		for (histN = 0 ; histN < LoopD ; ++histN)
		{
// TODO: formalize this? (with nLoopPeaksPerHistogramSample and loopProgress)
Uint16 approxCurrentPeakN = histN * nLoopPeaksPerHistogramSample ;
			x = loopX + histN ; peakH = HistogramH * peaks[approxCurrentPeakN] ;
			if (histN == loopProgress) { t = HistogramT ; b = HistogramB ; peakColor = HISTOGRAM_PEAK_CURRENT_INACTIVE_COLOR ; }
			else { t = HistogramY - (peakH / 2) ; b = t + peakH ; peakColor = HISTOGRAM_PEAK_INACTIVE_COLOR ; }
			if (loopProgress) vlineColor(surface , x , t , b , peakColor) ;
		}
#endif // #if DRAW_HISTOGRAMS

#if DRAW_PEAK_RINGS
		// draw the current sample value and loudest sample value in this loop as rings
		x = loopX + LOOP_PEAK_R ; r = scene->hiLoopPeaks[loopN] * LOOP_PEAK_R ;
		circleColor(surface , x , LoopY , r, LOOP_PEAK_MAX_COLOR) ;
// TODO: Loop class is private - pass in peaks[currentPeakN]
r = peaks[currentPeakN] * LOOP_PEAK_R ;
		circleColor(surface , x , LoopY , r, LOOP_PEAK_CURRENT_COLOR) ;
#endif // #if DRAW_PEAK_RINGS

#if DRAW_LOOPS
		// draw loop peaks
		loopImg = loopImgs[loopN] ; // TODO: we could cache these rotImgs if need be
		rotImg = rotozoomSurface(loopImg->loopSurface , currentPeakN * PIE_SLICE_DEGREES , 1.0 , 0) ;
		rotRect = {x - (rotImg->w - loopImg->loopRect.w) / 2 , LoopY - (rotImg->h - loopImg->loopRect.h) / 2 , 0 , 0} ;
		SDL_BlitSurface(rotImg , 0 , surface , &rotRect) ; SDL_FreeSurface(rotImg) ;
#endif // #if DRAW_LOOPS
	} // for (loopN)

	if (loopN == N_LOOPS || surface == inactiveSceneSurface) return ;

	// simplified histogram and peak rings for currently recording loop
	loopX = SceneL + (LoopW * loopN) ; frameL = loopX - 1 , frameR = loopX + LoopD + 1 ;
	roundedRectangleColor(surface , frameL , HistFrameT , frameR , HistFrameB , 5 , HIST_FRAME_ACTIVE_COLOR) ;
	loopFrameColor = (Loopidity::GetIsSaveLoop())? SCENE_PEAK_MAX_COLOR : HIST_FRAME_ACTIVE_COLOR ;
	roundedRectangleColor(surface , frameL - BORDER_PAD , LoopFrameT , frameR + BORDER_PAD , LoopFrameB , 5 , loopFrameColor) ;
	x = loopX + loopProgress ; t = HistogramT ; b = HistogramB ;
	if (loopProgress) vlineColor(surface , x , t , b , HISTOGRAM_PEAK_CURRENT_ACTIVE_COLOR) ;
	x = loopX + LOOP_PEAK_R ; r = *Loopidity::GetTransientPeakIn() * LOOP_PEAK_R ;
	circleColor(surface , x , LoopY , r, LOOP_PEAK_CURRENT_COLOR) ;
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
		Uint16 radius = (Uint16)(peaks[peakN] * (float)LOOP_PEAK_R) ;
		Sint16 begin = PIE_12_OCLOCK + (PieSliceDegrees * peakN) ;
		Sint16 end = begin + PIE_SLICE_DEGREES ;
		filledPieColor(maskSurface , LOOP_PEAK_R , LOOP_PEAK_R , radius , begin , end , LOOP_IMG_MASK_COLOR) ;
	}

	// mask loop gradient image
	Uint32 srcPixel , maskPixel ; Uint8* destPixel ; Uint16 xxxx ;
	SDL_LockSurface(loopSurface) ; SDL_LockSurface(maskSurface) ;
	for (Uint16 y = 0 ; y < LoopD ; ++y) for (Uint16 x = 0 ; x < LoopD ; ++x)
	{
		xxxx = x * LOOP_IMG_BYTES_PER_PIXEL ;
		destPixel = (Uint8*)loopSurface->pixels + (y * loopSurface->pitch) + xxxx ;
		if (x == LOOP_PEAK_R && y < LOOP_PEAK_R) srcPixel = LOOP_PEAK_CURRENT_COLOR ;
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
