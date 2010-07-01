#include "iiimccf-int.h"
#include "layer.h"
#include "lookupchoice.h"
#include <iostream>


extern IIIMCCF* iiimccf;

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
