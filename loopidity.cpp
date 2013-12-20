
#include "loopidity.h"


/* Loopidity class side private varables */

// setup
#if WAIT_FOR_JACK_INIT
#define INIT_JACK_TIMEOUT 10000 // nSeconds to wait for JACK to initialize
int Loopidity::InitJackTimeout = INIT_JACK_TIMEOUT ;
#endif // #if WAIT_FOR_JACK_INIT

// models and views
Scene*         Loopidity::Scenes[N_SCENES]        = {0} ;
SceneSdl*      Loopidity::SdlScenes[N_SCENES]     = {0} ;

// runtime state
unsigned int Loopidity::CurrentSceneN = 0 ;
unsigned int Loopidity::NextSceneN    = 0 ;

// runtime flags
#if WAIT_FOR_JACK_INIT
bool Loopidity::IsJackReady           = false ;
#endif // #if WAIT_FOR_JACK_INIT
bool Loopidity::IsRolling             = false ;
bool Loopidity::ShouldSceneAutoChange = false ;
bool Loopidity::IsEditMode            = false ;


/* Loopidity class side public functions */

/* main */

int Loopidity::Main(int argc , char** argv)
{
#if DEBUG_TRACE
cout << INIT_MSG << endl ;
#endif // #if DEBUG_TRACE

  // 'singleton' sanity check
  if (IsInitialized()) return false ;

  // parse command line arguments
  bool isMonitorInputs = true , isAutoSceneChange = true ; unsigned int recordBufferSize ;
  for (int argN = 0 ; argN < argc ; ++argN)
    if      (!strcmp(argv[argN] , MONITOR_ARG))      isMonitorInputs   = false ;
    else if (!strcmp(argv[argN] , SCENE_CHANGE_ARG)) isAutoSceneChange = false ;
    // TODO: user defined buffer sizes via command line
#if FIXED_AUDIO_BUFFER_SIZE
//    else if (!strcmp(argv[argN] , BUFFER_SIZE_ARG)) isAutoSceneChange = bufferSize = arg ;
  recordBufferSize = 0 ; // nyi
#else
  recordBufferSize = 0 ;
#endif // #if FIXED_AUDIO_BUFFER_SIZE

  // initialize Loopidity (controller) and instantiate Scenes (models and SdlScenes (views))
  if (!Init(isMonitorInputs , isAutoSceneChange , recordBufferSize)) return EXIT_FAILURE ;

  // initialize LoopiditySdl (view)
  vector<Sample>* peaksIn  = JackIO::GetPeaksIn() ;
  vector<Sample>* peaksOut = JackIO::GetPeaksOut() ;
  Sample* transientPeaks   = JackIO::GetTransientPeaks() ;
  if (!LoopiditySdl::Init(SdlScenes , peaksIn , peaksOut , transientPeaks))
    return EXIT_FAILURE ;

#if DEBUG_TRACE
cout << INIT_SUCCESS_MSG << endl ;
#endif // #if DEBUG_TRACE

  // draw initial
  LoopiditySdl::BlankScreen() ; LoopiditySdl::DrawHeader() ;

  // main loop
  bool done           = false ; SDL_Event event ;
  Uint16 timerStart   = SDL_GetTicks() , elapsed ;
  Uint16 guiLongCount = 0 ;
  while (!done)
  {
    // poll events and pass them off to our controller
    while (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
        case SDL_QUIT:            done = true ;              break ;
        case SDL_KEYDOWN:         HandleKeyEvent(&event) ;   break ;
        case SDL_MOUSEBUTTONDOWN: HandleMouseEvent(&event) ; break ;
        case SDL_USEREVENT:       HandleUserEvent(&event) ;  break ;
        default:                                             break ;
      }
    }
/*
Uint64 now = SDL_GetPerformanceCounter() ;
Uint64 elapsed = (now - DbgMainLoopTs) / SDL_GetPerformanceFrequency() ; DbgMainLoopTs = now ;
if (guiLongCount == GUI_UPDATE_LOW_PRIORITY_NICE)
  cout << "DbgMainLoopTs=" << elapsed << endl ;
*/
    // throttle framerate
    elapsed = SDL_GetTicks() - timerStart ;
    if (elapsed >= GUI_UPDATE_INTERVAL) timerStart = SDL_GetTicks() ;
    else { SDL_Delay(1) ; continue ; }

    // draw high priority
    JackIO::ScanTransientPeaks() ;
    LoopiditySdl::DrawScenes() ;
#if SCENE_NFRAMES_EDITABLE
    if (IsEditMode) LoopiditySdl::DrawEditScopes() ;
    else LoopiditySdl::DrawTransientScopes() ;
#else
    LoopiditySdl::DrawScopes() ;
#endif // #if SCENE_NFRAMES_EDITABLE

    // draw low priority
    if (!(guiLongCount = (guiLongCount + 1) % GUI_UPDATE_LOW_PRIORITY_NICE))
      LoopiditySdl::DrawStatusArea() ;

    LoopiditySdl::FlipScreen() ;
  } // while (!done)

  return EXIT_SUCCESS ;
}


