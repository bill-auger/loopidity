#ifndef _FEATURE_CONSTANTS_H_
#define _FEATURE_CONSTANTS_H_


// static features
//#define INIT_LOOPIDITY          1
#define INIT_JACK_BEFORE_SCENES 1
#define WAIT_FOR_JACK_INIT      0
#define FIXED_N_AUDIO_PORTS     1
//#define MEMORY_CHECK            1 // if 0 choose DEFAULT_AUDIO_BUFFER_SIZE wisely
#define FIXED_AUDIO_BUFFER_SIZE 0 // TODO: user defined/adjustable buffer sizes
#define SCENE_NFRAMES_EDITABLE  1

// runtime features
#define JACK_IO_READ_WRITE            1
#define JACK_IO_COPY                  1
#define HANDLE_KEYBOARD_EVENTS        1
#define HANDLE_MOUSE_EVENTS           0
#define HANDLE_USER_EVENTS            1
#define SCAN_LOOP_PEAKS               1
#define SCAN_PEAKS                    1
#define DRAW_STATUS                   1
#define DRAW_SCENES                   1
#define DRAW_EDIT_HISTOGRAM           (SCENE_NFRAMES_EDITABLE && 1)
#define DRAW_SCOPES                   1
#define DRAW_VUS                      1
#define DRAW_TEXT                     1
#define DRAW_DEBUG_TEXT               (DRAW_STATUS && 1)
#define AUTO_UNMUTE_LOOPS_ON_ROLLOVER 1

#if DRAW_STATUS
// #  define DRAW_MODE 0
/*
#  define UPDATE_MEMORY 0
#  define UPDATE_LOOP_PROGRESS 0
#  define UPDATE_VU 0
#  define VARDUMP 0
*/
#endif // #if DRAW_STATUS

#if DRAW_SCENES
#  define DRAW_SCENE_SCOPE             1
#  define DRAW_HISTOGRAMS              1
#  define DRAW_MUTED_HISTOGRAMS        1
#  define DRAW_PEAK_RINGS              1
#  define DRAW_LOOPS                   1
#  define DRAW_MUTED_LOOPS             0
#  define DRAW_RECORDING_LOOP          (DRAW_LOOPS && 1)
#  define DRAW_CURRENT_SCENE_INDICATOR 1
#endif // #if DRAW_SCENES

// Trace class features
#define DEBUG_TRACE              1
#define DEBUG_TRACE_ALL          0
#define DEBUG_TRACE_JACK         (DEBUG_TRACE && (DEBUG_TRACE_ALL || 1))
#define DEBUG_TRACE_LOOPIDITY    (DEBUG_TRACE && (DEBUG_TRACE_ALL || 1))
#define DEBUG_TRACE_LOOPIDITYSDL (DEBUG_TRACE && (DEBUG_TRACE_ALL || 1))
#define DEBUG_TRACE_SCENE        (DEBUG_TRACE && (DEBUG_TRACE_ALL || 1))
#define DEBUG_TRACE_SCENESDL     (DEBUG_TRACE && (DEBUG_TRACE_ALL || 1))
#define DEBUG_TRACE_TRACECLASS   (DEBUG_TRACE && (DEBUG_TRACE_ALL || 0))
#define DEBUG_TRACE_EVS          (DEBUG_TRACE && (DEBUG_TRACE_ALL || 1))
#define DEBUG_TRACE_IN           (DEBUG_TRACE && (DEBUG_TRACE_ALL || 1))
#define DEBUG_TRACE_OUT          (DEBUG_TRACE && (DEBUG_TRACE_ALL || 1))


#endif // #ifndef _FEATURE_CONSTANTS_H_
