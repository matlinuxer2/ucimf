#include <ucimf.h>
#include "imf.h"
#include "openvanilla.h"

int FOCUS;
Imf* imf=NULL;

char* ucimf_process_stdin( char** buf, int* ret )
{
  if( ret == 0 )
    return;
  
  /* The input method on/off switch */
  if( *buf == CTRL_SPACE ) //!?
  {
    switch( FOCUS )
    {
      case 0:
	FOCUS = 1;
	break;
      case 1:
	FOCUS = 0;
	break;
      default:
	FOCUS = 0;
	break;
    }
    return;
  }

  if( FOCUS == 0 )
    return;

  if( imf == NULL )
    imf = new OVImf;  //!?
  
  return imf->process_input( buf  );
}


void ucimf_cursor_position( int x, int y)
{
  if( imf == NULL )
    return; 

  imf->update_cursor_position(x, y);
}
