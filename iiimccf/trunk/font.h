#include <ft2build.h>
#include FT_FREETYPE_H
using namespace std;

struct CharBitMap;

class Font{
 public:
   ~Font();
   static Font* getInstance();
   bool Heidht(int fontheight );
   bool Width( int fontwidth );
   int  Height();
   int  Width();
   void render( int code, CharBitMap& tmp );
   
 protected:
   Font();


 private:
   static Font* _instance;
   char* font_path;
   int font_width;
   int font_height;

   /* freetype parameter */
   FT_Library library;
   FT_Face face;

};

