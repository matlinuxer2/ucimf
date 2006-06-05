#include "font.h"
#include <iostream>
#include <string>
#include <cstdlib>
using namespace std;

Font* Font::_instance = 0;

Font* Font::Instance()
{
  if ( _instance == 0 )
  {
    _instance = new Font;
  }
  return _instance;
}

Font::Font()
{
  fontpath = getenv("UCIMF_FONTPATH");

  font_width = 0;
  font_height = 0;

  FT_Init_FreeType( &library ); 
  FT_New_Face( library, fontpath.c_str(), 0, &face);
  FT_Set_Charmap( face, face->charmaps[0] );
}

Font::~Font()
{

}

void Font::load( int code, CharBitMap& charbitmap )
{
  FT_Error error;
  FT_Set_Char_Size( face, font_width*64 , font_height*64, 0, 0 );
  encoding = FT_ENCODING_UNICODE;
  charcode = static_cast<FT_ULong> code;
  /*
  cout.setf(ios_base::hex, ios_base::basefield );
  cout << "charcode: [" << charcode << "] in UTF-16" << endl;
  cout.setf(ios_base::dec, ios_base::basefield );
  */

  error = FT_Load_Char( face, charcode, FT_LOAD_DEFAULT );

  charbitmap.pBuf = face->glyph->charbitmap.buffer;
  charbitmap.h = face->glyph->charbitmap.rows;
  charbitmap.w = face->glyph->charbitmap.width;
  charbitmap.isMulti8 = true;
}


