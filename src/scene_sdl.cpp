/*\
|*|  Loopidity - multi-track multi-channel audio looper designed for live handsfree use
|*|  Copyright 2012-2017 bill-auger <https://github.com/bill-auger/loopidity/issues>
|*|
|*|  This file is part of the Loopidity program.
|*|
|*|  Loopidity is free software: you can redistribute it and/or modify
|*|  it under the terms of the GNU General Public License version 3
|*|  as published by the Free Software Foundation.
|*|
|*|  Loopidity is distributed in the hope that it will be useful,
|*|  but WITHOUT ANY WARRANTY; without even the implied warranty of
|*|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
|*|  GNU General Public License for more details.
|*|
|*|  You should have received a copy of the GNU General Public License
|*|  along with Loopidity.  If not, see <http://www.gnu.org/licenses/>.
\*/


#include "blobs/images.h"
#include "constants/view_constants.h"
#include "trace/trace.h"
#include "loopidity_sdl.h"


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

Sint16       SceneSdl::HistogramsT        = Histogram0 ; // startRolling()
Sint16       SceneSdl::HistogramsB        = Histogram0 ; // startRolling()
const Sint16 SceneSdl::HistFramesT        = HISTOGRAM_FRAMES_T ;
const Sint16 SceneSdl::HistFramesB        = HISTOGRAM_FRAMES_B ;
const Uint16 SceneSdl::HistSurfaceW       = HISTOGRAM_IMG_W ;
const Uint16 SceneSdl::HistSurfaceH       = HISTOGRAM_IMG_H ;
const Sint16 SceneSdl::HistFrameR         = HISTOGRAM_FRAME_R ;
const Sint16 SceneSdl::HistFrameB         = HISTOGRAM_FRAME_B ;
const float  SceneSdl::HistPeakH          = (float)HISTOGRAM_PEAK_H ;
const Sint16 SceneSdl::Histogram0         = HISTOGRAM_0 ;
const Uint16 SceneSdl::LoopD              = LOOP_DIAMETER ;
const Sint16 SceneSdl::LoopW              = LOOP_W ;
const Uint16 SceneSdl::Loop0              = LOOP_0 ;
const Sint16 SceneSdl::LoopsL             = LOOPS_L ;
const Sint16 SceneSdl::LoopsT             = LOOPS_T ;
const Uint16 SceneSdl::Loops0             = LOOPS_0 ;
const Uint16 SceneSdl::LoopsB             = LOOPS_B ;
const Sint16 SceneSdl::LoopFrameT         = LOOP_FRAMES_T ;
const Sint16 SceneSdl::LoopFrameB         = LOOP_FRAMES_B ;
const Uint16 SceneSdl::SceneH             = SCENE_H ;
const Uint16 SceneSdl::SceneL             = SCENE_L ;
const Uint16 SceneSdl::SceneR             = SCENE_R ;
const Uint16 SceneSdl::SceneFrameL        = SCENE_FRAME_L ;
const Uint16 SceneSdl::SceneFrameR        = SCENE_FRAME_R ;
const float  SceneSdl::PieSliceDegrees    = PIE_SLICE_DEGREES ;
const Uint16 SceneSdl::SECONDS_PER_HOUR   = N_SECONDS_PER_HOUR ;
const Uint8  SceneSdl::MINUTES_PER_HOUR   = N_MINUTES_PER_HOUR ;
const Uint8  SceneSdl::SECONDS_PER_MINUTE = N_SECONDS_PER_MINUTE ;


/* SceneSdl class side private functions */

