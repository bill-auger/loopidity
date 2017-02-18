/*\ Loopidity - multitrack audio looper designed for live handsfree use
|*| https://github.com/bill-auger/loopidity/issues/
|*| Copyright 2013,2015 Bill Auger - https://bill-auger.github.io/
|*|
|*| This file is part of Loopidity.
|*|
|*| Loopidity is free software: you can redistribute it and/or modify
|*| it under the terms of the GNU General Public License version 3
|*| as published by the Free Software Foundation.
|*|
|*| Loopidity is distributed in the hope that it will be useful,
|*| but WITHOUT ANY WARRANTY; without even the implied warranty of
|*| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
|*| GNU General Public License for more details.
|*|
|*| You should have received a copy of the GNU General Public License
|*| along with Loopidity.  If not, see <http://www.gnu.org/licenses/>.
\*/

/*\ Loopidity MVC classes:
|*|  Loopidity    - main  controller class (==                    0 instances)
|*|  LoopiditySDL - main  view       class (==                    0 instances)
|*|  Scene        - scene model      class (==           NUM_SCENES instances)
|*|  SceneSDL     - scene view       class (==           NUM_SCENES instances)
|*|  Loop         - loop  model      class (<= N_LOOPS * NUM_SCENES instances)
|*|  LoopSDL      - loop  view       class (<= N_LOOPS * NUM_SCENES instances)
|*|  JackIO       - JACK  wrapper    class (==                    0 instances)
|*|  Trace        - debug trace      class (==                    0 instances)
\*/


#include "loopidity.h"


/* entry point */

int main(int argc , char** argv) { return Loopidity::Main(argc , argv) ; }
