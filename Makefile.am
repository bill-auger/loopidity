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
                    src/trace/trace.cpp   \
                    src/trace/trace.h


CUSTOM_CXXFLAGS  = -DLOOPIDITY_BINDIR=\"$(bindir)\"
CUSTOM_CXXFLAGS += -DLOOPIDITY_DATADIR=\"$(loopiditydir)\"
CUSTOM_CXXFLAGS += -std=c++11 -Wall -Wextra -Werror
AM_CXXFLAGS      = $(CXXFLAGS) $(JACK_CFLAGS) $(SDL_CFLAGS) $(X11_CFLAGS) $(CUSTOM_CXXFLAGS)
AM_LDFLAGS       = $(LIBS) $(JACK_LIBS) $(SDL_LIBS) $(X11_LIBS)


installcheck-local:
	echo "installcheck-local"
	stat --printf='' $(bindir)/loopidity
	stat --printf='' $(loopiditydir)/
	stat --printf='' $(loopiditydir)/COPYING.fonts
	stat --printf='' $(loopiditydir)/histogram_gradient.bmp
	stat --printf='' $(loopiditydir)/LICENSES
	stat --printf='' $(loopiditydir)/loop_gradient.argb.bmp
	stat --printf='' $(loopiditydir)/scope_gradient.bmp
	stat --printf='' $(loopiditydir)/Purisa.ttf

uninstall-hook:
	-rmdir ${loopiditydir}

distclean-local:
	-rm -rf autom4te.cache/ src/.deps/ src/trace/.deps/        2> /dev/null || true
	-rm -f  aclocal.m4 autoscan.log configure.scan test-driver 2> /dev/null || true
