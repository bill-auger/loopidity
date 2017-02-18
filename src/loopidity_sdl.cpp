/*\ Loopidity - multitrack audio looper designed for live handsfree use
|*| https://github.com/bill-auger/loopidity/issues/
|*| Copyright 2013,2015 Bill Auger - https://bill-auger.github.io/
|*|
|*| This file is part of Loopidity.
|*|
|*| Loopidity is free software: you can redistribute it and/or modify
|*| it under the terms of the GNU General Public License version 3
|*| as published by the Free Software Foundation.
|*|
|*| Loopidity is distributed in the hope that it will be useful,
|*| but WITHOUT ANY WARRANTY; without even the implied warranty of
|*| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
|*| GNU General Public License for more details.
|*|
|*| You should have received a copy of the GNU General Public License
|*| along with Loopidity.  If not, see <http://www.gnu.org/licenses/>.
\*/


#include "loopidity_sdl.h"


/* LoopiditySdl class side private variables */

// window
SDL_Surface* LoopiditySdl::Screen     = 0 ;
SDL_Rect     LoopiditySdl::WinRect    = WIN_RECT ;
Uint32       LoopiditySdl::WinBgColor = 0 ;
const Uint16 LoopiditySdl::WinCenter  = WIN_CENTER ;

// header
SDL_Rect        LoopiditySdl::HeaderRectDim = HEADER_RECT_DIM ;
SDL_Rect        LoopiditySdl::HeaderRectC   = HEADER_RECT_C ;
TTF_Font*       LoopiditySdl::HeaderFont    = 0 ;
const SDL_Color LoopiditySdl::HeaderColor   = HEADER_TEXT_COLOR ;

// status
SDL_Rect        LoopiditySdl::StatusRectDim = STATUS_RECT_DIM ;
SDL_Rect        LoopiditySdl::StatusRectL   = STATUS_RECT_L ;
SDL_Rect        LoopiditySdl::StatusRectC   = STATUS_RECT_C ;
SDL_Rect        LoopiditySdl::StatusRectR   = STATUS_RECT_R ;
TTF_Font*       LoopiditySdl::StatusFont    = 0 ;
const SDL_Color LoopiditySdl::StatusColor   = STATUS_TEXT_COLOR ;
string          LoopiditySdl::StatusTextL   = "" ;
string          LoopiditySdl::StatusTextC   = "" ;
string          LoopiditySdl::StatusTextR   = "" ;

// scenes
SceneSdl**   LoopiditySdl::SdlScenes         = 0 ;
SDL_Surface* LoopiditySdl::ScopeGradient     = 0 ;
SDL_Surface* LoopiditySdl::HistogramGradient = 0 ;
SDL_Surface* LoopiditySdl::LoopGradient      = 0 ;

// scopes
SDL_Rect        LoopiditySdl::ScopeRect      = SCOPE_RECT ;
SDL_Rect        LoopiditySdl::MaskRect       = { 0 , 0 , 1 , 0 } ;
SDL_Rect        LoopiditySdl::GradientRect   = { 0 , 0 , 0 , 0 } ;
const Sint16    LoopiditySdl::Scope0         = SCOPE_0 ;
const Uint16    LoopiditySdl::ScopeR         = SCOPE_R ;
const float     LoopiditySdl::ScopePeakH     = SCOPE_PEAK_H ;
vector<Sample>* LoopiditySdl::PeaksIn ;
vector<Sample>* LoopiditySdl::PeaksOut ;
Sample*         LoopiditySdl::PeaksTransient = 0 ;

// DrawScenes() 'local' variables
Uint16       LoopiditySdl::CurrentSceneN = 0 ;
Uint16       LoopiditySdl::NextSceneN    = 0 ;
Uint32       LoopiditySdl::CurrentPeakN  = 0 ;
Uint16       LoopiditySdl::SceneProgress = 0 ;
Uint16       LoopiditySdl::SceneN        = 0 ;
SceneSdl*    LoopiditySdl::SdlScene      = 0 ;
SDL_Surface* LoopiditySdl::SceneSurface  = 0 ;
SDL_Rect*    LoopiditySdl::SceneRect     = 0 ;


