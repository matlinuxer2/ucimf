#include "font.h"
#include "graphdev.h"

#include <ft2build.h>
#include FT_FREETYPE_H

int main()
{
  Font font( "./font/ming.ttf" );
  int x=10;
  int y=10;
  int size=48;
  FT_ULong charcode=0x52d8;
  for( int i=0; i< 10 ; i++ )
  {
    font.load( x+i*size,y+i*size,size,FT_ENCODING_UNICODE, charcode);
    font.draw();
    charcode++;
  }
}
