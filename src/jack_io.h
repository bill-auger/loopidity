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
  public:

    /* JackIO class side public constants */

    static const Uint8 N_INPUT_CHANNELS ;
    static const Uint8 N_OUTPUT_CHANNELS ;


  private:

    /* JackIO class side private constants */
    static const Uint16 N_SCOPE_PEAKS ;
    static const Uint32 DEFAULT_BUFFER_SIZE ;
    static const Uint32 N_BYTES_PER_FRAME ;
    static const Uint32 GUI_UPDATE_IVL ;


    /* JackIO class side private varables */

    // JACK handles
    static jack_client_t* Client ;
#if FIXED_N_AUDIO_PORTS
    static jack_port_t*   InputPort1 ;
    static jack_port_t*   InputPort2 ;
    static jack_port_t*   InputPort3 ;
    static jack_port_t*   InputPort4 ;
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
    static std::vector<Sample> PeaksIn ;      // scope peaks (mono mix)
    static std::vector<Sample> PeaksOut ;     // scope peaks (mono mix)
#if FIXED_N_AUDIO_PORTS
    static Sample              PeaksVuIn[] ;  // VU peaks (per channel)
    static Sample              PeaksVuOut[] ; // VU peaks (per channel)
#else // TODO:
    static vector<Sample>      PeaksVuIn ;    // VU peaks (per channel)
    static vector<Sample>      PeaksVuOut ;   // VU peaks (per channel)
#endif // #if FIXED_N_AUDIO_PORTS

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
    static void                 SetCurrentScene(Scene* currentScene) ;
    static void                 SetNextScene   (Scene* nextScene) ;
    static std::vector<Sample>* GetPeaksIn     (void) ;
    static std::vector<Sample>* GetPeaksOut    (void) ;
    static Sample*              GetPeaksVuIn   (void) ;
    static Sample*              GetPeaksVuOut  (void) ;

    // helpers
    static void   ScanPeaks          (void) ;
    static Sample GetPeak            (Sample* buffer , Uint32 nFrames) ;
    static void   ResetTransientPeaks() ;


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
    static void         SetMetadata (jack_nframes_t sampleRate , jack_nframes_t nFramesPerPeriod) ;
    static void         Rollover    () ;
#endif // #if SCENE_NFRAMES_EDITABLE
} ;


#endif // #ifndef _JACK_IO_H_


