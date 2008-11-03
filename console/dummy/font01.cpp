/*
 * UCIMF - Unicode Console InputMethod Framework                                                    
 *
 * Copyright (c) 2006-2007 Chun-Yu Lee (Mat) and Open RazzmatazZ Laboratory (OrzLab)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "font01.h"
#include <cstdlib>
#include <iostream>
using namespace std;


const char* get_ft_error(FT_Error error)
{
	#undef __FTERRORS_H__
	#define FT_ERRORDEF(e,v,s) case e: return s;
	#define FT_ERROR_START_LIST
	#define FT_ERROR_END_LIST
	switch (error) {
		#include FT_ERRORS_H
		default:
		return "unknown";
	}
}



Font* Font::_instance = NULL;

Font* Font::getInstance()
{
  if ( _instance == NULL )
  {
    _instance = new Font;
  }
  return _instance;
}

Font::Font()
{
	FT_Error ft_err = 0;
	cerr << "Font instance init" << endl;
  char* font_path = getenv ("FONT_PATH");
  char*  width  = "";//getenv ("FONT_WIDTH") ;
  char*  height = "";//getenv ("FONT_HEIGHT") ;
	cerr << "pass0" << endl;
  font_width = atoi( width );
  font_height = atoi( height );
  if( font_path == NULL )
  {
	  font_path = "";
  }

  cerr << "fp: " << font_path << endl;


	cerr << "pass1" << endl;
  ft_err = FT_Init_FreeType( &library ); 
  if( ft_err ) cerr << "FreeType hurt: "<< get_ft_error( ft_err ) << endl;
  ft_err = FT_New_Face( library, font_path, 0, &face);
  if( ft_err ) cerr << "FreeType hurt: "<< get_ft_error( ft_err ) << endl;

	cerr << "pass2" << endl;
	cerr << font_width << endl;
	cerr << font_height << endl;
  /* get font size info */
  font_width = font_width ? font_width : 16;
  font_height = font_height ? font_height : 16;

	cerr << "pass3" << endl;

  cerr << "fw: "<< font_width << endl;
  cerr << font_path << endl;
  cerr << "fh: "<< font_height << endl;
  
  //FT_Set_Char_Size( face, font_width*64 , font_height*64, 0, 0 );
  FT_Set_Pixel_Sizes( face, font_width, font_height);
  //FT_Select_Charmap( face, FT_ENCODING_UNICODE );
	cerr << "pass4" << endl;

  if( face->available_sizes !=0 ) // stand for the pcf font.
  {
	cerr << "pass5" << endl;
    FT_Set_Charmap( face, face->charmaps[0] );

	cerr << "pass6" << endl;
    int fh = face->available_sizes->height;
    int fw = face->available_sizes->width;
    if( fh == 0 )
    {
      font_height = fw;
    }
    else
    {
      font_height = fh;
    }
    
    if( fw == 0 )
    {
      font_width = fh;
    }
    else
    {
      font_width = fw;
    }
  }
  
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

void Font::render( int code, CharBitMap& charbitmap )
{
  // FIXME: handle failure
  // FT_Error error;
  FT_ULong charcode = static_cast<FT_ULong>(code);
  
  int glyph_index = FT_Get_Char_Index( face, charcode );
  FT_Load_Glyph( face, glyph_index, FT_LOAD_DEFAULT );
  FT_Render_Glyph( face->glyph, FT_RENDER_MODE_MONO );
  
  FT_Bitmap bmap=face->glyph->bitmap;

  charbitmap.h = bmap.rows;
  charbitmap.w = bmap.width;
  charbitmap.wBytes = bmap.pitch; // pitch: how many bytes it has..
  int total_size = bmap.rows * bmap.pitch;
  charbitmap.pBuf = new char[ total_size ];
  memcpy( charbitmap.pBuf, bmap.buffer, total_size );
  charbitmap.isMulti8 = (bmap.width % 8) ? false : true;
}


int Font::length( int code )
{
  FT_ULong charcode = static_cast<FT_ULong>(code);

  int glyph_index = FT_Get_Char_Index( face, charcode );
  FT_Load_Glyph( face, glyph_index, FT_LOAD_DEFAULT );
  FT_Render_Glyph( face->glyph, FT_RENDER_MODE_MONO );
  
  int result = face->glyph->bitmap.width;
  
  return result;
}
