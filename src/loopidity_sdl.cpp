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


#include "loopidity_sdl.h"


/* LoopiditySdl class side private variables */

// common
const Uint8 LoopiditySdl::GUI_PAD  = GUIPAD ;
const Uint8 LoopiditySdl::GUI_PAD2 = GUI_PAD * 2 ;

// window
SDL_Surface* LoopiditySdl::Screen          = 0 ;             // Init()
SDL_Rect     LoopiditySdl::WinRect         = WIN_RECT ;
const Uint32 LoopiditySdl::WINDOW_BG_COLOR = WINDOWBGCOLOR ;
const Uint16 LoopiditySdl::WinCenter       = WIN_CENTER ;

// header
SDL_Rect        LoopiditySdl::HeaderRectDim = HEADER_RECT_DIM ;
SDL_Rect        LoopiditySdl::HeaderRectC   = HEADER_RECT_C ;
TTF_Font*       LoopiditySdl::HeaderFont    = 0 ;                 // Init()
const SDL_Color LoopiditySdl::HeaderColor   = HEADER_TEXT_COLOR ;

// status
SDL_Rect        LoopiditySdl::StatusRectDim = STATUS_RECT_DIM ;
SDL_Rect        LoopiditySdl::StatusRectL   = STATUS_RECT_L ;
SDL_Rect        LoopiditySdl::StatusRectC   = STATUS_RECT_C ;
SDL_Rect        LoopiditySdl::StatusRectR   = STATUS_RECT_R ;
TTF_Font*       LoopiditySdl::StatusFont    = 0 ;                 // Init()
const SDL_Color LoopiditySdl::StatusColor   = STATUS_TEXT_COLOR ;
std::string     LoopiditySdl::StatusTextL   = "" ;
std::string     LoopiditySdl::StatusTextC   = "" ;
std::string     LoopiditySdl::StatusTextR   = "" ;

// scenes
SceneSdl**   LoopiditySdl::SdlScenes         = 0 ; // Init()
SDL_Surface* LoopiditySdl::ScopeGradient     = 0 ; // Init()
SDL_Surface* LoopiditySdl::HistogramGradient = 0 ; // Init()
SDL_Surface* LoopiditySdl::LoopGradient      = 0 ; // Init()
SDL_Surface* LoopiditySdl::VuGradient        = 0 ; // Init()

// scopes
const Uint32         LoopiditySdl::SCOPE_IN_BG_COLOR      = WINDOW_BG_COLOR ;
const Uint32         LoopiditySdl::SCOPE_OUT_BG_COLOR     = WINDOW_BG_COLOR ;
const Uint32         LoopiditySdl::SCOPE_IN_BORDER_COLOR  = STATE_RECORDING_COLOR ;
const Uint32         LoopiditySdl::SCOPE_OUT_BORDER_COLOR = STATE_PLAYING_COLOR ;
const Uint16         LoopiditySdl::SCOPE_IN_L             = SCOPEINL ;
const Uint16         LoopiditySdl::SCOPE_OUT_R            = SCOPEOUTR ;
const Uint16         LoopiditySdl::SCOPE_IN_R             = SCOPEINR ;
const Uint16         LoopiditySdl::SCOPE_OUT_L            = SCOPEOUTL ;
const Sint16         LoopiditySdl::SCOPES_M               = SCOPE_0 ;
const float          LoopiditySdl::SCOPE_PEAK_H           = SCOPEPEAKH ;
SDL_Rect             LoopiditySdl::ScopeInRect            = SCOPE_IN_RECT ;
SDL_Rect             LoopiditySdl::ScopeOutRect           = SCOPE_OUT_RECT ;
SDL_Rect             LoopiditySdl::ScopeMaskRect          = SCOPE_MASK_RECT ;
SDL_Rect             LoopiditySdl::ScopeRect              = SCOPE_RECT ;
std::vector<Sample>* LoopiditySdl::PeaksIn ;                                      // Init()
std::vector<Sample>* LoopiditySdl::PeaksOut ;                                     // Init()

