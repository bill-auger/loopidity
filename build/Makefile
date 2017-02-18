
ifndef DESTDIR
  DESTDIR =
endif
ifndef PREFIX
  PREFIX = /usr/local
endif
BINDIR     = ./bin
DATADIR    = ./share/loopidity
ASSETSDIR  = ../assets
INSTALLDIR = $(DESTDIR)$(PREFIX)
ifeq (${MSYSTEM},MINGW32)
  MINGW      = 1
  SYSBIN_DIR = /usr/bin
endif


CC  = gcc
CXX = g++
AR  = ar
LD  = g++

CFLAGS = -std=gnu++0x -Wall -MMD
ifeq ($(CONFIG),debug)
  CFLAGS  += -g -DDEBUG=1
  LDFLAGS  =
  OBJDIR   = ./obj/Debug/__/src
  TARGET   = loopidity-dbg
else
  CFLAGS  += -O3
  LDFLAGS  = -s
  OBJDIR   = ./obj/Release/__/src
  TARGET   = loopidity
  CONFIG   = 'release'
endif
ifdef MINGW
  CFLAGS  += -I/usr/include -I/usr/include/SDL -D_GNU_SOURCE=1 -Dmain=SDL_main
  LDFLAGS += -L/usr/lib -lmingw32 -lSDLmain -lSDL -mwindows /usr/lib/libjack.lib /usr/lib/libSDL_gfx.dll.a /usr/lib/SDL_ttf.lib
else
  CFLAGS  += `sdl-config --cflags`
  LDFLAGS += `sdl-config --libs` `pkg-config  x11 --libs` `pkg-config  SDL_ttf --libs` `pkg-config  SDL_gfx --libs` `pkg-config  jack --libs`
endif


OBJECTS = $(OBJDIR)/jack_io.o       \
          $(OBJDIR)/loopidity.o     \
          $(OBJDIR)/loopidity_sdl.o \
          $(OBJDIR)/main.o          \
          $(OBJDIR)/scene.o         \
          $(OBJDIR)/scene_sdl.o     \
          $(OBJDIR)/trace.o
ASSETS = $(ASSETSDIR)/histogram_gradient.bmp \
         $(ASSETSDIR)/loop_gradient.argb.bmp \
         $(ASSETSDIR)/scope_gradient.bmp     \
         $(ASSETSDIR)/Purisa.ttf


all: before out after

before:
	@echo "building $(CONFIG) configuration"
	@test -d $(BINDIR)  || mkdir -p $(BINDIR)
	@test -d $(DATADIR) || mkdir -p $(DATADIR)
	@test -d $(OBJDIR)  || mkdir -p $(OBJDIR)

out: before $(OBJECTS)
	@echo "linking $(CONFIG) binary"
	@$(LD) -o $(BINDIR)/$(TARGET) $(OBJECTS) $(LDFLAGS)

after: before $(ASSETS)
	@echo "copying assets to data directory"
	@cp --no-clobber $(ASSETS) $(DATADIR)/


$(OBJECTS): $(OBJDIR)/%.o: ../src/%.cpp
	@echo "  -> compiling $*.cpp"
	@$(CXX) $(CFLAGS) -c $< -o $@


clean:
	@echo "cleaning $(CONFIG) configuration"
	@rm -rf $(BINDIR)
	@rm -rf $(DATADIR)
	@rm -rf $(OBJDIR)/*.o


assert-writable:
	@mkdir -p $(INSTALLDIR)/bin
	@mkdir -p $(INSTALLDIR)/share/loopidity
	@touch $(INSTALLDIR)/bin/$(TARGET) 2> /dev/null || \
          (echo '\nsuperuser privileges required - exiting\n' && exit 255)

install: all assert-writable
	@echo installing loopidity
	@install -Dm 755 $(BINDIR)/$(TARGET) $(INSTALLDIR)/bin/
	@install -Dm 644 $(ASSETS)           $(INSTALLDIR)/share/loopidity/

uninstall: assert-writable
	@echo uninstalling loopidity
	@rm     $(INSTALLDIR)/bin/$(TARGET)    2> /dev/null
	@rm -rf $(INSTALLDIR)/share/loopidity/ 2> /dev/null


-include $(OBJECTS:%.o=%.d)
