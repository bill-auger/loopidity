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


#ifndef _LOOPIDITY_H_
#define _LOOPIDITY_H_


#include <cstdlib>
#include <exception>    // Scene::Scene()
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <vector>
#ifndef _WIN32
#  include <unistd.h>
#endif // _WIN32

#ifdef _WIN32
#  undef main
#  define snprintf _snprintf
#endif // _WIN32

typedef float Sample ; // jack_default_audio_sample_t

class SceneSdl ;
#include "jack_io.h"
#include "loopidity_sdl.h"
#include "scene.h"
#include "scene_sdl.h"
#include "trace.h"


class Loopidity
{
  friend class JackIO ;
  friend class Trace ;

  private:
    /* Loopidity class side private constants */

    static const Uint8       N_SCENES ;
    static const Uint8       N_LOOPS ;
    static const std::string ASSETS_DIR ;
#if ! FIXED_N_AUDIO_PORTS
    static const Uint8       N_INPUT_CHANNELS ;
    static const Uint8       N_OUTPUT_CHANNELS ;
#endif // FIXED_N_AUDIO_PORTS


    /* Loopidity class side private variables */
#if WAIT_FOR_JACK_INIT
    // setup
    static int InitJackTimeout ;
#endif // #if WAIT_FOR_JACK_INIT

    // models and views
    static Scene*    Scenes   [NUM_SCENES] ;
    static SceneSdl* SdlScenes[NUM_SCENES] ;

    // runtime state
    static Uint8 CurrentSceneN ;
    static Uint8 NextSceneN ;

    // runtime flags
#if WAIT_FOR_JACK_INIT
    static bool IsJackReady ;
#endif // WAIT_FOR_JACK_INIT
#if INIT_JACK_BEFORE_SCENES
    static bool IsInitialized ;
#endif // INIT_JACK_BEFORE_SCENES
    static bool IsRolling ;
    static bool ShouldSceneAutoChange ;
    static bool IsEditMode ;


  public:

    /* Loopidity class side public functions */

    // main
    static int Main(int argc , char** argv) ;

    // getters/setters
//    static void         SetNFramesPerPeriod(   Uint32 nFrames) ;
    static Uint8 GetCurrentSceneN(void) ;
    static Uint8 GetNextSceneN(   void) ;
//     static Uint32 GetLoopPos(    void) ;
    static bool   GetIsRolling(    void) ;
//    static bool         GetShouldSaveLoop(     void) ;
//    static bool         GetDoesPulseExist(     void) ;
//    static bool         GetIsEditMode(         void) ;

    // view helpers
    static void UpdateView(Uint32 sceneN) ;
    static void OOM(       void) ;


private:

    /* Loopidity class side private functions */

    // setup
    static std::string GetAssetsDir() ;
#if ! INIT_JACK_BEFORE_SCENES
    static bool        IsInitialized() ;
#endif // INIT_JACK_BEFORE_SCENES
    static bool        Init(         bool   shouldMonitorInputs , bool shouldAutoSceneChange ,
                                     Uint32 recordBufferSize                                 ) ;
#if INIT_JACK_BEFORE_SCENES
#  if SCENE_NFRAMES_EDITABLE
    static void        SetMetadata(  SceneMetadata* sceneMetadata) ;
#  else
    static void        SetMetadata(  Uint32 sampleRate       , Uint32 nFramesPerPeriod ,
                                     Uint32 recordBufferSize                           ) ;
#  endif // #if SCENE_NFRAMES_EDITABLE
#else
    static void        SetMetadata(  Uint32 sampleRate , Uint32 nFramesPerPeriod) ;
#endif // #if INIT_JACK_BEFORE_SCENES
    static int         Cleanup(      int retval) ;

    // event handlers
    static void HandleKeyEvent(  SDL_Event* event) ;
    static void HandleMouseEvent(SDL_Event* event) ;
    static void HandleUserEvent( SDL_Event* event) ;
    static void OnLoopCreation(  Uint32* sceneNum , Loop** newLoop) ;
    static void OnSceneChange(   Uint32* sceneNum) ;

    // user actions
    static void ToggleAutoSceneChange(void) ;
    static void ToggleRecordingState( void) ;
    static void ToggleNextScene(      void) ;
    static void DeleteLoop(           Uint32 sceneN , Uint32 loopN) ;
    static void DeleteLastLoop(       void) ;
    static void IncLoopVol(           Uint32 sceneN , Uint32 loopN , bool IsInc) ;
    static void ToggleLoopIsMuted(    Uint32 sceneN , Uint32 loopN) ;
    static void ToggleSceneIsMuted(   void) ;
    static void ToggleEditMode(       void) ;
    static void ResetScene(           Uint32 sceneN) ;
    static void ResetCurrentScene(    void) ;
    static void Reset(                void) ;
} ;

#endif // #ifndef _LOOPIDITY_H_
