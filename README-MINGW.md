* install mingw-get then mingw-develp[er-tools , mingw32-base , mingw32-gcc-g++ packages
* add mingw and msys bin dirs to windows %PATH%
* install jack for windows
* download and extract SDL-mingw , SDL_gfx , SDL_ttf tarballs
  e.g. SDL-devel-1.2.15-mingw32.tar.gz
       SDL_gfx-2.0.25.tar.gz
       SDL_ttf-devel-2.0.11-VC.zip (pre-bult binaries)
* compile and install SDL, SDL_gfx, and SDL_ttf libs
* copy any pre-built libs and headers to system dirs
```bash
  $ cp /c/Program\ Files/Jack/includes/jack          /usr/include/
  $ cp /c/src/sdl/SDL-1.2.15/include/SDL/SDL.h       /usr/include/SDL/
  $ cp /c/src/sdl/SDL_gfx-2.0.25/SDL_gfxPrimitives.h /usr/include/SDL/
  $ cp /c/src/sdl/SDL_gfx-2.0.25/SDL_rotozoom.h      /usr/include/SDL/
  $ cp /c/src/sdl/SDL_ttf-2.0.11/include/SDL_ttf.h   /usr/include/SDL/
```
* copy static libs to system lib dir
```bash
  $ cp /c/Program\ Files/Jack/lib/libjack.lib        /usr/lib/
  $ cp /c/src/sdl/SDL_gfx-2.0.25/Release/SDL_gfx.lib /usr/lib/
  $ cp /c/src/sdl/SDL_ttf-2.0.11/lib/x86/SDL_ttf.lib /usr/lib/
```

workaround if system dirs are borked - e.g. GitBash gets MinGW confused
```bash
### * /usr/include/ is            /c/MinGW/msys/1.0/include
### * sdl headers must be in      /c/MinGW/msys/1.0/include
### * but jack headers must be in /c/MinGW/include

  $ cp -rf /usr/include/* /c/MinGW/include/
  $ rm -rf /usr/include/
  $ ln -s /c/MinGW/include /usr/include
```
