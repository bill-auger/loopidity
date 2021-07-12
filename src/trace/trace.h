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


#ifndef _TRACE_H_
#define _TRACE_H_

#include "../constants/feature_constants.h"

#define DBG(d)      Trace::Dbg(d)
#define ERR(d)      Trace::Err(d)
#define TRACE_EVS   Trace::TraceEvs
#define TRACE_IN    Trace::TraceIn
#define TRACE_OUT   Trace::TraceOut
#define TRACE_SCENE Trace::TraceScene

#if DEBUG_TRACE
#  ifdef _WIN32
#    define REDIRECT_WINDOWS_OUTPUT std::ofstream ofs("debug.log") ; cout.rdbuf(ofs.rdbuf()) ;
#  else // _WIN32
#    define REDIRECT_WINDOWS_OUTPUT ;
#  endif // _WIN32
#  define DEBUG_TRACE_MAIN_IN  REDIRECT_WINDOWS_OUTPUT if (DEBUG_TRACE_EVS) DBG(INIT_MSG) ;
#  define DEBUG_TRACE_MAIN_OUT if (!!exitStatus) ERR(INIT_FAIL_MSG) ;
#else
#  define DEBUG_TRACE_MAIN_IN  ;
#  define DEBUG_TRACE_MAIN_OUT ;
#endif

#if DRAW_DEBUG_TEXT
#  define DRAW_DEBUG_TEXT_L Trace::SetDbgTextC() ;
#  define DRAW_DEBUG_TEXT_R Trace::SetDbgTextR() ;
#else
#  define DRAW_DEBUG_TEXT_L ;
#  define DRAW_DEBUG_TEXT_R ;
#endif // #if DRAW_DEBUG_TEXT

