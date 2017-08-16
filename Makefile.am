loopiditydir      = $(pkgdatadir)
bin_PROGRAMS      = loopidity
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
loopidity_DATA    = assets/histogram_gradient.bmp \
                    assets/loop_gradient.argb.bmp \
                    assets/Purisa.ttf             \
                    assets/scope_gradient.bmp
doc_DATA          = COPYING                \
                    LICENSES               \
                    assets/COPYING.fonts   \
                    assets/LICENSES.assets
man1_MANS         = doc/loopidity.1
EXTRA_DIST        = $(man_MANS)


CUSTOM_CXXFLAGS  = -DLOOPIDITY_BINDIR=\"$(bindir)\"
CUSTOM_CXXFLAGS += -DLOOPIDITY_DATADIR=\"$(loopiditydir)\"
CUSTOM_CXXFLAGS += -Wall -Wextra -Wformat -Werror
AM_CXXFLAGS      = $(CXXFLAGS) $(JACK_CFLAGS) $(SDL_CFLAGS) $(X11_CFLAGS) $(CUSTOM_CXXFLAGS)
AM_LDFLAGS       = $(LIBS) $(JACK_LIBS) $(SDL_LIBS) $(X11_LIBS)


installcheck-local:
	echo "installcheck-local"
	stat --printf='' $(bindir)/loopidity
	stat --printf='' $(loopiditydir)/
	stat --printf='' $(loopiditydir)/histogram_gradient.bmp
	stat --printf='' $(loopiditydir)/loop_gradient.argb.bmp
	stat --printf='' $(loopiditydir)/scope_gradient.bmp
	stat --printf='' $(loopiditydir)/Purisa.ttf
	stat --printf='' $(docdir)/COPYING
	stat --printf='' $(docdir)/LICENSES
	stat --printf='' $(docdir)/COPYING.fonts
	stat --printf='' $(docdir)/LICENSES.assets
	stat --printf='' $(man1dir)/loopidity.1


uninstall-hook:
	-rmdir ${loopiditydir}
	-rmdir ${docdir}


distclean-local:
	-rm -rf autom4te.cache/ src/.deps/ src/trace/.deps/        2> /dev/null || true
	-rm -f  aclocal.m4 autoscan.log configure.scan test-driver 2> /dev/null || true
