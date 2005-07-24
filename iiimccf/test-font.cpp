#include "graphdev.h"
#include "basefont.h"
#include <iostream>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H

using namespace std;

  FT_Face face;
  FT_GlyphSlot slot;
GraphDev *gdev;
void my_draw_bitmap( FT_Bitmap* bitmap, int pos_left, int pos_top);

int main( )
{
  GraphDev::Open();
  gdev = GraphDev::mpGraphDev;
  
  const char*  fontpath = "./font/ming.ttf";
  
  FT_Library library;
  FT_Error error;
  FT_Init_FreeType( &library ); 
  FT_New_Face( library, fontpath, 0, &face);
  slot = face->glyph;
  FT_Set_Char_Size( face, 0, 160*64, 0, 0 );
  
  int pen_x= 300; 
  int pen_y= 200;

  FT_UInt glyph_index;
  FT_ULong charcode=0x52d8;
  
  error  = FT_Load_Char( face, charcode, FT_LOAD_RENDER );
  /*
  glyph_index = FT_Get_Char_Index( face, charcode );
  if( glyph_index == 0 )
  {
    cout << "glyph index not found..." << endl;
    return 0;
  }

  error = FT_Load_Glyph( face, glyph_index, FT_LOAD_DEFAULT );
  if( error!=0 )
  {
    cout << "FT_Load_Glyph fail" << endl;
    return -1;
  }
  error = FT_Render_Glyph( face->glyph, ft_render_mode_normal );
  */


  my_draw_bitmap( &slot->bitmap,
      pen_x + slot->bitmap_left,
      pen_y - slot->bitmap_top 
      );
}


void my_draw_bitmap( FT_Bitmap* bitmap, int pos_left, int pos_top)
{
  gdev->ClearScr();
  unsigned char* tmp = slot->bitmap.buffer;
  for (int i=0; i< bitmap->rows; i++ )
  {
    for( int j=0; j< bitmap->width; j++ )
    {
      if (*tmp)
	gdev->PutPixel(pos_left+j, pos_top+i, 3 );
      tmp++;
    }
  }
}

