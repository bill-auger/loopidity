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


#ifndef _LOOPIDITY_SDL_H_
#define _LOOPIDITY_SDL_H_


#include "loopidity.h"


// common
#define GUIPAD  4
#define GUIPAD2 (GUI_PAD * 2)

// timer intervals
#define GUI_UPDATE_INTERVAL          125
#define GUI_UPDATE_LOW_PRIORITY_NICE 8   // n high priority updates to pass

// window magnitudes
#define SCREEN_W        1024 // minimum screen resolution
#define SCREEN_H        768  // minimum screen resolution
#define WIN_TITLE_H     20   // approximate window decoration size
#define WIN_BORDER_W    2    // approximate window decoration size
#define WIN_BORDER_H    2    // approximate window decoration size
#define PIXEL_DEPTH     32
#define WIN_W           (SCREEN_W - (WIN_BORDER_W * 2))
#define WIN_H           (SCREEN_H - WIN_TITLE_H - WIN_BORDER_H)
#define WIN_RECT        { 0 , 0 , WIN_W , WIN_H }
#define WIN_CENTER      (WinRect.w / 2)

// header magnitudes
#define HEADER_FONT_SIZE 36
//#define HEADER_W         200 // approx 10 chars @ STATUS_FONT_SIZE 36
#define HEADER_W         360 // approx 18 chars @ STATUS_FONT_SIZE 36
#define HEADER_H         48  // HEADER_FONT_SIZE 36
#define HEADER_X         (WIN_CENTER - (HEADER_W / 2))
#define HEADER_RECT_DIM  { 0 , 0 , HEADER_W , HEADER_H }
#define HEADER_RECT_C    { (Sint16)HEADER_X , 0 , 0 , 0 }

// scope magnitudes
#define N_PEAKS_SCOPE     420 // TODO: maybe set this to scene width/2
#define SCOPE_W           N_PEAKS_SCOPE
#define SCOPES_H          (PEAK_RADIUS * 2)
#define SCOPEINL          (WIN_CENTER + GUI_PAD)
#define SCOPEOUTR         (WIN_CENTER - GUI_PAD)
#define SCOPEINR          (SCOPE_IN_L  + N_PEAKS_SCOPE)
#define SCOPEOUTL         (SCOPE_OUT_R - N_PEAKS_SCOPE)
//#define SCOPES_T           (WinRect.h - (STATUS_H * 2) - SCOPES_H)
#define SCOPES_T          (HEADER_H + Y_PADDING + (SCENE_H * Loopidity::N_SCENES))
#define SCOPE_0           (SCOPES_T + (SCOPES_H / 2)) ;
#define SCOPEPEAKH        ((float)SCOPES_H / 2.0)
#define SCOPE_IN_RECT     { (Sint16)SCOPE_IN_L  , (Sint16)SCOPES_T , SCOPE_W + 1 , SCOPES_H + 1 }
#define SCOPE_OUT_RECT    { (Sint16)SCOPE_OUT_L , (Sint16)SCOPES_T , SCOPE_W + 1 , SCOPES_H + 1 }
#define SCOPE_MASK_RECT   { 0 , 0 , 1 , 0 } ;
#define SCOPE_RECT        { 0 , 0 , 0 , 0 } ;
#define SCOPE_LOUD        0.95
#define SCOPE_OPTIMAL     0.8

// edit histogram
#define EDITOR_RECT { (Sint16)SCOPE_OUT_L , (Sint16)SCOPES_T , SCOPE_W + 2 + GUI_PAD2 , SCOPES_H }

// VUs magnitudes
#define VUW           12
#define VUSH          SCOPES_H
#define VUSB          SCOPES_T + SCOPES_H
#define VUS_T         SCOPES_T
#define VUS_IN_W      (GUI_PAD + ((VU_W + GUI_PAD) * JackIO::N_INPUT_CHANNELS ))
#define VUS_OUT_W     (GUI_PAD + ((VU_W + GUI_PAD) * JackIO::N_OUTPUT_CHANNELS))
#define VUSINL        (SCOPE_IN_R  + GUI_PAD2           )
#define VUSOUTL       (SCOPE_OUT_L - GUI_PAD - VUS_OUT_W)
#define VUS_IN_RECT   { (Sint16)VUS_IN_L  , (Sint16)VUS_T , (Uint16)VUS_IN_W  , VUS_H }
#define VUS_OUT_RECT  { (Sint16)VUS_OUT_L , (Sint16)VUS_T , (Uint16)VUS_OUT_W , VUS_H }
#define VUS_MASK_RECT { 0 , 0 , VU_W , 0 } ;
#define VU_RECT       { 0 , 0 , 0    , 0 } ;

