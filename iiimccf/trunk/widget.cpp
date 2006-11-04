#include "widget.h"
#include "shape.h"
#include <iostream>
using namespace std;


Window* Widget::getWindow()
{
  if( win == 0 )
  {
    win = new Window;
    win->wd = this;
  }
  return win;
}

GraphPort* Widget::getGraphPort()
{
  if( gp == 0 )
  {
    win = getWindow();
    gp = win->gp;
  }

  return gp;
}

void Widget::render()
{
  bool prev_visible = win->isVisible();
  // Estimate the width, height first
  win->hide();

  win->gp->setPseudo(true);
  draw();

  // Adjust geometrical parameters..

  // Real Draw 
  win->gp->push_bg_buf();
  win->gp->setPseudo(false);
  draw();
  win->gp->push_fg_buf();
  win->gp->pop_bg_buf();

  if( prev_visible == true )
  {
    win->show();
  }

}



/*
 * Implementation of Status bar.
 */
Status* Status::_instance = 0;

Status::Status()
{ 
  win = 0;
  gp = 0;
  win = getWindow();
  gp = getGraphPort();
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
  render();
}

void Status::set_im_name( char* new_im_name )
{
  ustring input( "UTF-8", new_im_name );
  im_name = input;
  render();
}

void Status::set_lang_name( char* new_lang_name )
{
  ustring input( "UTF-8", new_lang_name );
  lang_name = input;
  render();
}

void Status::draw()
{
 
  int border = 0;

  Text t;
  t.append( imf_name );
  t.append( im_name );
  t.append( lang_name );
  t.bgColor( 3 );
  t.fgColor( 5 );

  Rect r;
  r.w( t.x_max() + 2*border );
  r.h( t.y_max() + 2*border );
  r.c(3);
  
  gp->draw( 0, 0, &r);
  gp->draw( border , border , &t );
}

/*
 * Implementation of Preedit
 */

Preedit* Preedit::_instance = 0;

Preedit::Preedit()
{ 
  win = 0;
  gp = 0;
  win = getWindow();
  gp = getGraphPort();
}

Preedit* Preedit::getInstance()
{
  if( _instance == 0 )
  {
    _instance = new Preedit;
  }
  return _instance;
}

void Preedit::append( char* s)
{
  append( s, "UTF-8" );
}

void Preedit::append( char* s, const char* encoding)
{
  ustring input( encoding , s );
  ustring result = buf + input;
  buf = result;
  render();
}

void Preedit::clear()
{
  buf.clear();
  render();
}

void Preedit::draw()
{
 
  int border = 0;

  Text t;
  t.append( buf );
  t.bgColor( 3 );
  t.fgColor( 5 );

  Rect r;
  r.w( t.x_max() + 2*border );
  r.h( t.y_max() + 2*border );
  r.c(3);
  
  gp->draw( 0, 0, &r);
  gp->draw( border , border , &t );
}
  


/*

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
