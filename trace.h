
#ifndef _TRACE_H_
#define _TRACE_H_


#if DEBUG_TRACE
#define TRACE_EVS   Trace::TraceEvs
#define TRACE_IN    Trace::TraceIn
#define TRACE_OUT   Trace::TraceOut
#define TRACE_SCENE Trace::TraceScene
#define DEBUG_TRACE_LOOPIDITY_TOGGLESTATE_IN        if (TRACE_EVS(CurrentSceneN))   printf("\nUSER: SDLK_SPACE --> Loopidity::ToggleState(%d)\n\n" , CurrentSceneN) ; if (TRACE_IN(CurrentSceneN) && !TRACE_SCENE("Loopidity::ToggleState(%d)  IN" , Scenes[CurrentSceneN])) return ;
#define DEBUG_TRACE_LOOPIDITY_TOGGLESTATE_OUT       if (TRACE_OUT(CurrentSceneN))   TRACE_SCENE("Loopidity::ToggleState(%d) OUT" , Scenes[CurrentSceneN]) ;
#define DEBUG_TRACE_LOOPIDITY_TOGGLESCENE_IN        if (TRACE_EVS(CurrentSceneN))   printf("\nUSER: SDLK_KP0 --> Loopidity::ToggleScene(%d)\n\n" , CurrentSceneN) ; if (TRACE_IN(CurrentSceneN) && !TRACE_SCENE("Loopidity::ToggleScene(%d)  IN" , Scenes[CurrentSceneN])) return ;
#define DEBUG_TRACE_LOOPIDITY_TOGGLESCENE_OUT       if (TRACE_OUT(CurrentSceneN))   TRACE_SCENE("Loopidity::ToggleScene(%d) OUT" , Scenes[CurrentSceneN]) ;
#define DEBUG_TRACE_LOOPIDITY_DELETELASTLOOP_IN     if (TRACE_EVS(CurrentSceneN))   printf("\nUSER: SDLK_ESCAPE --> Loopidity::DeleteLastLoop(%d)\n\n" , CurrentSceneN) ; if (TRACE_IN(CurrentSceneN) && !TRACE_SCENE("Loopidity::DeleteLastLoop(%d)  IN" , Scenes[CurrentSceneN])) return ;
#define DEBUG_TRACE_LOOPIDITY_DELETELASTLOOP_OUT    if (TRACE_OUT(CurrentSceneN))   TRACE_SCENE("Loopidity::DeleteLoop(%d) OUT" , Scenes[CurrentSceneN]) ;
#define DEBUG_TRACE_LOOPIDITY_DELETELOOP_IN         if (TRACE_EVS(sceneN))          printf("\nUSER: SDL_BUTTON_MIDDLE --> Loopidity::DeleteLoop(%d)\n\n" , sceneN) ; if (TRACE_IN(sceneN) && !TRACE_SCENE("Loopidity::DeleteLoop(%d)  IN" , Scenes[sceneN])) return ;
#define DEBUG_TRACE_LOOPIDITY_DELETELOOP_OUT        if (TRACE_OUT(sceneN))          TRACE_SCENE("Loopidity::DeleteLoop(%d) OUT" , Scenes[sceneN]) ;
#define DEBUG_TRACE_LOOPIDITY_INCLOOPVOL_IN         if (TRACE_EVS(sceneN))          printf("\nUSER: %s --> Loopidity::IncLoopVol(%d)  IN vol=%f\n\n" , (isInc)? "SDL_BUTTON_WHEELUP" : "SDL_BUTTON_WHEELDOWN" , sceneN , Scenes[sceneN]->getLoop(loopN)->vol) ;
#define DEBUG_TRACE_LOOPIDITY_INCLOOPVOL_OUT        if (DEBUG_TRACE_OUT)            { char event[128] ; sprintf(event , "%s OUT vol=%3.1f\n\n" , "Loopidity::IncLoopVol(%d)" , *vol) ; TRACE_SCENE(event , Scenes[sceneN]) ; }
#define DEBUG_TRACE_LOOPIDITY_TOGGLELOOPISMUTED_IN  if (TRACE_EVS(sceneN))          { printf("\nUSER: SDLK_KP_ENTER --> Loopidity::ToggleLoopIsMuted(%d)\n\n" , sceneN) ; printf("\nUSER: SDL_BUTTON_LEFT --> Loopidity::ToggleLoopIsMuted(%d)\n\n" , sceneN) ; } if (TRACE_IN(sceneN) && !TRACE_SCENE("Loopidity::ToggleLoopIsMuted(%d)  IN" , Scenes[sceneN])) return ;
#define DEBUG_TRACE_LOOPIDITY_TOGGLELOOPISMUTED_OUT if (TRACE_OUT(sceneN))          TRACE_SCENE("Loopidity::ToggleLoopIsMuted(%d) OUT" , Scenes[sceneN]) ;
#define DEBUG_TRACE_LOOPIDITY_RESETSCENE_IN         if (TRACE_EVS(CurrentSceneN))   printf("\nUSER: KMOD_RSHIFT+SDLK_ESCAPE --> Loopidity::ResetScene(%d)\n\n" , sceneN) ; if (TRACE_IN(CurrentSceneN) && !TRACE_SCENE("Loopidity::ResetCurrentScene(%d)  IN" , Scenes[CurrentSceneN])) return ;
#define DEBUG_TRACE_LOOPIDITY_RESETSCENE_OUT        if (DEBUG_TRACE_OUT)            TRACE_SCENE("Loopidity::ResetScene(%d) OUT" , Scenes[sceneN]) ;
#define DEBUG_TRACE_LOOPIDITY_RESET_IN              if (TRACE_EVS(CurrentSceneN))   printf("\nUSER: KMOD_RCTRL+SDLK_ESCAPE --> Loopidity::Reset(%d)\n\n" , CurrentSceneN) ; if (TRACE_IN(CurrentSceneN) && !TRACE_SCENE("Loopidity::Reset(%d)  IN" , Scenes[CurrentSceneN])) return ;
#define DEBUG_TRACE_LOOPIDITY_RESET_OUT             if (TRACE_OUT(CurrentSceneN))   TRACE_SCENE("Loopidity::Reset(%d) OUT" , Scenes[CurrentSceneN]) ;
#define DEBUG_TRACE_LOOPIDITY_ONLOOPCREATION_IN     if (TRACE_EVS(sceneN))          printf("\nUSER: EVT_NEW_LOOP --> Loopidity::OnLoopCreation(%d)\n\n" , sceneN) ; if (TRACE_IN(sceneN) && !TRACE_SCENE("Loopidity::OnLoopCreation(%d)  IN" , Scenes[sceneN])) return ;
#define DEBUG_TRACE_LOOPIDITY_ONLOOPCREATION_OUT    if (TRACE_OUT(sceneN))          TRACE_SCENE("Loopidity::OnLoopCreation(%d) OUT" , Scenes[sceneN]) ;
#define DEBUG_TRACE_LOOPIDITY_ONSCENECHANGE_IN      if (TRACE_EVS(CurrentSceneN))   printf("\nUSER: EVT_SCENE_CHANGED --> Loopidity::OnSceneChange(%d)\n\n" , CurrentSceneN) ; if (TRACE_IN(CurrentSceneN) && !TRACE_SCENE("Loopidity::OnSceneChange(%d)  IN" , Scenes[CurrentSceneN])) return ;
#define DEBUG_TRACE_LOOPIDITY_ONSCENECHANGE_OUT     if (TRACE_OUT(nextSceneN))      TRACE_SCENE("Loopidity::OnSceneChange(%d)  OUT" , Scenes[nextSceneN]) ;
#define DEBUG_TRACE_LOOPIDITY_OOM_IN                                                TRACE_SCENE("Loopidity::OOM(%d)   IN" , Scenes[CurrentSceneN]) ;
#define DEBUG_TRACE_LOOPIDITYSDL_HANDLEKEYEVENT     if (DEBUG_TRACE_EVS)            switch (event->key.keysym.sym) { case SDLK_SPACE: printf("\nKEY: SDLK_SPACE\n") ; break ; case SDLK_KP0: printf("\nKEY: SDLK_KP0\n") ; break ; case SDLK_KP_ENTER: printf("\nKEY: SDLK_KP_ENTER\n") ; break ; case SDLK_ESCAPE: printf("\nKEY: SDLK_ESCAPE\n") ;  break ; default: break ; }
#define DEBUG_TRACE_SCENE_ADDLOOP_IN                if (TRACE_IN(sceneN)        && !TRACE_SCENE("Scene::addLoop(%d)  IN" , this)) return false ;
#define DEBUG_TRACE_SCENE_ADDLOOP_OUT               if (TRACE_OUT(sceneN))          TRACE_SCENE("Scene::addLoop(%d) OUT" , this) ;
#define DEBUG_TRACE_SCENE_DELETELOOP_IN             if (TRACE_IN(sceneN)        && !TRACE_SCENE("Scene::deleteLoop(%d)  IN" , this)) return ;
#define DEBUG_TRACE_SCENE_DELETELOOP_OUT            if (TRACE_OUT(sceneN))          TRACE_SCENE("Scene::deleteLoop(%d) OUT" , this) ;
#define DEBUG_TRACE_SCENE_RESET_IN                  if (TRACE_IN(sceneN)        && !TRACE_SCENE("Scene::reset(%d)  IN" , this)) return ; ;
#define DEBUG_TRACE_SCENE_RESET_OUT                 if (TRACE_OUT(sceneN))          TRACE_SCENE("Scene::reset(%d) OUT" , this) ;
#define DEBUG_TRACE_SCENE_SCANPEAKS_IN              if (TRACE_IN(sceneN)        && !TRACE_SCENE("Scene::scanPeaks(%d)  IN" , this)) return ;
#define DEBUG_TRACE_SCENE_SCANPEAKS_OUT             if (TRACE_OUT(sceneN))          TRACE_SCENE("Scene::scanPeaks(%d) OUT" , this) ;
#define DEBUG_TRACE_SCENE_RESCANPEAKS_IN            if (TRACE_IN(sceneN)        && !TRACE_SCENE("Scene::rescanPeaks(%d)  IN" , this)) return ;
#define DEBUG_TRACE_SCENE_RESCANPEAKS_OUT           if (TRACE_OUT(sceneN))          TRACE_SCENE("Scene::rescanPeaks(%d) OUT" , this) ;
#define DEBUG_TRACE_SCENE_TOGGLESTATE_IN            if (TRACE_IN(sceneN)        && !TRACE_SCENE("Scene::toggleState(%d)      IN" , this)) return ;
#define DEBUG_TRACE_SCENE_TOGGLESTATE_OUT           if (TRACE_OUT(sceneN))          TRACE_SCENE("Scene::toggleState(%d)     OUT" , this) ;
#define DEBUG_TRACE_SCENESDL_UPDATESTATUS_IN        if (TRACE_IN(scene->sceneN))    TRACE_SCENE("SceneSdl::updateState(%d)   IN" , scene) ;
#define DEBUG_TRACE_SCENESDL_UPDATESTATUS_OUT       if (TRACE_OUT(scene->sceneN))   TRACE_SCENE("SceneSdl::updateState(%d)  OUT" , scene) ;
#define DEBUG_TRACE_SCENESDL_ADDLOOP_IN             if (TRACE_IN(scene->sceneN) && (loopImgs.size() != scene->loops.size() - 1) && !TRACE_SCENE("SceneSdl::addLoop(%d) ERR" , scene)) return ;
#define DEBUG_TRACE_SCENESDL_ADDLOOP_OUT            if (TRACE_OUT(scene->sceneN))   TRACE_SCENE("SceneSdl::addLoop(%d) OUT" , scene) ;
#define DEBUG_TRACE_SCENESDL_DELETELOOP_IN          if (TRACE_IN(scene->sceneN) && !TRACE_SCENE("SceneSdl::deleteLoop(%d)  IN" , scene)) return ;
#define DEBUG_TRACE_SCENESDL_DELETELOOP_OUT         if (TRACE_OUT(scene->sceneN))   TRACE_SCENE("SceneSdl::deleteLoop(%d) OUT" , scene) ;
#else
#define DEBUG_TRACE_LOOPIDITY_TOGGLESTATE_IN        ;
#define DEBUG_TRACE_LOOPIDITY_TOGGLESTATE_OUT       ;
#define DEBUG_TRACE_LOOPIDITY_TOGGLESCENE_IN        ;
#define DEBUG_TRACE_LOOPIDITY_TOGGLESCENE_OUT       ;
#define DEBUG_TRACE_LOOPIDITY_DELETELASTLOOP_IN     ;
#define DEBUG_TRACE_LOOPIDITY_DELETELASTLOOP_OUT    ;
#define DEBUG_TRACE_LOOPIDITY_DELETELOOP_IN         ;
#define DEBUG_TRACE_LOOPIDITY_DELETELOOP_OUT        ;
#define DEBUG_TRACE_LOOPIDITY_INCLOOPVOL_IN         ;
#define DEBUG_TRACE_LOOPIDITY_INCLOOPVOL_OUT        ;
#define DEBUG_TRACE_LOOPIDITY_TOGGLELOOPISMUTED_IN  ;
#define DEBUG_TRACE_LOOPIDITY_TOGGLELOOPISMUTED_OUT ;
#define DEBUG_TRACE_LOOPIDITY_RESETSCENE_IN         ;
#define DEBUG_TRACE_LOOPIDITY_RESETSCENE_OUT        ;
#define DEBUG_TRACE_LOOPIDITY_RESET_IN              ;
#define DEBUG_TRACE_LOOPIDITY_RESET_OUT             ;
#define DEBUG_TRACE_LOOPIDITY_ONLOOPCREATION_IN     ;
#define DEBUG_TRACE_LOOPIDITY_ONLOOPCREATION_OUT    ;
#define DEBUG_TRACE_LOOPIDITY_ONSCENECHANGE_IN      ;
#define DEBUG_TRACE_LOOPIDITY_ONSCENECHANGE_OUT     ;
#define DEBUG_TRACE_LOOPIDITY_OOM_IN                ;
#define DEBUG_TRACE_LOOPIDITYSDL_HANDLEKEYEVENT     ;
#define DEBUG_TRACE_SCENE_ADDLOOP_IN                ;
#define DEBUG_TRACE_SCENE_ADDLOOP_OUT               ;
#define DEBUG_TRACE_SCENE_DELETELOOP_IN             ;
#define DEBUG_TRACE_SCENE_DELETELOOP_OUT            ;
#define DEBUG_TRACE_SCENE_RESET_IN                  ;
#define DEBUG_TRACE_SCENE_RESET_OUT                 ;
#define DEBUG_TRACE_SCENE_SCANPEAKS_IN              ;
#define DEBUG_TRACE_SCENE_SCANPEAKS_OUT             ;
#define DEBUG_TRACE_SCENE_RESCANPEAKS_IN            ;
#define DEBUG_TRACE_SCENE_RESCANPEAKS_OUT           ;
#define DEBUG_TRACE_SCENE_TOGGLESTATE_IN            ;
#define DEBUG_TRACE_SCENE_TOGGLESTATE_OUT           ;
#define DEBUG_TRACE_SCENESDL_UPDATESTATUS_IN        ;
#define DEBUG_TRACE_SCENESDL_UPDATESTATUS_OUT       ;
#define DEBUG_TRACE_SCENESDL_ADDLOOP_IN             ;
#define DEBUG_TRACE_SCENESDL_ADDLOOP_OUT            ;
#define DEBUG_TRACE_SCENESDL_DELETELOOP_IN          ;
#define DEBUG_TRACE_SCENESDL_DELETELOOP_OUT         ;
#endif // #if DEBUG_TRACE

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

