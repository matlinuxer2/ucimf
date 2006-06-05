#include <ft2build.h>
#include FT_FREETYPE_H
using namespace std;

struct CharBitMap;

class Font{
 public:
   ~Font();
   static Font* Instance();

   bool setFontHeidht(int fontheight );
   bool setFontWidth( int fontwidth );
   int  getFontHeight();
   int  getFontWidth();
   void render( int code, CharBitMap& tmp );
   
 protected:
   Font();


 private:
   static Font* _instance;
   string fontpath;
   int font_width;
   int font_height;

   /* freetype parameter */
   FT_Library library;
   FT_Face face;

};

