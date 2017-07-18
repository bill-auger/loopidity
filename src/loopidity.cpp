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


// #include <sys/types.h>
#include <sys/stat.h>

#include "loopidity.h"
#include "scene.h"


/* Loopidity class side public constants */

const Uint32      Loopidity::N_SCENES   = NUM_SCENES ;
const Uint32      Loopidity::N_LOOPS    = NUM_LOOPS ;
const std::string Loopidity::ASSETS_DIR = GetAssetsDir() ;


/* Loopidity class side private varables */

// setup
#if WAIT_FOR_JACK_INIT
#define INIT_JACK_TIMEOUT 10000 // nSeconds to wait for JACK to initialize
int Loopidity::InitJackTimeout = INIT_JACK_TIMEOUT ;
#endif // #if WAIT_FOR_JACK_INIT

// models and views
Scene*    Loopidity::Scenes[N_SCENES]    = {0} ;
SceneSdl* Loopidity::SdlScenes[N_SCENES] = {0} ;

// runtime state
Uint32 Loopidity::CurrentSceneN = 0 ;
Uint32 Loopidity::NextSceneN    = 0 ;

// runtime flags
#if WAIT_FOR_JACK_INIT
bool Loopidity::IsJackReady           = false ;
#endif // WAIT_FOR_JACK_INIT
#if  INIT_JACK_BEFORE_SCENES
bool Loopidity::IsInitialized         = false ;
#endif // INIT_JACK_BEFORE_SCENES
bool Loopidity::IsRolling             = false ;
bool Loopidity::ShouldSceneAutoChange = false ;
bool Loopidity::IsEditMode            = false ;


/* Loopidity class side public functions */

// main entry

int Loopidity::Main(int argc , char** argv)
{
  // sanity checks
#if INIT_JACK_BEFORE_SCENES
  if (IsInitialized          ) return false ;
#else // INIT_JACK_BEFORE_SCENES
  if (IsInitialized()        ) return false ;
#endif // INIT_JACK_BEFORE_SCENES
  if (N_SCENES + 2 < N_SCENES) return false ;
  if (ASSETS_DIR.empty()     ) return false ;

  // parse command line arguments
  bool   shouldMonitorInputs   = true ;
  bool   shouldAutoSceneChange = true ;
  Uint32 recordBufferSize ;
  for (int argN = 0 ; argN < argc ; ++argN)
    if      (!strcmp(argv[argN] , MONITOR_ARG))      shouldMonitorInputs   = false ;
    else if (!strcmp(argv[argN] , SCENE_CHANGE_ARG)) shouldAutoSceneChange = false ;
    // TODO: user defined buffer sizes via command line
#if FIXED_AUDIO_BUFFER_SIZE
    else if (!strcmp(argv[argN] , BUFFER_SIZE_ARG)) shouldAutoSceneChange = !!(bufferSize = arg) ;
#else
  recordBufferSize = 0 ;
#endif // #if FIXED_AUDIO_BUFFER_SIZE

  // initialize Loopidity (controller), JackIO (controller), and LoopiditySdl (main view)
  //     and instantiate Scenes (models) and SdlScenes (views)
  if (!Init(shouldMonitorInputs , shouldAutoSceneChange , recordBufferSize))
    return Cleanup(EXIT_FAILURE) ;

DEBUG_TRACE_LOOPIDITY_MAIN_MID

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
        case SDL_KEYDOWN:         HandleKeyEvent(  &event) ; break ;
        case SDL_MOUSEBUTTONDOWN: HandleMouseEvent(&event) ; break ;
        case SDL_USEREVENT:       HandleUserEvent( &event) ; break ;
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
    JackIO::ScanPeaks() ;
    LoopiditySdl::DrawScenes() ;
#if SCENE_NFRAMES_EDITABLE
    if (IsEditMode) LoopiditySdl::DrawEditor() ;
    else            LoopiditySdl::DrawScopes() ;
#else
    LoopiditySdl::DrawScopes() ;
#endif // #if SCENE_NFRAMES_EDITABLE
    LoopiditySdl::DrawVUs() ;
    JackIO::ResetTransientPeaks() ;

    // draw low priority
    if (!(guiLongCount = (guiLongCount + 1) % GUI_UPDATE_LOW_PRIORITY_NICE))
    {
      // LoopiditySdl::DrawMemory() ; // TODO: available system memory
      if (!Scenes[CurrentSceneN]->getDoesPulseExist())
        LoopiditySdl::SetStatusL(SdlScenes[CurrentSceneN]->makeDurationStatusText()) ;
      LoopiditySdl::DrawStatusArea() ;
    }

    LoopiditySdl::FlipScreen() ;
  } // while (!done)

