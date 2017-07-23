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


#include "loopidity.h"


/* Trace class side public constants */

/* constants */

const char* Trace::MODEL     = DEBUG_TRACE_MODEL ;
const char* Trace::MODEL_ERR = DEBUG_TRACE_MODEL_ERROR ;
const char* Trace::VIEW      = DEBUG_TRACE_VIEW ;
const char* Trace::VIEW_ERR  = DEBUG_TRACE_VIEW_ERROR ;

const Uint32 Trace::EVENT_LEN       = DEBUG_TRACE_EVENT_LEN ;
const Uint32 Trace::STATE_LEN       = DEBUG_TRACE_STATE_LEN ;
const Uint32 Trace::DESC_LEN        = DEBUG_TRACE_DESC_LEN ;
const Uint32 Trace::TRACE_STATE_LEN = DEBUG_TRACE_TRACE_STATE_LEN ;

const char* Trace::MODEL_STATE_FMT = DEBUG_TRACE_MODEL_STATE_FORMAT ;
const char* Trace::VIEW_STATE_FMT  = DEBUG_TRACE_VIEW_STATE_FORMAT ;
const char* Trace::MODEL_DESC_FMT  = DEBUG_TRACE_MODEL_DESC_FORMAT ;
const char* Trace::VIEW_DESC_FMT   = DEBUG_TRACE_VIEW_DESC_FORMAT ;
const char* Trace::MODEL_ERR_FMT   = DEBUG_TRACE_MODEL_ERROR_FORMAT ;
const char* Trace::VIEW_ERR_FMT    = DEBUG_TRACE_VIEW_ERROR_FORMAT ;


/* Trace class side private variables */

/* buffers */

char Trace::Event[EVENT_LEN + 1] = {0} ;
char Trace::State[STATE_LEN + 1] = {0} ;
char Trace::Desc [DESC_LEN  + 1] = {0} ;

Uint32 Trace::EventLen  = 0 ;
Uint32 Trace::SenderLen = 0 ;
Uint32 Trace::StateLen  = 0 ;
Uint32 Trace::DescLen   = 0 ;


/* Trace class side public functions */

bool Trace::SanityCheck(Uint32 sceneN)
{
  Scene*    scene    = Loopidity::Scenes   [sceneN] ;
  SceneSdl* sdlScene = Loopidity::SdlScenes[sceneN] ;

  Uint32 nLoops         = scene   ->loops        .size() ;
  Uint32 nHistogramImgs = sdlScene->histogramImgs.size() ;
  Uint32 nLoopImgs      = sdlScene->loopImgs     .size() ;

  return (nLoops == nHistogramImgs && nLoops == nLoopImgs) ;
}

void Trace::Dbg(std::string msg) { std::cout << "DBG: "   << msg << std::endl ; }

void Trace::Err(std::string msg) { std::cout << "ERROR: " << msg << std::endl ; }

#if DEBUG_TRACE
bool Trace::TraceEvs(Uint32 sceneN) { return ((DEBUG_TRACE_EVS) || !SanityCheck(sceneN)) ; }

bool Trace::TraceIn(Uint32 sceneN)  { return ((DEBUG_TRACE_IN ) || !SanityCheck(sceneN)) ; }

bool Trace::TraceOut(Uint32 sceneN) { return ((DEBUG_TRACE_OUT) || !SanityCheck(sceneN)) ; }
#endif // #if DEBUG_TRACE

bool Trace::TraceScene(const char* senderTemplate , Scene* scene)
{
  Uint32 sceneN = scene->sceneN ; SceneSdl* sdlScene = Loopidity::SdlScenes[sceneN] ;
  char sender[EVENT_LEN] ; snprintf(sender , EVENT_LEN , senderTemplate , sceneN) ;

  // mvc sanity checks
  bool isEq = SanityCheck(sceneN) ;
  const char *modelEvent , *modelDescFormat , *viewEvent , *viewDescFormat ;
  if (isEq)
  {
    modelEvent = MODEL ; modelDescFormat = MODEL_DESC_FMT ;
    viewEvent = VIEW ;   viewDescFormat  = VIEW_DESC_FMT ;
  }
  else
  {
    modelEvent = MODEL_ERR ; modelDescFormat = MODEL_ERR_FMT ;
    viewEvent  = VIEW_ERR ;  viewDescFormat  = VIEW_ERR_FMT ;
  }

  // model state dump
  TraceState(modelEvent , sender , MODEL_STATE_FMT , modelDescFormat ,
      Loopidity::GetIsRolling() , scene->shouldSaveLoop , scene->doesPulseExist , isEq) ;
  // view state dump
  TraceState(viewEvent , sender , VIEW_STATE_FMT , viewDescFormat ,
      scene->loops.size() , sdlScene->histogramImgs.size() , sdlScene->loopImgs.size() , isEq) ;
  std::cout << std::endl ;

  return isEq ;
}

