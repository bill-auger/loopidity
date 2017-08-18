#ifndef _IMAGES_H_
#define _IMAGES_H_


class GimpImage
{
  friend class LoopiditySdl ;

  GimpImage(const unsigned int image_width     , const unsigned int   image_height ,
            const unsigned int bytes_per_pixel , const unsigned char* pixel_data   ) :
            width        (image_width    ) ,
            height       (image_height   ) ,
            bytesPerPixel(bytes_per_pixel) ,
            pixelData    (pixel_data     ) {}


  const unsigned int   width ;
  const unsigned int   height ;
  const unsigned int   bytesPerPixel ;
  const unsigned char* pixelData ;


  static const GimpImage HistogramGradient ;
  static const GimpImage LoopGradient ;
  static const GimpImage ScopeGradient ;


/* debug helpers
 #include <SDL/SDL.h>

  static void dumpPixelFmt(const char* label , SDL_Surface* surface)
  {
    SDL_LockSurface(surface);
    SDL_PixelFormat* pixel_format = surface->format ;
    Uint32           red_mask     = pixel_format->Rmask ;
    Uint32           green_mask   = pixel_format->Gmask ;
    Uint32           blue_mask    = pixel_format->Bmask ;
    Uint32           alpha_mask   = pixel_format->Amask ;
    SDL_UnlockSurface(surface);

    printf("pixel masks: r=%x g=%x b=%x a=%x <-- %s\n" , red_mask , green_mask , blue_mask , alpha_mask , label) ;
  }

  static void dumpPixelVal(const char* label , SDL_Surface* surface)
  {
    SDL_LockSurface(surface);
    SDL_PixelFormat* pixel_format = surface->format ;
    Uint32           pixel_val    = *((Uint32*)surface->pixels)
    SDL_UnlockSurface(surface);

    Uint8 red   = (Uint8)(pixel_val & pixel_format->Rmask) >> pixel_format->Rshift << pixel_format->Rloss ;
    Uint8 green = (Uint8)(pixel_val & pixel_format->Gmask) >> pixel_format->Gshift << pixel_format->Gloss ;
    Uint8 blue  = (Uint8)(pixel_val & pixel_format->Bmask) >> pixel_format->Bshift << pixel_format->Bloss ;
    Uint8 alpha = (Uint8)(pixel_val & pixel_format->Amask) >> pixel_format->Ashift << pixel_format->Aloss ;

    printf("Pixel Color -> R: %x,  G: %x,  B: %x,  A: %x <-- %s\n" , red, green, blue, alpha, name);
  }
*/
} ;


#endif // _IMAGES_H_
