
#include "scene_sdl.h"


// DEBUG
void SceneSdl::makeMainDbgText(char* dbg)
{
	Uint16 peakN = ((float)scene->frameN / (float)scene->nFrames) * (float)N_LOOP_PEAKS ;
	sprintf(dbg , "Paint() SceneN=%d nLoops=%d PeakN=%d" , sceneN , scene->nLoops , peakN) ;
}
// DEBUG end


/* SceneSdl class private constants */

Uint16 const SceneSdl::XPadding(LOOP_PEAK_R / 4) ;
Uint16 const SceneSdl::YPadding(LOOP_PEAK_R / 3) ;
Uint16 const SceneSdl::HistogramH(YPadding) ;
Sint16 const SceneSdl::HistogramT(HistogramH - 2) ;
Sint16 const SceneSdl::HistogramY(HistogramT + (HistogramH / 2)) ;
Sint16 const SceneSdl::HistogramB(HistogramT + HistogramH) ;
Uint16 const SceneSdl::LoopD(LOOP_PEAK_R * 2) ;
Uint16 const SceneSdl::LoopW(XPadding + LoopD) ;
Uint16 const SceneSdl::LoopT(YPadding + HistogramH) ;
Uint16 const SceneSdl::LoopY(LoopT + LOOP_PEAK_R) ;
Uint16 const SceneSdl::LoopB(LoopT + LoopD - 1) ;
Uint16 const SceneSdl::SceneH((YPadding * 3) + HistogramH + LoopD) ;
Uint16 const SceneSdl::SceneL(XPadding) ;


/* SceneSdl class private functions */

SceneSdl::SceneSdl(Scene* scene , Uint16 sceneNum) :
		// constants
		scene(scene) , sceneN(sceneNum) , sceneY(HEADER_H + (SceneH * sceneN)) ,
		sceneW(LoopiditySdl::WinRect.w - (XPadding * 2)) , sceneR(SceneL + sceneW - 1)
{
	// variables
	sceneRect = {0 , sceneY , LoopiditySdl::WinRect.w , SceneH} ;
	activeSceneSurface = SDL_CreateRGBSurface(SDL_HWSURFACE , sceneW , SceneH , PIXEL_DEPTH , 0 , 0 , 0 , 0) ;
	inactiveSceneSurface = SDL_CreateRGBSurface(SDL_HWSURFACE , sceneW , SceneH , PIXEL_DEPTH , 0 , 0 , 0 , 0) ;
	SDL_SetAlpha(inactiveSceneSurface , SDL_SRCALPHA | SDL_RLEACCEL , 128) ;
	drawScene(inactiveSceneSurface) ;
}