#if DEBUG_TRACE_LOOPIDITY
#  define DEBUG_TRACE_LOOPIDITY_MAIN_MID                 if (DEBUG_TRACE_EVS) printf("%s\n" , INIT_SUCCESS_MSG) ;
#  define DEBUG_TRACE_LOOPIDITY_MAIN_OUT                 if (DEBUG_TRACE_EVS) printf("%s\n" , EXIT_SUCCESS_MSG) ;
#  define DEBUG_TRACE_LOOPIDITY_TOGGLERECORDINGSTATE_IN  if (TRACE_EVS(CurrentSceneN)) printf("\nUSER: SDLK_SPACE --> Loopidity::ToggleRecordingState(%d)\n\n" , CurrentSceneN) ; if (TRACE_IN(CurrentSceneN) && !TRACE_SCENE("Loopidity::ToggleRecordingState(%d)  IN" , Scenes[CurrentSceneN])) return ;
#  define DEBUG_TRACE_LOOPIDITY_TOGGLERECORDINGSTATE_OUT if (TRACE_OUT(CurrentSceneN)) TRACE_SCENE("Loopidity::ToggleRecordingState(%d) OUT" , Scenes[CurrentSceneN]) ;
#  define DEBUG_TRACE_LOOPIDITY_TOGGLENEXTSCENE_IN       if (TRACE_EVS(CurrentSceneN)) printf("\nUSER: SDLK_KP0 --> Loopidity::ToggleNextScene(%d)\n\n" , CurrentSceneN) ; if (TRACE_IN(CurrentSceneN) && !TRACE_SCENE("Loopidity::ToggleNextScene(%d)  IN" , Scenes[CurrentSceneN])) return ;
#  define DEBUG_TRACE_LOOPIDITY_TOGGLENEXTSCENE_OUT      if (TRACE_OUT(CurrentSceneN)) TRACE_SCENE("Loopidity::ToggleNextScene(%d) OUT" , Scenes[CurrentSceneN]) ;
#  define DEBUG_TRACE_LOOPIDITY_DELETELASTLOOP_IN        if (TRACE_EVS(CurrentSceneN)) printf("\nUSER: SDLK_ESCAPE --> Loopidity::DeleteLastLoop(%d)\n\n" , CurrentSceneN) ; if (TRACE_IN(CurrentSceneN) && !TRACE_SCENE("Loopidity::DeleteLastLoop(%d)  IN" , Scenes[CurrentSceneN])) return ;
#  define DEBUG_TRACE_LOOPIDITY_DELETELASTLOOP_OUT       if (TRACE_OUT(CurrentSceneN)) TRACE_SCENE("Loopidity::DeleteLoop(%d) OUT" , Scenes[CurrentSceneN]) ;
#  define DEBUG_TRACE_LOOPIDITY_DELETELOOP_IN            if (TRACE_EVS(sceneN))        printf("\nUSER: SDL_BUTTON_MIDDLE --> Loopidity::DeleteLoop(%d)\n\n" , sceneN) ; if (TRACE_IN(sceneN) && !TRACE_SCENE("Loopidity::DeleteLoop(%d)  IN" , Scenes[sceneN])) return ;
#  define DEBUG_TRACE_LOOPIDITY_DELETELOOP_OUT           if (TRACE_OUT(sceneN))        TRACE_SCENE("Loopidity::DeleteLoop(%d) OUT" , Scenes[sceneN]) ;
#  define DEBUG_TRACE_LOOPIDITY_INCLOOPVOL_IN            if (TRACE_EVS(sceneN))        printf("\nUSER: %s --> Loopidity::IncLoopVol(%d)  IN vol=%f\n\n" , (isInc)? "SDL_BUTTON_WHEELUP" : "SDL_BUTTON_WHEELDOWN" , sceneN , Scenes[sceneN]->getLoop(loopN)->vol) ;
#  define DEBUG_TRACE_LOOPIDITY_INCLOOPVOL_OUT           if (DEBUG_TRACE_OUT)          { char event[128] ; sprintf(event , "%s OUT vol=%3.1f\n\n" , "Loopidity::IncLoopVol(%d)" , *vol) ; TRACE_SCENE(event , Scenes[sceneN]) ; }
#  define DEBUG_TRACE_LOOPIDITY_TOGGLELOOPISMUTED_IN     if (TRACE_EVS(sceneN))        { printf("\nUSER: SDLK_KP_ENTER --> Loopidity::ToggleLoopIsMuted(%d)\n\n" , sceneN) ; printf("\nUSER: SDL_BUTTON_LEFT --> Loopidity::ToggleLoopIsMuted(%d)\n\n" , sceneN) ; } if (TRACE_IN(sceneN) && !TRACE_SCENE("Loopidity::ToggleLoopIsMuted(%d)  IN" , Scenes[sceneN])) return ;
#  define DEBUG_TRACE_LOOPIDITY_TOGGLELOOPISMUTED_OUT    if (TRACE_OUT(sceneN))        TRACE_SCENE("Loopidity::ToggleLoopIsMuted(%d) OUT" , Scenes[sceneN]) ;
#  define DEBUG_TRACE_LOOPIDITY_RESETSCENE_IN            if (TRACE_EVS(CurrentSceneN)) printf("\nUSER: KMOD_RSHIFT+SDLK_ESCAPE --> Loopidity::ResetScene(%d)\n\n" , sceneN) ; if (TRACE_IN(CurrentSceneN) && !TRACE_SCENE("Loopidity::ResetCurrentScene(%d)  IN" , Scenes[CurrentSceneN])) return ;
#  define DEBUG_TRACE_LOOPIDITY_RESETSCENE_OUT           if (DEBUG_TRACE_OUT)          TRACE_SCENE("Loopidity::ResetScene(%d) OUT" , Scenes[sceneN]) ;
#  define DEBUG_TRACE_LOOPIDITY_RESET_IN                 if (TRACE_EVS(CurrentSceneN)) printf("\nUSER: KMOD_RCTRL+SDLK_ESCAPE --> Loopidity::Reset(%d)\n\n" , CurrentSceneN) ; if (TRACE_IN(CurrentSceneN) && !TRACE_SCENE("Loopidity::Reset(%d)  IN" , Scenes[CurrentSceneN])) return ;
#  define DEBUG_TRACE_LOOPIDITY_RESET_OUT                if (TRACE_OUT(CurrentSceneN)) TRACE_SCENE("Loopidity::Reset(%d) OUT" , Scenes[CurrentSceneN]) ;
#  define DEBUG_TRACE_LOOPIDITY_ONLOOPCREATION_IN        if (TRACE_EVS(*sceneNum))     printf("\nUSER: EVT_NEW_LOOP --> Loopidity::OnLoopCreation(%d)\n\n" , *sceneNum) ; if (TRACE_IN(*sceneNum) && !TRACE_SCENE("Loopidity::OnLoopCreation(%d)  IN" , Scenes[*sceneNum])) return ;
#  define DEBUG_TRACE_LOOPIDITY_ONLOOPCREATION_OUT       if (TRACE_OUT(sceneN))        TRACE_SCENE("Loopidity::OnLoopCreation(%d) OUT" , Scenes[sceneN]) ;
#  define DEBUG_TRACE_LOOPIDITY_ONSCENECHANGE_IN         if (TRACE_EVS(CurrentSceneN)) printf("\nUSER: EVT_SCENE_CHANGED --> Loopidity::OnSceneChange(%d)\n\n" , CurrentSceneN) ; if (TRACE_IN(CurrentSceneN) && !TRACE_SCENE("Loopidity::OnSceneChange(%d)  IN" , Scenes[CurrentSceneN])) return ;
#  define DEBUG_TRACE_LOOPIDITY_ONSCENECHANGE_OUT        if (TRACE_OUT(NextSceneN))    TRACE_SCENE("Loopidity::OnSceneChange(%d)  OUT" , nextScene) ;
#  define DEBUG_TRACE_LOOPIDITY_OOM_IN                                                 TRACE_SCENE("Loopidity::OOM(%d)   IN" , Scenes[CurrentSceneN]) ;
#  define DEBUG_TRACE_LOOPIDITY_CLEANUP                  if (DEBUG_TRACE_EVS)          std::cout << "Loopidity::Cleanup()" << std::endl ;
#else // #if DEBUG_TRACE_LOOPIDITY
#  define DEBUG_TRACE_LOOPIDITY_MAIN_MID                 ;
#  define DEBUG_TRACE_LOOPIDITY_MAIN_OUT                 ;
#  define DEBUG_TRACE_LOOPIDITY_TOGGLERECORDINGSTATE_IN  ;
#  define DEBUG_TRACE_LOOPIDITY_TOGGLERECORDINGSTATE_OUT ;
#  define DEBUG_TRACE_LOOPIDITY_TOGGLENEXTSCENE_IN       ;
#  define DEBUG_TRACE_LOOPIDITY_TOGGLENEXTSCENE_OUT      ;
#  define DEBUG_TRACE_LOOPIDITY_DELETELASTLOOP_IN        ;
#  define DEBUG_TRACE_LOOPIDITY_DELETELASTLOOP_OUT       ;
#  define DEBUG_TRACE_LOOPIDITY_DELETELOOP_IN            ;
#  define DEBUG_TRACE_LOOPIDITY_DELETELOOP_OUT           ;
#  define DEBUG_TRACE_LOOPIDITY_INCLOOPVOL_IN            ;
#  define DEBUG_TRACE_LOOPIDITY_INCLOOPVOL_OUT           ;
#  define DEBUG_TRACE_LOOPIDITY_TOGGLELOOPISMUTED_IN     ;
#  define DEBUG_TRACE_LOOPIDITY_TOGGLELOOPISMUTED_OUT    ;
#  define DEBUG_TRACE_LOOPIDITY_RESETSCENE_IN            ;
#  define DEBUG_TRACE_LOOPIDITY_RESETSCENE_OUT           ;
#  define DEBUG_TRACE_LOOPIDITY_RESET_IN                 ;
#  define DEBUG_TRACE_LOOPIDITY_RESET_OUT                ;
#  define DEBUG_TRACE_LOOPIDITY_ONLOOPCREATION_IN        ;
#  define DEBUG_TRACE_LOOPIDITY_ONLOOPCREATION_OUT       ;
#  define DEBUG_TRACE_LOOPIDITY_ONSCENECHANGE_IN         ;
#  define DEBUG_TRACE_LOOPIDITY_ONSCENECHANGE_OUT        ;
#  define DEBUG_TRACE_LOOPIDITY_OOM_IN                   ;
#  define DEBUG_TRACE_LOOPIDITY_CLEANUP                  ;
#endif // #if DEBUG_TRACE_LOOPIDITY

