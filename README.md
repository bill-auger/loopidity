### This is Loopidity *- A multi-track, multi-channel, looping audio recorder for GNU/Linux and Windows - designed for live handsfree use*
| build status: | linux [![linux build status][linux-img]][travis] | windows [![windows build status][windows-img]][travis] | latest-dev [![latest build status][latest-img]][travis] |
| ------------- | ------------------------------------------------ | ------------------------------------------------------ | ------------------------------------------------------- |

#### build
build dependencies:
* libsdl1.2
* libSDL_gfx
* libSDL_ttf
* libjack2
* (also libx11 for *nix targets)

build recipe:
* windows builds: refer to [README-MINGW.md][README-MINGW]
* *nix builds:
```bash
  $ ./configure
  $ make
  # make install
```


[linux-img]:    https://travis-ci.org/bill-auger/loopidity.svg?branch=linux
[windows-img]:  https://travis-ci.org/bill-auger/loopidity.svg?branch=mingw
[latest-img]:   https://travis-ci.org/bill-auger/loopidity.svg
[travis]:       https://travis-ci.org/bill-auger/loopidity
[README-MINGW]: https://github.com/bill-auger/loopidity/blob/master/README-MINGW.md
