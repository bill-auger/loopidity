
#include "scene_sdl.h"


// DEBUG
void SceneSdl::makeMainDbgText(char* dbg)
{
	unsigned int peakN = ((float)scene->frameN / (float)scene->nFrames) * (float)N_PEAKS ;
	sprintf(dbg , "Paint() SceneN=%d nLoops=%d PeakN=%d" , sceneN , scene->nLoops , peakN) ;
}
// DEBUG end


/* SceneUpp class public functions and ImageMaker structs */

void SceneSdl::drawScene(SDL_Surface* screen)
{
/*
#if DRAW_BG_PARTIAL
	d.DrawRect(sceneL , sceneT , sceneW , sceneH , WIN_BG_COLOR) ;
#endif

	unsigned int PeakN = ((float)scene->frameN / (float)scene->nFrames) * (float)N_PEAKS ;

#if DRAW_DEBUG_TEXT
char dbg[255] ; sprintf(dbg , "drawScene(%d) PeakN=%d" , sceneN , PeakN) ; d.DrawText(SCENE_DEBUG_TEXT_POS , dbg , Roman(12) , White) ;
#endif

#if DRAW_PEAK_BARS
	// draw scene peak bars
	ImageDraw img(sceneW , LOOP_D) ; unsigned int hiScenePeak = scene->hiScenePeaks[PeakN] ;
	unsigned int t = (PEAK_H - hiScenePeak) + 1 , h = (hiScenePeak * 2) - 1 ;
	img.Alpha().DrawRect(0 , t , sceneW , h , White) ;
	img.DrawImage(0 , 0 , sceneW , LOOP_D , CreatePeaksBgGradientImgCached()) ;
	d.DrawImage(sceneL , maxPeakY , img) ;
	d.DrawLine(sceneL , maxPeakY , sceneW , maxPeakY , 1 , SCENE_PEAK_MAX_COLOR) ;
	d.DrawLine(sceneL , zeroPeakY , sceneW , zeroPeakY , 1 , SCENE_PEAK_ZERO_COLOR) ;
	d.DrawLine(sceneL , minPeakY , sceneW , minPeakY , 1 , SCENE_PEAK_MAX_COLOR) ;
#endif

	// draw loops
	for (unsigned int loopN = 0 ; loopN < scene->nLoops ; ++loopN)
	{
		unsigned int* peaks = scene->loops[loopN]->peaks ;
		unsigned int loopX = sceneL + (loopW * loopN) ;

#if DRAW_HISTOGRAMS
// TODO: draw histogram mixing all loops in this sceneN
// perhaps better to make this a cached image (it only changes omce per loop)

		// draw histogram border
		Color loopColor = (loopN == scene->nLoops - 1)? LOOP_ACTIVE_COLOR : LOOP_INACTIVE_COLOR ;
		unsigned int histogramR = loopX + LOOP_D ;
		d.DrawLine(loopX , histogramT , histogramR , histogramT , 1 , loopColor) ;
		d.DrawLine(loopX , histogramT , loopX , histogramB , 1 , loopColor) ;
		d.DrawLine(loopX , histogramB , histogramR , histogramB , 1 , loopColor) ;
		d.DrawLine(histogramR , histogramT , histogramR , histogramB , 1 , loopColor) ;

		// draw histogram
		for (unsigned int peakN = 0 ; peakN < N_PEAKS ; ++peakN)
		{
			unsigned int currentPeak = peaks[peakN] ;
			unsigned int l = loopX + (HISTOGRAM_SAMPLE_W * peakN) ;
			unsigned int t = histogramT + ((PEAK_H - currentPeak) * HISTOGRAM_Y_SCALE) ;
			unsigned int r = l ;
			unsigned int b = histogramT + ((PEAK_H + currentPeak) * HISTOGRAM_Y_SCALE) ;
			d.DrawLine(l , t , r , b , HISTOGRAM_SAMPLE_W , (peakN == PeakN)? HISTOGRAM_PEAK_CURRENT_COLOR : loopColor) ;
		}
#endif

#if DRAW_LOOP_PEAKS
		// draw the current sample value and loudest sample value in this loop as rings
		Vector<unsigned int> peakss ; peakss.Add(scene->hiLoopPeaks[loopN]) ; peakss.Add(scene->loops[loopN]->peaks[PeakN]) ;
		Vector<Color> colors ; colors.Add(LOOP_PEAK_MAX_COLOR) ; colors.Add(LOOP_PEAK_CURRENT_COLOR) ;
		for (unsigned ringN = 0 ; ringN < 2 ; ++ringN)
		{
			unsigned int sample = peakss[ringN] ;
			unsigned int l = loopX + PEAK_H - sample ;
			unsigned int t = (zeroPeakY - sample) + 1 ;
			unsigned int w , h ; w = h = (sample * 2) - 1 ;
			d.DrawArc(RectC(l , t , w , h) , Point(0 , 0) , Point(0 , 0) , 0 , colors[ringN]) ;
		}
#endif

#if DRAW_LOOPS
		// draw loop peaks
		d.DrawImage(loopX , maxPeakY , createLoopImgCached(loopN , PeakN)) ;
#endif
	} // for (loopN)
*/
}

