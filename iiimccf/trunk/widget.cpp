#include <iiimcf.h>
#include <iiimp.h>
#include "observer.h"
#include "graphdev.h"
#include "iiimccf-int.h"
#include "layer.h"
#include "widget.h"
#include <iostream>
using namespace std;

#define X_MIN 0
#define X_MAX 800
#define Y_MIN 0
#define Y_MAX 600


extern IIIMCCF* iiimccf;

Subject::Subject(){}
Subject::~Subject(){}

void Subject::notify()
{
  cout << "enter notify" << endl;
  for(int i=0; i< observers.size(); i++ )
  {
    cout << "enter observer 1" << endl;
    observers[i]->update();
    cout << "enter observer 2" << endl;
  }
}

void Subject::attach( Observer* obsr )
{
  observers.push_back( obsr );
}


void Subject::detach( Observer* obsr )
{
  observers.erase( find( observers.begin(), observers.end(), obsr) );
}

Observer::Observer(){}
Observer::~Observer(){}

TrackPoint::TrackPoint()
{
  x = 0;
  y = 0;
}
TrackPoint::~TrackPoint(){}

void TrackPoint::get_position( int& old_x, int& old_y)
{
  old_x = x;
  old_y = y;
}

void TrackPoint::set_position( int new_x, int new_y)
{
  if( x != new_x || y != new_y )
  {
    x = new_x;
    y = new_y;
    this->notify();
  }
}


Stts::Stts()
{
  visible = false;
  div = new Rectangle;
  title = new Text;
  pos_x = pos_y =0;
  shift_x = shift_y = 0;
}
Stts::~Stts()
{
  delete div;
  delete title;
}

void Stts::update()
{
  if( isVisible() )
  {
    div->pop( tmp );
  }
  else
  {
    ;
  }
  
  iiimccf->trkpt->get_position( pos_x, pos_y );
  
  IIIMF_status st; 
  IIIMCF_input_method *pims;
  const IIIMP_card16 *u16idname, *u16hrn, *u16domain;
  int num_of_ims;
  int cur_ims_id=iiimccf->cur_ims_id;
   
  st = iiimcf_get_supported_input_methods(iiimccf->handle, &num_of_ims, &pims);
  iiimcf_get_input_method_desc(pims[cur_ims_id], &u16idname, &u16hrn, &u16domain);
  
  vector<IIIMP_card16> vec_u16hrn(20);
  IIIMP_card16* pr=(IIIMP_card16*)u16idname;
  int vec_len=0;
  for( int i=0 ; (*(char*)pr) ; i++ )
  {
    vec_u16hrn[i]=*pr;
    vec_len++;
    pr++; 
  }
  vec_u16hrn.resize(vec_len);
  String buf4( vec_u16hrn );
  
  //delete title;
  title = new Text;
  title->append( buf4 );

  title->fh(16);
  title->fw(16);

  shift();
  
  title->x( pos_x - shift_x );
  title->y( pos_y - shift_y );
  title->fh(16);
  title->fw(16);
  title->fc(4);
  title->info();

  div->update( title->x(),
               title->y(),
	       title->x() + title->w(),
	       title->y() + title->h(),
	       0 );
  div->c(1);
 
  if( isVisible() )
  {
    div->push( tmp );
    div->render();
    title->render();
  }
  else
  { 
    ;
  }

}

void Stts::shift()
{
  shift_x = (pos_x + title->w()) - X_MAX ;
  if ( shift_x < -8 )
  {
    shift_x = 0 ;
  }
  else
  {
    shift_x = shift_x + 8 ; // shift back
  }

  shift_y = (pos_y + title->h() ) - Y_MAX ;
  if ( shift_y < - title->h() )
  {
    shift_y = - title->h() -1 ;
  }
  else
  {
    shift_y = title->h() +1 ;

  }


}

bool Stts::isVisible()
{
  return visible;
}

void Stts::setShow()
{
  visible = true;
}

void Stts::setHide()
{
  visible = false;
}

void Stts::show()
{
  if( isVisible() )
  {
    return;
  }
  else
  { 
    div->push( tmp );
    div->render();
    title->render();
    setShow();
  }

}

void Stts::hide()
{
  if( isVisible() )
  {
    setHide();
    div->pop( tmp );
  }
  else
  {
    return;
  }
}

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

void Lkc::empty()
{
  context = NULL;
  delete rect;
  rect = NULL;
  delete lkc_text;
  lkc_text = NULL;
}

