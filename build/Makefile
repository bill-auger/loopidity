
ifeq (${MSYSTEM} , MINGW32)
MINGW = 1
endif

WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres

INC =
RESINC =
LIBDIR =
ifdef MINGW
CFLAGS = -std=gnu++0x -Wall -I/usr/include -I/usr/include/SDL -D_GNU_SOURCE=1 -Dmain=SDL_main
LIB = /usr/lib/libjack.lib /usr/lib/libSDL_gfx.dll.a /usr/lib/SDL_ttf.lib
LDFLAGS = -L/usr/lib -lmingw32 -lSDLmain -lSDL -mwindows
else
CFLAGS = -std=gnu++0x -Wall `sdl-config --cflags`
LIB =
LDFLAGS = `sdl-config --libs` `pkg-config  x11 --libs` `pkg-config  SDL_ttf --libs` `pkg-config  SDL_gfx --libs` `pkg-config  jack --libs`
endif

INC_DEBUG = $(INC)
CFLAGS_DEBUG = $(CFLAGS) -g
RESINC_DEBUG = $(RESINC)
RCFLAGS_DEBUG = $(RCFLAGS)
LIBDIR_DEBUG = $(LIBDIR)
LIB_DEBUG = $(LIB)
LDFLAGS_DEBUG = $(LDFLAGS)
OBJDIR_DEBUG = ./obj/Debug
BINDIR_DEBUG = ./bin/Debug
DEP_DEBUG =
OUT_DEBUG = $(BINDIR_DEBUG)/loopidity-dbg

INC_RELEASE = $(INC)
CFLAGS_RELEASE = $(CFLAGS) -O3
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE = $(LDFLAGS) -s
OBJDIR_RELEASE = ./obj/Release
BINDIR_RELEASE = ./bin/Release
DEP_RELEASE =
OUT_RELEASE = $(BINDIR_RELEASE)/loopidity

ASSETS_DIR = ../assets
ifdef MINGW
SYSBIN_DIR = /usr/bin
endif

OBJ_DEBUG = $(OBJDIR_DEBUG)/__/src/jack_io.o       \
            $(OBJDIR_DEBUG)/__/src/loopidity.o     \
            $(OBJDIR_DEBUG)/__/src/loopidity_sdl.o \
            $(OBJDIR_DEBUG)/__/src/main.o          \
            $(OBJDIR_DEBUG)/__/src/scene.o         \
            $(OBJDIR_DEBUG)/__/src/scene_sdl.o     \
            $(OBJDIR_DEBUG)/__/src/trace.o
OBJ_RELEASE = $(OBJDIR_RELEASE)/__/src/jack_io.o       \
              $(OBJDIR_RELEASE)/__/src/loopidity.o     \
              $(OBJDIR_RELEASE)/__/src/loopidity_sdl.o \
              $(OBJDIR_RELEASE)/__/src/main.o          \
              $(OBJDIR_RELEASE)/__/src/scene.o         \
              $(OBJDIR_RELEASE)/__/src/scene_sdl.o     \
              $(OBJDIR_RELEASE)/__/src/trace.o
ASSETS = histogram_gradient.bmp \
         loop_gradient.argb.bmp \
         scope_gradient.bmp     \
         Purisa.ttf


all: debug release


clean: clean_debug clean_release


debug: before_debug out_debug after_debug

before_debug:
	@echo ; echo "building debug configuration" ;
	@test -d $(BINDIR_DEBUG) || mkdir -p $(BINDIR_DEBUG)
	@test -d $(OBJDIR_DEBUG)/__/src || mkdir -p $(OBJDIR_DEBUG)/__/src

after_debug: $(ASSETS) $(WINLIBS)

out_debug: before_debug $(OBJ_DEBUG) $(DEP_DEBUG)
	@echo "linking debug binary"
	@$(LD) $(LIBDIR_DEBUG) -o $(OUT_DEBUG) $(OBJ_DEBUG)  $(LDFLAGS_DEBUG) $(LIB_DEBUG)

$(OBJ_DEBUG): $(OBJDIR_DEBUG)/__/src/%.o: ../src/%.cpp
	@echo "  -> compiling $*.cpp"
	@$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c $< -o $@

clean_debug:
	@rm -f  $(OBJ_DEBUG) $(OUT_DEBUG)
	@rm -rf $(BINDIR_DEBUG)
	@rm -rf $(OBJDIR_DEBUG)/__/src


release: before_release out_release after_release

before_release:
	@echo ; echo "building release configuration" ;
	@test -d $(BINDIR_RELEASE) || mkdir -p $(BINDIR_RELEASE)
	@test -d $(OBJDIR_RELEASE)/__/src || mkdir -p $(OBJDIR_RELEASE)/__/src

after_release: $(ASSETS) $(WINLIBS)

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	@echo "linking release binary"
	@$(LD) $(LIBDIR_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE)  $(LDFLAGS_RELEASE) $(LIB_RELEASE)

$(OBJ_RELEASE): $(OBJDIR_RELEASE)/__/src/%.o: ../src/%.cpp
	@echo "  -> compiling $*.cpp"
	@$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c $< -o $@

clean_release:
	@rm -f  $(OBJ_RELEASE) $(OUT_RELEASE)
	@rm -rf $(BINDIR_RELEASE)
	@rm -rf $(OBJDIR_RELEASE)/__/src


$(ASSETS):
	@[ ! -d $(BINDIR_DEBUG)   ] || [ -f $(BINDIR_DEBUG)/$@   ] || \
            ( echo "copying asset to debug bin directory: '$@'"   ;   \
              cp $(ASSETS_DIR)/$@ $(BINDIR_DEBUG)/$@              )
	@[ ! -d $(BINDIR_RELEASE) ] || [ -f $(BINDIR_RELEASE)/$@ ] || \
            ( echo "copying asset to release bin directory: '$@'" ;   \
              cp $(ASSETS_DIR)/$@ $(BINDIR_RELEASE)/$@            )


.PHONY: before_debug after_debug clean_debug before_release after_release clean_release
