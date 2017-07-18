loopiditydir      = $(pkgdatadir)
bin_PROGRAMS      = loopidity
loopidity_DATA    = assets/*.bmp \
                    assets/*.ttf
loopidity_SOURCES = src/jack_io.cpp       \
                    src/jack_io.h         \
                    src/loopidity.cpp     \
                    src/loopidity.h       \
                    src/loopidity_sdl.cpp \
                    src/loopidity_sdl.h   \
                    src/main.cpp          \
                    src/scene.cpp         \
                    src/scene.h           \
                    src/scene_sdl.cpp     \
                    src/scene_sdl.h       \
                    src/trace.cpp         \
                    src/trace.h


CUSTOM_CXXFLAGS  = -DLOOPIDITY_BINDIR=\"$(bindir)\"
CUSTOM_CXXFLAGS += -DLOOPIDITY_DATADIR=\"$(loopiditydir)\"
CUSTOM_CXXFLAGS += -Wall -Wextra
# CUSTOM_CXXFLAGS += -Wall -Wextra -Werror
AM_CXXFLAGS      = $(CXXFLAGS) $(JACK_CFLAGS) $(SDL_CFLAGS) $(X11_CFLAGS) $(CUSTOM_CXXFLAGS)
AM_LDFLAGS       = $(LIBS) $(JACK_LIBS) $(SDL_LIBS) $(X11_LIBS)


uninstall-hook:
	-rmdir ${loopiditydir}

distclean-local:
	-rm -rf autom4te.cache/ src/.deps/ aclocal.m4
