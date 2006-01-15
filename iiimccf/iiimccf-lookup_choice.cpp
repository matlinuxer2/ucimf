#include "iiimccf-int.h"
#include <iostream>


extern IIIMCCF* iiimccf;
//class Lkc;
//static Lkc lkc;

/* 
 * Lookup Choice 
 * */
IIIMF_status
iiimccf_lookup_choice(
    IIIMCF_context context,
    IIIMCF_event event,
    IIIMCF_component current,
    IIIMCF_component parent
){

  IIIMF_status st;
  IIIMCF_event_type type;
  st = iiimcf_get_event_type( event, &type );
  if( st != IIIMF_STATUS_SUCCESS ) return st;
  
  switch( type ){	
	  case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE: 
		  debug( "lookup" );
		  break;
		  
	  case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE_START:
		  debug( "lookup start" );
		  iiimccf->lkc = new Lkc( context );
		  iiimccf->lkc->push();
		  iiimccf->lkc->show();
		  break;
	  
	  case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE_CHANGE:
		  debug( "lookup change" );
		  iiimccf->lkc->pop();
		  iiimccf->lkc->position( iiimccf->x, iiimccf->y );
		  iiimccf->lkc->update();
		  iiimccf->lkc->push();
		  iiimccf->lkc->draw();
		  break;
	  
	  case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE_DONE:
		  debug( "lookup done" );
		  iiimccf->lkc->pop();
		  iiimccf->lkc->position( iiimccf->x, iiimccf->y );
		  iiimccf->lkc->update();
		  break;
	  case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE_END: 
		  debug( "lookup end" );
		  iiimccf->lkc->hide();
		  delete iiimccf->lkc;
		  iiimccf->lkc = NULL;
		  break;
		  
	  default:
		  break;
  }
  
  return IIIMF_STATUS_SUCCESS;
}





Lkc::Lkc( IIIMCF_context new_context)
{
  context = new_context;
  cur_x=0;
  cur_y=0;
  rect = new Rectangle;
}

void Lkc::info()
{
  cout << " visible: " << visible << endl;
  cout << " cur_x:" << cur_x << " cur_y:" << cur_y << endl;
  cout << " current index:" << cur_idx << endl;
  lkc_text->info();
}

void Lkc::show()
{
  if( visible == true ) return;
  visible = true;
  //update();
  return;
}

void Lkc::hide()
{
  visible = false;
  return;
}

bool Lkc::update()
{

  if( visible == false ) return true;
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

  lkc_text->fw(24);
  lkc_text->fh(24);
  
  shift();
  
  lkc_text->x(cur_x - shift_x);
  lkc_text->y(cur_y - shift_y);
  lkc_text->info();
  rect->update( lkc_text->x()-6 , 
               lkc_text->y()-6 ,
               lkc_text->x() + lkc_text->w() +6,
	       lkc_text->y() + lkc_text->h() +6,
	       7);

  //return draw();
}

bool Lkc::position( int x, int y )
{
  //if( x < X_MIN | x > X_MAX | y < Y_MIN | y > Y_MAX ) return false;
  cur_x = x;
  cur_y = y;
  //return draw();
}

#define X_MIN 0
#define X_MAX 800
#define Y_MIN 0
#define Y_MAX 600


void Lkc::shift()
{
  shift_x = (cur_x + lkc_text->w()) - X_MAX ;
  if ( shift_x < -20 )
  {
    shift_x = -20 ;
  }
  else
  {
    shift_x = shift_x + ( X_MAX - cur_x ) + 20 ; // shift back
  }

  shift_y = (cur_y + lkc_text->h() ) - Y_MAX ;
  if ( shift_y < -30 )
  {
    shift_y = -30;
  }
  else
  {
    shift_y = shift_y + ( Y_MAX - cur_y ) + 30 ;

  }

}

void Lkc::push()
{
  rect->push( lkc_tmp );
}

void Lkc::pop()
{
  rect->pop( lkc_tmp );
}

bool Lkc::draw()
{
  cout << "---start of render---" << endl;
  rect->render();
  Rectangle up,left,right,bottom;
  int u,l,r,b;
  u=lkc_text->y() -3;
  l=lkc_text->x() -3;
  r=lkc_text->x() + lkc_text->w() +3;
  b=lkc_text->y() + lkc_text->h() +3;
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
