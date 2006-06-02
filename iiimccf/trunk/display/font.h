#include <ft2build.h>
#include FT_FREETYPE_H

using namespace std;

struct CharBitMap;

class Font{
 public:
   ~Font();
   static Font* Instance();
   void info();
   void status();
   void render( int code, int fw, int fh, CharBitMap& tmp );
   
 protected:
   Font( string fontpath );
   
   
 private:
   static Font* _instance;
   string fontpath;
   
   /* freetype parameter */
   FT_Library library;
   FT_Face face;
   FT_Error error;
   FT_GlyphSlot slot;
   
   /* temporary */
   FT_ULong charcode;
   FT_Encoding encoding;
   
   int font_width;
   int font_height;
   int font_color;
};

