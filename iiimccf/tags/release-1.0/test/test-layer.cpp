#include "graphdev.h"
#include "font.h"
#include "layer.h"
#include <ft2build.h>
#include FT_FREETYPE_H

Font font("./font/ming.ttf");
GraphDev* gdev;
int main()
{
  GraphDev::Open();
  gdev=GraphDev::mpGraphDev;
  Point p(30,50,3);
  Rectangle r( 52, 52, 100, 100, 4);
  r.render();
  p.render();

  Word wd( 120, 120, 30, FT_ENCODING_UNICODE, 0x52d8 );
  Word wd2( 150, 150, 120, FT_ENCODING_UNICODE, 0x51d8 );
  wd.render();
  wd2.render();
  return 0;
}
