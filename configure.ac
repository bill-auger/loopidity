#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([loopidity], [0.14.026], [https://github.com/bill-auger/loopidity/issues])
AC_CONFIG_SRCDIR([src/loopidity.cpp])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_FILES([Makefile src/Makefile])
AM_INIT_AUTOMAKE([subdir-objects foreign -Wall -Werror])
AM_SILENT_RULES([yes])
AC_LANG([C++])

AC_CANONICAL_HOST
case $host_os in
  mingw32.static|mingw32.shared)
    AC_MSG_NOTICE(["Detected mingw cross-compile ($build_os => $host_os)"])
    IS_MINGW=1
    ;;
esac
IS_MINGW=0
AC_MSG_NOTICE(["Checking for programs ..."])
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
# AX_CXX_COMPILE_STDCXX_11

AC_MSG_NOTICE(["Checking for typedefs, structures, and compiler characteristics ..."])
AC_TYPE_SIZE_T

AC_MSG_NOTICE(["Checking for header files  ..."])
AC_CHECK_HEADER([sys/stat.h],              [ ], [AC_MSG_ERROR("missing stat header"      )])
AC_CHECK_HEADER([unistd.h],                [ ], [AC_MSG_ERROR("missing unistd header"    )])
AC_CHECK_HEADER([cstdlib],                 [ ], [AC_MSG_ERROR("missing cstdlib headers"  )])
AC_CHECK_HEADER([exception],               [ ], [AC_MSG_ERROR("missing exception headers")])
AC_CHECK_HEADER([iostream],                [ ], [AC_MSG_ERROR("missing iostream headers" )])
AC_CHECK_HEADER([list],                    [ ], [AC_MSG_ERROR("missing list headers"     )])
AC_CHECK_HEADER([sstream],                 [ ], [AC_MSG_ERROR("missing sstream headers"  )])
AC_CHECK_HEADER([string],                  [ ], [AC_MSG_ERROR("missing string headers"   )])
AC_CHECK_HEADER([vector],                  [ ], [AC_MSG_ERROR("missing vector headers"   )])

