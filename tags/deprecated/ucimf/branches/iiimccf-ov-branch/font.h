#include <ft2build.h>
#include FT_FREETYPE_H

#include "graphdev.h"
//#include <string>
using namespace std;

//#define KFont   "/usr/share/fonts/arphicfonts/bkai00mp.ttf"
#define KFont "/usr/share/fonts/truetype/arphic/ukai.ttf"
//#define EFont   "/usr/lib/X11/fonts/misc/b16.pcf.gz"
#define EFont   "/usr/share/fonts/efont-unicode/b16.pcf.gz"

class Font{
 public:
   Font();
   Font( string fontpath );
   ~Font();
   void info();
   void status();
   void load( int code, int x, int y, int fw, int fh, int c );
   void draw();

 private:
   GraphDev *gdev;
   string fontpath;
   
   /* freetype parameter */
   FT_Library library;
   FT_Face face;
   FT_Error error;
   FT_GlyphSlot slot;
   
   /* temporary */
   FT_ULong charcode;
   FT_Encoding encoding;
   
   int pos_x, pos_y;
   int font_width;
   int font_height;
   int font_color;
		
};

