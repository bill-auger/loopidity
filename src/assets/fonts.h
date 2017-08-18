#ifndef _FONTS_H_
#define _FONTS_H_


class TtfFont
{
  friend class LoopiditySdl ;

  TtfFont(const unsigned int n_bytes , const unsigned char* binary_data) :
          nBbytes(n_bytes    ) ,
          data   (binary_data) {}


  const unsigned int   nBbytes ;
  const unsigned char* data ;


  static const TtfFont Purisa ;
} ;


#endif // _FONTS_H_