#if DEBUG_TRACE_JACK
#  define DEBUG_TRACE_JACK_INIT                      printf("JackIO::Init() shouldMonitorInputs=%d recordBufferSize=%d\n" , shouldMonitorInputs , recordBufferSize) ;
#  define DEBUG_TRACE_JACK_RESET                     printf("JackIO::Reset() sceneN=%d\n" , currentScene->sceneN) ;
#  define DEBUG_TRACE_JACK_PROCESS_CALLBACK_IN       ; // unsigned int DbgNextFrameN = (CurrentScene->currentFrameN + nFramesPerPeriod) ; if (DbgNextFrameN >= CurrentScene->endFrameN || !(DbgNextFrameN % 32768)) printf("JackIO::ProcessCallback() sceneN=%d currentFrameN=%d nFramesPerPeriod=%d CurrentScene->endFrameN=%d mod=%d\n" , CurrentScene->sceneN , CurrentScene->currentFrameN , nFramesPerPeriod , CurrentScene->endFrameN , ((CurrentScene->currentFrameN + nFramesPerPeriod) % CurrentScene->endFrameN)) ;
#  define DEBUG_TRACE_JACK_PROCESS_CALLBACK_ROLLOVER printf("JackIO::ProcessCallback() buffer rollover nLoops=%d isBaseLoop=%d beginFrameN=%d endFrameN=%d nSeconds=%d - %s\n" , nLoops , isBaseLoop , beginFrameN , endFrameN , (nFrames / SampleRate) , ((!isBaseLoop)? "" : ((endFrameN == EndFrameN)? "endFrameN invalid" : ((nFrames < MinLoopSize)? "nFrames invalid" : "valid")))) ;
#  define DEBUG_TRACE_JACK_PROCESS_CALLBACK_NEW_LOOP printf("JackIO::ProcessCallback() NewLoop isBaseLoop=%d\n" , isBaseLoop) ;
#  define DEBUG_TRACE_JACK_SETMETADATA               printf("JackIO::SetMetadata() SampleRate=%d nFramesPerPeriod=%d BeginFrameN=%d EndFrameN=%d modsane=%d\n" , SampleRate , nFramesPerPeriod , BeginFrameN , EndFrameN , (!(BeginFrameN % nFramesPerPeriod) && !(EndFrameN % nFramesPerPeriod))) ;
#else
#  define DEBUG_TRACE_JACK_INIT                      ;
#  define DEBUG_TRACE_JACK_RESET                     ;
#  define DEBUG_TRACE_JACK_PROCESS_CALLBACK_IN       ;
#  define DEBUG_TRACE_JACK_PROCESS_CALLBACK_ROLLOVER ;
#  define DEBUG_TRACE_JACK_PROCESS_CALLBACK_NEW_LOOP ;
#  define DEBUG_TRACE_JACK_SETMETADATA               ;
#endif // #if DEBUG_TRACE_JACK

