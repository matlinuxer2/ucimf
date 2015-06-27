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

#include "graphport.h"
#include "window.h"
#include "shape.h"
#include "graphdev.h"
#include <iostream>
#include "debug.h"
using namespace std;

GraphPort::GraphPort()
{
  x_tmp = y_tmp = 0;
  pseudo = true ;
  win = 0;
  buf_fg = buf_bg = 0;

  GraphDev::Open();
  gdev = GraphDev::mpGraphDev;
}

int GraphPort::OutChar( int x, int y, int fg, int bg, unsigned int charcode )
{
    //UrDEBUG("GraphPort::OutChar( %d, %d, %d, %d, %d )\n", x, y, fg, bg, charcode );
    int x_next = x+ x_tmp;
    if( gdev )
    {
	    x_next = gdev->OutChar( x+x_tmp, y+ y_tmp, fg, bg, charcode );
    }
    return x_next-x_tmp;
    //UrDEBUG("GraphPort::OutChar() End\n")
}



void GraphPort::PutPixel( int x, int y, int color)
{
    //UrDEBUG("GraphPort::PutPixel( %d, %d, %d )\n", x, y, color );

    if( !gdev ) return;
    gdev->PutPixel( x+x_tmp, y+y_tmp, color );
    //UrDEBUG("GraphPort::PutPixel() End\n");
}

void GraphPort::FillRect( int x, int y, int width, int height, int color) 
{
 //UrDEBUG("GraphPort::FillRect( %d, %d, %d, %d, %d )\n", x, y, width, height, color );
    if( !gdev ) return;
    gdev->FillRect( x+x_tmp, y+y_tmp, x+x_tmp+width, y+y_tmp+height, color );
 //UrDEBUG("GraphPort::FillRect() End()\n");
}

void GraphPort::DrawRect( int x, int y, int width, int height, int color) 
{
 //UrDEBUG("GraphPort::DrawRect( %d, %d, %d, %d, %d )\n", x, y, width, height, color );

    if( !gdev ) return;
    gdev->DrawRect( x+x_tmp, y+y_tmp, x+x_tmp+width, y+y_tmp+height, color );

 //UrDEBUG("GraphPort::DrawRect() End()\n");
}

void GraphPort::RevRect( int x, int y, int width, int height)
{

 //UrDEBUG("GraphPort::RevRect( %d, %d, %d, %d )\n", x, y, width, height );
    if( !gdev ) return;
    gdev->RevRect( x+x_tmp, y+y_tmp, x+x_tmp+width, y+y_tmp+height );
 //UrDEBUG("GraphPort::RevRect() End()\n");
}

int GraphPort::Height()
{
    if( !gdev ) return 0;
    return gdev->Height();
}

int GraphPort::Width()
{
    if( !gdev ) return 0;
    return gdev->Width();
}

void GraphPort::setPseudo( bool flag)
{
  if( flag != pseudo && flag == true && win!=0)
  {
    win->wh(0,0);
  }
  pseudo = flag;

}

void GraphPort::draw( int x, int y, Shape* sp )
{
  if( pseudo == true )
  {
    if( win !=0 )
    {
      win->measure( x + sp->x_max(), y+ sp->y_max() );
    }
  }
  else
  {
    setXY( win->x() +x , win->y()+y );
    sp->draw( this );
  }
}

void GraphPort::push_bg_buf()
{
  if( buf_bg != 0 )
  {
    buf_bg = 0;
  }

  if( !gdev ) return;
  gdev->SaveRect( win->x(), win->y(), win->x()+win->w(), win->y()+win->h(), &buf_bg );
}

void GraphPort::push_fg_buf()
{
  if( buf_fg != 0 )
  {
    buf_fg = 0;
  }

  if( !gdev ) return;
  gdev->SaveRect( win->x(), win->y(), win->x()+win->w(), win->y()+win->h(), &buf_fg );
}

void GraphPort::pop_bg_buf()
{
  if( buf_bg != 0 )
  {
    if( !gdev ) return;
    gdev->RstrRect( win->x(), win->y(), win->x()+win->w(), win->y()+win->h(), &buf_bg );
  }
}

void GraphPort::pop_fg_buf()
{
  if( buf_fg != 0 )
  {

    if( !gdev ) return;
    gdev->RstrRect( win->x(), win->y(), win->x()+win->w(), win->y()+win->h(), &buf_fg );
  }
}

void GraphPort::drop_fg_buf()
{
    if( buf_fg != NULL )
    {
      delete [] buf_fg;
      buf_fg = NULL;
    }
}

void GraphPort::drop_bg_buf()
{
    if( buf_bg != NULL )
    {
      delete [] buf_bg;
      buf_bg = NULL;
    }
}