SceneSdl::SceneSdl(Scene* a_scene , std::vector<Sample>* peaks_in) :
  // constants
  sceneT(      SCENE_T) ,
  sceneFrameT( SCENE_FRAME_T) ,
  sceneFrameB( SCENE_FRAME_B) ,
  sceneRect(   { 0 , SCENE_T , LoopiditySdl::WinRect.w , SceneH })
{
  // model
  scene   = a_scene ;
  sceneN  = scene->getSceneN() ;
  peaksIn = peaks_in ;

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
  scopeMaskRect = SCENE_MASK_RECT ;
  scopeGradRect = SCENE_RECT ;
  histogramRect = HISTOGRAM_RECT ;
  histMaskRect  = HISTOGRAM_MASK_RECT ;
  histGradRect  = HISTOGRAM_GRADIENT_RECT ;
  rotRect       = ROT_LOOP_IMG_RECT ;
  histogramImg  = NULL ;
  loopImg       = NULL ;
  loop          = NULL ;
  rotImg        = NULL ;

  // drawing backbuffers
  sceneRect.x = 0 ;
  sceneRect.y = sceneT ;
  sceneRect.w = LoopiditySdl::WinRect.w ;
  sceneRect.h = SceneH ;
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

// setup

void SceneSdl::reset()
{
  HistogramsT    = HistogramsB     = Histogram0 ;
  loopFrameColor = sceneFrameColor = STATE_IDLE_COLOR ;
  histogramImgs.clear() ; loopImgs.clear() ;
}

void SceneSdl::cleanup()
{
  if (!!activeSceneSurface  )
  { SDL_FreeSurface(activeSceneSurface  ) ; activeSceneSurface   = nullptr ; }
  if (!!inactiveSceneSurface)
  { SDL_FreeSurface(inactiveSceneSurface) ; inactiveSceneSurface = nullptr ; }
}


// getters/setters

void SceneSdl::startRolling() { HistogramsT = HISTOGRAMS_T ; HistogramsB = HISTOGRAMS_B ; }

void SceneSdl::updateState(Uint8 currentSceneN , bool isRolling)
{
DEBUG_TRACE_SCENESDL_UPDATESTATUS_IN

  bool isCurrentScene = scene->sceneN == currentSceneN ;
  sceneFrameColor     = (isCurrentScene) ? STATE_PLAYING_COLOR : STATE_IDLE_COLOR ;
  loopFrameColor      = (!isRolling           ) ? STATE_IDLE_COLOR      :
                        (scene->shouldSaveLoop) ? STATE_RECORDING_COLOR : STATE_PENDING_COLOR ;

  for (Uint16 loopN = 0 ; loopN < loopImgs.size() ; ++loopN)
  {
    Uint16 loopState = (!scene->getLoop(loopN)->isMuted) ? STATE_LOOP_PLAYING :
                       (!scene->isMuted                ) ? STATE_LOOP_PENDING : STATE_LOOP_MUTED ;
    getLoopView(&histogramImgs , loopN)->setStatus(loopState) ;
    getLoopView(&loopImgs      , loopN)->setStatus(loopState) ;
  }

  if (isCurrentScene) LoopiditySdl::SetStatusL(makeDurationStatusText()) ;

DRAW_DEBUG_TEXT_R
DEBUG_TRACE_SCENESDL_UPDATESTATUS_OUT
}

LoopSdl* SceneSdl::getLoopView(std::list<LoopSdl*>* imgs , Uint32 loopN)
{
  if (loopN >= imgs->size()) return NULL ;

  std::list<LoopSdl*>::iterator loopIter = imgs->begin() ; while (loopN--) ++loopIter ;
  return (*loopIter) ;
}


// drawing

void SceneSdl::drawScene(SDL_Surface* surface , Uint32 currentPeakN , Uint16 sceneProgress)
{
// TODO: perhaps scene scope could/should be output mix (is hiScenePeak now)
//       eg: hiScenePeaks[] is static so scenescope does not reflect loop->vol or loop->isMuted
// TODO: perhaps draw full width histogram/progress mixing all loops in this sceneN
// TODO: for better scene scope responsiveness we could add another peaks cache with N_PEAKS_FINE/guiInterval samples granularity (e.g. peaksMed)
// TODO: we could cache the rotImgs if need be; but as of now she's pretty slick

  SDL_FillRect(surface , 0 , LoopiditySdl::WINDOW_BG_COLOR) ;

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
    histogramImg    = getLoopView(&histogramImgs , loopN) ;
    histogramRect.x = histogramImg->loopL - 1 ;
    SDL_BlitSurface(histogramImg->currentSurface , 0 , surface , &histogramRect) ;
    vlineColor(surface , histogramImg->loopL + sceneProgress , HistogramsT , HistogramsB , PEAK_CURRENT_COLOR) ;
#endif // #if DRAW_HISTOGRAMS

#if DRAW_LOOPS
    // draw cached loop image
    loopImg = getLoopView(&loopImgs , loopN) ;
    rotImg  = rotozoomSurface(loopImg->currentSurface , currentPeakN * PieSliceDegrees , 1.0 , 0) ;
    rotRect = {(Sint16)(loopImg->loopC - (rotImg->w / 2)) , (Sint16)(Loops0 - (rotImg->h / 2)) , 0 , 0} ;
    SDL_BlitSurface(rotImg , 0 , surface , &rotRect) ; SDL_FreeSurface(rotImg) ;
#endif // #if DRAW_LOOPS

#if DRAW_PEAK_RINGS
// TODO: for efficiency these ringR could be computed and stored upon aLoopSdl creation

    // draw the current and loudest peaks in this loop as rings
    ringR = (Sint16)(scene->hiLoopPeaks[loopN] * (float)PEAK_RADIUS) ;
    circleColor(surface , loopImg->loopC , Loops0 , ringR , LOOP_PEAK_MAX_COLOR) ;
    ringR = (Sint16)(scene->getLoop(loopN)->getPeakFine(currentPeakN) * (float)PEAK_RADIUS) ;
    circleColor(surface , loopImg->loopC , Loops0 , ringR , PEAK_CURRENT_COLOR) ;
#endif // #if DRAW_PEAK_RINGS
  } // for (loopN)
}

