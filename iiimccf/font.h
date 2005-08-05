#include <ft2build.h>
#include FT_FREETYPE_H

#include "graphdev.h"

#define KFont   "/usr/share/fonts/arphicfonts/bkai00mp.ttf"
#define EFont_H "/usr/share/fonts/efont-unicode/h16.pcf.gz"
#define EFont_B "/usr/share/fonts/efont-unicode/b16.pcf.gz"
#define EFont_F "/usr/share/fonts/efont-unicode/f16.pcf.gz"

class Font{
 public:
   Font( const char* fontpath );
   ~Font();
   void info();
   void load( int x, int y, int size, FT_Encoding enc, FT_ULong rcode );
   void draw();

 private:
   GraphDev *gdev;
   
   /* freetype parameter */
   FT_Library library;
   FT_Face face;
   FT_Error error;
   FT_GlyphSlot slot;
   
   /* temporary */
   FT_ULong charcode;
   FT_Encoding encoding;
   
   int pos_x, pos_y;
   char* fontpath;
   int fontsize;
		
};
