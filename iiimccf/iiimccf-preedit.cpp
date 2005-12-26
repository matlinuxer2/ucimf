#include "iiimccf-int.h"
#include <iostream>

extern IIIMCCF* iiimccf;

IIIMF_status
iiimccf_preedit(
    IIIMCF_context context,
    IIIMCF_event event,
    IIIMCF_component current,
    IIIMCF_component parent
){

  Prdt prdt( context );
  IIIMF_status st;
  IIIMCF_event_type type;
  st = iiimcf_get_event_type( event, &type );
  if( st != IIIMF_STATUS_SUCCESS ) return st;
   
  switch( type ){
	  case IIIMCF_EVENT_TYPE_UI_PREEDIT:
		  mesg("preedit");
		  break;
		  
	  case IIIMCF_EVENT_TYPE_UI_PREEDIT_START:
		  mesg("preedit start");
		  prdt.show();
		  break;
		  
	  case IIIMCF_EVENT_TYPE_UI_PREEDIT_CHANGE:
		  mesg("preedit changed");
		  prdt.position(iiimccf->x, iiimccf->y);
		  cout << "RECV POS ((( "<< iiimccf->x << ", " << iiimccf->y << " )))" << endl;
		  prdt.info();
		  prdt.update();
		  break;
		  
	  case IIIMCF_EVENT_TYPE_UI_PREEDIT_DONE:
		  mesg("preedit done");
		  //prdt.position(iiimccf->x, iiimccf->y);
		  prdt.update();
		  break;
		  
	  case IIIMCF_EVENT_TYPE_UI_PREEDIT_END:
		  mesg("preedit end");
		  prdt.hide();
		  break;
		  
	  default:
		  mesg("preedit none");
		  break;
  }
  return IIIMF_STATUS_SUCCESS;
}





Prdt::Prdt( IIIMCF_context new_context)
{
  context = new_context;
  cur_x =0;
  cur_y =0;
}

void Prdt::info()
{
    IIIMF_status st;
    IIIMCF_text prdt_buf;
    
    st = iiimcf_get_preedit_text (context, &prdt_buf, &cur_pos);
    if (st == IIIMF_STATUS_SUCCESS) {
	char *str = iiimcf_text_to_utf8( prdt_buf );
	cout << "  Preedit(" << cur_pos << "):" << str << endl; 
	delete str;
    } else if (st == IIIMF_STATUS_NO_PREEDIT) {
	cout << "Preedit is disabled" << endl;
    } else {
	cout << "unknow err" << st << endl;
    }
}

void Prdt::show()
{
  if( visible == true ) return;
  visible = true;
  //update();
  return;
}

void Prdt::hide()
{
  visible = false;
  return;
}

bool Prdt::update()
{
  if( visible == false ) return true;
  IIIMF_status st;
  IIIMCF_text buf0;
  st = iiimcf_get_preedit_text( context, &buf0 , &cur_pos);
  if( st != IIIMF_STATUS_SUCCESS )
  {
    // throw error
    return false;
  }

  IIIMP_card16* buf1;
  st = iiimcf_get_text_utf16string( buf0, ( const IIIMP_card16** ) &buf1 );
  if( st != IIIMF_STATUS_SUCCESS )
  {
    // error from the conversion itself
    return false;
  }

  int buf1_len;
  st = iiimcf_get_text_length( buf0, &buf1_len ); 
  if( st != IIIMF_STATUS_SUCCESS ) return false;

  vector<IIIMP_card16> buf_utf16(buf1_len);
  IIIMP_card16 ch;
  int nfb;
  const IIIMP_card32 *pids, *pfbs;
  for( int i=0; i < buf1_len; i++ )
  {
    st = iiimcf_get_char_with_feedback( buf0, i, &ch, &nfb, &pids, &pfbs );
    if( st != IIIMF_STATUS_SUCCESS ) return false;
    buf_utf16[i] = ch;
  }
  
  String buf3( buf_utf16 );
  //delete prdt_text;

  prdt_text = new Text;
 
  prdt_text->append( buf3 );
  prdt_text->x(cur_x);
  prdt_text->y(cur_y);
  prdt_text->fh(16);
  prdt_text->fw(16);
  prdt_text->info();
  Rectangle r( prdt_text->x() , 
               prdt_text->y() ,
               prdt_text->x() + prdt_text->w() ,
	       prdt_text->y() + prdt_text->h() ,
	       4 );
  r.render();
  cout << "--end of update--" << endl;
  cout << "-----------------" << endl; 
  return draw();
}

bool Prdt::position( int x, int y )
{
  //if( x < X_MIN | x > X_MAX | y < Y_MIN | y > Y_MAX ) return false;
  cur_x = x;
  cur_y = y;
  cout << "( CUR_X, CUR_Y ) => ( "<< cur_x << " , " << cur_y << " )" << endl;
  //return draw();
}

bool Prdt::draw()
{
  cout << "---start of render---" << endl;
  prdt_text->render();
  cout << "---end of render-----" << endl;
  
  return true; 
}
