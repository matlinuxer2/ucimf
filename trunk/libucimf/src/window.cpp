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

#include "window.h"
#include "graphport.h"

Window::Window()
{
  visible = false;
  pos_x = pos_y = width = height =0;
  gp = new GraphPort;
  gp->win = this;
  wm = 0;
}

void Window::show()
{
  if( this != 0 && visible == false )
  {
    visible = true;
    gp->push_bg_buf();
    gp->pop_fg_buf();
  }
}

void Window::hide()
{
  if( this !=0 && visible == true )
  {
    visible = false;
    gp->push_fg_buf();
    gp->pop_bg_buf();
  }

}

void Window::x( int new_x )
{
  bool prev_visible = isVisible();

  hide();
  pos_x = new_x; 
  if( prev_visible == true )
  {
    show();
  }
}

void Window::y( int new_y )
{
  bool prev_visible = isVisible();

  hide();
  pos_y = new_y; 
  if( prev_visible == true )
  {
    show();
  }
}

void Window::w( int new_width )
{ 
  bool prev_visible = isVisible();

  hide();
  width = new_width; 
  if( wm != 0 )
  {
    wm->update( this );
  }
  if( prev_visible == true )
  {
    show();
  }
}

void Window::h( int new_height)
{ 
  bool prev_visible = isVisible();

  hide();
  height = new_height; 
  if( wm != 0 )
  {
    wm->update( this );
  }
  if( prev_visible == true )
  {
    show();
  }
}



void Window::measure( int max_x, int max_y )
{

  if( max_x > width )
    w( max_x );

  if( max_y > height )
    h( max_y );
}


