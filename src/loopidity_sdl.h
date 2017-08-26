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


#include <string>
#include <vector>

#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_rotozoom.h>
#include <SDL2/SDL_ttf.h>
#ifndef _WIN32
#  include <X11/Xlib.h>
#endif // _WIN32

#include "assets/fonts.h"
#include "assets/images.h"
#include "scene_sdl.h"


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
    static const Uint8  PIXEL_DEPTH ;
    static const Uint16 SCREEN_MIN_W ;
    static const Uint16 SCREEN_MIN_H ;
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
    static const Uint8  N_SCENES ;
    static const Uint8  N_LOOPS ;
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
#if FIXED_N_AUDIO_PORTS
    static const Uint8    N_INPUT_CHANNELS ;
    static const Uint8    N_OUTPUT_CHANNELS ;
#else // FIXED_N_AUDIO_PORTS // see loopidity.h
    static const Uint8    MAX_INPUT_CHANNELS ;
    static const Uint8    MAX_OUTPUT_CHANNELS ;
#endif // FIXED_N_AUDIO_PORTS
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
    static bool         Init         (SceneSdl**           sdlScenes  , std::vector<Sample>* peaksIn   ,
                                      std::vector<Sample>* peaksOut   , Sample*              peaksVuIn ,
                                      Sample*              peaksVuOut                                  ) ;
    static bool         IsInitialized(void) ; // TODO: make singleton
    static SDL_Surface* LoadGimpImage(GimpImage gimp_image) ;
    static void         SdlError     (const char* functionName) ;
    static void         TtfError     (const char* functionName) ;
    static void         Cleanup      (void) ;

    // drawing
    static void DrawHeader(         void) ;
    static void BlankScreen(        void) ;
    static void DrawScenes(         Uint8 currentSceneN , Uint8 nextSceneN) ;
#if SCENE_NFRAMES_EDITABLE
    static void DrawEditor(         Uint8 currentSceneN) ;
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
    static void Alert(              std::string message) ;

    // getters/settters
    static void SetStatusL(std::string msg) ;
    static void SetStatusC(std::string msg) ;
    static void SetStatusR(std::string msg) ;
//    static Uint32 GetAvailableMemory() ;
} ;


#endif // #ifndef _LOOPIDITY_SDL_H_
