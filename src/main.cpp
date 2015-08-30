/*
Loopidity MVC classes:
Loopidity    - main  controller class (== 0                    instances)
LoopiditySDL - main  view       class (== 0                    instances)
Scene        - scene model      class (== NUM_SCENES           instances)
SceneSDL     - scene view       class (== NUM_SCENES           instances)
Loop         - loop  model      class (<= NUM_SCENES * N_LOOPS instances)
LoopSDL      - loop  view       class (<= NUM_SCENES * N_LOOPS instances)
JackIO       - JACK  wrapper    class (== 0                    instances)
Trace        - debug trace      class (== 0                    instances)
*/


#ifdef _WIN32
#  include <fstream>
#endif // _WIN32

#include "loopidity.h"


/* entry point */

int main(int argc , char** argv)
{
DEBUG_TRACE_MAIN_IN

  int exitStatus = Loopidity::Main(argc , argv) ; Loopidity::Cleanup() ;

DEBUG_TRACE_MAIN_OUT

  return exitStatus ;
}
