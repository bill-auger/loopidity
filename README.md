### This is Loopidity *- A multi-track, multi-channel, looping audio recorder - Designed for live, handsfree use*


#### === how it's done ===
##### dependencies:
* libsdl1.2
* libSDL_gfx
* libSDL_ttf
* libjack2
* (also libx11 for *nix targets)

##### recipe:
* windows builds: refer to [README-MINGW.md][README-MINGW]
* *nix builds:

```bash
  $ ./configure
  $ make
  # make install
```

if the build does not succeed, you could try reconfiguring autotools first:

```bash
  $ autoreconf -ifv
  $ ./configure
  $ make
  # make install
```

NOTE: `make install` is optional. `loopidity` can be run in-tree.


#### === how it's run ===

* start JACK first!
* if installed:
```bash
  $ loopidity
```
* or, run in-tree:
```bash
  $ ./src/loopidity
```


#### === how it's fun ===

the [wiki][wiki] describes how loopidity works and how to use it.


[wiki]:         https://github.com/bill-auger/loopidity/wiki
[README-MINGW]: https://github.com/bill-auger/loopidity/blob/master/README-MINGW.md