DEBUG_TRACE_LOOPIDITY_MAIN_OUT

  return Cleanup(EXIT_SUCCESS) ;
}


// getters/setters

std::string Loopidity::GetAssetsDir()
{
  // determine proper path to assets on the current system
  std::string this_bin ;
  std::string this_dir ;

#ifdef _WIN32
this_dir="./" ; // FIXME
/* TODO:
  std::vector<wchar_t> path_buffer ;
  DWORD n_wchars = 0 ;
  while (n_wchars >= path_buffer.size())
  {
    path_buffer.resize(path_buffer.size() + 256) ;
    n_wchars = GetModuleFileName(0 , &path_buffer.at(0) , path_buffer.size()) ;
  }
  pathBuf.resize(n_wchars) ;
  wstring path(path_buffer.begin() , path_buffer.end()) ;
  this_dir = std::wstring_convert<std::codecvt_utf8<wchar_t> , wchar_t> converterX.to_bytes(path) ;
*/
#else // _WIN32
#define SCRATCH_BUFFER_SIZE 2048
  char path_buffer[SCRATCH_BUFFER_SIZE] ;
  if (::readlink("/proc/self/exe" , path_buffer , SCRATCH_BUFFER_SIZE) > 0)
    this_bin = std::string(path_buffer) ;
  this_dir = this_bin.substr(0 , this_bin.find_last_of("/\\")) ;
#endif // _WIN32

  std::string assets_dir = this_dir + "/assets" ;
  std::string data_dir   = LOOPIDITY_DATADIR ;
  struct stat stats ;

  bool does_assets_dir_exist = stat(assets_dir.c_str() , &stats) == 0 && stats.st_mode & S_IFDIR ;
  bool does_data_dir_exist   = stat(data_dir.c_str()   , &stats) == 0 && stats.st_mode & S_IFDIR ;
  bool is_uninstalled        = does_assets_dir_exist && this_dir.compare(LOOPIDITY_BINDIR) != 0 ;

  return (does_data_dir_exist) ? data_dir   + "/" :
         (is_uninstalled     ) ? assets_dir + "/" : "" ;
}

Uint32 Loopidity::GetCurrentSceneN() { return CurrentSceneN ; }

Uint32 Loopidity::GetNextSceneN() { return NextSceneN ; }

//Uint32 Loopidity::GetLoopPos() { return Scenes[CurrentSceneN]->getLoopPos() ; }

bool Loopidity::GetIsRolling() { return IsRolling ; }

//bool Loopidity::GetShouldSaveLoop() { return Scenes[CurrentSceneN]->shouldSaveLoop ; }

//bool Loopidity::GetDoesPulseExist() { return Scenes[CurrentSceneN]->doesPulseExist ; }

//bool Loopidity::GetIsEditMode() { return IsEditMode ; }


// view helpers

void Loopidity::UpdateView(Uint32 sceneN) { SdlScenes[sceneN]->updateState() ; }

void Loopidity::OOM() { DEBUG_TRACE_LOOPIDITY_OOM_IN LoopiditySdl::SetStatusC(OUT_OF_MEMORY_MSG) ; }


/* Loopidity class side private functions */

// setup

