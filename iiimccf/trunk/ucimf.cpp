#include <ucimf.h>
#include "imf.h"
#include "subject_observer.h"
#include "layer.h"


int FOCUS;
TrackPoint* cursor_position=0;
Window *preedit,*lookupchoice,*status;

void ucimf_on()
{
  FOCUS = 1;

}

void ucimf_off()
{


}

Imf* imf=0;

char* ucimf_process_stdin( char** buf, int* ret )
{
/*
  if( ret == 0 )
    return "";
  
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

  if( imf == 0 )
    imf = new OVImf;  //!?
  
  return imf->process_input( buf  );
 
*/
}


void ucimf_cursor_position( int x, int y)
{
  if( imf == 0 )
    return; 
  // imf->update_cursor_position(x, y);
  if( cursor_position == 0 )
    TrackPoint* cursor_position = new TrackPoint;

  cursor_position->set_position( x, y );
}

void ucimf_refresh_begin()
{
  preedit->hide();
  lookupchoice->hide();
  status->hide();
  // notify to push
}

void ucimf_refresh_end()
{
  preedit->show();
  lookupchoice->show();
  status->show();
  // notify to pop
}