/* NOTE: on RecordBuffer layout

    to allow for dynamic adjustment of seams and compensation for SDL key event delay
      the following are the buffer offsets used:

        invariant                  -->
            BeginFrameN <= beginFrameN <= currentFrameN < endFrameN <= EndFrameN
        initially                  -->
            beginFrameN     = currentFrameN = BeginFrameN = BufferMarginSize
            endFrameN       = EndFrameN     = (RecordBufferSize - BufferSize)
        on initial trigger         -->
            beginFrameN     = currentFrameN - TriggerLatencySize
        on accepting trigger       -->
            if (currentFrameN > beginFrameN + MINIMUM_LOOP_DURATION) // (issue #12)
                endFrameN     = currentFrameN
                nFrames       = endFrameN - TriggerLatencySize - beginFrameN
        after each rollover        -->
            beginFrameN     = BeginFrameN
            endFrameN       = BeginFrameN + nFrames
        after first rollover       -->
            currentFrameN   = BeginFrameN + TriggerLatencySize
        after subsequent rollovers -->
            currentFrameN   = BeginFrameN

    all of the above offsets are multiples of BufferSize (aka nFramesPerPeriod)
      excepting beginFrameN and nFrames

    on each rollover (TODO: this is currently only done when copying base loops)
        set currentFrameN = BeginFrameN (+ TriggerLatencySize if base loop)
        copy tail end of RecordBuffer back to the beginning of RecordBuffer
        this will be the LeadIn of the next loop (may or may not be part of a previous loop)
            let begin = (beginFrameN + nFrames) - BufferMarginSize
        initial base loop -->
            RecordBuffer[begin] upto RecordBuffer[endFrameN]
                --> RecordBuffer[0]
        subsequent loops  -->
            RecordBuffer[begin] upto RecordBuffer[endFrameN + TriggerLatencySize]
                --> RecordBuffer[0]

    on creation of each loop
        copy the entire buffer to include per loop LeadIn
            RecordBuffer[0] upto RecordBuffer[endFrameN]
                --> NewLoop[0]

    on creation of base loops
        set beginFrameN   = BeginFrameN
        set currentFrameN = BeginFrameN + TriggerLatencySize
        set endFrameN     = BeginFrameN + nFrames
        all subsequent loops will use beginFrameN and endFrameN (dynamically) as seams

    on next rollover following creation of each loop
        copy the leading BeginFrameN + BufferMarginSize to the end of the previous loop
          regardless of creating a new loop from the current data
        this will be the leadOut of the previous loop
            RecordBuffer[BeginFrameN] upto RecordBuffer[BeginFrameN + BufferMarginSize]
                --> PrevLoop[BeginFrameN + nFrames]


                            INITIAL RECORD BUFFER
          currentFrameN                                                 // dynamic offset
           beginFrameN                                        endFrameN // dynamic offsets
           BeginFrameN                                        EndFrameN // static offsets
                |                                                 |
 |<-MarginSize->|<--------(RecordBufferSize - MarginSize)-------->|     // sizes
 |<---LeadIn--->|<-----------------RolloverRange----------------->|     // 'zones'
 |<-------------------------RecordBuffer------------------------->|     // buffer


                              INITIAL BASE LOOP
                 beginFrameN                       endFrameN                  // offsets
thisLeadInBegin       |                                |                      // pointer
       |              |                                |
 |<-?->|<-MarginSize->|<-----------nFrames------------>|<-MarginSize->|<-?->| // sizes
 |     |<---LeadIn--->|<--------RolloverRange--------->|<--LeadOut--->|     | // 'zones'
 |     |<-------------------Source-------------------->|              |     | // data
 |     |<--------------------Dest--------------------->|<--Pending--->|     | // data
 |     |<--------------------------NewLoop--------------------------->|     | // dest
 |<------------------------------RecordBuffer------------------------------>| // source


                               SUBSEQUENT LOOPS
           beginFrameN                       endFrameN                        // offsets
thisLeadInBegin |                                |                            // pointer
 |              |                                |
 |<-MarginSize->|<-----------nFrames------------>|<-MarginSize->|<----?---->| // sizes
 |<---LeadIn--->|<--------RolloverRange--------->|<--LeadOut--->|           | // 'zones'
 |<-------------------Source-------------------->|              |           | // data
 |<--------------------Dest--------------------->|<--Pending--->|           | // data
 |<--------------------------NewLoop--------------------------->|           | // dest
 |<------------------------------RecordBuffer------------------------------>| // source


                             SHIFT NEXT LEAD-IN
           beginFrameN                      endFrameN                         // offsets
                |         nextLeadInBegin       |                             // pointer
                |                |              |
 |<-MarginSize->|                |<-MarginSize->|                           | // sizes
 |<----Dest---->|                |<---Source--->|                           | // data
 |<------------------------------RecordBuffer------------------------------>| // source/dest


                        COPY PREVIOUS LEAD_OUT (TODO)
           beginFrameN                       endFrameN                        // offsets
                |           mixBegin             |                            // pointer
                |              |                 |
 |<-MarginSize->|<-MarginSize->|                 |<-MarginSize->|           | // sizes
 |              |<---Source--->|                 |<----Dest---->|           | // data
 |<---LeadIn--->|<--------RolloverRange--------->|<--LeadOut--->|           | // 'zones'
 |<--------------------------NewLoop--------------------------->|           | // dest
 |<------------------------------RecordBuffer------------------------------>| // source
*/
