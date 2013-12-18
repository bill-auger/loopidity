
#include "scene_sdl.h"


/* LoopSdl class side private functions */

LoopSdl::LoopSdl(SDL_Surface* playingImg , SDL_Surface* mutedImg , Sint16 x , Sint16 y)
{
  // drawing backbuffers
  playingSurface = playingImg ;
  mutedSurface   = mutedImg ;
  currentSurface = playingImg ;

  // drawing coordinates
  loopL = x ;
  loopC = x + PEAK_RADIUS ;
  rect  = { x , y , 0 , 0 } ;
}

LoopSdl::~LoopSdl() { SDL_FreeSurface(playingSurface) ; SDL_FreeSurface(mutedSurface) ; }


/* LoopSdl instance side private functions */

// loop state

void LoopSdl::setStatus(Uint16 loopStatus)
{
  switch (loopStatus)
  {
    case STATE_LOOP_PLAYING: currentSurface = playingSurface ; break ;
    case STATE_LOOP_PENDING: currentSurface = mutedSurface ;   break ;
    case STATE_LOOP_MUTED:   currentSurface = mutedSurface ;   break ;
    default:                                                   break ;
  }
}


/* SceneSdl class side private constants */

Sint16       SceneSdl::HistogramsT     = HISTOGRAMS_T + ((HISTOGRAMS_B - HISTOGRAMS_T) / 2) ;
Sint16       SceneSdl::HistogramsB     = HistogramsT ; // reset()
const Sint16 SceneSdl::HistFramesT     = HISTOGRAM_FRAMES_T ;
const Sint16 SceneSdl::HistFramesB     = HISTOGRAM_FRAMES_B ;
const Uint16 SceneSdl::HistSurfaceW    = HISTOGRAM_IMG_W ;
const Uint16 SceneSdl::HistSurfaceH    = HISTOGRAM_IMG_H ;
const Sint16 SceneSdl::HistFrameR      = HISTOGRAM_FRAME_R ;
const Sint16 SceneSdl::HistFrameB      = HISTOGRAM_FRAME_B ;
const float  SceneSdl::HistPeakH       = (float)HISTOGRAM_PEAK_H ;
const Sint16 SceneSdl::Histogram0      = HISTOGRAM_0 ;
const Uint16 SceneSdl::LoopD           = LOOP_DIAMETER ;
const Sint16 SceneSdl::LoopW           = LOOP_W ;
const Uint16 SceneSdl::Loop0           = LOOP_0 ;
const Sint16 SceneSdl::LoopsL          = LOOPS_L ;
const Sint16 SceneSdl::LoopsT          = LOOPS_T ;
const Uint16 SceneSdl::Loops0          = LOOPS_0 ;
const Uint16 SceneSdl::LoopsB          = LOOPS_B ;
const Sint16 SceneSdl::LoopFrameT      = LOOP_FRAMES_T ;
const Sint16 SceneSdl::LoopFrameB      = LOOP_FRAMES_B ;
const Uint16 SceneSdl::SceneH          = SCENE_H ;
const Uint16 SceneSdl::SceneL          = SCENE_L ;
const Uint16 SceneSdl::SceneR          = SCENE_R ;
const Uint16 SceneSdl::SceneFrameL     = SCENE_FRAME_L ;
const Uint16 SceneSdl::SceneFrameR     = SCENE_FRAME_R ;
const float  SceneSdl::PieSliceDegrees = PIE_SLICE_DEGREES ;
const Uint8  SceneSdl::BytesPerPixel   = PIXEL_DEPTH / 8 ;


/* SceneSdl class side private functions */

