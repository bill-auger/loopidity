
// quantities
#define DEFAULT_AUDIO_BUFFER_SIZE 33554432 // 2^25 (approx 3 min @ 48k)
//#define DEFAULT_AUDIO_BUFFER_SIZE 25165824 // 1024 * 1024 * 24 (approx 135 sec @ 48k)
//#define DEFAULT_AUDIO_BUFFER_SIZE 16777216 // 2^24 (approx 90 sec @ 48k)
//#define DEFAULT_AUDIO_BUFFER_SIZE 8388608  // 2^23 (approx 45 sec @ 48k)
//#define DEFAULT_AUDIO_BUFFER_SIZE 2097152  // 2^21 (approx 10 sec @ 48k)
//#define DEFAULT_AUDIO_BUFFER_SIZE 1048576  // 2^20 (approx 5 sec @ 48k)
#define NUM_SCENES                 3
#define NUM_LOOPS                  9 // per scene
#define LOOP_VOL_INC               0.1
#if FIXED_N_AUDIO_PORTS
#  define N_IN_CHANNELS            4
#  define N_OUT_CHANNELS           2
#else  // FIXED_N_AUDIO_PORTS        // TODO: implement setting N_*_CHANNELS via cmd line arg - GetTransientPeaks and updateVUMeters are especially brittle now
#  define MAX_INPUT_CHANNELS       2 // TODO: nyi - N_INPUT_CHANNELS currently used only used for memory check and scope cache
#  define MAX_OUTPUT_CHANNELS      2 // TODO: nyi - N_OUTPUT_CHANNELS currently used only for scope cache
#endif // FIXED_N_AUDIO_PORTS
#if SCENE_NFRAMES_EDITABLE
#  define BUFFER_MARGIN_SIZE       SampleRate
#  define TRIGGER_LATENCY_SIZE     1280 // nFrames - kludge to compensate for keyboard delay - optimized for BufferSize <= 128
#  define MINIMUM_LOOP_DURATION    2    // nSeconds
#  if INIT_JACK_BEFORE_SCENES
#    if ALLOW_BUFFER_ROLLOVER
#      define INITIAL_END_FRAMEN     (RecordBufferSize - nFramesPerPeriod)
#    else
#      define INITIAL_END_FRAMEN     RecordBufferSize
#    endif // #if ALLOW_BUFFER_ROLLOVER
#  endif // #if INIT_JACK_BEFORE_SCENES
#endif // #if SCENE_NFRAMES_EDITABLE
#if DRAW_EDIT_HISTOGRAM
#  define N_PEAKS_FINE             720 // should be divisible into 360
#else
#  define N_PEAKS_FINE             360 // should be divisible into 360
#endif // #if DRAW_EDIT_HISTOGRAM
#define PEAK_RADIUS                50 // TODO: PEAK_RADIUS and LOOP_DIAMETER are GUI specific - should probably be defined elsewhere
#define LOOP_DIAMETER              ((PEAK_RADIUS * 2) + 1)
#define N_PEAKS_COURSE             LOOP_DIAMETER

// string constants
#define APP_NAME                "Loopidity"
//#define CONNECT_ARG             "--connect"
#define MONITOR_ARG             "--nomon"
#define SCENE_CHANGE_ARG        "--noautoscenechange"
#define JACK_NAME               "loopidity"
#define JACK_INPUT1_PORT_NAME   "in1"
#define JACK_INPUT2_PORT_NAME   "in2"
#define JACK_INPUT3_PORT_NAME   "in3"
#define JACK_INPUT4_PORT_NAME   "in4"
#define JACK_OUTPUT1_PORT_NAME  "outL"
#define JACK_OUTPUT2_PORT_NAME  "outR"
#define LOOPIDITY_REINIT_MSG    "ERROR: Loopidity::Main() and Loopidity::Init() should be called only once"
#define N_SCENES_ERR_MSG        "ERROR: N_SCENES out of range - quitting"
#define INVALID_METADATA_MSG    "ERROR: Scene metadata state insane"
#define LOOPIDITY_INIT_FAIL_MSG "ERROR: Could not initialize Loopidity"
#define JACK_INIT_FAIL_MSG      "ERROR: Could not initialize JACK"
//#define FREEMEM_FAIL_MSG        "ERROR: Could not determine available memory - quitting"
#define INSUFFICIENT_MEMORY_MSG "ERROR: Insufficient memory initializng buffers - quitting"
#define JACK_SW_FAIL_MSG        "ERROR: Could not register JACK client - quitting"
#define JACK_HW_FAIL_MSG        "ERROR: Could not open ports for JACK - quitting"
#define OUT_OF_MEMORY_MSG       "ERROR: Out of Memory"

// sdl user events
#define EVT_NEW_LOOP      1
#define EVT_SCENE_CHANGED 2

// error states
#define JACK_INIT_SUCCESS 0
#define JACK_MEM_FAIL     1
#define JACK_SW_FAIL      2
#define JACK_HW_FAIL      3