// edit hostogram
SDL_Rect LoopiditySdl::EditorRect = EDITOR_RECT ;

// VUs
const Uint32   LoopiditySdl::VUS_IN_BORDER_COLOR  = VUSINBORDERCOLOR ;
const Uint32   LoopiditySdl::VUS_OUT_BORDER_COLOR = VUSOUTBORDERCOLOR ;
const Uint32   LoopiditySdl::VUS_IN_BG_COLOR      = VUSINBGCOLOR ;
const Uint32   LoopiditySdl::VUS_OUT_BG_COLOR     = VUSOUTBGCOLOR ;
const Uint8    LoopiditySdl::VU_W                 = VUW ;
const Uint16   LoopiditySdl::VUS_H                = VUSH ;
const Uint16   LoopiditySdl::VUS_B                = VUSB ;
const Uint16   LoopiditySdl::VUS_IN_L             = VUSINL ;
const Uint16   LoopiditySdl::VUS_OUT_L            = VUSOUTL ;
SDL_Rect       LoopiditySdl::VusInRect            = VUS_IN_RECT ;
SDL_Rect       LoopiditySdl::VusOutRect           = VUS_OUT_RECT ;
SDL_Rect       LoopiditySdl::VuMaskRect           = VUS_MASK_RECT ;
SDL_Rect       LoopiditySdl::VuRect               = VU_RECT ;
Sample*        LoopiditySdl::PeaksVuIn            = nullptr ;           // Init()
Sample*        LoopiditySdl::PeaksVuOut           = nullptr ;           // Init()

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

