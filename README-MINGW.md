
install mingw-get then mingw-develp[er-tools , mingw32-base , mingw32-gcc-g++ packages
add mingw and msys bin dirs to windows %PATH%
install jack for windows
download and extract SDL-mingw , SDL_gfx , SDL_ttf tarballs
  e.g. SDL-devel-1.2.15-mingw32.tar.gz
       SDL_gfx-2.0.25.tar.gz
       SDL_ttf-devel-2.0.11-VC.zip (pre-bult binaries)
compile and install SDL and SDL_gfx libs
copy headers to system include dir
```
TODO: only the SDL_ttf files must be manually copied if using pre-bult binaries
TODO: this must be borked
        /usr/include/ is            /c/MinGW/msys/1.0/include
        sdl headers must be in      /c/MinGW/msys/1.0/include
        but jack headers must be in /c/MinGW/include
  $ cp -rf /usr/include/* /c/MinGW/include/
  $ rm -rf /usr/include/
  $ ln -s /c/MinGW/include /usr/include
  $ cp /c/Program\ Files/Jack/includes/jack          /usr/include/
  $ cp /c/src/sdl/SDL-1.2.15/include/SDL/SDL.h       /usr/include/SDL/
  $ cp /c/src/sdl/SDL_gfx-2.0.25/SDL_gfxPrimitives.h /usr/include/SDL/
  $ cp /c/src/sdl/SDL_gfx-2.0.25/SDL_rotozoom.h      /usr/include/SDL/
  $ cp /c/src/sdl/SDL_ttf-2.0.11/include/SDL_ttf.h   /usr/include/SDL/
```
copy static libs to system lib dir
```
  $ cp /c/Program\ Files/Jack/lib/libjack.lib        /usr/lib/
  $ cp /c/src/sdl/SDL_gfx-2.0.25/Release/SDL_gfx.lib /usr/lib/
  $ cp /c/src/sdl/SDL_ttf-2.0.11/lib/x86/SDL_ttf.lib /usr/lib/
```
copy assets to build bin dirs
```
  $ cp ./assets/*                                          ./build/bin/Debug/
  $ cp ./assets/*                                          ./build/bin/Release
```
copy dynamic libs to build bin dirs
```
  $ cp /c/src/sdl/SDL-1.2.15/lib/x86/SDL.dll               ./build/bin/Debug/
  $ cp /c/src/sdl/SDL_gfx-2.0.25/Release/SDL_gfx.dll       ./build/bin/Debug/
  $ cp /c/src/sdl/SDL_ttf-2.0.11/lib/x86/SDL_ttf.dll       ./build/bin/Debug/
  $ cp /c/src/sdl/SDL_ttf-2.0.11/lib/x86/libfreetype-6.dll ./build/bin/Debug/
  $ cp ./build/bin/Debug/*.dll                             ./build/bin/Release
```
compile like:
```
  $ cd build
  $ MINGW=1 make
```