Lkc::Lkc()
{
  context = NULL;
  visible = false;
  rect = new Rectangle;
  lkc_text = new Text;
  cur_x=20;
  cur_y=20;

}

Lkc::Lkc( IIIMCF_context new_context)
{
  context = new_context;
  visible = false;
  cur_x=20;
  cur_y=20;
  lkc_text = new Text;
  rect = new Rectangle;
}

void Lkc::info()
{
  cout << " visible: " << visible 
       << " cur_x:" << cur_x 
       << " cur_y:" << cur_y 
       << " current index:" << cur_idx 
       << endl;
  lkc_text->info();
}

bool Lkc::isVisible()
{
  return visible;
}

void Lkc::setShow()
{
  visible = true;
  return;
}

void Lkc::setHide()
{
  visible = false;
  return;
}

void Lkc::show()
{
  if( isVisible()  || context == NULL )
  {
    return;
  }
  else
  {
    setShow();
    rect->push( lkc_tmp );
    draw();
  }
}

void Lkc::hide()
{
  if( isVisible() && context != NULL )
  {
    rect->pop( lkc_tmp );
    setHide();
  }
  else
  {
    return;
  }
}

bool Lkc::update()
{
  if( context == NULL )
  {
    return true;
  }

  hide();

  IIIMF_status st;
  IIIMCF_lookup_choice ilc;
  st = iiimcf_get_lookup_choice( context, &ilc );
  if( st != IIIMF_STATUS_SUCCESS )
  {
    // throw error
    return false;
  }
  
  int lkc_size, idx_first, idx_last;
  st = iiimcf_get_lookup_choice_size( ilc, &lkc_size, &idx_first, &idx_last, &cur_idx );
  if( st!= IIIMF_STATUS_SUCCESS ) return false;

  // lkc_text clear?;
  delete lkc_text;
  lkc_text = new Text;
  for( int i=0; i< lkc_size; i++ )
  { 
    String row_utf16;
    IIIMCF_text cand, label;
    int flag;
    iiimcf_get_lookup_choice_item( ilc, i, &cand, &label, &flag );
    if( flag & IIIMCF_LOOKUP_CHOICE_ITEM_ENABLED )
    {
      String cand2( text_to_vector(cand) );
      String label2( text_to_vector(label) );
      
      row_utf16.push_string ( label2 );
      //row_utf16.push_char( ' ' );
      row_utf16.push_string( cand2 );
      
      lkc_text->append( row_utf16 );
    }
    
  }

  cur_x = iiimccf->x;
  cur_y = iiimccf->y;
  
  lkc_text->fw(16);
  lkc_text->fh(16);

  
  shift();
  
  lkc_text->x(cur_x - shift_x);
  lkc_text->y(cur_y - shift_y);
  lkc_text->info();
  rect->update( lkc_text->x()-6 , 
               lkc_text->y()-6 ,
               lkc_text->x() + lkc_text->w() +6,
	       lkc_text->y() + lkc_text->h() +6,
	       7);

  info();
  show();
}


#define X_MIN 0
#define X_MAX 800
#define Y_MIN 0
#define Y_MAX 600


void Lkc::shift()
{
  shift_x = (cur_x + lkc_text->w()) - X_MAX ;
  if ( shift_x < -8 )
  {
    shift_x = 0 ;
  }
  else
  {
    shift_x = shift_x + 8 ; // shift back
  }

  shift_y = (cur_y + lkc_text->h() ) - Y_MAX ;
  if ( shift_y < -3*lkc_text->h() )
  {
    shift_y = -3*lkc_text->fh() - 9 ;
 }
  else
  {
    shift_y = 3*lkc_text->fh() + lkc_text->h() + 9;

  }

}



bool Lkc::draw()
{
  rect->render();
  Rectangle up,left,right,bottom;
  int u,l,r,b;
  u=lkc_text->y() -3;
  l=lkc_text->x() -3;
  r=lkc_text->x() + lkc_text->w() +3;
  b=lkc_text->y() + lkc_text->h() +3;
  cout << " u:" << u
       << " l:" << l
       << " r:" << r
       << " b:" << b << endl;
  up.update( l, u, r, u,8);
  left.update( l,u,l,b ,8);
  right.update( r,u,r,b,15);
  bottom.update( l,b,r,b,15);
  up.render();
  left.render();
  right.render();
  bottom.render();

  lkc_text->fc(16);
  lkc_text->render();
  cout << "---end of render-----" << endl;
  
  return true; 
}