SceneSdl::SceneSdl(Scene* aScene , Uint16 sceneN) :
  // constants
  sceneT(      SCENE_T) ,
  sceneFrameT( SCENE_FRAME_T) ,
  sceneFrameB( SCENE_FRAME_B) ,
  sceneRect(   { 0 , sceneT , LoopiditySdl::WinRect.w , SceneH })
{
  // model
  scene = aScene ;

  // drawScene() instance variables
  loopFrameColor  = STATE_IDLE_COLOR ;
  sceneFrameColor = (!sceneN)? STATE_PLAYING_COLOR : STATE_IDLE_COLOR ;

  // drawScene() , drawHistogram() , and drawRecordingLoop() 'local' variables
  currentPeakN  = 0 ;
  hiScenePeak   = 0 ;
  loopN         = 0 ;
  histPeakN     = 0 ;
  peakH         = 0 ;
  loopL         = 0 ;
  loopC         = 0 ;
  histFrameL    = 0 ;
  histFrameR    = 0 ;
  ringR         = 0 ;
  loopFrameL    = 0 ;
  loopFrameR    = 0 ;
  scopeMaskRect = SCOPE_MASK_RECT ;
  scopeGradRect = SCOPE_GRADIENT_RECT ;
  histogramRect = HISTOGRAM_RECT ;
  rotRect       = ROT_LOOP_IMG_RECT ;
  histMaskRect  = HISTOGRAM_MASK_RECT ;
  histGradRect  = HISTOGRAM_GRADIENT_RECT ;
  histogramImg  = NULL ;
  loopImg       = NULL ;
  loop          = NULL ;
  rotImg        = NULL ;

  // drawing backbuffers
  activeSceneSurface   = createHwSurface(sceneRect.w , SceneH) ;
  inactiveSceneSurface = createHwSurface(sceneRect.w , SceneH) ;

  // init
  SDL_SetAlpha(inactiveSceneSurface , SDL_SRCALPHA | SDL_RLEACCEL , 128) ;
  drawScene(inactiveSceneSurface , 0 , 0) ;
}


// helpers

void SceneSdl::PixelRgb2Greyscale(SDL_PixelFormat* fmt , Uint32* pixel)
{
  Uint8 r , g , b , lum ; SDL_GetRGB(*pixel , fmt , &r , &g , &b) ;
  lum = (r * 0.3) + (g * 0.59) + (b * 0.11) ; *pixel = SDL_MapRGB(fmt , lum , lum , lum) ;
}


// getters/setters

Sint16 SceneSdl::GetLoopL(Uint16 loopN) { return LoopsL + (LoopW * loopN) ; }


/* SceneSdl instance side private functions */

// getters/setters

LoopSdl* SceneSdl::getLoop(list<LoopSdl*>* imgs , unsigned int loopN)
{
  if (loopN >= imgs->size()) return NULL ;

  list<LoopSdl*>::iterator loopIter = imgs->begin() ; while (loopN--) ++loopIter ;
  return (*loopIter) ;
}

void SceneSdl::updateStatus()
{
DEBUG_TRACE_SCENESDL_UPDATESTATUS_IN

  sceneFrameColor = (scene->sceneN == Loopidity::GetCurrentSceneN())?
      STATE_PLAYING_COLOR : (scene->sceneN == Loopidity::GetNextSceneN())?
          STATE_PENDING_COLOR : STATE_IDLE_COLOR ;

  loopFrameColor = (!scene->isRolling)? STATE_IDLE_COLOR :
      (scene->shouldSaveLoop)? STATE_RECORDING_COLOR : STATE_PENDING_COLOR ;

  for (Uint16 loopN = 0 ; loopN < loopImgs.size() ; ++loopN)
  {
    Uint16 loopState = (!scene->getLoop(loopN)->isMuted)?
        STATE_LOOP_PLAYING : ((!scene->isMuted)?
        STATE_LOOP_PENDING : STATE_LOOP_MUTED) ;
    getLoop(&histogramImgs , loopN)->setStatus(loopState) ;
    getLoop(&loopImgs , loopN)->setStatus(loopState) ;
  }

DRAW_DEBUG_TEXT_R
DEBUG_TRACE_SCENESDL_UPDATESTATUS_OUT
}

void SceneSdl::startRolling() { HistogramsT = HISTOGRAMS_T ; HistogramsB = HISTOGRAMS_B ; }

void SceneSdl::reset()
{
  HistogramsT    = HistogramsB     = HISTOGRAMS_T + ((HISTOGRAMS_B - HISTOGRAMS_T) / 2) ;
  loopFrameColor = sceneFrameColor = STATE_IDLE_COLOR ;
  histogramImgs.clear() ; loopImgs.clear() ; Loopidity::UpdateView(scene->sceneN) ;
}

void SceneSdl::cleanup() { SDL_FreeSurface(activeSceneSurface) ; SDL_FreeSurface(inactiveSceneSurface) ; }


// drawing

