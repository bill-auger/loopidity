loopiditydir        = $(pkgdatadir)
bin_PROGRAMS        = loopidity
loopidity_SOURCES   = jack_io.cpp               \
                      jack_io.h                 \
                      loopidity.cpp             \
                      loopidity.h               \
                      loopidity_sdl.cpp         \
                      loopidity_sdl.h           \
                      main.cpp                  \
                      scene.cpp                 \
                      scene.h                   \
                      scene_sdl.cpp             \
                      scene_sdl.h               \
                      data/fonts.cpp            \
                      data/fonts.h              \
                      data/images.cpp           \
                      data/images.h             \
                      data/histogram_gradient.h \
                      data/loop_gradient.h      \
                      data/scope_gradient.h     \
                      trace/trace.cpp           \
                      trace/trace.h
# loopidity_CFLAGS    = -O0
loopidity_CXXFLAGS  = $(JACK_CFLAGS) $(SDL_CFLAGS) $(SDL_gfx_CFLAGS) $(SDL_ttf_CFLAGS) $(X11_CFLAGS)
loopidity_CXXFLAGS += -DLOOPIDITY_BINDIR=\"$(bindir)\"
loopidity_CXXFLAGS += -DLOOPIDITY_DATADIR=\"$(loopiditydir)\"
loopidity_CXXFLAGS += -Wall -Wextra -Wformat -Werror
loopidity_LDADD     = $(JACK_LIBS) $(SDL_LIBS) $(SDL_gfx_LIBS) $(SDL_ttf_LIBS) $(X11_LIBS)

# AM_CFLAGS   = $(CFLAGS) $(loopidity_CFLAGS)
# AM_CXXFLAGS = $(CFLAGS) $(CXXFLAGS) $(loopidity_CXXFLAGS)
