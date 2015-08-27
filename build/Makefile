
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
LIB = /usr/lib/libjack.lib /usr/lib/SDL_ttf.lib /usr/lib/SDL_gfx.lib
LDFLAGS = -L/usr/lib -lmingw32 -lSDLmain -lSDL -mwindows
else
CFLAGS = -std=c++11 -Wall `sdl-config --cflags`
LIB = /usr/lib/i386-linux-gnu/libSDL_gfx.so /usr/lib/i386-linux-gnu/libSDL_ttf.so /usr/lib/i386-linux-gnu/libjack.so
LDFLAGS = `sdl-config --libs` `pkg-config  x11 --libs`
endif

INC_DEBUG = $(INC)
CFLAGS_DEBUG = $(CFLAGS) -g
RESINC_DEBUG = $(RESINC)
RCFLAGS_DEBUG = $(RCFLAGS)
LIBDIR_DEBUG = $(LIBDIR)
LIB_DEBUG = $(LIB)
LDFLAGS_DEBUG = $(LDFLAGS)
OBJDIR_DEBUG = ./obj/Debug
DEP_DEBUG =
OUT_DEBUG = ./bin/Debug/loopidity

INC_RELEASE = $(INC)
CFLAGS_RELEASE = $(CFLAGS) -O3
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE = $(LDFLAGS) -s
OBJDIR_RELEASE = ./obj/Release
DEP_RELEASE =
OUT_RELEASE = ./bin/Release/loopidity

OBJ_DEBUG = $(OBJDIR_DEBUG)/__/src/jack_io.o $(OBJDIR_DEBUG)/__/src/loopidity.o $(OBJDIR_DEBUG)/__/src/loopidity_sdl.o $(OBJDIR_DEBUG)/__/src/main.o $(OBJDIR_DEBUG)/__/src/scene.o $(OBJDIR_DEBUG)/__/src/scene_sdl.o $(OBJDIR_DEBUG)/__/src/trace.o

OBJ_RELEASE = $(OBJDIR_RELEASE)/__/src/jack_io.o $(OBJDIR_RELEASE)/__/src/loopidity.o $(OBJDIR_RELEASE)/__/src/loopidity_sdl.o $(OBJDIR_RELEASE)/__/src/main.o $(OBJDIR_RELEASE)/__/src/scene.o $(OBJDIR_RELEASE)/__/src/scene_sdl.o $(OBJDIR_RELEASE)/__/src/trace.o

all: debug release

clean: clean_debug clean_release

before_debug:
	test -d ./bin/Debug || mkdir -p ./bin/Debug
	test -d $(OBJDIR_DEBUG)/__/src || mkdir -p $(OBJDIR_DEBUG)/__/src

after_debug:
	[ -f ./bin/Debug/histogram_gradient.bmp ] || cp ../assets/histogram_gradient.bmp ./bin/Debug
	[ -f ./bin/Debug/loop_gradient.argb.bmp ] || cp ../assets/loop_gradient.argb.bmp ./bin/Debug
	[ -f ./bin/Debug/scope_gradient.bmp     ] || cp ../assets/scope_gradient.bmp     ./bin/Debug
	[ -f ./bin/Debug/Purisa.ttf             ] || cp ../assets/Purisa.ttf             ./bin/Debug

debug: before_debug out_debug after_debug

out_debug: before_debug $(OBJ_DEBUG) $(DEP_DEBUG)
	$(LD) $(LIBDIR_DEBUG) -o $(OUT_DEBUG) $(OBJ_DEBUG)  $(LDFLAGS_DEBUG) $(LIB_DEBUG)

$(OBJDIR_DEBUG)/__/src/jack_io.o: ../src/jack_io.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../src/jack_io.cpp -o $(OBJDIR_DEBUG)/__/src/jack_io.o

$(OBJDIR_DEBUG)/__/src/loopidity.o: ../src/loopidity.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../src/loopidity.cpp -o $(OBJDIR_DEBUG)/__/src/loopidity.o

$(OBJDIR_DEBUG)/__/src/loopidity_sdl.o: ../src/loopidity_sdl.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../src/loopidity_sdl.cpp -o $(OBJDIR_DEBUG)/__/src/loopidity_sdl.o

$(OBJDIR_DEBUG)/__/src/main.o: ../src/main.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../src/main.cpp -o $(OBJDIR_DEBUG)/__/src/main.o

$(OBJDIR_DEBUG)/__/src/scene.o: ../src/scene.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../src/scene.cpp -o $(OBJDIR_DEBUG)/__/src/scene.o

$(OBJDIR_DEBUG)/__/src/scene_sdl.o: ../src/scene_sdl.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../src/scene_sdl.cpp -o $(OBJDIR_DEBUG)/__/src/scene_sdl.o

$(OBJDIR_DEBUG)/__/src/trace.o: ../src/trace.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../src/trace.cpp -o $(OBJDIR_DEBUG)/__/src/trace.o

clean_debug:
	rm -f $(OBJ_DEBUG) $(OUT_DEBUG)
	rm -rf ./bin/Debug
	rm -rf $(OBJDIR_DEBUG)/__/src

before_release:
	test -d ./bin/Release || mkdir -p ./bin/Release
	test -d $(OBJDIR_RELEASE)/__/src || mkdir -p $(OBJDIR_RELEASE)/__/src

after_release:
	[ -f ./bin/Release/histogram_gradient.bmp ] || cp ../assets/histogram_gradient.bmp ./bin/Release
	[ -f ./bin/Release/loop_gradient.argb.bmp ] || cp ../assets/loop_gradient.argb.bmp ./bin/Release
	[ -f ./bin/Release/scope_gradient.bmp     ] || cp ../assets/scope_gradient.bmp     ./bin/Release
	[ -f ./bin/Release/Purisa.ttf             ] || cp ../assets/Purisa.ttf             ./bin/Release

release: before_release out_release after_release

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) $(LIBDIR_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE)  $(LDFLAGS_RELEASE) $(LIB_RELEASE)

$(OBJDIR_RELEASE)/__/src/jack_io.o: ../src/jack_io.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../src/jack_io.cpp -o $(OBJDIR_RELEASE)/__/src/jack_io.o

$(OBJDIR_RELEASE)/__/src/loopidity.o: ../src/loopidity.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../src/loopidity.cpp -o $(OBJDIR_RELEASE)/__/src/loopidity.o

$(OBJDIR_RELEASE)/__/src/loopidity_sdl.o: ../src/loopidity_sdl.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../src/loopidity_sdl.cpp -o $(OBJDIR_RELEASE)/__/src/loopidity_sdl.o

$(OBJDIR_RELEASE)/__/src/main.o: ../src/main.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../src/main.cpp -o $(OBJDIR_RELEASE)/__/src/main.o

$(OBJDIR_RELEASE)/__/src/scene.o: ../src/scene.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../src/scene.cpp -o $(OBJDIR_RELEASE)/__/src/scene.o

$(OBJDIR_RELEASE)/__/src/scene_sdl.o: ../src/scene_sdl.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../src/scene_sdl.cpp -o $(OBJDIR_RELEASE)/__/src/scene_sdl.o

$(OBJDIR_RELEASE)/__/src/trace.o: ../src/trace.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../src/trace.cpp -o $(OBJDIR_RELEASE)/__/src/trace.o

clean_release:
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf ./bin/Release
	rm -rf $(OBJDIR_RELEASE)/__/src

.PHONY: before_debug after_debug clean_debug before_release after_release clean_release