void SceneSdl::drawScene(SDL_Surface* surface , unsigned int currentPeakN , Uint16 sceneProgress)
{
// TODO: perhaps scene scope could/should be output mix (is hiScenePeak now)
//		(e.g) hiScenePeaks[] is static so scenescope does not reflect loop->vol or loop->isMuted
// TODO: perhaps draw full width histogram/progress mixing all loops in this sceneN
// TODO: for better scene scope responsiveness we could add another peaks cache with N_PEAKS_FINE/guiInterval samples granularity (e.g. peaksMed)
// TODO: we could cache the rotImgs if need be but as of now she's pretty slick

  SDL_FillRect(surface , 0 , LoopiditySdl::WinBgColor) ;

#if DRAW_SCENE_SCOPE
  // draw peak gradient mask
  hiScenePeak     = (Uint16)(scene->hiScenePeaks[currentPeakN] * (float)PEAK_RADIUS) ;
  scopeMaskRect.y = Loop0 - hiScenePeak ; scopeMaskRect.h = (hiScenePeak * 2) + 1 ;
  scopeGradRect.y = Loops0 - hiScenePeak ;
  SDL_BlitSurface(LoopiditySdl::ScopeGradient , &scopeMaskRect , surface , &scopeGradRect) ;

  // draw scene scope max , zero , and , min peak lines
  hlineColor(surface , SceneL , SceneR , LoopsT , SCOPE_PEAK_MAX_COLOR) ;
  hlineColor(surface , SceneL , SceneR , Loops0 , SCOPE_PEAK_ZERO_COLOR) ;
  hlineColor(surface , SceneL , SceneR , LoopsB , SCOPE_PEAK_MAX_COLOR) ;
#endif // #if DRAW_SCENE_SCOPE

  // draw loops
  for (loopN = 0 ; loopN < scene->loops.size() ; ++loopN)
  {
#if DRAW_HISTOGRAMS
    histogramImg    = getLoop(&histogramImgs , loopN) ;
    histogramRect.x = histogramImg->loopL - 1 ;
    SDL_BlitSurface(histogramImg->currentSurface , 0 , surface , &histogramRect) ;
    vlineColor(surface , histogramImg->loopL + sceneProgress , HistogramsT , HistogramsB , PEAK_CURRENT_COLOR) ;
#endif // #if DRAW_HISTOGRAMS

#if DRAW_LOOPS
    // draw cached loop image
    loopImg = getLoop(&loopImgs , loopN) ;
    rotImg  = rotozoomSurface(loopImg->currentSurface , currentPeakN * PieSliceDegrees , 1.0 , 0) ;
    rotRect = {(Sint16)(loopImg->loopC - (rotImg->w / 2)) , (Sint16)(Loops0 - (rotImg->h / 2)) , 0 , 0} ;
    SDL_BlitSurface(rotImg , 0 , surface , &rotRect) ; SDL_FreeSurface(rotImg) ;
#endif // #if DRAW_LOOPS

#if DRAW_PEAK_RINGS
    // draw the current and loudest peaks in this loop as rings
// TODO: for efficiency these ringR could be computed and stored upon aLoopSdl creation
    ringR = (Sint16)(scene->hiLoopPeaks[loopN] * (float)PEAK_RADIUS) ;
    circleColor(surface , loopImg->loopC , Loops0 , ringR , LOOP_PEAK_MAX_COLOR) ;
    ringR = (Sint16)(scene->getLoop(loopN)->getPeakFine(currentPeakN) * (float)PEAK_RADIUS) ;
    circleColor(surface , loopImg->loopC , Loops0 , ringR , PEAK_CURRENT_COLOR) ;
#endif // #if DRAW_PEAK_RINGS
  } // for (loopN)
}

