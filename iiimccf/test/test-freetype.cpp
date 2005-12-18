#include <iostream>
#include <string>
#include "graphdev.h"
//#include "basefont.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

using std::cout;
using std::endl;

int main( int argc, char argv[] )
{
  GraphDev::Open();
  GraphDev *gdev = GraphDev::mpGraphDev;
  gdev->FillRect( 10, 20, 10, 20 , 3);

  std::string fontpath="./font/ming.ttf";
  
  FT_Library library;
  {
    int error = FT_Init_FreeType( &library );
    if( error )
    {
	cout << "freetype init error" << endl;
    }
    FT_Face face;
    error = FT_New_Face( library, fontpath.c_str(), 0, &face  );
    if( error == FT_Err_Unknown_File_Format )
    {
	cout << "font not support" << endl;
    }
    cout << "face infomation: " << endl;
    cout << "face number is : " << face->num_faces << endl;
    cout << "face glyphs number is : " << face->num_glyphs << endl;
    cout << "faces's style name is : " << face->style_name << endl;
    cout << "units per EM : " << face->units_per_EM << endl;
    cout << "num_fixed_sizes : " << face->num_fixed_sizes << endl;
    if ( face->charmap == NULL )
      cout << "No charMap is selected" << endl;
    cout << "charmap number is : " << face->num_charmaps << endl;

    for ( int i=0; i< face->num_charmaps; ++i )
    {
      cout << " platform : " << face->charmaps[i]->platform_id <<
	      " encoding : " << face->charmaps[i]->encoding_id << endl;
    }

    //string text="我";
    long unsigned int text=101;
    unsigned int a1 = 0x62;
    unsigned int a2 = 0x11;
/*    text & a1;
    text << 16;
    text & a2;
*/  
    FT_ULong  charcode;                                              
    FT_UInt   gindex;                                                
    charcode = FT_Get_First_Char( face, &gindex ); 
    cout << charcode << endl; 
    cout << text << endl;
    
    int glyph_index = FT_Get_Char_Index( face, text );
    
    charcode=0x21002;

    cout << charcode << endl;
    cout << glyph_index << endl;
    if ( glyph_index==0 )
    {
      cout << "glyph index not found" << endl;
      return 0;
    }

    error = FT_Set_Char_Size(face, 0, 10*64, 360, 360 );
    
    FT_Set_Pixel_Sizes( face, 16, 16);

    FT_Matrix matrix;
    FT_Int load_flags=FT_LOAD_DEFAULT;
    error = FT_Load_Glyph( face, glyph_index, load_flags );
    error = FT_Render_Glyph( face->glyph, (FT_Render_Mode) 0 );
    FT_GlyphSlot slot=face->glyph;

  }
  
  return 0;
}