void SceneSdl::setDims(SDL_Rect winRect , bool isFullScreen)
{
	// calculate screen coordinates for GUI layout
	unsigned int winWidth = winRect.w , winHeight = winRect.h ;

	xPadding = (int)X_PADDING ;
	loopW = xPadding + LOOP_D ;
	sceneW = winWidth - (xPadding * 2) ;
	sceneH = Y_PADDING + LOOP_D ;
	sceneX = xPadding ;
	sceneY = Y_OFFSET + (sceneH * sceneN) ;
	if (isFullScreen) { sceneL = sceneX ; sceneT = sceneY ; }
	else { sceneL = sceneT = 0 ; } // for drawing partial
	histogramT = sceneT + HISTOGRAM_H ;
	histogramB = histogramT + HISTOGRAM_H ;
	maxPeakY = sceneT + Y_PADDING ;
	zeroPeakY = maxPeakY + PEAK_H ;
	minPeakY = maxPeakY + LOOP_D ;
}

/*
Image SceneUpp::createLoopPeaksMask(unsigned int loopN , unsigned int peakN)
{
	unsigned int* peaks = scene->loops[loopN]->peaks ;
	unsigned int rr = LOOP_IMG_RES , c = rr / 2 ; Point focus(c , c) , p , loopSeamPoint ;
	ImageDraw id(rr , rr) ; id.DrawRect(0 , 0 , rr , rr , Black) ;
	for (unsigned int sliceN = 0 ; sliceN < N_PEAKS ; ++sliceN)
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

		if (!peakN) loopSeamPoint = vertices[1] ; peakN = (++peakN) % N_PEAKS ;
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


Image SceneUpp::createSceneImg()
{
	ImageDraw id(sceneW , sceneH) ; drawScene(id) ;

#if ! DRAW_SCENE_FADE
	return id ;
#else
	ImageBuffer ib(id) ; ib.SetKind(IMAGE_OPAQUE) ;
	unsigned int fadeB = sceneT + sceneH , y , x ; float lum = 0.5 ;
	for (y = sceneT ; y < fadeB ; ++y) for (x = 0 ; x < sceneW ; ++x)
		{ RGBA& px = ib[y][x] ; px.r *= lum ; px.g *= lum ; px.b *= lum ; }
	Premultiply(ib) ; return ib ;
#endif
}

struct SceneImgMaker : ImageMaker
{
	// TODO: we only really need to keep one of these around
	// once the scene changes the prev can be disposed (perhaps just store it in a var)
	unsigned int sceneN ;
	unsigned int checksum ;
	unsigned int w ;
	unsigned int h ;
	SceneUpp* scene ;
	virtual String Key() const ; virtual Image Make() const ;
} ;

String SceneImgMaker::Key() const
	{ return String(AsString(sceneN) + '_' + AsString(checksum) + '_' + AsString(w) + '_' + AsString(h)) ;}

Image SceneImgMaker::Make() const { return scene->createSceneImg() ; }

Image SceneUpp::createSceneImgCached(unsigned int w , unsigned int h)
{
	SceneImgMaker im ; im.sceneN = sceneN ; im.w = w ; im.h = h ; im.scene = this ;
	unsigned int checksum = 0 ;
	for (unsigned int loopN = 0 ; loopN < scene->nLoops ; ++loopN)
		im.checksum += scene->loops[loopN]->peaks[0] ;
	return MakeImage(im) ;
}
*/
