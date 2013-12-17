/*
Loopidity MVC classes:
Loopidity    - main  controller class (== 0                  instances)
LoopiditySDL - main  view       class (== 0                  instances)
Scene        - scene model      class (== N_SCENES           instances)
SceneSDL     - scene view       class (== N_SCENES           instances)
Loop         - loop  model      class (<= N_SCENES * N_LOOPS instances)
LoopSDL      - loop  view       class (<= N_SCENES * N_LOOPS instances)
JackIO       - JACK  wrapper    class (== 0                  instances)
Trace        - debug trace      class (== 0                  instances)
*/

#include "loopidity.h"


/* entry point */

int main(int argc , char** argv)
{
	int exitStatus = Loopidity::Main(argc , argv) ; Loopidity::Cleanup() ;

#if DEBUG_TRACE
if (exitStatus) cout << INIT_FAIL_MSG << endl ;
#endif

	return exitStatus ;
}
