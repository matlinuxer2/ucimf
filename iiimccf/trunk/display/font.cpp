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

/*
   void Font::status()
   {
   cout << "fontpath: " << fontpath << endl;
   cout << "encoding: " << "UTF-16" << endl;
   }

   void Font::info()
   {
   cout << "--Font info--" << endl;
   cout << "font_width: " << font_width << endl;
   cout << "font_height: " << font_height << endl;
   cout << "font_color: " << font_color << endl;
   cout << "--info end---" << endl;
   }
   */

void Font::load( int code, int fw, int fh, CharBitMap& bitmap )
{
  FT_Error error;
  FT_Set_Char_Size( face, fw*64 , fh*64, 0, 0 );
  encoding = FT_ENCODING_UNICODE;
  charcode = static_cast<FT_ULong> code;
  /*
  cout.setf(ios_base::hex, ios_base::basefield );
  cout << "charcode: [" << charcode << "] in UTF-16" << endl;
  cout.setf(ios_base::dec, ios_base::basefield );
  */

  error = FT_Load_Char( face, charcode, FT_LOAD_DEFAULT );

  bitmap.pBuf = face->glyph->bitmap.buffer;
  bitmap.h = face->glyph->bitmap.rows;
  bitmap.w = face->glyph->bitmap.width;
  bitmap.isMulti8 = true;
}