void SceneSdl::drawRecordingLoop(SDL_Surface* aSurface , Uint16 sceneProgress)
{
#if DRAW_RECORDING_LOOP
  // simplified histogram and transient peak ring for currently recording loop
  loopL = LoopsL + (LoopW * scene->loops.size()) ;

  histFrameL = loopL - 1 ; histFrameR = histFrameL + HISTOGRAM_FRAME_R ;
  LoopiditySdl::DrawBorder(aSurface , histFrameL , HistFramesT , histFrameR , HistFramesB , loopFrameColor) ;
  loopFrameL = histFrameL - BORDER_PAD ; loopFrameR = histFrameR + BORDER_PAD ;
  LoopiditySdl::DrawBorder(aSurface , loopFrameL , LoopFrameT , loopFrameR , LoopFrameB , loopFrameColor) ;

  vlineColor(aSurface , loopL + sceneProgress , HistogramsT , HistogramsB , PEAK_CURRENT_COLOR) ;

  loopC = loopL + PEAK_RADIUS ; ringR = (*peaksIn)[0] * (float)PEAK_RADIUS ;
  circleColor(aSurface , loopC , Loops0 , ringR , PEAK_CURRENT_COLOR) ;
#endif
}

void SceneSdl::drawSceneStateIndicator(SDL_Surface* aSurface , Uint8 nextSceneN)
{
  // scene frame
  LoopiditySdl::DrawBorder(aSurface , SceneFrameL , sceneFrameT , SceneFrameR , sceneFrameB , sceneFrameColor) ;

//if (scene->sceneN == Loopidity::GetNextSceneN() && sceneFrameColor == STATE_PLAYING_COLOR)
  if (scene->sceneN != nextSceneN) return ;

// TODO: drawing nextScene indicator outside of scene frame
//          (instead of drawing frame different color if nextScene)
//          everything below the header will need to be shifted down 1 px
//          then make ToggleFrameColor , ToggleFrameL , ToggleFrameR class constants
const Uint32 ToggleFrameColor = STATE_PENDING_COLOR ;
const Uint16 ToggleFrameL     = SceneFrameL - 4 ;
const Uint16 ToggleFrameR     = SceneFrameR + 4 ;

  // nextScene indicator frame
  Uint16 toggleFrameT = sceneFrameT - 5 ; Uint16 toggleFrameB = sceneFrameB + 5 ;
  LoopiditySdl::DrawBorder(aSurface , ToggleFrameL , toggleFrameT , ToggleFrameR , toggleFrameB , ToggleFrameColor) ;
}

