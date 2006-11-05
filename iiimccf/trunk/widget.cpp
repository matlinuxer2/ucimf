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
  if( buf.size() > 0 )
  {
    t.append( buf );
  }
  t.bgColor( 3 );
  t.fgColor( 5 );

  Rect r;
  r.w( t.x_max() + 2*border );
  r.h( t.y_max() + 2*border );
  r.c(3);
  
  gp->draw( 0, 0, &r);
  gp->draw( border , border , &t );
}
  

LookupChoice* LookupChoice::_instance = 0;

LookupChoice::LookupChoice()
{ 
  win = 0;
  gp = 0;
  win = getWindow();
  gp = getGraphPort();
}

LookupChoice* LookupChoice::getInstance()
{
  if( _instance == 0 )
  {
    _instance = new LookupChoice;
  }
  return _instance;
}

void LookupChoice::append( char* s)
{
  append( s, "UTF-8" );
}

void LookupChoice::append( char* s, const char* encoding)
{
  ustring input( encoding , s );
  if( bufs.empty() )
  {
    bufs.push_back( input );
  }
  else
  {
    ustring last =  bufs.back();
    ustring result = last + input;
    bufs.pop_back();
    bufs.push_back(result);
  }
}

void LookupChoice::append_next( char* s)
{
  append_next( s, "UTF-8" );
}

void LookupChoice::append_next( char* s, const char* encoding)
{
  ustring input( encoding , s );
  bufs.push_back( input );
}

void LookupChoice::clear()
{
  bufs.clear();
  render();
}

void LookupChoice::draw()
{
 
  int border = 0;

  Text t;
  for( int i=0 ; i<bufs.size(); i++ )
  {
    t.append_next( bufs[i] );
  }
  t.bgColor( 3 );
  t.fgColor( 5 );

  Rect r;
  r.w( t.x_max() + 2*border );
  r.h( t.y_max() + 2*border );
  r.c(3);
  
  gp->draw( 0, 0, &r);
  gp->draw( border , border , &t );
}
  