#if DEBUG_TRACE_LOOPIDITYSDL
#  define DEBUG_TRACE_LOOPIDITYSDL_HANDLEKEYEVENT if (DEBUG_TRACE_EVS) switch (event->key.keysym.sym) { case SDLK_SPACE: printf("\nKEY: SDLK_SPACE\n") ; break ; case SDLK_KP0: printf("\nKEY: SDLK_KP0\n") ; break ; case SDLK_KP_ENTER: printf("\nKEY: SDLK_KP_ENTER\n") ; break ; case SDLK_ESCAPE: printf("\nKEY: SDLK_ESCAPE\n") ;  break ; default: break ; }
#else
#  define DEBUG_TRACE_LOOPIDITYSDL_HANDLEKEYEVENT ;
#endif // #if DEBUG_TRACE_LOOPIDITYSDL

#if DEBUG_TRACE_SCENE
#  define DEBUG_TRACE_SCENE_BEGINRECORDING_IN        if (TRACE_IN(sceneN) && !TRACE_SCENE("Scene::beginRecording(%d)     IN" , this)) return ; ;
#  define DEBUG_TRACE_SCENE_TOGGLERECORDINGSTATE_IN  if (TRACE_IN(sceneN) && !TRACE_SCENE("Scene::toggleRecordingState(%d)      IN" , this)) return ;
#  define DEBUG_TRACE_SCENE_TOGGLERECORDINGSTATE_OUT if (TRACE_OUT(sceneN))   TRACE_SCENE("Scene::toggleRecordingState(%d)     OUT" , this) ;
#  define DEBUG_TRACE_SCENE_ADDLOOP_IN               if (TRACE_IN(sceneN) && !TRACE_SCENE("Scene::addLoop(%d)  IN" , this)) return false ;
#  define DEBUG_TRACE_SCENE_ADDLOOP_OUT              if (TRACE_OUT(sceneN))   TRACE_SCENE("Scene::addLoop(%d) OUT" , this) ;
#  define DEBUG_TRACE_SCENE_DELETELOOP_IN            if (TRACE_IN(sceneN) && !TRACE_SCENE("Scene::deleteLoop(%d)     IN" , this)) return ;
#  define DEBUG_TRACE_SCENE_DELETELOOP_OUT           if (TRACE_OUT(sceneN))   TRACE_SCENE("Scene::deleteLoop(%d)    OUT" , this) ;
#  define DEBUG_TRACE_SCENE_RESET_IN                 if (TRACE_IN(sceneN) && !TRACE_SCENE("Scene::reset(%d)  IN" , this)) return ; ;
#  define DEBUG_TRACE_SCENE_RESET_OUT                if (TRACE_OUT(sceneN))   TRACE_SCENE("Scene::reset(%d) OUT" , this) ;
#  define DEBUG_TRACE_SCENE_SCANPEAKS_IN             if (TRACE_IN(sceneN) && !TRACE_SCENE("Scene::scanPeaks(%d)  IN" , this)) return ;
#  define DEBUG_TRACE_SCENE_SCANPEAKS_OUT            if (TRACE_OUT(sceneN))   TRACE_SCENE("Scene::scanPeaks(%d) OUT" , this) ;
#  define DEBUG_TRACE_SCENE_RESCANPEAKS_IN           if (TRACE_IN(sceneN) && !TRACE_SCENE("Scene::rescanPeaks(%d)  IN" , this)) return ;
#  define DEBUG_TRACE_SCENE_RESCANPEAKS_OUT          if (TRACE_OUT(sceneN))   TRACE_SCENE("Scene::rescanPeaks(%d) OUT" , this) ;
#else
#  define DEBUG_TRACE_SCENE_BEGINRECORDING_IN        ;
#  define DEBUG_TRACE_SCENE_TOGGLERECORDINGSTATE_IN  ;
#  define DEBUG_TRACE_SCENE_TOGGLERECORDINGSTATE_OUT ;
#  define DEBUG_TRACE_SCENE_ADDLOOP_IN               ;
#  define DEBUG_TRACE_SCENE_ADDLOOP_OUT              ;
#  define DEBUG_TRACE_SCENE_DELETELOOP_IN            ;
#  define DEBUG_TRACE_SCENE_DELETELOOP_OUT           ;
#  define DEBUG_TRACE_SCENE_RESET_IN                 ;
#  define DEBUG_TRACE_SCENE_RESET_OUT                ;
#  define DEBUG_TRACE_SCENE_SCANPEAKS_IN             ;
#  define DEBUG_TRACE_SCENE_SCANPEAKS_OUT            ;
#  define DEBUG_TRACE_SCENE_RESCANPEAKS_IN           ;
#  define DEBUG_TRACE_SCENE_RESCANPEAKS_OUT          ;
#endif // #if DEBUG_TRACE_SCENE