// getters/setters

unsigned int Loopidity::GetCurrentSceneN() { return CurrentSceneN ; }

unsigned int Loopidity::GetNextSceneN() { return NextSceneN ; }

//unsigned int Loopidity::GetLoopPos() { return Scenes[CurrentSceneN]->getLoopPos() ; }

bool Loopidity::GetIsRolling() { return IsRolling ; }

//bool Loopidity::GetShouldSaveLoop() { return Scenes[CurrentSceneN]->shouldSaveLoop ; }

//bool Loopidity::GetDoesPulseExist() { return Scenes[CurrentSceneN]->doesPulseExist ; }

//bool Loopidity::GetIsEditMode() { return IsEditMode ; }


/* Loopidity class side private functions */

// setup

bool Loopidity::IsInitialized() { return !!Scenes[0] ; }

bool Loopidity::Init(bool shouldMonitorInputs , bool shouldAutoSceneChange ,
                     unsigned int recordBufferSize)
{
  // disable AutoSceneChange if SCENE_CHANGE_ARG given
  if (!shouldAutoSceneChange) ToggleAutoSceneChange() ;

  // instantiate Scenes (models) and SdlScenes (views)
  for (unsigned int sceneN = 0 ; sceneN < N_SCENES ; ++sceneN)
  {
    Scenes[sceneN]    = new Scene(sceneN , JackIO::GetRecordBufferSize()) ;
    SdlScenes[sceneN] = new SceneSdl(Scenes[sceneN] , sceneN) ;
    UpdateView(sceneN) ;

    if (!Scenes[sceneN] || !SdlScenes[sceneN]) return false ;
  }

  // initialize JACK
  string errMsg ;
  switch (JackIO::Init(Scenes[0] , 0 , shouldMonitorInputs , recordBufferSize))
  {
    case JACK_FAIL:      errMsg = JACK_FAIL_MSG ;
    case JACK_BUFF_FAIL: errMsg = ZERO_BUFFER_SIZE_MSG ;
    case JACK_MEM_FAIL:  errMsg = INSUFFICIENT_MEMORY_MSG ;
                         LoopiditySdl::Alert(errMsg.c_str()) ; return false ;
  }
#if WAIT_FOR_JACK_INIT
  // wait for JACK metadata
  InitJackTimeout = 0 ;
  while (!IsJackReady && (InitJackTimeout -= 100) > 0) usleep(100000) ;
  return IsJackReady ;
#else
  return true ;
#endif // #if WAIT_FOR_JACK_INIT
}

void Loopidity::SetMetaData(unsigned int sampleRate , unsigned int frameSize , unsigned int nFramesPerPeriod)
#if WAIT_FOR_JACK_INIT
  { Scene::SetMetaData(sampleRate , frameSize , nFramesPerPeriod) ; IsJackReady = true ; }
#else
  { Scene::SetMetaData(sampleRate , frameSize , nFramesPerPeriod) ; }
#endif // #if WAIT_FOR_JACK_INIT

void Loopidity::Cleanup()
{
  for (unsigned int sceneN = 0 ; sceneN < N_SCENES ; ++sceneN)
    if (SdlScenes[sceneN]) SdlScenes[sceneN]->cleanup() ;
  LoopiditySdl::Cleanup() ;
}


// event handlers

void Loopidity::HandleKeyEvent(SDL_Event* event)
{
#if HANDLE_KEYBOARD_EVENTS
DEBUG_TRACE_LOOPIDITYSDL_HANDLEKEYEVENT

  switch (event->key.keysym.sym)
  {
    case SDLK_SPACE:    ToggleRecordingState() ; break ;
    case SDLK_KP0:      ToggleNextScene()      ; break ;
    case SDLK_KP_ENTER: ToggleSceneIsMuted()   ; break ;
    case SDLK_RETURN:   ToggleEditMode()       ; break ;
    case SDLK_ESCAPE:   switch (event->key.keysym.mod)
    {
      case KMOD_RCTRL:    Reset()              ; break ;
      case KMOD_RSHIFT:   ResetCurrentScene()  ; break ;
      default:            DeleteLastLoop()     ; break ;
    }
    default:                                     break ;
  }
#endif // #if HANDLE_KEYBOARD_EVENTS
}