/* LoopiditySdl class side private functions */

// setup

bool LoopiditySdl::IsInitialized() { return !!Screen ; }

bool LoopiditySdl::Init(SceneSdl** sdlScenes , vector<Sample>* peaksIn ,
                        vector<Sample>* peaksOut , Sample* peaksTransient)
{
  if (IsInitialized()) return false ;
  if (!sdlScenes || !peaksIn || !peaksOut || !peaksTransient) return false ;

  // set handles to view instances and VU scopes/peaks caches
  SdlScenes      = sdlScenes ;
  PeaksIn        = peaksIn ;
  PeaksOut       = peaksOut ;
  PeaksTransient = peaksTransient ;

  // detect screen resolution
#ifdef _WIN32
  // TODO:
#else // _WIN32
  Display* display = XOpenDisplay(NULL) ; XWindowAttributes winAttr ;
  Uint16 screenN   = DefaultScreen(display) ;
  if (!XGetWindowAttributes(display, RootWindow(display , screenN) , &winAttr))
    { printf(X11_ERROR_MSG) ; return false ; }
  if (winAttr.width < SCREEN_W || winAttr.height < SCREEN_H)
    { printf(RESOLUTION_ERROR_MSG , SCREEN_W , SCREEN_H) ; return false ; }
#endif // _WIN32

  // initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) { SdlError(SDL_INIT_ERROR_TEXT) ; return false ; }
  atexit(SDL_Quit) ;
  Screen = SDL_SetVideoMode(WinRect.w , WinRect.h , PIXEL_DEPTH , SDL_SCREEN_FLAGS) ;
  if (!Screen) { SdlError(SDL_SETVIDEOMODE_ERROR_TEXT) ; return false ; }

  // set input params
  if (SDL_EnableKeyRepeat(0 , 0)) { SdlError(SDL_KEYREPEAT_ERROR_TEXT) ; return false ; }
  SDL_EventState(SDL_MOUSEMOTION , SDL_IGNORE) ;

  // set default window params
  WinBgColor = SDL_MapRGB(Screen->format , 16 , 16 , 16) ;
  SDL_WM_SetCaption(APP_NAME , APP_NAME) ;
/*
  SDL_Surface* icon = SDL_LoadBMP("icon.bmp") ;
  SDL_SetColorKey(icon , SDL_SRCCOLORKEY , SDL_MapRGB(icon->format , 255 , 0 , 255)) ;
  SDL_WM_SetIcon(icon , NULL) ;
*/

  // load images
  std::string scope_img_path     = Loopidity::GetAssetsPath(SCOPE_IMG_PATH    ) ;
  std::string histogram_img_path = Loopidity::GetAssetsPath(HISTOGRAM_IMG_PATH) ;
  std::string loop_img_path      = Loopidity::GetAssetsPath(LOOP_IMG_PATH     ) ;
  if (!(ScopeGradient     = SDL_LoadBMP(scope_img_path    .c_str())) ||
      !(HistogramGradient = SDL_LoadBMP(histogram_img_path.c_str())) ||
      !(LoopGradient      = SDL_LoadBMP(loop_img_path     .c_str()))  )
    { SdlError(SDL_LOADBMP_ERROR_TEXT) ; return false ; }

  // load fonts
  std::string header_font_path = Loopidity::GetAssetsPath(HEADER_FONT_PATH) ;
  std::string status_font_path = Loopidity::GetAssetsPath(STATUS_FONT_PATH) ;
  if (TTF_Init()) { TtfError(TTF_INIT_ERROR_MSG) ; return false ; }
  if (!(HeaderFont = TTF_OpenFont(header_font_path.c_str() , HEADER_FONT_SIZE)) ||
      !(StatusFont = TTF_OpenFont(status_font_path.c_str() , STATUS_FONT_SIZE))  )
    { TtfError(TTF_OPENFONT_ERROR_MSG) ; return false ; }

  return true ;
}

void LoopiditySdl::SdlError(const char* functionName) { printf(SDL_ERROR_FMT , functionName , SDL_GetError()) ; }