void SceneSdl::drawScene(SDL_Surface* surface)
{
	Uint16 PeakN = ((float)scene->frameN / (float)scene->nFrames) * (float)N_LOOP_PEAKS ;
	Uint16 loopN , peakN ; float* peaks ; float currentPeak ;
	Sint16 loopX , peakH , x , t , b , r ; bool isCurrentLoop ;
	Uint32 borderColor , peakColorCurrent , peakColorOther , peakColor ;

	SDL_FillRect(surface , 0 , LoopiditySdl::WinBgColor) ;
#if DRAW_DEBUG_TEXT
char dbg[255] ; sprintf(dbg , "drawScene(%d) PeakN=%d" , sceneN , PeakN) ; LoopiditySdl::SetStatusL(dbg) ;
#endif

#if DRAW_PEAK_BARS // TODO: perhaps this should be the output scope
	// draw peak gradient mask
	Uint16 hiScenePeak = (Uint16)(scene->hiScenePeaks[PeakN] * (float)LOOP_PEAK_R) ;
	SDL_Rect maskRect ; maskRect.x = 0 ; maskRect.y = LOOP_PEAK_R - hiScenePeak ;
	maskRect.w = sceneW ; maskRect.h = (hiScenePeak * 2) ;
	SDL_Rect gradientRect ; gradientRect.x = SceneL ; gradientRect.y = LoopT + maskRect.y ;
	SDL_BlitSurface(LoopiditySdl::SceneBgGradient , &maskRect , surface , &gradientRect) ;

	// draw scene max , zero , and , min peak lines
	hlineColor(surface , SceneL , sceneR , LoopT , SCENE_PEAK_MAX_COLOR) ;
	hlineColor(surface , SceneL , sceneR , LoopY , SCENE_PEAK_ZERO_COLOR) ;
	hlineColor(surface , SceneL , sceneR , LoopB , SCENE_PEAK_MAX_COLOR) ;
#endif // #if DRAW_PEAK_BARS

	// draw loops
	for (loopN = 0 ; loopN < scene->nLoops ; ++loopN)
	{
		peaks = scene->loops[loopN]->peaks ; loopX = SceneL + (LoopW * loopN) ;

#if DRAW_HISTOGRAMS
// TODO: draw histogram mixing all loops in this sceneN
// perhaps better to make this a cached image (it only changes omce per loop)

		// determine appropriate colors
		isCurrentLoop = (loopN == scene->nLoops - 1) ;
		if (isCurrentLoop)
		{
			borderColor = HISTOGRAM_BORDER_ACTIVE_COLOR ;
			peakColorCurrent = HISTOGRAM_PEAK_CURRENT_ACTIVE_COLOR ;
			peakColorOther = HISTOGRAM_PEAK_ACTIVE_COLOR ;
		}
		else
		{
			borderColor = HISTOGRAM_BORDER_INACTIVE_COLOR ;
			peakColorCurrent = HISTOGRAM_PEAK_CURRENT_INACTIVE_COLOR ;
			peakColorOther = HISTOGRAM_PEAK_INACTIVE_COLOR ;
		}
		// draw histogram border
		roundedRectangleColor(surface , loopX - 1 , HistogramT - 1 , loopX + LoopD + 1 , HistogramB + 1 , 5 , borderColor) ;
		// draw histogram
		for (peakN = 0 ; peakN < N_LOOP_PEAKS ; ++peakN)
		{
			currentPeak = peaks[peakN] ; peakH = HistogramH * currentPeak ; x = loopX + peakN ;
			if (peakN == PeakN) { t = HistogramT ; b = HistogramB ; peakColor = peakColorCurrent ; }
			else { t = HistogramY - (peakH / 2) ; b = t + peakH ; peakColor = peakColorOther ; }
			vlineColor(surface , x , t , b , peakColor) ;
		}
#endif // #if DRAW_HISTOGRAMS

#if DRAW_LOOP_PEAKS
		// draw the current sample value and loudest sample value in this loop as rings
		x = loopX + LOOP_PEAK_R ;
		r = scene->hiLoopPeaks[loopN] * LOOP_PEAK_R ;
		circleColor(surface , x , LoopY , r, LOOP_PEAK_MAX_COLOR) ;
		r = peaks[PeakN] * LOOP_PEAK_R ;
		circleColor(surface , x , LoopY , r, LOOP_PEAK_CURRENT_COLOR) ;
#endif // #if DRAW_LOOP_PEAKS

#if DRAW_LOOPS
		// draw loop peaks
		d.DrawImage(loopX , maxPeakY , createLoopImgCached(loopN , PeakN)) ;
#endif // #if DRAW_LOOPS
	} // for (loopN)
}