#if ! INIT_JACK_BEFORE_SCENES
#  if WAIT_FOR_JACK_INIT
bool Loopidity::IsInitialized() { return IsJackReady && !!Scenes[0] && !!SceneSdl[0] ; }
#  else // #if WAIT_FOR_JACK_INIT
bool Loopidity::IsInitialized() { return !!Scenes[0] ; }
#  endif // #if WAIT_FOR_JACK_INIT
#endif // INIT_JACK_BEFORE_SCENES
bool Loopidity::Init(bool   shouldMonitorInputs , bool shouldAutoSceneChange ,
                     Uint32 recordBufferSize                                 )
{
  // disable AutoSceneChange if SCENE_CHANGE_ARG given
  if (!shouldAutoSceneChange) ToggleAutoSceneChange() ;

#if INIT_JACK_BEFORE_SCENES

  // initialize JACK
  switch (JackIO::Init(shouldMonitorInputs , recordBufferSize))
  {
    case JACK_MEM_FAIL: LoopiditySdl::Alert(INSUFFICIENT_MEMORY_MSG) ; return false ; break ;
    case JACK_SW_FAIL:  LoopiditySdl::Alert(JACK_SW_FAIL_MSG       ) ; return false ; break ;
    case JACK_HW_FAIL:  LoopiditySdl::Alert(JACK_HW_FAIL_MSG       ) ; return false ; break ;
    default:                                                                          break ;
  }

#  if WAIT_FOR_JACK_INIT
  // wait for JACK metadata
  while (!IsJackReady && (InitJackTimeout -= 100) > 0) usleep(100000) ;
  if (!IsJackReady) return false ; // via SetMetadata()
#  endif // WAIT_FOR_JACK_INIT
#endif // INIT_JACK_BEFORE_SCENES

  std::vector<Sample>* peaksIn    = JackIO::GetPeaksIn() ;
  std::vector<Sample>* peaksOut   = JackIO::GetPeaksOut() ;
  Sample*              peaksVuIn  = JackIO::GetPeaksVuIn() ;
  Sample*              peaksVuOut = JackIO::GetPeaksVuOut() ;

  // instantiate Scenes (models) and SdlScenes (views)
  for (Uint32 sceneN = 0 ; sceneN < N_SCENES ; ++sceneN)
  {
    try
    {
#if INIT_JACK_BEFORE_SCENES
      Scenes   [sceneN] = new Scene(sceneN) ;
      SdlScenes[sceneN] = new SceneSdl(Scenes[sceneN] , peaksIn) ;
#else // INIT_JACK_BEFORE_SCENES
      Scenes   [sceneN] = new Scene(sceneN , JackIO::GetRecordBufferSize()) ;
      SdlScenes[sceneN] = new SceneSdl(Scenes[sceneN] , peaksIn) ;
#endif // INIT_JACK_BEFORE_SCENES
    }
    catch(std::exception& ex) { LoopiditySdl::Alert(ex.what()) ; return false ; }

    UpdateView(sceneN) ;
  }

#if INIT_JACK_BEFORE_SCENES
  JackIO::Reset(Scenes[0]) ;
#else // INIT_JACK_BEFORE_SCENES
  // initialize JACK
  switch (JackIO::Init(Scenes[0] , shouldMonitorInputs , recordBufferSize))
  {
    case JACK_MEM_FAIL: LoopiditySdl::Alert(INSUFFICIENT_MEMORY_MSG) ; return false ; break ;
    case JACK_SW_FAIL:  LoopiditySdl::Alert(JACK_SW_FAIL_MSG       ) ; return false ; break ;
    case JACK_HW_FAIL:  LoopiditySdl::Alert(JACK_HW_FAIL_MSG       ) ; return false ; break ;
    default:                                                                          break ;
  }
#  if WAIT_FOR_JACK_INIT
  // wait for JACK metadata
  while (!IsJackReady && (InitJackTimeout -= 100) > 0) usleep(100000) ;
  if (!IsJackReady)                                    return false ; // via SetMetadata()
#  endif // WAIT_FOR_JACK_INIT
#endif // INIT_JACK_BEFORE_SCENES

  // initialize LoopiditySdl (view)
#if INIT_JACK_BEFORE_SCENES
  IsInitialized = LoopiditySdl::Init(SdlScenes , peaksIn , peaksOut , peaksVuIn , peaksVuOut) ;

  if (!IsInitialized) Cleanup(EXIT_FAILURE) ;

  return IsInitialized ;
#else // INIT_JACK_BEFORE_SCENES
  bool is_initialized = LoopiditySdl::Init(SdlScenes , peaksIn , peaksOut , peaksVuIn , peaksVuOut) ;

  if (!is_initialized) Cleanup(EXIT_FAILURE) ;

  return is_initialized ;
#endif // INIT_JACK_BEFORE_SCENES
}

