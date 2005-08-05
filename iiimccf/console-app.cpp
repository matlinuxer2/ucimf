/* The program file is to emulate the console application's request.
 * To see whether the IIIMCCF works properly.
 */

#include <stdio.h>
#include "iiimccf.h"

int main( int argc, char** argv )
{

  iiimccf_init();
  iiimccf_scrn( 1024, 768, 75 );

  iiimccf_on();
  iiimccf_pos( 200, 200 );

  iiimccf_proc( IIIMF_KEYCODE_A, 'a', 0);
  iiimccf_proc( IIIMF_KEYCODE_K, 'k', 0);
  iiimccf_proc( IIIMF_KEYCODE_U, 'u', 0);
  iiimccf_proc( IIIMF_KEYCODE_1, '1', 0);
  iiimccf_proc( IIIMF_KEYCODE_ENTER, 0, 0);
  
  iiimccf_off();

  iiimccf_exit();
  

  return 0;
}