void Loopidity::HandleMouseEvent(SDL_Event* event)
{
#if HANDLE_MOUSE_EVENTS
  Uint16 x = event->button.x ; Uint16 y = event->button.y ;
  if (x < MOUSE_SCENES_L || x > MOUSE_SCENES_R ||
      y < MOUSE_SCENES_T || y > MOUSE_SCENES_B) return ;

  unsigned int sceneN = (y - MOUSE_SCENES_T) / SCENE_H ;
  unsigned int loopN  = (x - MOUSE_SCENES_L) / LOOP_W ;
  switch (event->button.button)
  {
    case SDL_BUTTON_LEFT:      ToggleLoopIsMuted(sceneN , loopN) ;  break ;
    case SDL_BUTTON_MIDDLE:    DeleteLoop(sceneN , loopN) ;         break ;
    case SDL_BUTTON_RIGHT:                                          break ;
    case SDL_BUTTON_WHEELUP:   IncLoopVol(sceneN , loopN , true) ;  break ;
    case SDL_BUTTON_WHEELDOWN: IncLoopVol(sceneN , loopN , false) ; break ;
    default:                                                        break ;
  }
#endif // #if HANDLE_MOUSE_EVENTS
}

void Loopidity::HandleUserEvent(SDL_Event* event)
{
#if HANDLE_USER_EVENTS
  void* data1 = event->user.data1 ; void* data2 = event->user.data2 ;
  switch (event->user.code)
  {
    case EVT_NEW_LOOP:      OnLoopCreation((unsigned int*)data1 , (Loop**)data2) ; break ;
    case EVT_SCENE_CHANGED: OnSceneChange((unsigned int*)data1) ;                  break ;
    default:                                                                       break ;
  }
#endif // #if HANDLE_USER_EVENTS
}

void Loopidity::OnLoopCreation(unsigned int* sceneNum , Loop** newLoop)
{
DEBUG_TRACE_LOOPIDITY_ONLOOPCREATION_IN

  unsigned int sceneN = *sceneNum ;      Loop* aLoop        = *newLoop ;
  Scene* scene        = Scenes[sceneN] ; SceneSdl* sdlScene = SdlScenes[sceneN] ;
  if (scene->addLoop(aLoop)) sdlScene->addLoop(aLoop , scene->loops.size() - 1) ;

  UpdateView(sceneN) ;

DEBUG_TRACE_LOOPIDITY_ONLOOPCREATION_OUT
}

void Loopidity::OnSceneChange(unsigned int* nextSceneN)
{
DEBUG_TRACE_LOOPIDITY_ONSCENECHANGE_IN

//  if (!Scenes[CurrentSceneN]->isRolling) { Scenes[CurrentSceneN]->reset() ; }
//else { Scenes[CurrentSceneN]->startRolling() ; SdlScenes[CurrentSceneN]->startRolling() ; }

  unsigned int prevSceneN = CurrentSceneN ; CurrentSceneN = NextSceneN = *nextSceneN ;
  SceneSdl* prevSdlScene  = SdlScenes[prevSceneN] ;
  Scene* nextScene        = Scenes[NextSceneN] ;
  prevSdlScene->drawScene(prevSdlScene->inactiveSceneSurface , 0 , 0) ;
  UpdateView(prevSceneN) ; UpdateView(NextSceneN) ;

  if (ShouldSceneAutoChange) do ToggleNextScene() ; while (!nextScene->loops.size()) ;

DEBUG_TRACE_LOOPIDITY_ONSCENECHANGE_OUT
}


// user actions

void Loopidity::ToggleAutoSceneChange() { ShouldSceneAutoChange = !ShouldSceneAutoChange ; }

void Loopidity::ToggleRecordingState()
{
//DEBUG_TRACE_LOOPIDITY_TOGGLERECORDINGSTATE_IN

  if (IsRolling) Scenes[CurrentSceneN]->toggleRecordingState() ;
  else { IsRolling = true ; Scenes[CurrentSceneN]->beginRecording() ; SdlScenes[CurrentSceneN]->startRolling() ; }
  UpdateView(CurrentSceneN) ;

DEBUG_TRACE_LOOPIDITY_TOGGLERECORDINGSTATE_OUT
}

