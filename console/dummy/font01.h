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

#include <ft2build.h>
#include FT_FREETYPE_H

struct CharBitMap {
    char* pBuf;         // point to bitmap
    int BufLen;         // include blank line height
    char* pLast;        // point to last scan line
    int ExtLen;         // blank linebyte's bitmap len
    int h;              // height
    int w;              // width
    int wBytes;         // width / 8
    bool isMulti8;      // width is multi 8 or not?
};

class Font{
 public:
   ~Font();
   static Font* getInstance();
   int  Height();
   int  Width();
   void render( int code, CharBitMap& tmp );
   int  length( int code );
   
 //protected:
   Font();

 //private:
   static Font* _instance;
   char* font_path;
   int font_width;
   int font_height;

   /* freetype parameter */
   FT_Library library;
   FT_Face face;

};