LoopSdl* SceneSdl::drawHistogram(Loop* aLoop)
{
#if DRAW_HISTOGRAMS
  SDL_Surface* histogramGradient = LoopiditySdl::HistogramGradient ;
  SDL_Surface* playingSurface    = createSwSurface(HistSurfaceW , HistSurfaceH) ;
  SDL_Surface* mutedSurface      = createSwSurface(HistSurfaceW , HistSurfaceH) ;

  // draw histogram border
  LoopiditySdl::DrawBorder(playingSurface , 0 , 0 , HistFrameR , HistFrameB , STATE_PLAYING_COLOR) ;

  // draw histogram
  for (histPeakN = 0 ; histPeakN < N_PEAKS_COURSE ; ++histPeakN)
  {
    peakH          = aLoop->getPeakCourse(histPeakN) * HistPeakH ;
    histMaskRect.y = Histogram0 - peakH ;             histMaskRect.h = (peakH * 2) + 1 ;
    histMaskRect.x = histGradRect.x = 1 + histPeakN ; histGradRect.y = histMaskRect.y ;
    SDL_BlitSurface(histogramGradient , &histMaskRect , playingSurface , &histGradRect) ;
  }

#  if DRAW_MUTED_HISTOGRAMS
  SDL_LockSurface(playingSurface) ; SDL_LockSurface(mutedSurface) ;
  Uint8* destPixel ; Uint32 srcPixel ;
  for (Uint16 y = 0 ; y < HistSurfaceH ; ++y) for (Uint16 x = 0 ; x < HistSurfaceW ; ++x)
  {
    Uint16 nMutedRowBytes   = y * mutedSurface  ->pitch ;
    Uint16 nMutedColBytes   = x * mutedSurface  ->format->BytesPerPixel ;
    Uint16 nPlayingRowBytes = y * playingSurface->pitch ;
    Uint16 nPlayingColBytes = x * playingSurface->format->BytesPerPixel ;

    destPixel =            (Uint8*)mutedSurface->pixels   + nMutedRowBytes   + nMutedColBytes ;
    srcPixel  = *(Uint32*)((Uint8*)playingSurface->pixels + nPlayingRowBytes + nPlayingColBytes) ;
    PixelRgb2Greyscale(playingSurface->format , &srcPixel) ;
    *(Uint32*)destPixel = srcPixel ;
  }
  SDL_UnlockSurface(playingSurface) ; SDL_UnlockSurface(mutedSurface) ;
#  endif // #if DRAW_MUTED_HISTOGRAMS

  return new LoopSdl(playingSurface , mutedSurface , GetLoopL(loopN) , LoopsT) ;
#endif // #if DRAW_HISTOGRAMS
}

