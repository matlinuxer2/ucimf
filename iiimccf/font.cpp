#include <iostream>
using namespace std;
#include "font.h"
#include "graphdev.h"
#include <ft2build.h>
#include FT_FREETYPE_H

Font::Font( const char *fpath)
{
  FT_Init_FreeType( &library ); 
  FT_New_Face( library, fpath, 0, &face);
  slot = face->glyph;
  GraphDev::Open();
  gdev = GraphDev::mpGraphDev;    
}
Font::~Font()
{

}

void Font::info()
{
  cout << "fontpath: " << fontpath << endl;
  cout << "fontsize: " << fontsize << endl;
}

void Font::load( int x, int y, int size, FT_Encoding enc, FT_ULong code)
{
  pos_x = x;
  pos_y = y;
  fontsize = size;
  FT_Set_Char_Size( face, 0, fontsize*64, 0, 0 );
  encoding = enc;
  charcode = code;
}
  
void Font::draw()
{
    error = FT_Load_Char( face, charcode, FT_LOAD_RENDER );
    unsigned char* tmp = slot->bitmap.buffer;
    int pos_left = pos_x + slot->bitmap_left;
    int pos_top = pos_y + slot->bitmap_top;
    int color=3;

    for (int i=0; i< slot->bitmap.rows; i++ )
    {
      for( int j=0; j< slot->bitmap.width; j++ )
      {
	if (*tmp)
        gdev->PutPixel(pos_left+j, pos_top+i, color );
        tmp++;
      }
    }
}
    