// status magnitudes
#define STATUS_FONT_SIZE 12
#define STATUS_H         20  // STATUS_FONT_SIZE 12
#define STATUS_W         256 // approx 36 chars @ STATUS_FONT_SIZE 12
#define STATUS_Y         (WinRect.h - STATUS_H)
#define STATUS_L         0
#define STATUS_C         ((WinRect.w / 2) - (STATUS_W / 2))
#define STATUS_R         (WinRect.w - STATUS_W)
#define STATUS_RECT_DIM  { 0 , 0 , STATUS_W , STATUS_H }
#define STATUS_RECT_L    { (Sint16)STATUS_L , (Sint16)STATUS_Y , 0 , 0 }
#define STATUS_RECT_C    { (Sint16)STATUS_C , (Sint16)STATUS_Y , 0 , 0 }
#define STATUS_RECT_R    { (Sint16)STATUS_R , (Sint16)STATUS_Y , 0 , 0 }

// mouse magnitudes
#define MOUSE_SCENES_L (LOOPS_L - BORDER_PAD)
#define MOUSE_SCENES_R (MOUSE_SCENES_L + (LOOP_W * Loopidity::N_LOOPS))
#define MOUSE_SCENES_T (HEADER_H - (Y_PADDING / 2))
#define MOUSE_SCENES_B (MOUSE_SCENES_T + (SCENE_H * Loopidity::N_SCENES))

// edit histogram magnitudes
#if SCENE_NFRAMES_EDITABLE
#  define EDIT_HISTOGRAM_GRADUATIONS_GRANULARITY 500
#  define EDIT_HISTOGRAM_GRADUATION_H            12
#endif // #if SCENE_NFRAMES_EDITABLE

// external assets
#define SCOPE_IMG_PATH     "scope_gradient.bmp"
#define HISTOGRAM_IMG_PATH "histogram_gradient.bmp"
#define LOOP_IMG_PATH      "loop_gradient.argb.bmp"
#define PURISA_TTF_PATH    "Purisa.ttf"

// fonts and colors
#define HEADER_FONT_PATH       PURISA_TTF_PATH
#define HEADER_TEXT_COLOR      { 255 , 0 , 255 , 0 }
#define STATUS_FONT_PATH       PURISA_TTF_PATH
#define STATUS_TEXT_COLOR      { 255 , 0 , 255 , 0 }
#define WINDOWBGCOLOR          0xFF111111
#define VUSINBGCOLOR           0xFF221122
#define VUSOUTBGCOLOR          0xFF112222 // NOTE: SDL_FillRect()          takes 0xAARRGGBB
#define VUSINBORDERCOLOR       0x882288FF // NOTE: roundedRectangleColor() takes 0xRRGGBBAA
#define VUSOUTBORDERCOLOR      0x228888FF
// #define INSCOPE_QUIET_COLOR    0x00ff00FF
// #define INSCOPE_OPTIMAL_COLOR  0xFFFF00FF
// #define INSCOPE_LOUD_COLOR     0xFF0000FF
// #define OUTSCOPE_QUIET_COLOR   0x00FF00FF
// #define OUTSCOPE_OPTIMAL_COLOR 0xFFFF00FF
// #define OUTSCOPE_LOUD_COLOR    0xFF0000FF

// string constants
#define HEADER_TEXT                 "This is Loopidity"
#define X11_ERROR_MSG               "ERROR: LoopiditySdl::Init(): XGetWindowAttributes(): can't get root window geometry - quitting\n"
#define RESOLUTION_ERROR_MSG        "ERROR: screen resolution must be at least %dx%d - quitting\n"
#define SDL_ERROR_FMT               "ERROR: %s(): %s\n"
#define SDL_INIT_ERROR_TEXT         "SDL_Init"
#define SDL_SETVIDEOMODE_ERROR_TEXT "SDL_SetVideoMode"
#define SDL_KEYREPEAT_ERROR_TEXT    "SDL_EnableKeyRepeat"
#define SDL_LOADBMP_ERROR_TEXT      "SDL_LoadBMP"
#define TTF_ERROR_FMT               "ERROR: %s(): %s\n"
#define TTF_INIT_ERROR_MSG          "TTF_Init"
#define TTF_OPENFONT_ERROR_MSG      "TTF_OpenFont"

// flags
#define SDL_SCREEN_FLAGS SDL_HWSURFACE | SDL_DOUBLEBUF


class LoopiditySdl
{
  friend class Loopidity ;
  friend class SceneSdl ;
  friend class Trace ;


  private:

    /* LoopiditySdl class side private variables */

    // common
    static const Uint8 GUI_PAD ;
    static const Uint8 GUI_PAD2 ;

    // window
    static SDL_Surface* Screen ;
    static SDL_Rect     WinRect ;
    static const Uint32 WINDOW_BG_COLOR ;
    static const Uint16 WinCenter ;

    // header
    static SDL_Rect        HeaderRectDim ;
    static SDL_Rect        HeaderRectC ;
    static TTF_Font*       HeaderFont ;
    static const SDL_Color HeaderColor ;