void Loopidity::ToggleNextScene()
{
DEBUG_TRACE_LOOPIDITY_TOGGLENEXTSCENE_IN

  unsigned int prevSceneN = NextSceneN ; NextSceneN = (NextSceneN + 1) % N_SCENES ;
  UpdateView(prevSceneN) ; UpdateView(NextSceneN) ;
  JackIO::SetNextScene(Scenes[NextSceneN]) ;

  if (!IsRolling)
  {
    prevSceneN = CurrentSceneN ; CurrentSceneN = NextSceneN ;
    Scenes[prevSceneN]->reset() ; // SdlScenes[prevSceneN]->reset() ;
    UpdateView(prevSceneN) ; UpdateView(NextSceneN) ;
    JackIO::SetCurrentScene(Scenes[NextSceneN]) ;
  }

DEBUG_TRACE_LOOPIDITY_TOGGLENEXTSCENE_OUT
}

void Loopidity::DeleteLoop(unsigned int sceneN , unsigned int loopN)
{
DEBUG_TRACE_LOOPIDITY_DELETELOOP_IN

  SdlScenes[sceneN]->deleteLoop(loopN) ; Scenes[sceneN]->deleteLoop(loopN) ;

DEBUG_TRACE_LOOPIDITY_DELETELOOP_OUT
}

void Loopidity::DeleteLastLoop()
{
DEBUG_TRACE_LOOPIDITY_DELETELASTLOOP_IN

  DeleteLoop(CurrentSceneN , Scenes[CurrentSceneN]->loops.size() - 1) ;

DEBUG_TRACE_LOOPIDITY_DELETELASTLOOP_OUT
}

void Loopidity::IncLoopVol(unsigned int sceneN , unsigned int loopN , bool isInc)
{
DEBUG_TRACE_LOOPIDITY_INCLOOPVOL_IN

  Loop* loop = Scenes[sceneN]->getLoop(loopN) ; if (!loop) return ;

  float* vol = &loop->vol ;
  if (isInc) { *vol += LOOP_VOL_INC ; if (*vol > 1.0) *vol = 1.0 ; }
  else { *vol -= LOOP_VOL_INC ; if (*vol < 0.0) *vol = 0.0 ; }

DEBUG_TRACE_LOOPIDITY_INCLOOPVOL_OUT
}

void Loopidity::ToggleLoopIsMuted(unsigned int sceneN , unsigned int loopN)
{
DEBUG_TRACE_LOOPIDITY_TOGGLELOOPISMUTED_IN

  Loop* loop = Scenes[sceneN]->getLoop(loopN) ; loop->isMuted = !loop->isMuted ;

DEBUG_TRACE_LOOPIDITY_TOGGLELOOPISMUTED_OUT
}

void Loopidity::ToggleSceneIsMuted()
  { Scene* scene = Scenes[CurrentSceneN] ; scene->isMuted = !scene->isMuted ; }

void Loopidity::ToggleEditMode() { IsEditMode = !IsEditMode ; }

void Loopidity::ResetScene(unsigned int sceneN)
{
DEBUG_TRACE_LOOPIDITY_RESETSCENE_IN

  Scenes[sceneN]->reset() ; SdlScenes[sceneN]->reset() ;

DEBUG_TRACE_LOOPIDITY_RESETSCENE_OUT
}

void Loopidity::ResetCurrentScene() { ResetScene(CurrentSceneN) ; }

void Loopidity::Reset()
{
DEBUG_TRACE_LOOPIDITY_RESET_IN

  CurrentSceneN = NextSceneN = 0 ; JackIO::Reset(Scenes[0] , 0) ;
  for (unsigned int sceneN = 0 ; sceneN < N_SCENES ; ++sceneN) ResetScene(sceneN) ;

DEBUG_TRACE_LOOPIDITY_RESET_OUT
}


// helpers

void Loopidity::UpdateView(unsigned int sceneN) { SdlScenes[sceneN]->updateStatus() ; }
// { if (Scenes[sceneN]->isRolling) SdlScenes[sceneN]->updateStatus() ; }

void Loopidity::OOM() { DEBUG_TRACE_LOOPIDITY_OOM_IN LoopiditySdl::SetStatusC(OUT_OF_MEMORY_MSG) ; }