LoopSdl* SceneSdl::drawHistogram(Loop* loop)
{
#if DRAW_HISTOGRAMS
  SDL_Surface* HistogramGradient = LoopiditySdl::HistogramGradient ;
  SDL_Surface* playingSurface    = createSwSurface(HistSurfaceW , HistSurfaceH) ;
  SDL_Surface* mutedSurface      = createSwSurface(HistSurfaceW , HistSurfaceH) ;

  // draw histogram border
  roundedRectangleColor(playingSurface , 0 , 0 , HistFrameR , HistFrameB , 5 , STATE_PLAYING_COLOR) ;

  // draw histogram
  for (histPeakN = 0 ; histPeakN < N_PEAKS_COURSE ; ++histPeakN)
  {
    peakH          = loop->getPeakCourse(histPeakN) * HistPeakH ;
    histMaskRect.y = Histogram0 - peakH ;             histMaskRect.h = (peakH * 2) + 1 ;
    histMaskRect.x = histGradRect.x = 1 + histPeakN ; histGradRect.y = histMaskRect.y ;
    SDL_BlitSurface(HistogramGradient , &histMaskRect , playingSurface , &histGradRect) ;
  }

#if DRAW_MUTED_HISTOGRAMS
  SDL_LockSurface(playingSurface) ; SDL_LockSurface(mutedSurface) ;
  Uint16 nBytes ; Uint8* destPixel ; Uint32 srcPixel ;
  for (Uint16 y = 0 ; y < HistSurfaceH ; ++y) for (Uint16 x = 0 ; x < HistSurfaceW ; ++x)
  {
    nBytes    = x * BytesPerPixel ;
    destPixel =            (Uint8*)mutedSurface->pixels   + (y * mutedSurface->pitch)   + nBytes ;
    srcPixel  = *(Uint32*)((Uint8*)playingSurface->pixels + (y * playingSurface->pitch) + nBytes) ;
    PixelRgb2Greyscale(playingSurface->format , &srcPixel) ;
    *(Uint32*)destPixel = srcPixel ;
  }
  SDL_UnlockSurface(playingSurface) ; SDL_UnlockSurface(mutedSurface) ;
#endif // #if DRAW_MUTED_HISTOGRAMS

  return new LoopSdl(playingSurface , mutedSurface , GetLoopL(loopN) , LoopsT) ;
#endif // #if DRAW_HISTOGRAMS
}

LoopSdl* SceneSdl::drawLoop(Loop* loop , Uint16 loopN)
{
#if DRAW_LOOPS
  SDL_Surface* loopBgGradient = LoopiditySdl::LoopGradient ;
  SDL_Surface* playingSurface = SDL_DisplayFormat(loopBgGradient) ;
  SDL_Surface* mutedSurface   = SDL_DisplayFormat(loopBgGradient) ;
  SDL_SetColorKey(loopBgGradient , SDL_SRCCOLORKEY , LOOP_IMG_MASK_COLOR) ;
  SDL_SetAlpha(playingSurface    , SDL_SRCALPHA    , 255) ;
  SDL_SetAlpha(mutedSurface      , SDL_SRCALPHA    , 255) ;

  // draw loop mask
  SDL_Surface* maskSurface = createSwSurface(LoopD , LoopD) ;
  for (Uint16 peakN = 0 ; peakN < N_PEAKS_FINE ; ++peakN)
  {
    Uint16 radius = (Uint16)(loop->getPeakFine(peakN) * (float)PEAK_RADIUS) ;
    Sint16 begin  = PIE_12_OCLOCK + (PieSliceDegrees * peakN) ;
    Sint16 end    = begin + PieSliceDegrees ;
    filledPieColor(maskSurface , PEAK_RADIUS , PEAK_RADIUS , radius , begin , end , LOOP_IMG_MASK_COLOR) ;
  }

  // mask loop gradient image
  SDL_LockSurface(loopBgGradient) ; SDL_LockSurface(maskSurface) ;
  SDL_LockSurface(playingSurface) ; SDL_LockSurface(mutedSurface) ;
  Uint16 nBytes ; Uint32 srcPixel , maskPixel ; Uint8 *playingDestPixel , *mutedDestPixel ;
  for (Uint16 y = 0 ; y < LoopD ; ++y) for (Uint16 x = 0 ; x < LoopD ; ++x)
  {
    nBytes = x * BytesPerPixel ;
    playingDestPixel = (Uint8*)playingSurface->pixels + (y * playingSurface->pitch) + nBytes ;
    mutedDestPixel = (Uint8*)mutedSurface->pixels + (y * mutedSurface->pitch) + nBytes ;
    if (x == PEAK_RADIUS && y < PEAK_RADIUS) { srcPixel = PEAK_CURRENT_COLOR ; maskPixel = true ; }
    else
    {
      maskPixel = *(Uint32*)((Uint8*)maskSurface->pixels + (y * maskSurface->pitch) + nBytes) ;
      srcPixel = (maskPixel)?
          *(Uint32*)((Uint8*)loopBgGradient->pixels + (y * loopBgGradient->pitch) + nBytes) :
          playingSurface->format->colorkey ;
    }
    *(Uint32*)playingDestPixel = srcPixel ;
    if (maskPixel) PixelRgb2Greyscale(playingSurface->format , &srcPixel) ;
    *(Uint32*)mutedDestPixel = srcPixel ;
  }
  SDL_UnlockSurface(maskSurface)    ; SDL_FreeSurface(maskSurface) ;
  SDL_UnlockSurface(playingSurface) ; SDL_UnlockSurface(mutedSurface) ;
  SDL_UnlockSurface(loopBgGradient) ;

  return new LoopSdl(playingSurface , mutedSurface , GetLoopL(loopN) , LoopsT) ;
#endif // #if DRAW_LOOPS
}

