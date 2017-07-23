#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([loopidity], [0.14.003], [https://github.com/bill-auger/loopidity/issues])
AC_CONFIG_SRCDIR([src/loopidity.cpp])
AM_INIT_AUTOMAKE([subdir-objects foreign -Wall -Werror])
AC_CONFIG_FILES([Makefile])
AM_SILENT_RULES([yes])
AC_LANG([C++])

AC_MSG_NOTICE(["Checking for programs."])
AC_PROG_CXX

AC_MSG_NOTICE(["Checking for libraries."])
AC_CHECK_LIB([jack],    [jack_client_open             ], [ ], [AC_MSG_ERROR("jack library does not provide 'jack_client_open'"             )])
AC_CHECK_LIB([jack],    [jack_set_process_callback    ], [ ], [AC_MSG_ERROR("jack library does not provide 'jack_set_process_callback'"    )])
AC_CHECK_LIB([jack],    [jack_set_sample_rate_callback], [ ], [AC_MSG_ERROR("jack library does not provide 'jack_set_sample_rate_callback'")])
AC_CHECK_LIB([jack],    [jack_set_buffer_size_callback], [ ], [AC_MSG_ERROR("jack library does not provide 'jack_set_buffer_size_callback'")])
AC_CHECK_LIB([jack],    [jack_on_shutdown             ], [ ], [AC_MSG_ERROR("jack library does not provide 'jack_on_shutdown'"             )])
AC_CHECK_LIB([jack],    [jack_activate                ], [ ], [AC_MSG_ERROR("jack library does not provide 'jack_activate'"                )])
AC_CHECK_LIB([jack],    [jack_get_sample_rate         ], [ ], [AC_MSG_ERROR("jack library does not provide 'jack_get_sample_rate'"         )])
AC_CHECK_LIB([jack],    [jack_port_get_buffer         ], [ ], [AC_MSG_ERROR("jack library does not provide 'jack_port_get_buffer'"         )])
AC_CHECK_LIB([jack],    [jack_get_buffer_size         ], [ ], [AC_MSG_ERROR("jack library does not provide 'jack_get_buffer_size'"         )])
AC_CHECK_LIB([jack],    [jack_client_close            ], [ ], [AC_MSG_ERROR("jack library does not provide 'jack_client_close'"            )])
AC_CHECK_LIB([jack],    [jack_port_register           ], [ ], [AC_MSG_ERROR("jack library does not provide 'jack_port_register'"           )])
AC_CHECK_LIB([SDL],     [SDL_Init                     ], [ ], [AC_MSG_ERROR("SDL library does not provide 'SDL_Init'"                      )])
AC_CHECK_LIB([SDL_gfx], [hlineColor                   ], [ ], [AC_MSG_ERROR("SDL_gfx library does not provide 'hlineColor'"                )])
AC_CHECK_LIB([SDL_gfx], [vlineColor                   ], [ ], [AC_MSG_ERROR("SDL_gfx library does not provide 'vlineColor'"                )])
AC_CHECK_LIB([SDL_gfx], [roundedRectangleColor        ], [ ], [AC_MSG_ERROR("SDL_gfx library does not provide 'roundedRectangleColor'"     )])
AC_CHECK_LIB([SDL_gfx], [circleColor                  ], [ ], [AC_MSG_ERROR("SDL_gfx library does not provide 'circleColor'"               )])
AC_CHECK_LIB([SDL_gfx], [filledPieColor               ], [ ], [AC_MSG_ERROR("SDL_gfx library does not provide 'filledPieColor'"            )])
AC_CHECK_LIB([SDL_gfx], [rotozoomSurface              ], [ ], [AC_MSG_ERROR("SDL_gfx library does not provide 'rotozoomSurface'"           )])
AC_CHECK_LIB([SDL_gfx], [zoomSurface                  ], [ ], [AC_MSG_ERROR("SDL_gfx library does not provide 'zoomSurface'"               )])
AC_CHECK_LIB([SDL_ttf], [TTF_OpenFont                 ], [ ], [AC_MSG_ERROR("SDL_ttf library does not provide 'TTF_OpenFont'"              )])
AC_CHECK_LIB([SDL_ttf], [TTF_Init                     ], [ ], [AC_MSG_ERROR("SDL_ttf library does not provide 'TTF_Init'"                  )])
AC_CHECK_LIB([SDL_ttf], [TTF_CloseFont                ], [ ], [AC_MSG_ERROR("SDL_ttf library does not provide 'TTF_CloseFont'"             )])
AC_CHECK_LIB([SDL_ttf], [TTF_RenderText_Solid         ], [ ], [AC_MSG_ERROR("SDL_ttf library does not provide 'TTF_RenderText_Solid'"      )])
AC_CHECK_LIB([X11],     [XOpenDisplay                 ], [ ], [AC_MSG_ERROR("x11 library does not provide 'XOpenDisplay'"                  )])
AC_CHECK_LIB([X11],     [XGetWindowAttributes         ], [ ], [AC_MSG_ERROR("x11 library does not provide 'XGetWindowAttributes'"          )])

AC_MSG_NOTICE(["Checking for header files."])
AC_PATH_X
AC_CHECK_HEADER([jack/jack.h],             [ ], [AC_MSG_ERROR("missing jack headers"   )])
AC_CHECK_HEADER([SDL/SDL.h],               [ ], [AC_MSG_ERROR("missing SDL headers"    )])
AC_CHECK_HEADER([SDL/SDL_gfxPrimitives.h], [ ], [AC_MSG_ERROR("missing SDL_gfx headers")])
AC_CHECK_HEADER([SDL/SDL_rotozoom.h],      [ ], [AC_MSG_ERROR("missing SDL_gfx headers")])
AC_CHECK_HEADER([SDL/SDL_ttf.h],           [ ], [AC_MSG_ERROR("missing SDL_ttf headers")])
AC_CHECK_HEADER([X11/Xlib.h],              [ ], [AC_MSG_ERROR("missing x11 headers"    )])

AC_MSG_NOTICE(["Checking for pkg-config flags."])
PKG_PROG_PKG_CONFIG([])
PKG_CHECK_MODULES([JACK],   [jack               ])
PKG_CHECK_MODULES([SDL],    [sdl SDL_gfx SDL_ttf])
PKG_CHECK_MODULES([X11],    [x11                ])

AC_MSG_NOTICE(["Checking for typedefs, structures, and compiler characteristics."])
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

AC_MSG_NOTICE(["Checking for library functions."])
AC_CHECK_FUNC([atexit], [ ], [AC_MSG_ERROR("C library does not provide 'atexit'")])
AC_CHECK_FUNC([memset], [ ], [AC_MSG_ERROR("C library does not provide 'memset'")])

AC_MSG_NOTICE(["Generating output files."])
AC_OUTPUT