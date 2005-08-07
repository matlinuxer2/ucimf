/* The program file is to emulate the console application's request.
 * To see whether the IIIMCCF works properly.
 */

#include <stdio.h>
#include <iostream>
#include "iiimccf.h" 
using namespace std;

int main( int argc, char** argv )
{

  iiimccf_init();
  iiimccf_show_ims();
  iiimccf_scrn( 1024, 768, 75 );
  
  iiimccf_on();
  iiimccf_pos( 200, 200 );

  // iiimccf_proc( IIIMF_KEYCODE_5, '5', 0);
  // iiimccf_proc( IIIMF_KEYCODE_2, '2', 0);
  // iiimccf_proc( IIIMF_KEYCODE_D, 'd', 0);
  // iiimccf_proc( IIIMF_KEYCODE_8, '8', 0);
  
  cout << "--Key-w----------------------" << endl;
  iiimccf_proc( IIIMF_KEYCODE_W, 'w', 0);
  cout << "--Key-o----------------------" << endl;
  iiimccf_proc( IIIMF_KEYCODE_O, 'o', 0);
  cout << "--Key-1----------------------" << endl;
  iiimccf_proc( IIIMF_KEYCODE_1, '1', 0);
  cout << "--Key-E----------------------" << endl;
  iiimccf_proc( IIIMF_KEYCODE_ENTER, 0, 0);
  cout << "--Key-none-------------------" << endl;
  
  iiimccf_off();

  iiimccf_exit();
  

  return 0;
}
