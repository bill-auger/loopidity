### This is Loopidity *- A multi-track, multi-channel, looping audio recorder for GNU/Linux and Windows - designed for live handsfree use*

[![Join the chat at https://gitter.im/bill-auger/loopidity](https://badges.gitter.im/bill-auger/loopidity.svg)](https://gitter.im/bill-auger/loopidity?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

| build status: | linux [![linux build status][linux-img]][travis] | windows [![windows build status][windows-img]][travis] | latest-dev [![latest build status][latest-img]][travis] |
| ------------- | ------------------------------------------------ | ------------------------------------------------------ | ------------------------------------------------------- |

#### build
* binaries coming soon - bake yer own for now
* build requires libsdl1.2 , libSDL_gfx , libSDL_ttf and libjack2 (also X11 on *nix)
* compiler must support c++11 features
* windows builds refer to [README-MINGW.md][README-MINGW]
* build with gnu toolchain
```bash
  $ cd build
  $ make release
  $ make debug
  $ make         # [ all ]
```
* or compile, run/debug, and/or clean via helper script
```bash
  $ ./mk debug
  $ ./mk release
  $ ./mk debug   clean
  $ ./mk release clean
```
* build with code::blocks
```bash
    $  cd build
    $  codeblocks loopidity.cbp
```


[linux-img]:    https://travis-ci.org/bill-auger/loopidity.svg?branch=linux
[windows-img]:  https://travis-ci.org/bill-auger/loopidity.svg?branch=mingw
[latest-img]:   https://travis-ci.org/bill-auger/loopidity.svg
[travis]:       https://travis-ci.org/bill-auger/loopidity
[README-MINGW]: https://github.com/bill-auger/loopidity/blob/master/README-MINGW.md