void SceneSdl::drawRecordingLoop(SDL_Surface* surface , Uint16 sceneProgress)
{
#if DRAW_RECORDING_LOOP
  // simplified histogram and transient peak ring for currently recording loop
  loopL = LoopsL + (LoopW * scene->loops.size()) ;

  histFrameL = loopL - 1 ; histFrameR = histFrameL + HISTOGRAM_FRAME_R ;
  roundedRectangleColor(surface , histFrameL , HistFramesT , histFrameR , HistFramesB , 5 , loopFrameColor) ;

  loopFrameL = histFrameL - BORDER_PAD ; loopFrameR = histFrameR + BORDER_PAD ;
  roundedRectangleColor(surface , loopFrameL , LoopFrameT , loopFrameR , LoopFrameB , 5 , loopFrameColor) ;

  vlineColor(surface , loopL + sceneProgress , HistogramsT , HistogramsB , PEAK_CURRENT_COLOR) ;

  loopC = loopL + PEAK_RADIUS ; ringR = *Loopidity::GetTransientPeakIn() * (float)PEAK_RADIUS ;
  circleColor(surface , loopC , Loops0 , ringR , PEAK_CURRENT_COLOR) ;
#endif
}

void SceneSdl::drawSceneStateIndicator(SDL_Surface* surface)
{
  roundedRectangleColor(surface , SceneFrameL , sceneFrameT , SceneFrameR , sceneFrameB , 5 , sceneFrameColor) ;

if (scene->sceneN == Loopidity::GetNextSceneN() && sceneFrameColor == STATE_PLAYING_COLOR) roundedRectangleColor(surface , SceneFrameL - 4 , sceneFrameT - 5 , SceneFrameR + 4 , sceneFrameB + 5 , 5 , STATE_PENDING_COLOR) ;
}


// helpers

SDL_Surface* SceneSdl::createHwSurface(Sint16 w , Sint16 h)
  { return SDL_CreateRGBSurface(SDL_HWSURFACE , w , h , PIXEL_DEPTH , 0 , 0 , 0 , 0) ; }

SDL_Surface* SceneSdl::createSwSurface(Sint16 w , Sint16 h)
	{ return SDL_CreateRGBSurface(SDL_SWSURFACE , w , h , PIXEL_DEPTH , 0 , 0 , 0 , 0) ; }

void SceneSdl::addLoop(Loop* newLoop , Uint16 loopN)
{
DEBUG_TRACE_SCENESDL_ADDLOOP_IN

  if (loopImgs.size() != scene->loops.size() - 1) return ;

#if DRAW_HISTOGRAMS
  histogramImgs.push_back(drawHistogram(newLoop)) ;
#endif // #if DRAW_HISTOGRAMS

#if DRAW_LOOPS
  loopImgs.push_back(drawLoop(newLoop , loopN)) ;
#endif // #if DRAW_LOOPS

DEBUG_TRACE_SCENESDL_ADDLOOP_OUT
}

void SceneSdl::deleteLoop(unsigned int loopN)
{
DEBUG_TRACE_SCENESDL_DELETELOOP_IN

  if (loopN >= loopImgs.size()) return ;

  list<LoopSdl*>::iterator histogramImgIter = histogramImgs.begin() ;
  list<LoopSdl*>::iterator loopImgIter      = loopImgs.begin() ;
  while (loopN--)             { ++histogramImgIter ; ++loopImgIter ; }
  if (!histogramImgs.empty()) histogramImgs.erase(histogramImgIter) ;
  if (!loopImgs.empty())      loopImgs.erase(loopImgIter) ;

DEBUG_TRACE_SCENESDL_DELETELOOP_OUT
}
