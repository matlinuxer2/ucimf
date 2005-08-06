#include "iostream"
#include "graphdev.h"
#include "basefont.h"
using std::cout;
using std::endl;

int main( int argc, char argv[] )
{
  GraphDev::Open();
  GraphDev *gdev = GraphDev::mpGraphDev;
  gdev->FillRect( 10, 20, 100, 200 , 3);
  return 0;
}
