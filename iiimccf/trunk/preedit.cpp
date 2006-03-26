#include "iiimccf-int.h"
#include "layer.h"
#include "preedit.h"
#include <iostream>

extern IIIMCCF* iiimccf;

void Prdt::empty()
{
  context = NULL;
  delete rect;
  rect = NULL;
  delete prdt_text;
  prdt_text = NULL;
}

Prdt::Prdt()
{
  context = NULL; 
  visible = false;
  rect = new Rectangle;
  prdt_text = new Text;
  cur_x =0;
  cur_y =0;
  shift_x = 0;
  shift_y = 0;
}

Prdt::Prdt( IIIMCF_context new_context)
{
  context = new_context;
  visible = false;
  rect = new Rectangle;
  prdt_text = new Text;
  cur_x =0;
  cur_y =0;
  shift_x = 0;
  shift_y = 0;
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


bool Prdt::update()
{
  if( context == NULL )
  {
    return true;
  }
  
  hide();
  
  cur_x = iiimccf->x;
  cur_y = iiimccf->y;
  
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
  
  prdt_text = new Text;
  
 
  prdt_text->append( buf3 );
  prdt_text->fh(16);
  prdt_text->fw(16);

  shift();
  
  prdt_text->x(cur_x - shift_x);
  prdt_text->y(cur_y - shift_y);
  prdt_text->fc(4);
  prdt_text->info();
  
  rect->update( prdt_text->x() , 
               prdt_text->y() ,
               prdt_text->x() + prdt_text->w() ,
	       prdt_text->y() + prdt_text->h() -1 ,
	       0 );
  rect->c(0);
 
  show();
}

#define X_MIN 0
#define X_MAX 800
#define Y_MIN 0
#define Y_MAX 600

void Prdt::shift()
{
  shift_x = (cur_x + prdt_text->w()) - X_MAX ;
  if ( shift_x < -8 )
  {
    shift_x = 0 ;
  }
  else
  {
    shift_x = shift_x + 8 ; // shift back
  }

  shift_y = (cur_y + prdt_text->h() ) - Y_MAX ;
  if ( shift_y < -2*prdt_text->h() )
  {
    shift_y = -2*prdt_text->fh() - 2;
  }
  else
  {
    shift_y = prdt_text->fh() + prdt_text->h() + 2;
  }


}

bool Prdt::isVisible()
{
  return visible;
}

void Prdt::setShow()
{
  visible = true;
  return;
}

void Prdt::setHide()
{
  visible = false;
  return;
}

void Prdt::show()
{
  if( isVisible() || context == NULL )
  {
    return;
  }
  else
  {
    rect->push( prdt_tmp );
    rect->render();
    prdt_text->render();
    setShow();
  }

}

void Prdt::hide()
{
  if( isVisible() && context != NULL )
  {
    setHide();
    rect->pop( prdt_tmp );
  }
  else
  {
    return;
  }
}