#if INIT_JACK_BEFORE_SCENES
#  if SCENE_NFRAMES_EDITABLE
void Loopidity::SetMetadata(SceneMetadata* sceneMetadata)
{
  Scene::SetMetadata(sceneMetadata) ;
#if WAIT_FOR_JACK_INIT
  IsJackReady = true ;
#endif // #if WAIT_FOR_JACK_INIT
}
#  else
void Loopidity::SetMetadata(Uint32 sampleRate       , Uint32 nFramesPerPeriod ,
                            Uint32 recordBufferSize                           )
{
  Scene::SetMetadata(sampleRate , nFramesPerPeriod , recordBufferSize) ;
#if WAIT_FOR_JACK_INIT
  IsJackReady = true ;
#endif // #if WAIT_FOR_JACK_INIT
}
#  endif // #if SCENE_NFRAMES_EDITABLE
#else
void Loopidity::SetMetadata(Uint32 sampleRate , Uint32 nFramesPerPeriod)
#  if SCENE_NFRAMES_EDITABLE
{
  Scene::SetMetadata(sampleRate , nFramesPerPeriod) ;
  Scenes[1]->currentFrameN = Scenes[1]->beginFrameN = sampleRate ; // KLUDGE
  Scenes[2]->currentFrameN = Scenes[2]->beginFrameN = sampleRate ; // KLUDGE
}
#else
  { Scene::SetMetadata(sampleRate , nFramesPerPeriod) ; }
#  endif // #if SCENE_NFRAMES_EDITABLE
#endif // #if INIT_JACK_BEFORE_SCENES

int Loopidity::Cleanup(int exit_status)
{
DEBUG_TRACE_LOOPIDITY_CLEANUP

  for (Uint32 sceneN = 0 ; sceneN < N_SCENES ; ++sceneN)
    if (!!SdlScenes[sceneN]) SdlScenes[sceneN]->cleanup() ;
  LoopiditySdl::Cleanup() ;

  return exit_status ;
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
  Uint16 x = event->button.x ; Uint16 y = event->button.y ;
  if (x < MOUSE_SCENES_L || x > MOUSE_SCENES_R ||
      y < MOUSE_SCENES_T || y > MOUSE_SCENES_B  ) return ;

#if HANDLE_MOUSE_EVENTS
  Uint32 sceneN = (y - MOUSE_SCENES_T) / SCENE_H ;
  Uint32 loopN  = (x - MOUSE_SCENES_L) / LOOP_W ;
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
    case EVT_NEW_LOOP:      OnLoopCreation((Uint32*)data1 , (Loop**)data2) ; break ;
    case EVT_SCENE_CHANGED: OnSceneChange((Uint32*)data1) ;                  break ;
    default:                                                                 break ;
  }
#endif // #if HANDLE_USER_EVENTS
}

void Loopidity::OnLoopCreation(Uint32* sceneNum , Loop** newLoop)
{
DEBUG_TRACE_LOOPIDITY_ONLOOPCREATION_IN

  Uint32 sceneN = *sceneNum ;      Loop*     aLoop    = *newLoop ;
  Scene* scene  = Scenes[sceneN] ; SceneSdl* sdlScene = SdlScenes[sceneN] ;
  if (scene->addLoop(aLoop)) sdlScene->addLoop(aLoop , scene->loops.size() - 1) ;

  UpdateView(sceneN) ;

DEBUG_TRACE_LOOPIDITY_ONLOOPCREATION_OUT
}

void Loopidity::OnSceneChange(Uint32* nextSceneN)
{
DEBUG_TRACE_LOOPIDITY_ONSCENECHANGE_IN

//  if (!Scenes[CurrentSceneN]->isRolling) { Scenes[CurrentSceneN]->reset() ; }
//else { Scenes[CurrentSceneN]->startRolling() ; SdlScenes[CurrentSceneN]->startRolling() ; }

  Uint32    prevSceneN   = CurrentSceneN ; CurrentSceneN = NextSceneN = *nextSceneN ;
  SceneSdl* prevSdlScene = SdlScenes[prevSceneN] ;
  Scene*    nextScene    = Scenes   [NextSceneN] ;
  prevSdlScene->drawScene(prevSdlScene->inactiveSceneSurface , 0 , 0) ;
  UpdateView(prevSceneN) ; UpdateView(NextSceneN) ;

  if (ShouldSceneAutoChange) do ToggleNextScene() ; while (!nextScene->loops.size()) ;

DEBUG_TRACE_LOOPIDITY_ONSCENECHANGE_OUT
}


// user actions

void Loopidity::ToggleAutoSceneChange() { ShouldSceneAutoChange = !ShouldSceneAutoChange ; }