void LoopiditySdl::TtfError(const char* functionName) { printf(TTF_ERROR_FMT , functionName , TTF_GetError()) ; }

void LoopiditySdl::Cleanup()
{
  if (HeaderFont)        TTF_CloseFont(HeaderFont) ;
  if (StatusFont)        TTF_CloseFont(StatusFont) ;
  if (ScopeGradient)     SDL_FreeSurface(ScopeGradient) ;
  if (HistogramGradient) SDL_FreeSurface(HistogramGradient) ;
  if (LoopGradient)      SDL_FreeSurface(LoopGradient) ;
  if (Screen)            SDL_FreeSurface(Screen) ;
}


// drawing

void LoopiditySdl::BlankScreen() { SDL_FillRect(Screen , 0 , WinBgColor) ; }

void LoopiditySdl::DrawHeader() { DrawText(HEADER_TEXT , Screen , HeaderFont , &HeaderRectC , &HeaderRectDim , HeaderColor) ; }

void LoopiditySdl::DrawScenes()
{
#if DRAW_SCENES
  CurrentSceneN = Loopidity::GetCurrentSceneN() ; NextSceneN = Loopidity::GetNextSceneN() ;
  for (SceneN = 0 ; SceneN < Loopidity::N_SCENES ; ++SceneN)
  {
    SdlScene = SdlScenes[SceneN] ; SceneRect = const_cast<SDL_Rect*>(&SdlScene->sceneRect) ;
    SDL_FillRect(Screen , SceneRect , WinBgColor) ;
    if (SceneN == CurrentSceneN)
    {
      SceneSurface = SdlScene->activeSceneSurface ;
      CurrentPeakN = SdlScene->scene->getCurrentPeakN() ;
      SceneProgress = ((float)CurrentPeakN / (float)N_PEAKS_FINE) * (PEAK_RADIUS * 2) ;
      SdlScene->drawScene(SceneSurface , CurrentPeakN , SceneProgress) ;

#if DRAW_RECORDING_LOOP
      if (SdlScene->scene->loops.size() < Loopidity::N_LOOPS)
        SdlScene->drawRecordingLoop(SceneSurface , SceneProgress) ;
#endif
    }
    else SceneSurface = SdlScene->inactiveSceneSurface ;

    SDL_BlitSurface(SceneSurface , 0 , Screen , SceneRect) ;
    SdlScene->drawSceneStateIndicator(Screen) ;
  }

//SDL_FillRect(Screen , SceneRect , WinBgColor) ;
DRAW_DEBUG_TEXT_L
#endif // #if DRAW_SCENES
}

#if SCENE_NFRAMES_EDITABLE
void LoopiditySdl::DrawEditScopes()
{
#  if DRAW_EDIT_HISTOGRAM
  SDL_FillRect(Screen , &ScopeRect , WinBgColor) ;

  CurrentSceneN       = Loopidity::GetCurrentSceneN() ;
  Scene* currentScene = SdlScenes[CurrentSceneN]->scene ;
  Loop* baseLoop      = currentScene->getLoop(0) ;
  if (baseLoop)
  {
    const Uint16 scopeL = (WIN_CENTER - (N_PEAKS_FINE / 2)) ;
    CurrentPeakN        = currentScene->getCurrentPeakN() ;
    SceneProgress       = ((float)CurrentPeakN / N_PEAKS_FINE) * N_PEAKS_FINE ;
    for (Uint16 peakN = 0 ; peakN < N_PEAKS_FINE ; ++peakN)
    {
      // histogram
      Sint16 histogramH = (Uint16)(baseLoop->getPeakFine(peakN) * ScopePeakH) ;
      MaskRect.y        = (Sint16)ScopePeakH - histogramH ;
      MaskRect.h        = (histogramH * 2) + 1 ;
      GradientRect.x    = scopeL + peakN ;
      GradientRect.y    = Scope0 - histogramH ;
      SDL_BlitSurface(ScopeGradient , &MaskRect , Screen , &GradientRect) ;

      if (peakN != CurrentPeakN) continue ;

      // progress
      Sint16 progressX = scopeL + SceneProgress ;
      Sint16 progressT = Scope0 - ScopePeakH ;
      Sint16 progressB = Scope0 + ScopePeakH ;
      vlineColor(Screen , progressX , progressT , progressB , PEAK_CURRENT_COLOR) ;
    }

    // graduations
    Uint32 nFrames        = currentScene->nFrames ;
    Uint32 nFramesPerPeak = currentScene->nFramesPerPeak ;
    for (Uint32 frameN = 0 ; frameN < nFrames ; frameN = frameN + 10)
    {
      if (!(frameN % EDIT_HISTOGRAM_GRADUATIONS_GRANULARITY))
      {
        Sint16 gradX = scopeL + (frameN / nFramesPerPeak) ;
        Sint16 gradT = Scope0 + ScopePeakH - EDIT_HISTOGRAM_GRADUATION_H ;
        Sint16 gradB = Scope0 + ScopePeakH ;
        vlineColor(Screen , gradX , gradT , gradB , SCOPE_PEAK_ZERO_COLOR) ;
      }
    }
  }
#  endif // #if DRAW_EDIT_HISTOGRAM
}