#if DEBUG_TRACE_SCENESDL
#  define DEBUG_TRACE_SCENESDL_UPDATESTATUS_IN  if (TRACE_IN(scene->sceneN))    TRACE_SCENE("SceneSdl::updateState(%d)   IN" , scene) ;
#  define DEBUG_TRACE_SCENESDL_UPDATESTATUS_OUT if (TRACE_OUT(scene->sceneN))   TRACE_SCENE("SceneSdl::updateState(%d)  OUT" , scene) ;
#  define DEBUG_TRACE_SCENESDL_ADDLOOP_IN       if (TRACE_IN(scene->sceneN) && (loopImgs.size() != scene->loops.size() - 1) && !TRACE_SCENE("SceneSdl::addLoop(%d) ERR" , scene)) return ;
#  define DEBUG_TRACE_SCENESDL_ADDLOOP_OUT      if (TRACE_OUT(scene->sceneN))   TRACE_SCENE("SceneSdl::addLoop(%d) OUT" , scene) ;
#  define DEBUG_TRACE_SCENESDL_DELETELOOP_IN    if (TRACE_IN(scene->sceneN) && !TRACE_SCENE("SceneSdl::deleteLoop(%d)   IN" , scene)) return ;
#  define DEBUG_TRACE_SCENESDL_DELETELOOP_OUT   if (TRACE_OUT(scene->sceneN))   TRACE_SCENE("SceneSdl::deleteLoop(%d)  OUT" , scene) ;
#else
#  define DEBUG_TRACE_SCENESDL_UPDATESTATUS_IN  ;
#  define DEBUG_TRACE_SCENESDL_UPDATESTATUS_OUT ;
#  define DEBUG_TRACE_SCENESDL_ADDLOOP_IN       ;
#  define DEBUG_TRACE_SCENESDL_ADDLOOP_OUT      ;
#  define DEBUG_TRACE_SCENESDL_DELETELOOP_IN    ;
#  define DEBUG_TRACE_SCENESDL_DELETELOOP_OUT   ;
#endif // #if DEBUG_TRACE_SCENESDL