bool LoopiditySdl::Init(SceneSdl**           sdlScenes  , std::vector<Sample>* peaksIn   ,
                        std::vector<Sample>* peaksOut   , Sample*              peaksVuIn ,
                        Sample*              peaksVuOut                                  )
{
  // sanity checks
  if (IsInitialized()                                                 ) return false ;
  if (!sdlScenes || !peaksIn || !peaksOut || !peaksVuIn || !peaksVuOut) return false ;

  // set handles to view instances and VU scopes/peaks caches
  SdlScenes  = sdlScenes ;
  PeaksIn    = peaksIn ;
  PeaksOut   = peaksOut ;
  PeaksVuIn  = peaksVuIn ;
  PeaksVuOut = peaksVuOut ;

  // detect screen resolution
#ifdef _WIN32
  // TODO:
#else // _WIN32
  Display* display = XOpenDisplay(NULL) ; XWindowAttributes winAttr ;
  Uint16   screenN = DefaultScreen(display) ;
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
  SDL_WM_SetCaption(APP_NAME , APP_NAME) ;
/*
  SDL_Surface* icon = SDL_LoadBMP("icon.bmp") ;
  SDL_SetColorKey(icon , SDL_SRCCOLORKEY , SDL_MapRGB(icon->format , 255 , 0 , 255)) ;
  SDL_WM_SetIcon(icon , NULL) ;
*/

  // load images
  std::string scope_img_path     = Loopidity::ASSETS_DIR + SCOPE_IMG_PATH     ;
  std::string histogram_img_path = Loopidity::ASSETS_DIR + HISTOGRAM_IMG_PATH ;
  std::string loop_img_path      = Loopidity::ASSETS_DIR + LOOP_IMG_PATH      ;
  if (!(ScopeGradient     = SDL_LoadBMP(scope_img_path    .c_str())   ) ||
      !(HistogramGradient = SDL_LoadBMP(histogram_img_path.c_str())   ) ||
      !(LoopGradient      = SDL_LoadBMP(loop_img_path     .c_str())   ) ||
      !(VuGradient        = zoomSurface(ScopeGradient , 1.0 , 2.0 , 0))  )
    { SdlError(SDL_LOADBMP_ERROR_TEXT) ; return false ; }

  // load fonts
  std::string header_font_path = Loopidity::ASSETS_DIR + HEADER_FONT_PATH ;
  std::string status_font_path = Loopidity::ASSETS_DIR + STATUS_FONT_PATH ;
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

void LoopiditySdl::BlankScreen() { SDL_FillRect(Screen , 0 , WINDOW_BG_COLOR) ; }

void LoopiditySdl::DrawHeader() { DrawText(HEADER_TEXT , Screen , HeaderFont , &HeaderRectC , &HeaderRectDim , HeaderColor) ; }

void LoopiditySdl::DrawScenes()
{
#if DRAW_SCENES
  CurrentSceneN = Loopidity::GetCurrentSceneN() ; NextSceneN = Loopidity::GetNextSceneN() ;
  for (SceneN = 0 ; SceneN < Loopidity::N_SCENES ; ++SceneN)
  {
    SdlScene = SdlScenes[SceneN] ; SceneRect = const_cast<SDL_Rect*>(&SdlScene->sceneRect) ;
    SDL_FillRect(Screen , SceneRect , WINDOW_BG_COLOR) ;
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

//SDL_FillRect(Screen , SceneRect , WINDOW_BG_COLOR) ;
DRAW_DEBUG_TEXT_L
#endif // #if DRAW_SCENES
}

#if SCENE_NFRAMES_EDITABLE
void LoopiditySdl::DrawEditor()
{
#  if DRAW_EDIT_HISTOGRAM
  SDL_FillRect(Screen , &EditorRect , WINDOW_BG_COLOR) ;

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
      Sint16 histogramH = (Uint16)(baseLoop->getPeakFine(peakN) * SCOPE_PEAK_H) ;
      ScopeMaskRect.y   = (Sint16)SCOPE_PEAK_H - histogramH ;
      ScopeMaskRect.h   = (histogramH * 2) + 1 ;
      ScopeRect.x       = scopeL + peakN ;
      ScopeRect.y       = SCOPES_M - histogramH ;
      SDL_BlitSurface(ScopeGradient , &ScopeMaskRect , Screen , &ScopeRect) ;

      if (peakN != CurrentPeakN) continue ;

      // progress
      Sint16 progressX = scopeL + SceneProgress ;
      Sint16 progressT = SCOPES_M - SCOPE_PEAK_H ;
      Sint16 progressB = SCOPES_M + SCOPE_PEAK_H ;
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
        Sint16 gradT = SCOPES_M + SCOPE_PEAK_H - EDIT_HISTOGRAM_GRADUATION_H ;
        Sint16 gradB = SCOPES_M + SCOPE_PEAK_H ;
        vlineColor(Screen , gradX , gradT , gradB , SCOPE_PEAK_ZERO_COLOR) ;
      }
    }
  }
#  endif // #if DRAW_EDIT_HISTOGRAM
}
#endif // SCENE_NFRAMES_EDITABLE

void LoopiditySdl::DrawScopes()
{
#if DRAW_SCOPES
  SDL_FillRect(Screen , &ScopeInRect  , SCOPE_IN_BG_COLOR     ) ;
  SDL_FillRect(Screen , &ScopeOutRect , SCOPE_OUT_BG_COLOR    ) ;
  DrawBorder  (Screen , ScopeInRect   , SCOPE_IN_BORDER_COLOR ) ;
  DrawBorder  (Screen , ScopeOutRect  , SCOPE_OUT_BORDER_COLOR) ;

  for (Uint16 peak_n = 0 ; peak_n < N_PEAKS_SCOPE ; ++peak_n)
  {
    DrawScope(PeaksIn  , SCOPE_IN_R  , peak_n) ; // input scope
    DrawScope(PeaksOut , SCOPE_OUT_R , peak_n) ; // output scope
  }
#endif // #if DRAW_SCOPES
}

void LoopiditySdl::DrawScope(std::vector<Sample>* peaks , Uint16 peaks_r , Uint16 peak_n)
{
  Uint16 peak_x   = peaks_r - peak_n ;
  Uint16 peak_h   = (Uint16)((*peaks)[peak_n] * SCOPE_PEAK_H) ;
  ScopeMaskRect.y = (Sint16)SCOPE_PEAK_H - peak_h ;
  ScopeMaskRect.h = (peak_h * 2) + 1 ;
  ScopeRect.x     = peak_x ;
  ScopeRect.y     = SCOPES_M - peak_h ;
  SDL_BlitSurface(ScopeGradient , &ScopeMaskRect , Screen , &ScopeRect) ;
}

