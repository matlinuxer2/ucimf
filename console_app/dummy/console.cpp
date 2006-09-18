#include "graphdev.h"

int main( int argc, char* argv[])
{
  GraphDev::Open();
  GraphDev* gdev = GraphDev::mpGraphDev;
  
  gdev->FillRect( 10,20,100,100,7);  
  gdev->PutPixel( 100,200,7);  
  int base = 0x4e00;
  for( int i=0; i<1; i++ )
  {
      gdev->OutChar( 206, 206, 7,9, base + i );
  }
  base = 0x5341;
  for( int i=0; i<1; i++ )
  {
      gdev->OutChar( 250, 206, 7,9, base + i );
  }

  gdev->OutChar( 0, 300, 7,9, 43 );
  gdev->OutChar( 32, 300, 7,9, 47);
  gdev->OutChar( 64, 300, 7,9, 48);
  gdev->OutChar( 96, 300, 7,9, 49);
  gdev->OutChar( 128, 300, 7,9,124 );
  gdev->OutChar( 160, 300, 7,9,0x6211 );

} 
