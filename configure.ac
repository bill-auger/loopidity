#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([loopidity], [0.14.003], [https://github.com/bill-auger/loopidity/issues])
AC_CONFIG_SRCDIR([src/scene_sdl.cpp])
AM_INIT_AUTOMAKE([subdir-objects foreign -Wall -Werror])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([jack   ] , [jack_client_open             ])
AC_CHECK_LIB([jack   ] , [jack_set_process_callback    ])
AC_CHECK_LIB([jack   ] , [jack_set_sample_rate_callback])
AC_CHECK_LIB([jack   ] , [jack_set_buffer_size_callback])
AC_CHECK_LIB([jack   ] , [jack_on_shutdown             ])
AC_CHECK_LIB([jack   ] , [jack_activate                ])
AC_CHECK_LIB([jack   ] , [jack_get_sample_rate         ])
AC_CHECK_LIB([jack   ] , [jack_port_get_buffer         ])
AC_CHECK_LIB([jack   ] , [jack_get_buffer_size         ])
AC_CHECK_LIB([jack   ] , [jack_client_close            ])
AC_CHECK_LIB([jack   ] , [jack_port_register           ])
AC_CHECK_LIB([SDL    ] , [SDL_Init])
AC_CHECK_LIB([SDL_gfx] , [hlineColor           ])
AC_CHECK_LIB([SDL_gfx] , [vlineColor           ])
AC_CHECK_LIB([SDL_gfx] , [roundedRectangleColor])
AC_CHECK_LIB([SDL_gfx] , [circleColor          ])
AC_CHECK_LIB([SDL_gfx] , [filledPieColor       ])
AC_CHECK_LIB([SDL_gfx] , [rotozoomSurface      ])
AC_CHECK_LIB([SDL_gfx] , [zoomSurface          ])
AC_CHECK_LIB([SDL_ttf] , [TTF_OpenFont        ])
AC_CHECK_LIB([SDL_ttf] , [TTF_Init            ])
AC_CHECK_LIB([SDL_ttf] , [TTF_CloseFont       ])
AC_CHECK_LIB([SDL_ttf] , [TTF_RenderText_Solid])
AC_CHECK_LIB([X11    ] , [XOpenDisplay        ])
AC_CHECK_LIB([X11    ] , [DefaultScreen       ])
AC_CHECK_LIB([X11    ] , [XGetWindowAttributes])

# Checks for header files.
AC_PATH_X
AC_CHECK_HEADERS([jack/jack.h            ])
AC_CHECK_HEADERS([SDL/SDL.h              ])
AC_CHECK_HEADERS([SDL/SDL_gfxPrimitives.h])
AC_CHECK_HEADERS([SDL/SDL_rotozoom.h     ])
AC_CHECK_HEADERS([SDL/SDL_ttf.h          ])
AC_CHECK_HEADERS([X11/Xlib.h             ])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([atexit memset])

AC_CONFIG_FILES([Makefile assets/Makefile src/Makefile])
AC_OUTPUT
