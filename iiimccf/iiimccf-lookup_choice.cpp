#include "iiimccf-int.h"
#include <iostream>

Lkc::Lkc( IIIMCF_context new_context)
{
  context = new_context;
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
  update();
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

  return draw();
}

bool Lkc::position( int x, int y )
{
  //if( x < X_MIN | x > X_MAX | y < Y_MIN | y > Y_MAX ) return false;
  cur_x = x;
  cur_y = y;
  return draw();
}

bool Lkc::draw()
{
  lkc_text->fw(24);
  lkc_text->fh(24);
  lkc_text->x(600);
  lkc_text->y(400);
  lkc_text->info();
  Rectangle r( lkc_text->x() , 
               lkc_text->y() ,
               lkc_text->x() + lkc_text->w() ,
	       lkc_text->y() + lkc_text->h() ,
	       2);
  r.render();
  cout << "---start of render---" << endl;
  lkc_text->render();
  cout << "---end of render-----" << endl;
  
  return true; 
}
