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


#include "../loopidity.h"


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
const char* Trace::MODEL_ERR_FMT   = DEBUG_TRACE_MODEL_DESC_FORMAT DEBUG_TRACE_MODEL_ERROR_FORMAT ;
const char* Trace::VIEW_ERR_FMT    = DEBUG_TRACE_VIEW_DESC_FORMAT DEBUG_TRACE_VIEW_ERROR_FORMAT ;


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

bool Trace::SanityCheck(Uint8 sceneN)
{
  Scene*    scene    = Loopidity::Scenes   [sceneN] ;
  SceneSdl* sdlScene = Loopidity::SdlScenes[sceneN] ;

  Uint8 nLoops         = scene   ->loops        .size() ;
  Uint8 nHistogramImgs = sdlScene->histogramImgs.size() ;
  Uint8 nLoopImgs      = sdlScene->loopImgs     .size() ;

  return (nLoops == nHistogramImgs && nLoops == nLoopImgs) ;
}

void Trace::Dbg(std::string msg) { std::cout << "DBG: "   << msg << std::endl ; }

void Trace::Err(std::string msg) { std::cout << "ERROR: " << msg << std::endl ; }

#if DEBUG_TRACE
bool Trace::TraceEvs(Uint8 sceneN) { return ((DEBUG_TRACE_EVS) || !SanityCheck(sceneN)) ; }

bool Trace::TraceIn(Uint8 sceneN)  { return ((DEBUG_TRACE_IN ) || !SanityCheck(sceneN)) ; }

bool Trace::TraceOut(Uint8 sceneN) { return ((DEBUG_TRACE_OUT) || !SanityCheck(sceneN)) ; }
#endif // #if DEBUG_TRACE

bool Trace::TraceScene(const char* senderTemplate , Scene* scene)
{
  Uint8     sceneN   = scene->sceneN ;
  SceneSdl* sdlScene = Loopidity::SdlScenes[sceneN] ;
  char      sender[EVENT_LEN] ; snprintf(sender , EVENT_LEN , senderTemplate , sceneN) ;

  // mvc sanity checks
  bool isEq = SanityCheck(sceneN) ;
  const char *modelEvent , *modelDescFormat , *viewEvent , *viewDescFormat ;
  if (isEq)
  {
    modelEvent = MODEL ; modelDescFormat = MODEL_DESC_FMT ;
    viewEvent  = VIEW ;  viewDescFormat  = VIEW_DESC_FMT ;
  }
  else
  {
    modelEvent = MODEL_ERR ; modelDescFormat = MODEL_ERR_FMT ;
    viewEvent  = VIEW_ERR ;  viewDescFormat  = VIEW_ERR_FMT ;
  }


#if DEBUG_TRACE_TRACECLASS && DEBUG_TRACE_IN
std::cout << "Trace::TraceScene()<-" << sender                         <<
             " loops.size="          << scene   ->loops        .size() <<
             " histogramImgs.size="  << sdlScene->histogramImgs.size() <<
             " loopImgs.size="       << sdlScene->loopImgs     .size() <<
             " isEq="                << isEq                           << std::endl ;
#endif // #if DEBUG_TRACE_TRACECLASS && DEBUG_TRACE_IN


  // model state dump
  bool isRolling      = Loopidity::GetIsRolling() ;
  bool doesPulseExist = scene->doesPulseExist ;
  bool shouldSaveLoop = scene->shouldSaveLoop ;
  TraceState(modelEvent , sender         , MODEL_STATE_FMT , modelDescFormat ,
             isRolling  , doesPulseExist , shouldSaveLoop                    ) ;

  // view state dump
  Uint8 nLoops         = scene   ->loops        .size() ;
  Uint8 nLoopImgs      = sdlScene->loopImgs     .size() ;
  Uint8 nHistogramImgs = sdlScene->histogramImgs.size() ;
  TraceState(viewEvent , sender    , VIEW_STATE_FMT , viewDescFormat ,
             nLoops    , nLoopImgs , nHistogramImgs                  ) ;

  std::cout << std::endl ;

  return isEq ;
}

