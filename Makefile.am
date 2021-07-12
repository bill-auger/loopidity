loopiditydir      = $(pkgdatadir)
bin_PROGRAMS      = loopidity
loopidity_SOURCES = src/jack_io.cpp                \
                    src/jack_io.h                  \
                    src/loopidity.cpp              \
                    src/loopidity.h                \
                    src/loopidity_sdl.cpp          \
                    src/loopidity_sdl.h            \
                    src/main.cpp                   \
                    src/scene.cpp                  \
                    src/scene.h                    \
                    src/scene_sdl.cpp              \
                    src/scene_sdl.h                \
                    src/blobs/fonts.cpp            \
                    src/blobs/fonts.h              \
                    src/blobs/images.cpp           \
                    src/blobs/images.h             \
                    src/blobs/histogram_gradient.h \
                    src/blobs/loop_gradient.h      \
                    src/blobs/scope_gradient.h     \
                    src/trace/trace.cpp            \
                    src/trace/trace.h
doc_DATA          = COPYING              \
                    LICENSES             \
                    blobs/COPYING.fonts  \
                    blobs/LICENSES.blobs
man1_MANS         = doc/loopidity.1
EXTRA_DIST        = $(man_MANS)


loopidity_CXXFLAGS  = $(JACK_CFLAGS) $(SDL_CFLAGS) $(SDL_gfx_CFLAGS) $(SDL_ttf_CFLAGS) $(X11_CFLAGS)
loopidity_CXXFLAGS += -DLOOPIDITY_BINDIR=\"$(bindir)\"
loopidity_CXXFLAGS += -DLOOPIDITY_DATADIR=\"$(loopiditydir)\"
loopidity_CXXFLAGS += -Wall -Wextra -Wformat -Werror
loopidity_LDADD     = $(JACK_LIBS) $(SDL_LIBS) $(SDL_gfx_LIBS) $(SDL_ttf_LIBS) $(X11_LIBS)


installcheck-local:
	echo "installcheck-local"
	stat --printf='' $(bindir)/loopidity
	stat --printf='' $(loopiditydir)/
	stat --printf='' $(docdir)/COPYING
	stat --printf='' $(docdir)/LICENSES
	stat --printf='' $(docdir)/COPYING.fonts
	stat --printf='' $(docdir)/LICENSES.blobs
	stat --printf='' $(man1dir)/loopidity.1


uninstall-hook:
	-rmdir ${loopiditydir}
	-rmdir ${docdir}


distclean-local:
	-rm -rf autom4te.cache/ build-aux/ src/.deps/ src/trace/.deps/ 2> /dev/null || true
	-rm -f  aclocal.m4 autoscan.log configure.scan test-driver     2> /dev/null || true
