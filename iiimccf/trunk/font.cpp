#include "graphdev.h"
#include "font.h"

Font* Font::_instance = 0;

Font* Font::getInstance()
{
  if ( _instance == 0 )
  {
    _instance = new Font;
  }
  return _instance;
}

Font::Font()
{
  font_path = getenv("UCIMF_FONTPATH");

  FT_Init_FreeType( &library ); 
  FT_New_Face( library, font_path, 0, &face);

  /* get font size info */
  info();
  font_width=16;
  font_height=16;
  
  FT_Set_Char_Size( face, font_width*64 , font_height*64, 0, 0 );
  FT_Set_Charmap( face, face->charmaps[0] );
  //FT_Select_Charmap( face, FT_ENCODING_UNICODE );
}

Font::~Font()
{
  FT_Done_Face(face);
  FT_Done_FreeType(library);
}

int Font::Height()
{
  return font_height;
}

int Font::Width()
{
  return font_width;
}

void Font::info()
{
  /*
    num_faces
    face_index
    num_glyphs
    family_name
    style_name
    num_fixed_sizes
    available_sizes
    num_charmaps
    size
    charmap

     
    */
}

void Font::render( int code, CharBitMap& charbitmap )
{
  FT_Error error;
  FT_ULong charcode = static_cast<FT_ULong>(code);
  /*
  cout.setf(ios_base::hex, ios_base::basefield );
  cout << "charcode: [" << charcode << "] in UTF-16" << endl;
  cout.setf(ios_base::dec, ios_base::basefield );
  */

  error = FT_Load_Char( face, charcode, FT_LOAD_DEFAULT );

  charbitmap.pBuf = (char*)face->glyph->bitmap.buffer;
  charbitmap.h = face->glyph->bitmap.rows;
  charbitmap.w = face->glyph->bitmap.width;
  charbitmap.wBytes = face->glyph->bitmap.pitch;
  charbitmap.isMulti8 = (charbitmap.w % 8) ? false : true;
}