#define INIT_MSG          "main(): init"
#define INIT_SUCCESS_MSG  "Loopidity::Main(): init success - entering sdl loop"
#define INIT_FAIL_MSG     "Loopidity::Main(): init failed - quitting"
#define EXIT_SUCCESS_MSG  "Loopidity::Main(): done - exited sdl loop"
#define GETPEAK_ERROR_MSG "Loopidity::GetPeak(): subscript out of range"

#define DEBUG_TRACE_MODEL            "MODEL: "
#define DEBUG_TRACE_MODEL_ERROR      "ERROR: "
#define DEBUG_TRACE_VIEW             "VIEW:  "
#define DEBUG_TRACE_VIEW_ERROR       "ERROR: "
#define DEBUG_TRACE_CONTROLLER       DEBUG_TRACE_MODEL
#define DEBUG_TRACE_CONTROLLER_ERROR DEBUG_TRACE_MODEL_ERROR

#define DEBUG_TRACE_MODEL_STATE_FORMAT "%d%d%d   "
#define DEBUG_TRACE_VIEW_STATE_FORMAT  "   %d%d%d"

#define DEBUG_TRACE_MODEL_DESC_FORMAT  "isRecording=%d shouldSaveLoop=%d doesPulseExist=%d"
#define DEBUG_TRACE_VIEW_DESC_FORMAT   "nLoops=%d nHistogramImgs=%d nLoopImgs=%d"
#define DEBUG_TRACE_MODEL_ERROR_FORMAT "MODEL: Scene::mvc inconsistent"
#define DEBUG_TRACE_VIEW_ERROR_FORMAT  "VIEW:  SceneSdl::mvc inconsistent"

#define DEBUG_TRACE_EVENT_LEN        36
#define DEBUG_TRACE_STATE_LEN        6
#define DEBUG_TRACE_DESC_LEN         128
#define DEBUG_TRACE_TRACE_STATE_LEN  255


#include <iostream>

class Scene ;


class Trace
{
  public:

    /* class side public constants */

    // constants
    static const char         *MODEL , *MODEL_ERR , *VIEW , *VIEW_ERR ;
    static const unsigned int  EVENT_LEN , STATE_LEN , DESC_LEN , TRACE_STATE_LEN ;
    static const char         *MODEL_STATE_FMT , *VIEW_STATE_FMT ;
    static const char         *MODEL_DESC_FMT  , *VIEW_DESC_FMT  ;
    static const char         *MODEL_ERR_FMT   , *VIEW_ERR_FMT   ;


  private:

    /* class side private variables */

    // buffers
    static       char          Event[DEBUG_TRACE_EVENT_LEN + 1] ;
    static       char          State[DEBUG_TRACE_STATE_LEN + 1] ;
    static       char          Desc [DEBUG_TRACE_DESC_LEN  + 1] ;
    static const char         *EventType , *SenderClass , *StateFormat , *DescFormat ;
    static       unsigned int  EventLen  ,  SenderLen   ,  StateLen    ,  DescLen ;


  public:

    /* class side public functions */

    static bool SanityCheck(unsigned int sceneN) ;
    static void Dbg(        std::string msg) ;
    static void Err(        std::string msg) ;
    static bool TraceEvs(   unsigned int sceneN) ;
    static bool TraceIn(    unsigned int sceneN) ;
    static bool TraceOut(   unsigned int sceneN) ;
    static bool TraceScene( const char* senderTemplate , Scene* scene) ;
    static void TraceState( const char* event       , const char* sender     ,
                            const char* stateFormat , const char* descFormat ,
                            bool bool0 , bool bool1 , bool bool2 , bool isEq ) ;

#if DRAW_DEBUG_TEXT
    static void SetDbgTextC() ; public: static void SetDbgTextR() ;
#endif // #if DRAW_DEBUG_TEXT
} ;


#endif // #ifndef _TRACE_H_
