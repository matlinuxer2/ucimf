/*
 * UCIMF - Unicode Console InputMethod Framework                                                    
 *
 * Copyright (c) 2006-2007 Chun-Yu Lee (Mat) and Open RazzmatazZ Laboratory (OrzLab)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "widget.h"
#include "shape.h"
#include "options.h"
#include <iostream>
#include <stdlib.h>
using namespace std;

#define BLACK_color 0


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
Options* option = Options::getInstance();

  bg_color = atoi( option->getOption("STATUS_BG_COLOR") ); 
  fg_color = atoi( option->getOption("STATUS_FG_COLOR") ); 
  border_color = atoi( option->getOption("STATUS_BORDER_COLOR") ); 
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

void Status::clear()
{
  //set_imf_name("");
  //set_im_name("");
  //set_lang_name("");
  set_imf_status((char*)"", (char*)"", (char*)"");
}

void Status::set_imf_status( char* new_imf_name, char* new_im_name, char* new_lang_name )
{
  ustring input( "UTF-8", new_imf_name );
  ustring input2( "UTF-8", new_im_name );
  ustring input3( "UTF-8", new_lang_name);
  imf_name = input;
  im_name = input2;
  lang_name = input3;
  render();
}

void Status::set_imf_name( char* new_imf_name )
{
  ustring input( "UTF-8", new_imf_name );
  imf_name = input;
  return;
}

void Status::set_im_name( char* new_im_name )
{
  ustring input( "UTF-8", new_im_name );
  im_name = input;
  return;
}

void Status::set_lang_name( char* new_lang_name )
{
  ustring input( "UTF-8", new_lang_name );
  lang_name = input;
  return;
}

void Status::set_fg_color( int fg )
{
  fg_color = fg;
}

void Status::set_bg_color( int bg )
{
  bg_color = bg;
}

void Status::set_border_color( int border )
{
  border_color = border;
}

void Status::draw()
{
 
  int border = 3;

  Text t;
  t.append( imf_name );
  t.bgColor( bg_color );
  t.fgColor( fg_color );
  
  Text t2;
  t2.append( im_name );
  t2.bgColor( bg_color );
  t2.fgColor( fg_color );

  Rect r;
  r.w( t.x_max() + t2.x_max() + 2*border );
  r.h( max( t.y_max(), t2.y_max() ) + 1*border );
  r.c( BLACK_color );
  r.toFill(true);
  
  Rect r1;
  r1.w( t.x_max() + 1*border );
  r1.h( max( t.y_max(), t2.y_max() ) + 1*border );
  r1.c( border_color );
  r1.toFill(false);
  
  Rect r2;
  r2.w( t2.x_max() + 1*border );
  r2.h( max( t.y_max(), t2.y_max() ) + 1*border );
  r2.c( border_color );
  r2.toFill(false);
  
  gp->draw( 0, 0, &r);
  gp->draw( 0, 0, &r1);
  gp->draw( border + t.x_max() , 0, &r2);

  gp->draw( border , border , &t );
  gp->draw( border + t.x_max() + border, border, &t2 );
}

/*
 * Implementation of Preedit
 */

Preedit* Preedit::_instance = 0;

Preedit::Preedit()
{ 
Options* option = Options::getInstance();
  bg_color = atoi( option->getOption("PREEDIT_BG_COLOR") ); 
  fg_color = atoi( option->getOption("PREEDIT_FG_COLOR") ); 
  border_color = atoi( option->getOption("PREEDIT_BORDER_COLOR") ); 
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

void Preedit::set_fg_color( int fg )
{
  fg_color = fg;
}

void Preedit::set_bg_color( int bg )
{
  bg_color = bg;
}

void Preedit::set_border_color( int border )
{
  border_color = border;
}

void Preedit::clear()
{
  buf.clear();
  render();
}

void Preedit::draw()
{
 
  int border = 3;

  Text t;
  if( buf.size() > 0 )
  {
    t.append( buf );
  }
  t.bgColor( bg_color );
  t.fgColor( fg_color );

  Rect r;
  r.w( t.x_max() + 1*border );
  r.h( t.y_max() + 1*border );
  r.c( BLACK_color );
  r.toFill(true);
  
  Rect r2;
  r2.w( t.x_max() + 1*border );
  r2.h( t.y_max() + 1*border );
  r2.toFill(false);
  r2.c( border_color );
  
  gp->draw( 0, 0, &r);
  gp->draw( 0, 0, &r2);
  gp->draw( border , border , &t );

}
  

LookupChoice* LookupChoice::_instance = 0;

LookupChoice::LookupChoice()
{ 
Options* option = Options::getInstance();
  bg_color = atoi( option->getOption("LOOKUPCHOICE_BG_COLOR") ); 
  fg_color = atoi( option->getOption("LOOKUPCHOICE_FG_COLOR") ); 
  border_color = atoi( option->getOption("LOOKUPCHOICE_BORDER_COLOR") ); 
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

void LookupChoice::set_fg_color( int fg )
{
  fg_color = fg;
}

void LookupChoice::set_bg_color( int bg )
{
  bg_color = bg;
}

void LookupChoice::set_border_color( int border )
{
  border_color = border;
}

void LookupChoice::clear()
{
  bufs.clear();
  render();
}

void LookupChoice::draw()
{
 
  int border = 3;

  Text t;
  for( size_t i=0 ; i<bufs.size(); i++ )
  {
    t.append_next( bufs[i] );
  }
  t.bgColor( bg_color );
  t.fgColor( fg_color );

  Rect r;
  r.w( t.x_max() + 1*border );
  r.h( t.y_max() + 1*border );
  r.toFill(true);
  r.c( BLACK_color );
  
  Rect r2;
  r2.w( t.x_max() + 1*border );
  r2.h( t.y_max() + 1*border );
  r2.toFill(false);
  r2.c( border_color);
  
  gp->draw( 0, 0, &r);
  gp->draw( 0, 0, &r2);
  gp->draw( border , border , &t );
}
  
