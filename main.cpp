#ifdef __cplusplus
    #include <cstdlib>
#else
    #include <stdlib.h>
#endif

#ifdef __APPLE__
#include <SDL/SDL.h>
#else
#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#endif


int main ( int argc, char** argv )
{
	// initialize SDL video
	if (SDL_Init(SDL_INIT_VIDEO) < 0) { printf("SDL: %s\n" , SDL_GetError()) ; return 1 ; }
	atexit(SDL_Quit) ;
	SDL_Surface* screen = SDL_SetVideoMode(640 , 480 , 16 , SDL_HWSURFACE | SDL_DOUBLEBUF) ;
	if (!screen) { printf("SDL: %s\n" , SDL_GetError()) ; return 1 ; }

	bool done = false ; SDL_Event event ;
	while (!done)
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT: done = true ; break ;
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_ESCAPE: done = true ; break ;
					}
				break ;
			} // switch (event.type)
		} // while (SDL_PollEvent(&event))

		SDL_FillRect(screen , 0 , SDL_MapRGB(screen->format , 0 , 0 , 0)) ;

		Sint16 x = 200 , y = 200 , radius = 100 , begin = 0 , end = 180 ;
		Uint8 r = 255 , g = 255 , b = 255 , a = 255 ;
		filledPieRGBA(screen , x , y , radius , begin , end , r , g , b , a) ;

		SDL_Flip(screen) ;
	} // while (!done)

	return 0;
}