void LoopiditySdl::DrawTransientScopes()
#else
void LoopiditySdl::DrawScopes()
#endif // #if SCENE_NFRAMES_EDITABLE
{
#if DRAW_SCOPES
  SDL_FillRect(Screen , &ScopeRect , WinBgColor) ;
  for (Uint16 peakN = 0 ; peakN < N_PEAKS_TRANSIENT ; ++peakN)
  {
    Sint16 inX  = ScopeR - peakN , outX = WinCenter - peakN ;
    Sint16 inH  = (Uint16)((*PeaksIn)[peakN] * ScopePeakH) ;
    Sint16 outH = (Uint16)((*PeaksOut)[peakN] * ScopePeakH) ;

    // input scope
    MaskRect.y     = (Sint16)ScopePeakH - inH ; MaskRect.h = (inH * 2) + 1 ;
    GradientRect.x = inX ; GradientRect.y = Scope0 - inH ;
    SDL_BlitSurface(ScopeGradient , &MaskRect , Screen , &GradientRect) ;

    // output scope
    MaskRect.y     = (Sint16)ScopePeakH - outH ; MaskRect.h = (outH * 2) + 1 ;
    GradientRect.x = outX ; GradientRect.y = Scope0 - outH ;
    SDL_BlitSurface(ScopeGradient , &MaskRect , Screen , &GradientRect) ;
  }
#endif // #if DRAW_SCOPES
}

void LoopiditySdl::DrawText(string text , SDL_Surface* surface , TTF_Font* font , SDL_Rect* screenRect , SDL_Rect* cropRect , SDL_Color fgColor)
{
#if DRAW_STATUS
  if (!text.size()) return ;

  SDL_Surface* textSurface = TTF_RenderText_Solid(font , text.c_str() , fgColor) ;
  if (!textSurface) { TtfError(TTF_INIT_ERROR_MSG) ; return ; }

  SDL_FillRect(surface , screenRect , WinBgColor) ;
  SDL_BlitSurface(textSurface , cropRect , surface , screenRect) ; SDL_FreeSurface(textSurface) ;
#endif // #if DRAW_STATUS
}

void LoopiditySdl::DrawStatusArea()
{
  DrawText(StatusTextL , Screen , StatusFont , &StatusRectL , &StatusRectDim , StatusColor) ;
  DrawText(StatusTextC , Screen , StatusFont , &StatusRectC , &StatusRectDim , StatusColor) ;
  DrawText(StatusTextR , Screen , StatusFont , &StatusRectR , &StatusRectDim , StatusColor) ;
}

void LoopiditySdl::FlipScreen() { SDL_Flip(Screen) ; }

void LoopiditySdl::Alert(string msg) { cout << msg << endl ; }


// getters/settters

void LoopiditySdl::SetStatusL(string text) { StatusTextL = text ; }

void LoopiditySdl::SetStatusC(string text) { StatusTextC = text ; }

void LoopiditySdl::SetStatusR(string text) { StatusTextR = text ; }