void Trace::TraceState(const char* event       , const char* sender     ,
                       const char* stateFormat , const char* descFormat ,
                       Uint8 int0 , Uint8 int1 , Uint8 int2             )
{
#if DEBUG_TRACE_TRACECLASS && DEBUG_TRACE_IN
printf("Trace::TraceState(): %s int0=%d int1=%d int2=%d\n" , sender , int0  , int1 , int2) ;
#endif // #if DEBUG_TRACE_TRACECLASS && DEBUG_TRACE_IN


#if DEBUG_TRACE
  memset(Event , ' ' , EVENT_LEN) ; memset(State , ' ' , STATE_LEN) ; memset(Desc , ' ' , DESC_LEN) ;

  EventLen = strlen(event) ; if (EventLen > EVENT_LEN) EventLen = EVENT_LEN ;
  memcpy(Event , event , EventLen) ;
  SenderLen = strlen(sender) ; if (EventLen + SenderLen > EVENT_LEN) SenderLen = EVENT_LEN - EventLen ;
  memcpy(Event + EventLen , sender , SenderLen) ;

  char state[STATE_LEN + 1] ; memset(state , ' ' , STATE_LEN) ; // state[STATE_LEN] = '\0' ;
  snprintf(state , STATE_LEN + 1 , stateFormat , int0 , int1 , int2) ;
  StateLen = strlen(state) ; if (StateLen > STATE_LEN) StateLen = STATE_LEN ;
  memcpy(State , state , StateLen) ;

  char desc[DESC_LEN + 1] ; memset(desc , ' ' , DESC_LEN) ; desc[DESC_LEN] = '\0' ;
  snprintf(desc , DESC_LEN , descFormat , int0 , int1 , int2) ;
  DescLen = strlen(desc) ; if (DescLen > DESC_LEN) DescLen = DESC_LEN ;
  memcpy(Desc , desc , DescLen + 1) ;

  Event[EVENT_LEN] = State[STATE_LEN] = Desc[DESC_LEN] = '\0' ;
  std::cout << Event << " " << State << " " << Desc << std::endl ;
#else // #if DEBUG_TRACE
  event = "" ; sender = event ; stateFormat = sender ; descFormat = stateFormat ; event = descFormat ; // suppress warnings
#endif // #if DEBUG_TRACE
#if DEBUG_TRACE_TRACECLASS && DEBUG_TRACE_IN && ! DEBUG_TRACE
  int0 = 0 ; int1 = int0 ; int2 = int1 ; int0 = int2 = *sender ; // suppress warnings
#endif // #if DEBUG_TRACE_TRACECLASS && DEBUG_TRACE_IN && ! DEBUG_TRACE


#if DEBUG_TRACE_TRACECLASS && DEBUG_TRACE_OUT
std::cout << "Trace::TraceState(): Event(" << strlen(Event) << ")='" << Event << "'" << std::endl ; // for (int i=0 ; i < strlen(Event) ; ++i) printf("Event[%d]=%c\n" , i , Event[i]) ;
std::cout << "Trace::TraceState(): State(" << strlen(State) << ")='" << State << "'" << std::endl ; // for (int i=0 ; i < strlen(State) ; ++i) printf("State[%d]=%c\n" , i , State[i]) ;
std::cout << "Trace::TraceState(): Desc("  << strlen(Desc)  << ")='" << Desc  << "'" << std::endl ; // for (int i=0 ; i < strlen(Desc) ; ++i) printf("Desc[%d]=%c\n" , i , Desc[i]) ;
#endif // #if DEBUG_TRACE_TRACECLASS && DEBUG_TRACE_OUT
}

#if DRAW_DEBUG_TEXT
void Trace::SetDbgTextC() { char dbg[TRACE_STATE_LEN] ; Uint8 sceneN = Loopidity::CurrentSceneN ; snprintf(dbg , TRACE_STATE_LEN , "NextSceneN=%d PeakN=%d" , Loopidity::NextSceneN , Loopidity::GetIsRolling() * Loopidity::Scenes[sceneN]->getCurrentPeakN()) ; LoopiditySdl::SetStatusC(dbg) ; }
void Trace::SetDbgTextR() { char dbg[TRACE_STATE_LEN] ; Uint8 sceneN = Loopidity::CurrentSceneN ; snprintf(dbg , TRACE_STATE_LEN , "%d%d%d %u%u%u" , Loopidity::GetIsRolling() , Loopidity::Scenes[sceneN]->doesPulseExist , Loopidity::Scenes[sceneN]->shouldSaveLoop , (unsigned int)Loopidity::Scenes[sceneN]->loops.size() , (unsigned int)Loopidity::SdlScenes[sceneN]->loopImgs.size() , (unsigned int)Loopidity::SdlScenes[sceneN]->histogramImgs.size()) ; LoopiditySdl::SetStatusR(dbg) ; }
#endif // #if DRAW_DEBUG_TEXT
