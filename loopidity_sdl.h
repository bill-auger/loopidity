
#ifndef _LOOPIDITY_SDL_H_
#define _LOOPIDITY_SDL_H_


#include "loopidity.h"
class SceneSdl ;

using namespace std ;


// intervals
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
#define N_PEAKS_TRANSIENT 480 // TODO: maybe set this to scene width/2
#define SCOPE_W           (N_PEAKS_TRANSIENT * 2)
#define SCOPE_H           (PEAK_RADIUS * 2)
#define SCOPE_L           (WIN_CENTER - N_PEAKS_TRANSIENT)
#define SCOPE_R           (WIN_CENTER + N_PEAKS_TRANSIENT)
//#define SCOPE_T           (WinRect.h - (STATUS_H * 2) - SCOPE_H)
#define SCOPE_T           (HEADER_H + Y_PADDING + (SCENE_H * N_SCENES))
#define SCOPE_0           (SCOPE_T + (SCOPE_H / 2)) ;
#define SCOPE_PEAK_H      ((float)SCOPE_H / 2.0)
#define SCOPE_RECT        { (Sint16)SCOPE_L , (Sint16)SCOPE_T , SCOPE_W + 1 , SCOPE_H + 1 }
#define SCOPE_LOUD        0.95
#define SCOPE_OPTIMAL     0.8

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
#define MOUSE_SCENES_R (MOUSE_SCENES_L + (LOOP_W * N_LOOPS))
#define MOUSE_SCENES_T (HEADER_H - (Y_PADDING / 2))
#define MOUSE_SCENES_B (MOUSE_SCENES_T + (SCENE_H * N_SCENES))

// edit histogram magnitudes
#if SCENE_NFRAMES_EDITABLE
#  define EDIT_HISTOGRAM_GRADUATIONS_GRANULARITY 500
#  define EDIT_HISTOGRAM_GRADUATION_H            12
#  define KLUDGE_OFFSET_N_FRAMES                 (EDIT_HISTOGRAM_GRADUATIONS_GRANULARITY * 3)
#endif // #if SCENE_NFRAMES_EDITABLE

// fonts and colors
#define INSCOPE_QUIET_COLOR    0x00ff00ff
#define INSCOPE_OPTIMAL_COLOR  0xffff00ff
#define INSCOPE_LOUD_COLOR     0xff0000ff
#define OUTSCOPE_QUIET_COLOR   0x00ff00ff
#define OUTSCOPE_OPTIMAL_COLOR 0xffff00ff
#define OUTSCOPE_LOUD_COLOR    0xff0000ff
#define HEADER_FONT            "/usr/share/fonts/truetype/tlwg/Purisa.ttf"
#define HEADER_TEXT_COLOR      { 255 , 0 , 255 }
#define STATUS_FONT            "/usr/share/fonts/truetype/tlwg/Purisa.ttf"
#define STATUS_TEXT_COLOR      { 255 , 0 , 255 }

// external images
#define SCOPE_IMG     "scope_gradient.bmp"
#define HISTOGRAM_IMG "histogram_gradient.bmp"
#define LOOP_IMG      "loop_gradient.argb.bmp"

// string constants
#define HEADER_TEXT                 "This is Loopidity"
#define X11_ERROR_MSG               "ERROR: LoopiditySdl::Init(): XGetWindowAttributes(): can't get root window geometry - quitting\n"
#define RESOLUTION_ERROR_MSG        "ERROR: screen resolution must be at least %dx%d - quitting\n"
#define SDL_ERROR_FMT               "ERROR: %s(): %s\n"
#define SDL_INIT_ERROR_TEXT         "SDL_Init"
#define SDL_SETVIDEOMODE_ERROR_TEXT "SDL_SetVideoMode"
#define SDL_KEYREPEAT_ERROR_TEXT    "SDL_EnableKeyRepeat()"
#define SDL_LOADBMP_ERROR_TEXT      "SDL_LoadBMP()"
#define TTF_ERROR_FMT               "ERROR: %s(): %s\n"
#define TTF_INIT_ERROR_MSG          "TTF_Init()"
#define TTF_OPENFONT_ERROR_MSG      "TTF_OpenFont()"

// flags
#define SDL_SCREEN_FLAGS SDL_HWSURFACE | SDL_DOUBLEBUF


class LoopiditySdl
{
  friend class Loopidity ;
  friend class SceneSdl ;
  friend class Trace ;

  private:

    /* class side private variables */

    // window
    static SDL_Surface* Screen ;
    static SDL_Rect     WinRect ;
    static Uint32       WinBgColor ;
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
    static string          StatusTextL ;
    static string          StatusTextC ;
    static string          StatusTextR ;

    // scenes
    static SceneSdl**   SdlScenes ;
    static SDL_Surface* ScopeGradient ;
    static SDL_Surface* HistogramGradient ;
    static SDL_Surface* LoopGradient ;

    // scopes
    static SDL_Rect        ScopeRect ;
    static SDL_Rect        MaskRect ;
    static SDL_Rect        GradientRect ;
    static const Sint16    Scope0 ;
    static const Uint16    ScopeR ;
    static const float     ScopePeakH ;
    static vector<Sample>* PeaksIn ;
    static vector<Sample>* PeaksOut ;
    static Sample*         PeaksTransient ;

    // DrawScenes() 'local' variables
    static Uint16       CurrentSceneN ;
    static Uint16       NextSceneN ;
    static Uint32       CurrentPeakN ;
    static Uint16       SceneProgress ;
    static Uint16       SceneN ;
    static SceneSdl*    SdlScene ;
    static SDL_Surface* SceneSurface ;
    static SDL_Rect*    SceneRect ;


    /* class side private functions */

    // setup
    static bool IsInitialized(void) ; // TODO: make singleton
    static bool Init(         SceneSdl** sdlScenes , vector<Sample>* peaksIn ,
                              vector<Sample>* peaksOut , Sample* peaksTransient) ;
    static void SdlError(     const char* functionName) ;
    static void TtfError(     const char* functionName) ;
    static void Cleanup(      void) ;

    // drawing
    static void DrawHeader(           void) ;
    static void BlankScreen(          void) ;
    static void DrawScenes(           void) ;
#if SCENE_NFRAMES_EDITABLE
    static void DrawEditScopes(       void) ;
    static void DrawTransientScopes(  void) ;
#else
    static void DrawScopes(           void) ;
#endif // #if SCENE_NFRAMES_EDITABLE
    static void DrawText(             string text , SDL_Surface* surface , TTF_Font* font ,
                                      SDL_Rect* screenRect , SDL_Rect* cropRect ,
                                      SDL_Color fgColor) ;
    static void DrawStatusArea(       void) ;
    static void FlipScreen(           void) ;
    static void Alert(                const char* msg) ;

    // getters/settters
    static void SetStatusL(string msg) ;
    static void SetStatusC(string msg) ;
    static void SetStatusR(string msg) ;

//    static Uint32 GetAvailableMemory() ;


// DEBUG begin
static Uint16 DbgFramerateTs ;
//static Uint64 DbgMainLoopTs ;
// DEBUG end
} ;


#endif // #ifndef _LOOPIDITY_SDL_H_