/*
Image SceneUpp::createLoopPeaksMask(unsigned int loopN , unsigned int peakN)
{
	unsigned int* peaks = scene->loops[loopN]->peaks ;
	unsigned int rr = LOOP_IMG_RES , c = rr / 2 ; Point focus(c , c) , p , loopSeamPoint ;
	ImageDraw id(rr , rr) ; id.DrawRect(0 , 0 , rr , rr , Black) ;
	for (unsigned int sliceN = 0 ; sliceN < N_LOOP_PEAKS ; ++sliceN)
	{
		int start = PIE_12_OCLOCK + (PIE_SLICE_DEGREES * sliceN) , intX , intY ;
		double x , y , dxy ;  Vector<Point> vertices ; vertices << focus ;
		for (int i = 0 ; i <= PIE_SLICE_DEGREES ; ++i)
		{
			intX = fround(x = focus.x + peaks[peakN] * cos((start + i) * M_PI / 1800)) ;
			intY = fround(y = focus.y + peaks[peakN] * sin((start + i) * M_PI / 1800)) ;
			dxy = (x - intX) * (x - intX) + (y - intY) * (y - intY) ;
			if (dxy < 0.1 || i == 0 || i == PIE_SLICE_DEGREES) vertices << Point(intX , intY) ;
		}
		vertices << focus ;
		id.DrawPolygon(vertices , LOOP_IMG_MASK_COLOR , 0 , LOOP_IMG_MASK_COLOR , 0 , Null) ;

		if (!peakN) loopSeamPoint = vertices[1] ; peakN = (++peakN) % N_LOOP_PEAKS ;
	}

	id.DrawLine(focus , loopSeamPoint , 0 , LOOP_PEAK_CURRENT_COLOR) ;

	return id ;
}


Image SceneUpp::CreateLoopGradientImg()
{
	unsigned int rr = LOOP_IMG_RES , r = rr / 2 , r2 = r * r , y , x ;
	ImageBuffer ib(rr , rr) ; float gradScale = 1.1 ;
	for (y = 0 ; y < rr ; ++y) for (x = 0 ; x < rr ; ++x)
	{
		RGBA& px = ib[y][x] ;
		unsigned int q = ((x - r) * (x - r) + (y - r) * (y - r)) * 256 / r2 / gradScale ;
		unsigned int gradVal = q < 256 ? q : 0 ;
		px.r = gradVal ; px.g = 255 - gradVal ; px.b = 0 ; px.a = 255 ;
	}
	Premultiply(ib) ; return ib ;
}

struct LoopGradientImgMaker : ImageMaker
	{ virtual String Key() const ; virtual Image Make() const ; } ;

String LoopGradientImgMaker::Key() const { return String(LOOP_GRADIENT_IMAGE_KEY) ; }

Image LoopGradientImgMaker::Make() const { return SceneUpp::CreateLoopGradientImg() ; }

Image SceneUpp::CreateLoopGradientImgCached() { LoopGradientImgMaker im ; return MakeImage(im) ; }


Image SceneUpp::createLoopImg(unsigned int loopN , unsigned int peakN)
{
	ImageDraw img(LOOP_IMG_RES , LOOP_IMG_RES) ;

#if ! DRAW_LOOP_MASKS
#define LOOP_IMG_ALPHA 255
	img.Alpha().DrawEllipse(0 , 0 , LOOP_IMG_RES , LOOP_IMG_RES , Color(LOOP_IMG_ALPHA , 0 , 0)) ;
	img.Alpha().DrawText(0 , 0 , " MASK" , Roman(36) , LOOP_IMG_MASK_COLOR) ;
#else
	img.Alpha().DrawImage(0 , 0 , createLoopPeaksMask(loopN , peakN)) ; // alpha mask
#endif

#if ! DRAW_LOOP_GRADIENTS
	img.DrawEllipse(0 , 0 , LOOP_IMG_RES , LOOP_IMG_RES , White) ;
#else
	img.DrawImage(0 , 0 , CreateLoopGradientImgCached()) ; // rgb channel
#endif

	return img ;
}

struct LoopImgMaker : ImageMaker
{
	unsigned int sceneN ;
	unsigned int loopN ;
	unsigned int peakN ;
	SceneUpp* scene ;
	virtual String Key() const ;
	virtual Image Make() const ;
} ;

String LoopImgMaker::Key() const { return String(AsString(sceneN) + '_' + AsString(loopN) + '_' + AsString(peakN)) ; }

Image LoopImgMaker::Make() const { return scene->createLoopImg(loopN , peakN) ; }

Image SceneUpp::createLoopImgCached(unsigned int loopN , unsigned int peakN)
{
	LoopImgMaker im ;
	im.peakN = sceneN ; im.loopN = loopN ; im.peakN = peakN ; im.scene = this ;
	return MakeImage(im) ;
}


Image SceneUpp::CreatePeaksBgGradientImg()
{
	// NOTE: pixel values are fixed to Y so LOOP_BG_IMG_RES must be 256 for full bright
	unsigned int rr = LOOP_BG_IMG_RES , rFadeRate = 2 , gFadeRate = 16 , y , x ;
	ImageBuffer ib(rr , rr) ; float intensity = 0.25 ;
	for (y = 0 ; y < rr ; ++y) for (x = 0 ; x < rr ; ++x)
	{
		RGBA& px = ib[y][x] ; px.b = 0 ; px.a = 255 ;
		int rFadeOut = y * rFadeRate ; int rFadeIn = (rr - y) * rFadeRate ;
		int gFadeIn = y * gFadeRate ; int gFadeOut = (rr - y) * gFadeRate ;
		if (rFadeOut > 255) rFadeOut = 255 ; if (rFadeIn > 255) rFadeIn = 255 ;
		if (gFadeOut > 255) gFadeOut = 255 ; if (gFadeIn > 255) gFadeIn = 255 ;
		if (y < 128)	{ px.r = 255 - rFadeOut ; px.g = gFadeIn ; }
		else 					{ px.r = 255 - rFadeIn ; px.g = gFadeOut ; }
		px.r *= intensity ; px.g *= intensity ;

//if (x < 16) px.r = 0 ; else if (x > 240) px.g = 0 ;
	}
	ib.SetKind(IMAGE_OPAQUE) ; Premultiply(ib) ; return ib ;
}

struct PeaksBgGradientImgMaker : ImageMaker
	{ virtual String Key() const ; virtual Image Make() const ; } ;

String PeaksBgGradientImgMaker::Key() const { return String(LOOP_BG_GRADIENT_IMAGE_KEY) ; }

Image PeaksBgGradientImgMaker::Make() const { return SceneUpp::CreatePeaksBgGradientImg() ; }

Image SceneUpp::CreatePeaksBgGradientImgCached() { PeaksBgGradientImgMaker im ; return MakeImage(im) ; }
*/