void Loopidity::ToggleRecordingState()
{
DEBUG_TRACE_LOOPIDITY_TOGGLERECORDINGSTATE_IN

  if (IsRolling) Scenes[CurrentSceneN]->toggleRecordingState() ;
  else { IsRolling = true ; Scenes[CurrentSceneN]->beginRecording() ; SdlScenes[CurrentSceneN]->startRolling() ; }
  UpdateView(CurrentSceneN) ;

DEBUG_TRACE_LOOPIDITY_TOGGLERECORDINGSTATE_OUT
}

void Loopidity::ToggleNextScene()
{
DEBUG_TRACE_LOOPIDITY_TOGGLENEXTSCENE_IN

  Uint32 prevSceneN = NextSceneN ; NextSceneN = (NextSceneN + 1) % N_SCENES ;
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

void Loopidity::DeleteLoop(Uint32 sceneN , Uint32 loopN)
{
DEBUG_TRACE_LOOPIDITY_DELETELOOP_IN

  if (!loopN) ResetScene(sceneN) ;
  else { SdlScenes[sceneN]->deleteLoop(loopN) ; Scenes[sceneN]->deleteLoop(loopN) ; }

DEBUG_TRACE_LOOPIDITY_DELETELOOP_OUT
}

void Loopidity::DeleteLastLoop()
{
DEBUG_TRACE_LOOPIDITY_DELETELASTLOOP_IN

  DeleteLoop(CurrentSceneN , Scenes[CurrentSceneN]->getNLoops() - 1) ;

DEBUG_TRACE_LOOPIDITY_DELETELASTLOOP_OUT
}

void Loopidity::IncLoopVol(Uint32 sceneN , Uint32 loopN , bool isInc)
{
DEBUG_TRACE_LOOPIDITY_INCLOOPVOL_IN

  Loop* loop = Scenes[sceneN]->getLoop(loopN) ; if (!loop) return ;

  float* vol = &loop->vol ;
  if (isInc) { *vol += LOOP_VOL_INC ; if (*vol > 1.0) *vol = 1.0 ; }
  else { *vol -= LOOP_VOL_INC ; if (*vol < 0.0) *vol = 0.0 ; }

DEBUG_TRACE_LOOPIDITY_INCLOOPVOL_OUT
}

void Loopidity::ToggleLoopIsMuted(Uint32 sceneN , Uint32 loopN)
{
DEBUG_TRACE_LOOPIDITY_TOGGLELOOPISMUTED_IN

  Loop* loop = Scenes[sceneN]->getLoop(loopN) ; loop->isMuted = !loop->isMuted ;

DEBUG_TRACE_LOOPIDITY_TOGGLELOOPISMUTED_OUT
}

void Loopidity::ToggleSceneIsMuted()
{
  Scene* scene = Scenes[CurrentSceneN] ; scene->isMuted = !scene->isMuted ;
}

void Loopidity::ToggleEditMode() { IsEditMode = !IsEditMode ; }

void Loopidity::ResetScene(Uint32 sceneN)
{
DEBUG_TRACE_LOOPIDITY_RESETSCENE_IN

  Scenes[sceneN]->reset() ; SdlScenes[sceneN]->reset() ; UpdateView(sceneN) ;

  bool doesAnyPulseExist = false ;
  for (sceneN = 0 ; sceneN < N_SCENES ; ++sceneN)
{
    doesAnyPulseExist |= Scenes[sceneN]->getDoesPulseExist() ;
// TODO: extract this into doesAnyPulseExist()
//DBG("Loopidity::ResetScene() Scenes[%d]->getDoesPulseExist()=%d doesAnyPulseExist=%d\n" , sceneN , Scenes[sceneN]->getDoesPulseExist() , doesAnyPulseExist) ;
}
  IsRolling = doesAnyPulseExist ;

DEBUG_TRACE_LOOPIDITY_RESETSCENE_OUT
}

void Loopidity::ResetCurrentScene() { ResetScene(CurrentSceneN) ; }

void Loopidity::Reset()
{
DEBUG_TRACE_LOOPIDITY_RESET_IN

  for (Uint32 sceneN = 0 ; sceneN < N_SCENES ; ++sceneN) ResetScene(sceneN) ;
  IsRolling = false ; CurrentSceneN = NextSceneN = 0 ; JackIO::Reset(Scenes[0]) ;

DEBUG_TRACE_LOOPIDITY_RESET_OUT
}