void LoopiditySdl::DrawVUs()
{
  SDL_FillRect(Screen , &VusInRect  , VUS_IN_BG_COLOR     ) ;
  SDL_FillRect(Screen , &VusOutRect , VUS_OUT_BG_COLOR    ) ;

  for (int channelN = 0 ; channelN < JackIO::N_INPUT_CHANNELS  ; ++channelN)
    DrawVU(PeaksVuIn  , VUS_IN_L  , channelN) ;
  for (int channelN = 0 ; channelN < JackIO::N_OUTPUT_CHANNELS ; ++channelN)
    DrawVU(PeaksVuOut , VUS_OUT_L , channelN) ;

  DrawBorder  (Screen , VusInRect   , VUS_IN_BORDER_COLOR ) ;
  DrawBorder  (Screen , VusOutRect  , VUS_OUT_BORDER_COLOR) ;
}

void LoopiditySdl::DrawVU(Sample* peaks , Uint16 vu_l , Uint8 channel_n)
{
#if DRAW_VUS
    Uint16 peak_h = (Uint16)(peaks[channel_n] * VUS_H) ;
    VuMaskRect.y  = (Sint16)VUS_H - peak_h ;
    VuMaskRect.h  = peak_h ;
    VuRect.x      = vu_l + GUI_PAD + ((VU_W + GUI_PAD) * channel_n) ;
    VuRect.y      = VUS_B - peak_h ;
    SDL_BlitSurface(VuGradient , &VuMaskRect , Screen , &VuRect) ;
#endif // DRAW_VUS
}

void LoopiditySdl::DrawText(std::string text       , SDL_Surface* surface  , TTF_Font* font   ,
                            SDL_Rect*   screenRect , SDL_Rect*    cropRect , SDL_Color fgColor)
{
#if DRAW_STATUS
  if (!text.size()) return ;

  SDL_Surface* textSurface = TTF_RenderText_Solid(font , text.c_str() , fgColor) ;
  if (!textSurface) { TtfError(TTF_INIT_ERROR_MSG) ; return ; }

  SDL_FillRect(surface , screenRect , WINDOW_BG_COLOR) ;
  SDL_BlitSurface(textSurface , cropRect , surface , screenRect) ; SDL_FreeSurface(textSurface) ;
#endif // #if DRAW_STATUS
}

void LoopiditySdl::DrawStatusArea()
{
  DrawText(StatusTextL , Screen , StatusFont , &StatusRectL , &StatusRectDim , StatusColor) ;
  DrawText(StatusTextC , Screen , StatusFont , &StatusRectC , &StatusRectDim , StatusColor) ;
  DrawText(StatusTextR , Screen , StatusFont , &StatusRectR , &StatusRectDim , StatusColor) ;
}

void LoopiditySdl::DrawBorder(SDL_Surface* a_surface , Uint16 l , Uint16 t , Uint16 r , Uint16 b , Uint32 color)
{
  roundedRectangleColor(a_surface , l , t , r , b , 5 , color) ;
}

void LoopiditySdl::DrawBorder(SDL_Surface* a_surface , SDL_Rect a_rect , Uint32 color)
{
  DrawBorder(a_surface , a_rect.x , a_rect.y , a_rect.x + a_rect.w , a_rect.y + a_rect.h , color) ;
}

void LoopiditySdl::FlipScreen() { SDL_Flip(Screen) ; }

void LoopiditySdl::Alert(std::string msg) { std::cout << msg << std::endl ; }


// getters/settters

void LoopiditySdl::SetStatusL(std::string text) { StatusTextL = text ; }

void LoopiditySdl::SetStatusC(std::string text) { StatusTextC = text ; }

void LoopiditySdl::SetStatusR(std::string text) { StatusTextR = text ; }
