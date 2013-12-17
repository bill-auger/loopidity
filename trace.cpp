
#include "loopidity.h"

using namespace std ;


/* Trace class side public constants */

/* constants */

const char* Trace::MODEL     = DEBUG_TRACE_MODEL ;
const char* Trace::MODEL_ERR = DEBUG_TRACE_MODEL_ERROR ;
const char* Trace::VIEW      = DEBUG_TRACE_VIEW ;
const char* Trace::VIEW_ERR  = DEBUG_TRACE_VIEW_ERROR ;

const char* Trace::MODEL_STATE_FMT = DEBUG_TRACE_MODEL_STATE_FORMAT ;
const char* Trace::VIEW_STATE_FMT  = DEBUG_TRACE_VIEW_STATE_FORMAT ;
const char* Trace::MODEL_DESC_FMT  = DEBUG_TRACE_MODEL_DESC_FORMAT ;
const char* Trace::VIEW_DESC_FMT   = DEBUG_TRACE_VIEW_DESC_FORMAT ;
const char* Trace::MODEL_ERR_FMT   = DEBUG_TRACE_MODEL_ERROR_FORMAT ;
const char* Trace::VIEW_ERR_FMT    = DEBUG_TRACE_VIEW_ERROR_FORMAT ;

const unsigned int Trace::EVENT_LEN = DEBUG_TRACE_EVENT_LEN ;
const unsigned int Trace::STATE_LEN = DEBUG_TRACE_STATE_LEN ;
const unsigned int Trace::DESC_LEN  = DEBUG_TRACE_DESC_LEN ;


/* Trace class side private variables */

/* buffers */

char Trace::Event[EVENT_LEN + 1] = {0} ;
char Trace::State[STATE_LEN + 1] = {0} ;
char Trace::Desc[DESC_LEN + 1]   = {0} ;

unsigned int Trace::EventLen  = 0 ;
unsigned int Trace::SenderLen = 0 ;
unsigned int Trace::StateLen  = 0 ;
unsigned int Trace::DescLen   = 0 ;


/* Trace class side public functions */

bool Trace::SanityCheck(unsigned int sceneN)
{
  Scene*    scene    = Loopidity::Scenes[sceneN] ;
  SceneSdl* sdlScene = Loopidity::SdlScenes[sceneN] ;

  unsigned int nLoops         = scene->loops.size() ;
  unsigned int nHistogramImgs = sdlScene->histogramImgs.size() ;
  unsigned int nLoopImgs      = sdlScene->loopImgs.size() ;

  return (nLoops == nHistogramImgs && nLoops == nLoopImgs) ;
}

#if DEBUG_TRACE
bool Trace::TraceEvs(unsigned int sceneN) { return (DEBUG_TRACE_EVS || !SanityCheck(sceneN)) ; }

bool Trace::TraceIn(unsigned int sceneN)  { return (DEBUG_TRACE_IN  || !SanityCheck(sceneN)) ; }

bool Trace::TraceOut(unsigned int sceneN) { return (DEBUG_TRACE_OUT || !SanityCheck(sceneN)) ; }
#endif // #if DEBUG_TRACE

bool Trace::TraceScene(const char* senderTemplate , Scene* scene)
{
  unsigned int sceneN = scene->sceneN ; SceneSdl* sdlScene = Loopidity::SdlScenes[sceneN] ;
  char sender[Trace::EVENT_LEN] ; sprintf(sender , senderTemplate , sceneN) ;

  // mvc sanity checks
  bool isEq = SanityCheck(sceneN) ;
  const char *modelEvent , *modelDescFormat , *viewEvent , *viewDescFormat ;
  if (isEq)
  {
    modelEvent = Trace::MODEL ; modelDescFormat = Trace::MODEL_DESC_FMT ;
    viewEvent = Trace::VIEW ; viewDescFormat = Trace::VIEW_DESC_FMT ;
  }
  else
  {
    modelEvent = Trace::MODEL_ERR ; modelDescFormat = Trace::MODEL_ERR_FMT ;
    viewEvent = Trace::VIEW_ERR ; viewDescFormat = Trace::VIEW_ERR_FMT ;
  }

  // model state dump
  Trace::TraceState(modelEvent , sender , Trace::MODEL_STATE_FMT , modelDescFormat ,
      scene->isRolling , scene->shouldSaveLoop , scene->doesPulseExist , isEq) ;
  // view state dump
  Trace::TraceState(viewEvent , sender , Trace::VIEW_STATE_FMT , viewDescFormat ,
      scene->loops.size() , sdlScene->histogramImgs.size() , sdlScene->loopImgs.size() , isEq) ;
  cout << endl ;

  return isEq ;
}

void Trace::TraceState(const char* event , const char* sender ,
                       const char* stateFormat , const char* descFormat ,
                       bool bool0 , bool bool1 , bool bool2 , bool isEq)
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
  cout << Event << " " << State << " " << Desc << endl ;

#endif // #if DEBUG_TRACE

#if DEBUG_TRACE_CLASS && DEBUG_TRACE_OUT
cout << "Trace::TraceState(): Event(" << strlen(Event) << ")='" << Event << "'" << endl ; // for (int i=0 ; i < strlen(Event) ; ++i) printf("Event[%d]=%c\n" , i , Event[i]) ;
cout << "Trace::TraceState(): State(" << strlen(State) << ")='" << State << "'" << endl ; // for (int i=0 ; i < strlen(State) ; ++i) printf("State[%d]=%c\n" , i , State[i]) ;
cout << "Trace::TraceState(): Desc("  << strlen(Desc)  << ")='" << Desc  << "'" << endl ; // for (int i=0 ; i < strlen(Desc) ; ++i) printf("Desc[%d]=%c\n" , i , Desc[i]) ;
#endif // #if DEBUG_TRACE_CLASS && DEBUG_TRACE_OUT
}

#if DRAW_DEBUG_TEXT
void Trace::SetDbgTextL() { char dbg[255] ; unsigned int sceneN = Loopidity::CurrentSceneN ; sprintf(dbg , "NextSceneN=%d SceneN=%d PeakN=%d" , Loopidity::NextSceneN , sceneN , Loopidity::Scenes[sceneN]->getCurrentPeakN()) ; LoopiditySdl::SetStatusL(dbg) ; }
void Trace::SetDbgTextR() { char dbg[255] ; unsigned int sceneN = Loopidity::CurrentSceneN ; sprintf(dbg , "%d%d%d %d%d%d" , Loopidity::Scenes[sceneN]->isRolling , Loopidity::Scenes[sceneN]->shouldSaveLoop , Loopidity::Scenes[sceneN]->doesPulseExist , Loopidity::Scenes[sceneN]->loops.size() , Loopidity::SdlScenes[sceneN]->histogramImgs.size() , Loopidity::SdlScenes[sceneN]->loopImgs.size()) ; LoopiditySdl::SetStatusR(dbg) ; }
#endif // #if DRAW_DEBUG_TEXT
