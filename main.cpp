/*
Loopidity MVC classes:
Loopidity    - controller class
LoopiditySDL - main view class
Scene        - scene model class
SceneSDL     - scene view class
Loop         - loop model class
LoopSDL      - loop view class
JackIO       - JACK audio i/o wrapper class
Trace        - debug trace class
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