void Trace::TraceState(const char* event       , const char* sender     ,
                       const char* stateFormat , const char* descFormat ,
                       bool bool0 , bool bool1 , bool bool2 , bool /*isEq*/ )
{
#if DEBUG_TRACE_CLASS && DEBUG_TRACE_IN
cout << "Trace::TraceState(): '" << sender << "' bool0=" << bool0 << " bool1=" << bool1 << "bool2=" << bool2 << endl ;
#endif // #if DEBUG_TRACE_CLASS && DEBUG_TRACE_IN

#if DEBUG_TRACE

  memset(Event , ' ' , EVENT_LEN) ; memset(State , ' ' , STATE_LEN) ; memset(Desc , ' ' , DESC_LEN) ;

  EventLen = strlen(event) ; if (EventLen > EVENT_LEN) EventLen = EVENT_LEN ;
  memcpy(Event , event , EventLen) ;
  SenderLen = strlen(sender) ; if (EventLen + SenderLen > EVENT_LEN) SenderLen = EVENT_LEN - EventLen ;
  memcpy(Event + EventLen , sender , SenderLen) ;

  char state[STATE_LEN + 1] ; memset(state , ' ' , STATE_LEN) ; // state[STATE_LEN] = '\0' ;
  snprintf(state , STATE_LEN + 1 , stateFormat , bool0 , bool1 , bool2) ;
  StateLen = strlen(state) ; if (StateLen > STATE_LEN) StateLen = STATE_LEN ;
  memcpy(State , state , StateLen) ;

  char desc[DESC_LEN + 1] ; memset(desc , ' ' , DESC_LEN) ; desc[DESC_LEN] = '\0' ;
  snprintf(desc , DESC_LEN , descFormat , bool0 , bool1 , bool2) ;
  DescLen = strlen(desc) ; if (DescLen > DESC_LEN) DescLen = DESC_LEN ;
  memcpy(Desc , desc , DescLen + 1) ;

  Event[EVENT_LEN] = State[STATE_LEN] = Desc[DESC_LEN] = '\0' ;
  std::cout << Event << " " << State << " " << Desc << std::endl ;

#endif // #if DEBUG_TRACE

#if DEBUG_TRACE_CLASS && DEBUG_TRACE_OUT
cout << "Trace::TraceState(): Event(" << strlen(Event) << ")='" << Event << "'" << endl ; // for (int i=0 ; i < strlen(Event) ; ++i) printf("Event[%d]=%c\n" , i , Event[i]) ;
cout << "Trace::TraceState(): State(" << strlen(State) << ")='" << State << "'" << endl ; // for (int i=0 ; i < strlen(State) ; ++i) printf("State[%d]=%c\n" , i , State[i]) ;
cout << "Trace::TraceState(): Desc("  << strlen(Desc)  << ")='" << Desc  << "'" << endl ; // for (int i=0 ; i < strlen(Desc) ; ++i) printf("Desc[%d]=%c\n" , i , Desc[i]) ;
#endif // #if DEBUG_TRACE_CLASS && DEBUG_TRACE_OUT
}

#if DRAW_DEBUG_TEXT
void Trace::SetDbgTextC() { char dbg[TRACE_STATE_LEN] ; Uint32 sceneN = Loopidity::CurrentSceneN ; snprintf(dbg , TRACE_STATE_LEN , "NextSceneN=%d SceneN=%d PeakN=%d" , Loopidity::NextSceneN , sceneN , Loopidity::Scenes[sceneN]->getCurrentPeakN()) ; LoopiditySdl::SetStatusC(dbg) ; }
void Trace::SetDbgTextR() { char dbg[TRACE_STATE_LEN] ; Uint32 sceneN = Loopidity::CurrentSceneN ; snprintf(dbg , TRACE_STATE_LEN , "%d%d%d %lu%lu%lu" , Loopidity::GetIsRolling() , Loopidity::Scenes[sceneN]->shouldSaveLoop , Loopidity::Scenes[sceneN]->doesPulseExist , Loopidity::Scenes[sceneN]->loops.size() , Loopidity::SdlScenes[sceneN]->histogramImgs.size() , Loopidity::SdlScenes[sceneN]->loopImgs.size()) ; LoopiditySdl::SetStatusR(dbg) ; }
#endif // #if DRAW_DEBUG_TEXT
