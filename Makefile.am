loopiditydir      = $(pkgdatadir)
bin_PROGRAMS      = loopidity
loopidity_DATA    = assets/COPYING.fonts          \
                    assets/COPYING.images         \
                    assets/histogram_gradient.bmp \
                    assets/LICENSES               \
                    assets/loop_gradient.argb.bmp \
                    assets/README.md              \
                    assets/Purisa.ttf             \
                    assets/scope_gradient.bmp
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


installcheck-local:
	echo "installcheck-local"
	stat $(bindir)/loopidity                    > /dev/null
	stat $(loopiditydir)/                       > /dev/null
	stat $(loopiditydir)/COPYING.fonts          > /dev/null
	stat $(loopiditydir)/histogram_gradient.bmp > /dev/null
	stat $(loopiditydir)/LICENSES               > /dev/null
	stat $(loopiditydir)/loop_gradient.argb.bmp > /dev/null
	stat $(loopiditydir)/scope_gradient.bmp     > /dev/null
	stat $(loopiditydir)/Purisa.ttf             > /dev/null

uninstall-hook:
	-rmdir ${loopiditydir}

distclean-local:
	-rm -rf autom4te.cache/ src/.deps/ aclocal.m4
