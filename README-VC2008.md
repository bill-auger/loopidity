README-VC2008

install jack for windows
download and extract SDL-mingw , SDL_gfx , SDL_ttf tarballs
get a version of SDL that includes SDL_config_win32.h instead of SDL_config_windows.h
change SDL_config_win32.h line 35
```
  #elif defined(_MSC_VER) && (_MSC_VER < 1400)
```
adjust "C/C++ -> Additional Include Directories" to point to the following headers
```
  jack.h              - e.g. C:\Program Files\Jack\includes\jack\
  SDL.h               - e.g. C:\src\sdl\SDL-1.2.15\include\SDL\
  SDL_gfxPrimitives.h - e.g. C:\src\sdl\SDL_gfx-2.0.25\
  SDL_ttf.h           - e.g  C:\src\sdl\SDL_ttf-2.0.11\include\
```
adjust "Linker -> Additional Library Directories" to point to the following libs
```
  libjack.lib         - e.g. C:\Program Files\Jack\lib\
  SDL.lib             - e.g. C:\src\sdl\SDL-1.2.15\lib\x86\
  SDL_gfx.lib         - e.g. C:\src\sdl\SDL_gfx-2.0.25\Debug\
  SDL_ttf.lib         - e.g. C:\src\sdl\SDL_ttf-2.0.11\lib\x86\
```
copy assets to build bin dirs
```
  $ COPY .\assets\*                                          .\build\Debug\
  $ COPY .\assets\*                                          .\build\Release\
```
copy dynamic libs to build bin dirs
```
  $ COPY C:\src\sdl\SDL-1.2.15-MinGW\lib\x86\SDL.dll         .\build\Debug\
  $ COPY C:\src\sdl\SDL_gfx-2.0.25\Release\SDL_gfx.dll       .\build\Debug\
  $ COPY C:\src\sdl\SDL_ttf-2.0.11\lib\x86\SDL_ttf.dll       .\build\Debug\
  $ COPY C:\src\sdl\SDL_ttf-2.0.11\lib\x86\libfreetype-6.dll .\build\Debug\
  $ COPY .\build\Debug\*.dll                                 .\build\Release\
```
