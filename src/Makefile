
VERSION = v0.14.003

ifndef DESTDIR
  DESTDIR =
endif
ifndef PREFIX
  PREFIX = /usr/local
endif
BUILDDIR   = ../build
BINDIR     = bin
DATADIR    = share/loopidity
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

CFLAGS = -std=gnu++0x -Wall -MMD -DVERSION=$(VERSION)
ifeq ($(CONFIG),debug)
  CFLAGS  += -g -DDEBUG=1
  LDFLAGS  =
  OBJDIR   = $(BUILDDIR)/obj/debug/src
  TARGET   = loopidity-dbg
else
  CFLAGS  += -O3
  LDFLAGS  = -s
  OBJDIR   = $(BUILDDIR)/obj/release/src
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
	@test -d $(BUILDDIR)/$(BINDIR)  || mkdir -p $(BUILDDIR)/$(BINDIR)
	@test -d $(BUILDDIR)/$(DATADIR) || mkdir -p $(BUILDDIR)/$(DATADIR)
	@test -d $(BUILDDIR)/$(OBJDIR)  || mkdir -p $(BUILDDIR)/$(OBJDIR)

out: before $(OBJECTS)
	@echo "linking $(CONFIG) binary"
	@$(LD) -o $(BUILDDIR)/$(BINDIR)/$(TARGET) $(OBJECTS) $(LDFLAGS)

$(OBJECTS): $(OBJDIR)/%.o: ../src/%.cpp
	@echo "  -> compiling $*.cpp"
	@$(CXX) $(CFLAGS) -c $(BUILDDIR)/$< -o $@

after: before out $(ASSETS)
	@echo "copying assets to data directory"
	@cp --no-clobber $(ASSETS) $(BUILDDIR)/$(DATADIR)/


clean:
	@echo "cleaning all configurations"
	rm -rf $(BUILDDIR)/$(BINDIR)/
	rm -rf $(BUILDDIR)/$(DATADIR)/
	rm -rf $(BUILDDIR)/$(OBJDIR)/


assert-writable:
	@mkdir -p $(INSTALLDIR)/$(BINDIR)
	@mkdir -p $(INSTALLDIR)/$(DATADIR)
	@touch $(INSTALLDIR)/$(BINDIR)/$(TARGET) 2> /dev/null || \
          (echo '\nsuperuser privileges required - exiting\n' && exit 255)

install: all assert-writable
	@echo installing loopidity
	@install -Dm 755 $(BINDIR)/$(TARGET) $(INSTALLDIR)/$(BINDIR)/
	@install -Dm 644 $(ASSETS)           $(INSTALLDIR)/$(DATADIR)/

uninstall: assert-writable
	@echo uninstalling loopidity
	@rm     $(INSTALLDIR)/$(BINDIR)/$(TARGET) 2> /dev/null
	@rm -rf $(INSTALLDIR)/$(DATADIR)/         2> /dev/null


-include $(OBJECTS:%.o=%.d)
