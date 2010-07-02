#include "iostream"
#include "graphdev.h"
#include "basefont.h"
#include "hzdecoder.h"
#include <string>
using std::cout;
using std::endl;

HzDecoder* gpDecoder;
BaseFont*  gpHzFont;
int main( int argc, char argv[] )
{
  GraphDev::Open();
  GraphDev *gdev = GraphDev::mpGraphDev;
  gdev->FillRect( 10, 20, 10, 20 , 3);

  string fontpath="./font/big5-16.bpsf";
//  string fontpath="/usr/share/fonts/unifont/unifont.pcf.gz";
  int fontwidth=16;
  int fontheight=16;
  BaseFont f( fontpath, fontwidth, fontheight );
  gpHzFont = &f;

  char c1, c2;
  c1='a'; c2='c';
  gdev->OutChar( 10, 10, 0, 3, c1, c2);  

  return 0;
}
