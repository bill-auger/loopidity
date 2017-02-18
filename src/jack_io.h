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


#ifndef _JACK_IO_H_
#define _JACK_IO_H_


#include "loopidity.h"


class JackIO
{
  private:

    /* JackIO class side private constants */

    static const Uint32 N_PORTS ;
    static const Uint32 N_INPUT_PORTS ;
    static const Uint32 N_OUTPUT_PORTS ;
    static const Uint32 N_TRANSIENT_PEAKS ;
    static const Uint32 DEFAULT_BUFFER_SIZE ;
    static const Uint32 N_BYTES_PER_FRAME ;
    static const Uint32 GUI_UPDATE_IVL ;


    /* JackIO class side private varables */

    // JACK handles
    static jack_client_t* Client ;
#if FIXED_N_AUDIO_PORTS
    static jack_port_t*   InputPort1 ;
    static jack_port_t*   InputPort2 ;
    static jack_port_t*   OutputPort1 ;
    static jack_port_t*   OutputPort2 ;
#else // TODO: much
#endif // #if FIXED_N_AUDIO_PORTS

    // app state
    static Scene* CurrentScene ;
    static Scene* NextScene ;

    // audio data
    static Uint32  RecordBufferSize ;
#if FIXED_N_AUDIO_PORTS
    static Sample* RecordBuffer1 ;
    static Sample* RecordBuffer2 ;
#  if SCENE_NFRAMES_EDITABLE
/*
    static Sample* LeadInBuffer1 ;
    static Sample* LeadInBuffer2 ;
    static Sample* LeadOutBuffer1 ;
    static Sample* LeadOutBuffer2 ;
*/
#  endif // #if SCENE_NFRAMES_EDITABLE
#else // TODO: much
#endif // #if FIXED_N_AUDIO_PORTS

    // peaks data
    static std::vector<Sample> PeaksIn ;                       // scope peaks (mono mix)
    static std::vector<Sample> PeaksOut ;                      // scope peaks (mono mix)
#if FIXED_N_AUDIO_PORTS
    static Sample              TransientPeaks[N_AUDIO_PORTS] ; // VU peaks (per channel)
#else // TODO:
    static Sample              TransientPeaks[N_AUDIO_PORTS] ; // VU peaks (per channel)
#endif // #if FIXED_N_AUDIO_PORTS
    static Sample              TransientPeakInMix ;
//    static Sample              TransientPeakOutMix ;

    // event structs
    static SDL_Event NewLoopEvent ;
    static Uint32    NewLoopEventSceneN ;
    static Loop*     NewLoopEventLoop ;
    static SDL_Event SceneChangeEvent ;
    static Uint32    SceneChangeEventSceneN ;

    // metadata
    static jack_nframes_t SampleRate ;
//    static Uint32       NBytesPerSecond ;
//    static jack_nframes_t     NFramesPerPeriod ;
#if SCENE_NFRAMES_EDITABLE
    static Uint32         MinLoopSize ;
    static Uint32         BufferMarginSize ;
    static Uint32         TriggerLatencySize ;
#  if INIT_JACK_BEFORE_SCENES
    static Uint32         EndFrameN ;
#  endif // #if INIT_JACK_BEFORE_SCENES
    static Uint32         BeginFrameN ;
    static Uint32         BufferMarginsSize ;
    static Uint32         BytesPerPeriod ;
    static Uint32         BufferMarginBytes ;
    static Uint32         TriggerLatencyBytes ;
#else
#  if INIT_JACK_BEFORE_SCENES
    static Uint32         EndFrameN ;
#  endif // #if INIT_JACK_BEFORE_SCENES
    static Uint32         BytesPerPeriod ;
#endif // #if SCENE_NFRAMES_EDITABLE
    static Uint32         FramesPerGuiInterval ;

    // misc flags
    static bool ShouldMonitorInputs ;


  public:

    /* JackIO class side public functions */

    // setup
#if INIT_JACK_BEFORE_SCENES
    static Uint32 Init(bool shouldMonitorInputs , Uint32 recordBufferSize) ;
#else
    static Uint32 Init(Scene* currentScene     , bool shouldMonitorInputs ,
                       Uint32 recordBufferSize                            ) ;
#endif // #if INIT_JACK_BEFORE_SCENES
    static void Reset( Scene* currentScene) ;

    // getters/setters
#if !INIT_JACK_BEFORE_SCENES
    static Uint32          GetRecordBufferSize(void) ;
#endif // #if !INIT_JACK_BEFORE_SCENES
/*
    static Uint32    GetNFramesPerPeriod(void) ;
    static Uint32    GetFrameSize(       void) ;
    static Uint32    GetSampleRate(      void) ;
    static Uint32    GetNBytesPerSecond(void) ;
*/
    static void                 SetCurrentScene(   Scene* currentScene) ;
    static void                 SetNextScene(      Scene* nextScene) ;
    static std::vector<Sample>* GetPeaksIn(        void) ;
    static std::vector<Sample>* GetPeaksOut(       void) ;
    static Sample*              GetTransientPeaks( void) ;
    static Sample*              GetTransientPeakIn(void) ;
//    static Sample*              GetTransientPeakOut(   void) ;

    // helpers
    static void   ScanTransientPeaks(void) ;
    static Sample GetPeak(           Sample* buffer , Uint32 nFrames) ;


  private:

    /* JackIO class side private functions */

    // JACK server callbacks
    static int  ProcessCallback(   jack_nframes_t nFramesPerPeriod , void* unused) ;
    static int  SampleRateCallback(jack_nframes_t sampleRate ,       void* unused) ;
    static int  BufferSizeCallback(jack_nframes_t nFramesPerPeriod , void* unused) ;
    static void ShutdownCallback(                                    void* unused) ;

    // helpers
    static jack_port_t* RegisterPort(const char* portName , unsigned long portType) ;
#if SCENE_NFRAMES_EDITABLE
    static void         SetMetadata( jack_nframes_t sampleRate , jack_nframes_t nFramesPerPeriod) ;
#endif // #if SCENE_NFRAMES_EDITABLE
} ;


#endif // #ifndef _JACK_IO_H_