#define DEBUG_TRACE_EVENT_LEN 36
#define DEBUG_TRACE_STATE_LEN 6
#define DEBUG_TRACE_DESC_LEN  128


#include <iostream>


class Trace
{
  public:
		static bool SanityCheck(unsigned int sceneN) ;
		static bool TraceEvs(   unsigned int sceneN) ;
		static bool TraceIn(    unsigned int sceneN) ;
		static bool TraceOut(   unsigned int sceneN) ;
		static bool TraceScene( const char* senderTemplate , Scene* scene) ;
    static void TraceState( const char* event , const char* sender ,
                            const char* stateFormat , const char* descFormat ,
                            bool bool0 , bool bool1 , bool bool2 , bool isEq) ;

		// constants
		static const char         *MODEL            , *MODEL_ERR      , *VIEW    , *VIEW_ERR ;
		static const char         *MODEL_STATE_FMT  , *VIEW_STATE_FMT ;
		static const char         *MODEL_DESC_FMT   , *VIEW_DESC_FMT  ;
		static const char         *MODEL_ERR_FMT    , *VIEW_ERR_FMT   ;
		static const unsigned int EVENT_LEN         , STATE_LEN       , DESC_LEN ;

	private:

		// buffers
		static char         Event[DEBUG_TRACE_EVENT_LEN + 1] ;
		static char         State[DEBUG_TRACE_STATE_LEN + 1] ;
		static char         Desc[DEBUG_TRACE_DESC_LEN + 1] ;
		static const char   *EventType , *SenderClass , *StateFormat , *DescFormat ;
		static unsigned int EventLen   , SenderLen    , StateLen     , DescLen ;

#if DRAW_DEBUG_TEXT
public: static void SetDbgTextL() ; public: static void SetDbgTextR() ;
#endif // #if DRAW_DEBUG_TEXT
} ;


#endif // #if _TRACE_H_
