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
   bool setFontHeidht(int fontheight );
   bool setFontWidth( int fontwidth );
   int  getFontHeight();
   int  getFontWidth();
   void render( int code, int fw, int fh, CharBitMap& tmp );
   
 protected:
   Font();


 private:
   static Font* _instance;
   string fontpath;

   /* freetype parameter */
   FT_Library library;
   FT_Face face;
   //FT_GlyphSlot slot;

   /* temporary */
   //FT_ULong charcode;
   //FT_Encoding encoding;

   int font_width;
   int font_height;
   //int font_color;
};

