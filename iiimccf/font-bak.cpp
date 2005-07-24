#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H


string fontpath="./font/ming.tff"
FT_Library library;
FT_Face face;
FT_GlyphSlot slot = face->glyph;
int pen_x, pen_y, n;

error = FT_Init_FreeType( &library ); 
if ( error ){ cout << "error 1" << endl; }

error = FT_New_Face( library, fontpath, 0, &face);
if ( error ){ cout << "error 2" << endl; }

error = FT_Set_Char_Size( 
    face, 	/* handle to face object */  
    0, 		/* char_width in 1/64th of points */  
    16*64, 	/* char_height in 1/64th of points */  
    0,	 	/* horizontal device resolution */  
    0  		/* vertical device resolution */a
    );

pen_x= 300;
pen_y= 200;
for ( n = 0 ; n < num_chars; n++ )
{
  FT_UInt glyph_index;

  glyph_index = FT_Get_Char_Index( face, text[n] );
  
  error = FT_Load_Glyph( face, glyph_index, FT_LOAD_DEFAULT );

  if( error )
    continue;

  error = FT_Render_Glyph( face->glyph, ft_render_mode_normal );
  if( error )
    continue;

  my_draw_bitmap( &slot->bitmap,
      pen_x + slot->bitmap_left,
      pen_y - slot->bitmap_top );
  
  pen_x += slot->advance.x >> 6;
  pen_y += slot->advance.y >> 6;
  
}



/*
error = FT_Load_Glyph( 
    face, 		/* handle to face object */  
    glyph_index, 	/* glyph index */  
    load_flags  	/* load flags, see below */
    );

error = FT_Render_Glyph( 
    face->glyph, /* glyph slot */  
    render_mode  /* render mode */
    );

 */
