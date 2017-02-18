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


#ifndef _SCENE_H_
#define _SCENE_H_


#include "loopidity.h"


class InvalidMetadataException: public exception
{
  virtual const char* what() const throw() { return INVALID_METADATA_MSG ; }
} ;


#if SCENE_NFRAMES_EDITABLE
typedef struct SceneMetadata
{
  Uint32 sampleRate ;
  Uint32 nFramesPerPeriod ;
  Uint32 bytesPerFrame ;
  Uint32 minLoopSize ;
  Uint32 triggerLatencySize ;
  Uint32 beginFrameN ;
  Uint32 endFrameN ;
} SceneMetadata ;
#endif // SCENE_NFRAMES_EDITABLE


class Loop
{
  friend class JackIO ;
  friend class Loopidity ;
  friend class Scene ;
friend class SceneSdl ;


  private:

    /* Loop class side private funcrtions  */

    Loop(Uint32 nFrames) ;
    ~Loop() ;


    /* Loop instance side private varables */

    // audio data
    Sample* buffer1 ;
    Sample* buffer2 ;

    // peaks cache
    Sample peaksFine  [N_PEAKS_FINE  ] ;
    Sample peaksCourse[N_PEAKS_COURSE] ;

    // loop state
    float vol ;
    bool  isMuted ;


  public:

    /* Loop instance side public functions */

    Sample getPeakFine(  Uint32 peakN) ;
    Sample getPeakCourse(Uint32 peakN) ;
} ;


class Scene
{
  friend class JackIO ;
  friend class Loopidity ;
  friend class LoopiditySdl ;
  friend class SceneSdl ;
  friend class Trace ;


  public:

    /* Scene class side public constants */

    static const Uint32 N_FINE_PEAKS ;
    static const Uint32 N_COURSE_PEAKS ;


  private:

    /* Scene class side private constants */

    static const InvalidMetadataException* INVALID_METADATA_EXCEPTION ;


    /* Scene class side private varables */

    // peaks cache
    static float  FinePeaksPerCoursePeak ;
    static Uint32 NFinePeaksPerCoursePeak ;

    // sample metedata
    static Uint32 SampleRate ;
    static Uint32 FramesPerPeriod ;
    static Uint32 TriggerLatencySize ;
#if SCENE_NFRAMES_EDITABLE
    static Uint32 MinLoopSize ;
#endif // #if SCENE_NFRAMES_EDITABLE
#if SCENE_NFRAMES_EDITABLE && INIT_JACK_BEFORE_SCENES
    static Uint32 BytesPerFrame ;
    static Uint32 BeginFrameN ;
    static Uint32 EndFrameN ;
#endif // #if SCENE_NFRAMES_EDITABLE && INIT_JACK_BEFORE_SCENES
#if !SCENE_NFRAMES_EDITABLE || !INIT_JACK_BEFORE_SCENES
    static Uint32 RecordBufferSize ;
#endif // #if !SCENE_NFRAMES_EDITABLE || !WAIT_FOR_JACK_INITWAIT_FOR_JACK_INIT


    /* Scene class side private functions */

    // setup
#if INIT_JACK_BEFORE_SCENES
    Scene(Uint32 sceneNum) ;
#else
#  if SCENE_NFRAMES_EDITABLE
    Scene(Uint32 sceneNum , Uint32 endFrameN) ;
#  else
    Scene(Uint32 sceneNum , Uint32 bufferSize) ;
#  endif // #if SCENE_NFRAMES_EDITABLE
#endif // #if INIT_JACK_BEFORE_SCENES

    // getters/setters
#if INIT_JACK_BEFORE_SCENES
#  if SCENE_NFRAMES_EDITABLE
    static void SetMetadata(SceneMetadata* sceneMetadata) ;
#  else
    static void SetMetadata(Uint32 sampleRate       , Uint32 nFramesPerPeriod ,
                            Uint32 recordBufferSize                           ) ;
#  endif // #if SCENE_NFRAMES_EDITABLE
#else
    static void SetMetadata(Uint32 sampleRate , Uint32 nFramesPerPeriod) ;
#endif // #if INIT_JACK_BEFORE_SCENES


    /* Scene instance side private varables */

    // identity
    Uint32 sceneN ;

    // audio data
    list<Loop*> loops ;

    // peaks cache
    float  hiScenePeaks[N_PEAKS_FINE] ; // the loudest of the currently playing samples in the current scene
    float  hiLoopPeaks [NUM_LOOPS] ;    // the loudest sample for each loop of the current scene
    float  highestScenePeak ;           // the loudest of all samples in all loops of the current scene (nyi)
    Uint32 nFramesPerPeak ;             // # of samples per fine peak (hiScenePeaks , hiLoopPeaks , peaksFine)

    // buffer iteration
    Uint32 currentFrameN ;
#if SCENE_NFRAMES_EDITABLE
    Uint32 beginFrameN ;
    Uint32 endFrameN ;
#endif // #if SCENE_NFRAMES_EDITABLE
    Uint32 nFrames ;
    Uint32 nBytes ;
    Uint32 nSeconds ;

    // scene state
    bool shouldSaveLoop ;
    bool doesPulseExist ;
    bool isMuted ;


    /* Scene instance side private functions */

    // scene state
    void beginRecording(      void) ;
    void toggleRecordingState(void) ;

    // audio data
    bool addLoop(   Loop* newLoop) ;
    void deleteLoop(Uint32 loopN) ;
    void reset(     void) ;

    // peaks cache
    void scanPeaks(  Loop* loop , Uint32 loopN) ;
    void rescanPeaks(void) ;

    // getters/setters
    Loop* getLoop(Uint32 loopN) ;
//    Uint32 getLoopPos(  void) ;


  public:

    /* Scene instance side public functions */

    // getters/setters
    Uint32 getSceneN() ;
    Uint32 getDoesPulseExist() ;
    Uint32 getNLoops() ;
    Uint32 getCurrentPeakN() ;
    float  getCurrentSeconds() ;
    float  getTotalSeconds() ;
} ;


#endif // #ifndef _SCENE_H_
