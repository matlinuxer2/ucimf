#include "widget.h"
#include "shape.h"
#include <iostream>
using namespace std;

/*
Widget::Widget()
{
  win = new Window;
  gp = win->getGraphPort();

  isPseudo = true;
}

Widget::~Widget()
{
  delete win;
  win = 0;
  gp = 0;
}
*/

void Widget::render()
{
  // Estimate the width, height first
  gp->push_fg_buf();
  gp->pop_bg_buf();
  gp->setPseudo(true);
  draw();

  // Adjust geometrical parameters..

  // Real Draw 
  gp->push_bg_buf();
  gp->setPseudo(false);
  draw();
  gp->push_fg_buf();

  if( win->isVisible() == true )
  {
    gp->pop_bg_buf();
  }

}



/*
 * Implementation of Status bar.
 */
Status* Status::_instance = 0;

Status::Status()
{ 

}

Status* Status::getInstance()
{
  if( _instance == 0 )
  {
    _instance = new Status;
  }
  return _instance;
}

void Status::set_imf_name( char* new_imf_name )
{
  ustring input( "UTF-8", new_imf_name );
  imf_name = input;
}

void Status::set_im_name( char* new_im_name )
{
  ustring input( "UTF-8", new_im_name );
  im_name = input;
}

void Status::set_lang_name( char* new_lang_name )
{
  ustring input( "UTF-8", new_lang_name );
  lang_name = input;
}

void Status::draw()
{
 
  Text t;
  t.append( imf_name );
  t.append( im_name );
  t.append( lang_name );
  t.bgColor( 0 );
  t.fgColor( 1 );
  gp->draw( 0,0, &t );
  
}
  
/*




Prdt::Prdt()
{
  prdt_text = new Text;
  cur_x =0;
  cur_y =0;
  shift_x = 0;
  shift_y = 0;
}

Prdt::~Prdt()
{
  hide();
}


bool Prdt::update()
{
  hide();
  show();
}





Lkc::Lkc()
{
  lkc_text = new Text;
  cur_x=20;
  cur_y=20;
}


Lkc::~Lkc()
{
  hide();
}

bool Lkc::update()
{
  hide();
  show();
}


bool Lkc::draw()
{
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


*/