    // status
    static SDL_Rect        StatusRectDim ;
    static SDL_Rect        StatusRectL ;
    static SDL_Rect        StatusRectC ;
    static SDL_Rect        StatusRectR ;
    static TTF_Font*       StatusFont ;
    static const SDL_Color StatusColor ;
    static std::string     StatusTextL ;
    static std::string     StatusTextC ;
    static std::string     StatusTextR ;

    // scenes
    static SceneSdl**   SdlScenes ;
    static SDL_Surface* ScopeGradient ;
    static SDL_Surface* HistogramGradient ;
    static SDL_Surface* LoopGradient ;
    static SDL_Surface* VuGradient ;

    // scopes
    static const Uint32         SCOPE_IN_BG_COLOR ;
    static const Uint32         SCOPE_OUT_BG_COLOR ;
    static const Uint32         SCOPE_IN_BORDER_COLOR ;
    static const Uint32         SCOPE_OUT_BORDER_COLOR ;
    static const Uint16         SCOPE_IN_L ;
    static const Uint16         SCOPE_IN_R ;
    static const Uint16         SCOPE_OUT_L ;
    static const Uint16         SCOPE_OUT_R ;
    static const Sint16         SCOPES_M ;
    static const float          SCOPE_PEAK_H ;
    static SDL_Rect             ScopeInRect ;
    static SDL_Rect             ScopeOutRect ;
    static SDL_Rect             ScopeMaskRect ;
    static SDL_Rect             ScopeRect ;
    static std::vector<Sample>* PeaksIn ;
    static std::vector<Sample>* PeaksOut ;

    // edit histogram
    static SDL_Rect EditorRect ;

    // VUs
    static const Uint32   VUS_IN_BORDER_COLOR ;
    static const Uint32   VUS_OUT_BORDER_COLOR ;
    static const Uint32   VUS_IN_BG_COLOR ;
    static const Uint32   VUS_OUT_BG_COLOR ;
    static const Uint8    VU_W ;
    static const Uint16   VUS_H ;
    static const Uint16   VUS_B ;
    static const Uint16   VUS_IN_L ;
    static const Uint16   VUS_OUT_L ;
    static       SDL_Rect VusInRect  ;
    static       SDL_Rect VusOutRect  ;
    static       SDL_Rect VuMaskRect ;
    static       SDL_Rect VuRect ;
    static const float    VuPeakH ;
    static       Sample*  PeaksVuIn ;
    static       Sample*  PeaksVuOut ;

    // DrawScenes() 'local' variables
    static Uint16       CurrentSceneN ;
    static Uint16       NextSceneN ;
    static Uint32       CurrentPeakN ;
    static Uint16       SceneProgress ;
    static Uint16       SceneN ;
    static SceneSdl*    SdlScene ;
    static SDL_Surface* SceneSurface ;
    static SDL_Rect*    SceneRect ;


    /* LoopiditySdl class side private functions */

    // setup
    static bool IsInitialized(void) ; // TODO: make singleton
    static bool Init(         SceneSdl**           sdlScenes  , std::vector<Sample>* peaksIn   ,
                              std::vector<Sample>* peaksOut   , Sample*              peaksVuIn ,
                              Sample*              peaksVuOut                                  ) ;
    static void SdlError(     const char* functionName) ;
    static void TtfError(     const char* functionName) ;
    static void Cleanup(      void) ;

    // drawing
    static void DrawHeader(         void) ;
    static void BlankScreen(        void) ;
    static void DrawScenes(         void) ;
#if SCENE_NFRAMES_EDITABLE
    static void DrawEditor(         void) ;
#endif // #if SCENE_NFRAMES_EDITABLE
    static void DrawScopes(         void) ;
    static void DrawScope (         std::vector<Sample>* peaks , Uint16 peaks_x , Uint16 peak_n) ;
    static void DrawVUs(            void) ;
    static void DrawVU(             Sample* peaks , Uint16 vu_x , Uint8 channel_n) ;
    static void DrawText(           std::string text     , SDL_Surface* surface    ,
                                    TTF_Font*   font     , SDL_Rect*    screenRect ,
                                    SDL_Rect*   cropRect , SDL_Color    fgColor    ) ;
    static void DrawStatusArea(     void) ;
    static void FlipScreen(         void) ;
    static void DrawBorder(         SDL_Surface* a_surface , Uint16 l , Uint16 t    ,
                                    Uint16       r         , Uint16 b , Uint32 color) ;
    static void DrawBorder(         SDL_Surface* a_surface , SDL_Rect a_rect , Uint32 color) ;
    static void Alert(              std::string msg) ;

    // getters/settters
    static void SetStatusL(std::string msg) ;
    static void SetStatusC(std::string msg) ;
    static void SetStatusR(std::string msg) ;
//    static Uint32 GetAvailableMemory() ;
} ;


#endif // #ifndef _LOOPIDITY_SDL_H_