LoopSdl* SceneSdl::drawLoop(Loop* aLoop , Uint16 loopN)
{
#if DRAW_LOOPS
  SDL_Surface* loopGradient   = SDL_DisplayFormat(LoopiditySdl::LoopGradient) ;
  SDL_Surface* playingSurface = createSwSurface(loopGradient->w , loopGradient->h) ;
  SDL_Surface* mutedSurface   = createSwSurface(loopGradient->w , loopGradient->h) ;

  // draw loop mask
  SDL_Surface* maskSurface = createSwSurface(LoopD , LoopD) ;
  for (Uint16 peakN = 0 ; peakN < N_PEAKS_FINE ; ++peakN)
  {
    Uint16 radius = (Uint16)(aLoop->getPeakFine(peakN) * (float)PEAK_RADIUS) ;
    Sint16 begin  = PIE_12_OCLOCK + (PieSliceDegrees * peakN) ;
    Sint16 end    = begin + PieSliceDegrees ;
    filledPieColor(maskSurface , PEAK_RADIUS , PEAK_RADIUS , radius , begin , end , LOOP_IMG_MASK_COLOR) ;
  }

  // mask loop gradient image
  SDL_LockSurface(loopGradient) ;   SDL_LockSurface(maskSurface) ;
  SDL_LockSurface(playingSurface) ; SDL_LockSurface(mutedSurface) ;
  Uint32 srcPixel , maskPixel ; Uint8 *playingDestPixel , *mutedDestPixel ;
  for (Uint16 y = 0 ; y < LoopD ; ++y) for (Uint16 x = 0 ; x < LoopD ; ++x)
  {
    playingDestPixel = (Uint8*)playingSurface->pixels + (y * playingSurface->pitch) + (x * playingSurface->format->BytesPerPixel) ;
    mutedDestPixel = (Uint8*)mutedSurface->pixels + (y * mutedSurface->pitch) + (x * mutedSurface->format->BytesPerPixel) ;
    if (x == PEAK_RADIUS && y < PEAK_RADIUS) { srcPixel = PEAK_CURRENT_COLOR ; maskPixel = true ; }
    else
    {
      Uint16 nMaskRowBytes     = y * maskSurface->pitch ;
      Uint16 nMaskColBytes     = x * maskSurface->format->BytesPerPixel ;
      Uint16 nGradientRowBytes = y * loopGradient->pitch ;
      Uint16 nGradientColBytes = x * loopGradient->format->BytesPerPixel ;

      maskPixel = *(Uint32*)((Uint8*)maskSurface ->pixels + nMaskRowBytes     + nMaskColBytes    ) ;
      srcPixel  = *(Uint32*)((Uint8*)loopGradient->pixels + nGradientRowBytes + nGradientColBytes) ;
      if (!maskPixel) srcPixel = playingSurface->format->colorkey                                                 ;
    }
    *(Uint32*)playingDestPixel = srcPixel ;
    if (maskPixel) PixelRgb2Greyscale(playingSurface->format , &srcPixel) ;
    *(Uint32*)mutedDestPixel = srcPixel ;
  }
  SDL_UnlockSurface(maskSurface)    ; SDL_FreeSurface(maskSurface) ;
  SDL_UnlockSurface(playingSurface) ; SDL_UnlockSurface(mutedSurface) ;
  SDL_UnlockSurface(loopGradient) ;

  return new LoopSdl(playingSurface , mutedSurface , GetLoopL(loopN) , LoopsT) ;
#endif // #if DRAW_LOOPS
}


// images

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

void SceneSdl::deleteLoop(Uint8 loopN)
{
DEBUG_TRACE_SCENESDL_DELETELOOP_IN

  if (loopN >= loopImgs.size()) return ;

  std::list<LoopSdl*>::iterator histogramImgIter = histogramImgs.begin() ;
  std::list<LoopSdl*>::iterator loopImgIter      = loopImgs.begin() ;

  while (loopN--) { ++histogramImgIter ; ++loopImgIter ; }
  if (!histogramImgs.empty()) histogramImgs.erase(histogramImgIter) ;
  if (!loopImgs.empty()     ) loopImgs     .erase(loopImgIter     ) ;

DEBUG_TRACE_SCENESDL_DELETELOOP_OUT
}


// helpers

SDL_Surface* SceneSdl::createHwSurface(Sint16 w , Sint16 h)
  { return SDL_CreateRGBSurface(SDL_HWSURFACE , w , h , LoopiditySdl::PIXEL_DEPTH , 0 , 0 , 0 , 0) ; }

SDL_Surface* SceneSdl::createSwSurface(Sint16 w , Sint16 h)
  { return SDL_CreateRGBSurface(SDL_SWSURFACE , w , h , LoopiditySdl::PIXEL_DEPTH , 0 , 0 , 0 , 0) ; }

std::string SceneSdl::makeDurationStatusText()
{
  Uint32 nSeconds = scene->getTotalSeconds() ; char statusText[32] ;
  snprintf(statusText , 32 , "Scene: %d - %d:%02d:%02d" , sceneN ,
          ( nSeconds / SECONDS_PER_HOUR) ,
          ((nSeconds / SECONDS_PER_MINUTE) % MINUTES_PER_HOUR) ,
          ( nSeconds % SECONDS_PER_MINUTE)) ;

  return std::string(statusText) ;
}
