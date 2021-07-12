#ifndef _FONTS_H_
#define _FONTS_H_


/**
 * TtfFont class
 *
 * wrapper for xxd raw dump of a TTF font
 *
 * create with `xxd --include afont.ttf afont.h`
 * #include the .h in fonts.cpp
 */
class TtfFont
{
  friend class LoopiditySdl ;

  TtfFont(const unsigned int n_bytes , const unsigned char* raw_data) :
          nBbytes(n_bytes ) ,
          data   (raw_data) {}


  const unsigned int   nBbytes ;
  const unsigned char* data ;


  static const TtfFont Purisa ;
} ;


#endif // _FONTS_H_