AC_MSG_NOTICE(["Checking for libraries ..."])
AC_CHECK_FUNC([atexit], [ ], [AC_MSG_ERROR("C library does not provide 'atexit'")])
AC_CHECK_FUNC([memset], [ ], [AC_MSG_ERROR("C library does not provide 'memset'")])
if test "$IS_MINGW" == "0" ; then
AC_CHECK_LIB([jack],    [jack_activate                ], [ ], [AC_MSG_ERROR("jack library does not provide 'jack_activate'"                )])
AC_CHECK_LIB([jack],    [jack_client_close            ], [ ], [AC_MSG_ERROR("jack library does not provide 'jack_client_close'"            )])
AC_CHECK_LIB([jack],    [jack_client_open             ], [ ], [AC_MSG_ERROR("jack library does not provide 'jack_client_open'"             )])
AC_CHECK_LIB([jack],    [jack_get_buffer_size         ], [ ], [AC_MSG_ERROR("jack library does not provide 'jack_get_buffer_size'"         )])
AC_CHECK_LIB([jack],    [jack_get_sample_rate         ], [ ], [AC_MSG_ERROR("jack library does not provide 'jack_get_sample_rate'"         )])
AC_CHECK_LIB([jack],    [jack_on_shutdown             ], [ ], [AC_MSG_ERROR("jack library does not provide 'jack_on_shutdown'"             )])
AC_CHECK_LIB([jack],    [jack_port_get_buffer         ], [ ], [AC_MSG_ERROR("jack library does not provide 'jack_port_get_buffer'"         )])
AC_CHECK_LIB([jack],    [jack_port_register           ], [ ], [AC_MSG_ERROR("jack library does not provide 'jack_port_register'"           )])
AC_CHECK_LIB([jack],    [jack_set_buffer_size_callback], [ ], [AC_MSG_ERROR("jack library does not provide 'jack_set_buffer_size_callback'")])
AC_CHECK_LIB([jack],    [jack_set_process_callback    ], [ ], [AC_MSG_ERROR("jack library does not provide 'jack_set_process_callback'"    )])
AC_CHECK_LIB([jack],    [jack_set_sample_rate_callback], [ ], [AC_MSG_ERROR("jack library does not provide 'jack_set_sample_rate_callback'")])
AC_CHECK_LIB([SDL],     [SDL_CreateRGBSurface         ], [ ], [AC_MSG_ERROR("SDL library does not provide 'SDL_CreateRGBSurface'"          )])
AC_CHECK_LIB([SDL],     [SDL_Delay                    ], [ ], [AC_MSG_ERROR("SDL library does not provide 'SDL_Delay'"                     )])
AC_CHECK_LIB([SDL],     [SDL_DisplayFormat            ], [ ], [AC_MSG_ERROR("SDL library does not provide 'SDL_DisplayFormat'"             )])
AC_CHECK_LIB([SDL],     [SDL_EnableKeyRepeat          ], [ ], [AC_MSG_ERROR("SDL library does not provide 'SDL_EnableKeyRepeat'"           )])
AC_CHECK_LIB([SDL],     [SDL_EventState               ], [ ], [AC_MSG_ERROR("SDL library does not provide 'SDL_EventState'"                )])
AC_CHECK_LIB([SDL],     [SDL_FillRect                 ], [ ], [AC_MSG_ERROR("SDL library does not provide 'SDL_FillRect'"                  )])
AC_CHECK_LIB([SDL],     [SDL_Flip                     ], [ ], [AC_MSG_ERROR("SDL library does not provide 'SDL_Flip'"                      )])
AC_CHECK_LIB([SDL],     [SDL_FreeSurface              ], [ ], [AC_MSG_ERROR("SDL library does not provide 'SDL_FreeSurface'"               )])
AC_CHECK_LIB([SDL],     [SDL_GetError                 ], [ ], [AC_MSG_ERROR("SDL library does not provide 'SDL_GetError'"                  )])
AC_CHECK_LIB([SDL],     [SDL_GetRGB                   ], [ ], [AC_MSG_ERROR("SDL library does not provide 'SDL_GetRGB'"                    )])
AC_CHECK_LIB([SDL],     [SDL_GetTicks                 ], [ ], [AC_MSG_ERROR("SDL library does not provide 'SDL_GetTicks'"                  )])
AC_CHECK_LIB([SDL],     [SDL_Init                     ], [ ], [AC_MSG_ERROR("SDL library does not provide 'SDL_Init'"                      )])
AC_CHECK_LIB([SDL],     [SDL_LoadBMP_RW               ], [ ], [AC_MSG_ERROR("SDL library does not provide 'SDL_LoadBMP_RW'"                )])
AC_CHECK_LIB([SDL],     [SDL_LockSurface              ], [ ], [AC_MSG_ERROR("SDL library does not provide 'SDL_LockSurface'"               )])
AC_CHECK_LIB([SDL],     [SDL_MapRGB                   ], [ ], [AC_MSG_ERROR("SDL library does not provide 'SDL_MapRGB'"                    )])
AC_CHECK_LIB([SDL],     [SDL_PollEvent                ], [ ], [AC_MSG_ERROR("SDL library does not provide 'SDL_PollEvent'"                 )])
AC_CHECK_LIB([SDL],     [SDL_PushEvent                ], [ ], [AC_MSG_ERROR("SDL library does not provide 'SDL_PushEvent'"                 )])
AC_CHECK_LIB([SDL],     [SDL_Quit                     ], [ ], [AC_MSG_ERROR("SDL library does not provide 'SDL_Quit'"                      )])
AC_CHECK_LIB([SDL],     [SDL_RWFromFile               ], [ ], [AC_MSG_ERROR("SDL library does not provide 'SDL_RWFromFile'"                )])
AC_CHECK_LIB([SDL],     [SDL_SetAlpha                 ], [ ], [AC_MSG_ERROR("SDL library does not provide 'SDL_SetAlpha'"                  )])
AC_CHECK_LIB([SDL],     [SDL_SetColorKey              ], [ ], [AC_MSG_ERROR("SDL library does not provide 'SDL_SetColorKey'"               )])
AC_CHECK_LIB([SDL],     [SDL_SetVideoMode             ], [ ], [AC_MSG_ERROR("SDL library does not provide 'SDL_SetVideoMode'"              )])
AC_CHECK_LIB([SDL],     [SDL_UnlockSurface            ], [ ], [AC_MSG_ERROR("SDL library does not provide 'SDL_UnlockSurface'"             )])
AC_CHECK_LIB([SDL],     [SDL_UpperBlit                ], [ ], [AC_MSG_ERROR("SDL library does not provide 'SDL_UpperBlit'"                 )])
AC_CHECK_LIB([SDL],     [SDL_WM_SetCaption            ], [ ], [AC_MSG_ERROR("SDL library does not provide 'SDL_WM_SetCaption'"             )])
AC_CHECK_LIB([SDL_gfx], [circleColor                  ], [ ], [AC_MSG_ERROR("SDL_gfx library does not provide 'circleColor'"               )])
AC_CHECK_LIB([SDL_gfx], [filledPieColor               ], [ ], [AC_MSG_ERROR("SDL_gfx library does not provide 'filledPieColor'"            )])
AC_CHECK_LIB([SDL_gfx], [hlineColor                   ], [ ], [AC_MSG_ERROR("SDL_gfx library does not provide 'hlineColor'"                )])
AC_CHECK_LIB([SDL_gfx], [roundedRectangleColor        ], [ ], [AC_MSG_ERROR("SDL_gfx library does not provide 'roundedRectangleColor'"     )])
AC_CHECK_LIB([SDL_gfx], [vlineColor                   ], [ ], [AC_MSG_ERROR("SDL_gfx library does not provide 'vlineColor'"                )])
AC_CHECK_LIB([SDL_gfx], [rotozoomSurface              ], [ ], [AC_MSG_ERROR("SDL_gfx library does not provide 'rotozoomSurface'"           )])
AC_CHECK_LIB([SDL_gfx], [zoomSurface                  ], [ ], [AC_MSG_ERROR("SDL_gfx library does not provide 'zoomSurface'"               )])
AC_CHECK_LIB([SDL_ttf], [TTF_CloseFont                ], [ ], [AC_MSG_ERROR("SDL_ttf library does not provide 'TTF_CloseFont'"             )])
AC_CHECK_LIB([SDL_ttf], [TTF_Init                     ], [ ], [AC_MSG_ERROR("SDL_ttf library does not provide 'TTF_Init'"                  )])
AC_CHECK_LIB([SDL_ttf], [TTF_OpenFont                 ], [ ], [AC_MSG_ERROR("SDL_ttf library does not provide 'TTF_OpenFont'"              )])
AC_CHECK_LIB([SDL_ttf], [TTF_RenderText_Solid         ], [ ], [AC_MSG_ERROR("SDL_ttf library does not provide 'TTF_RenderText_Solid'"      )])
AC_CHECK_LIB([X11],     [XGetWindowAttributes         ], [ ], [AC_MSG_ERROR("x11 library does not provide 'XGetWindowAttributes'"          )])
AC_CHECK_LIB([X11],     [XOpenDisplay                 ], [ ], [AC_MSG_ERROR("x11 library does not provide 'XOpenDisplay'"                  )])
fi

AC_MSG_NOTICE(["Checking for pkg-config flags ..."])
PKG_PROG_PKG_CONFIG([])
PKG_CHECK_MODULES([JACK],    [jack   ])
PKG_CHECK_MODULES([SDL],     [sdl    ])
PKG_CHECK_MODULES([SDL_gfx], [SDL_gfx])
PKG_CHECK_MODULES([SDL_ttf], [SDL_ttf])
if test "$IS_MINGW" == "0" ; then
PKG_CHECK_MODULES([X11],     [x11    ])
fi

AC_MSG_NOTICE(["Generating output files ..."])
AC_OUTPUT
