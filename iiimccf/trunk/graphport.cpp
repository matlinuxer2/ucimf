#include "graphport.h"
#include "window.h"
#include "shape.h"
#include "graphdev.h"

GraphPort::GraphPort()
{
  x_tmp = y_tmp = 0;
  pseudo = true ;
  win = 0;

  gdev = GraphDev::mpGraphDev;
  GraphDev::Open();
}

void GraphPort::OutChar( int x, int y, int fg, int bg, unsigned int charcode )
{
    gdev->OutChar( x+x_tmp, y+ y_tmp, fg, bg, charcode );
}


void GraphPort::PutPixel( int x, int y, int color)
{
    gdev->PutPixel( x+x_tmp, y+y_tmp, color );
}

void GraphPort::FillRect( int x, int y, int width, int height, int color) 
{
    gdev->FillRect( x+x_tmp, y+y_tmp, x+x_tmp+width, y+y_tmp+height, color );
}

void GraphPort::RevRect( int x, int y, int width, int height)
{
    gdev->RevRect( x+x_tmp, y+y_tmp, x+x_tmp+width, y+y_tmp+height );
}

void GraphPort::draw( int x, int y, Shape* sp )
{
  if( pseudo == true )
  {
    if( win !=0 )
    {
      win->measure( x, y, x + sp->x_max(), y+ sp->y_max() );
    }
  }
  else
  {
    setXY( x, y );
    sp->draw( this );
  }
}

void GraphPort::push_bg_buf()
{
  if( buf_bg != 0 )
  {
    buf_bg = 0;
  }

  gdev->SaveRect( win->x(), win->y(), win->x()+win->w(), win->y()+win->h(), &buf_bg );
}

void GraphPort::push_fg_buf()
{
  if( buf_fg != 0 )
  {
    gdev->SaveRect( win->x(), win->y(), win->x()+win->w(), win->y()+win->h(), &buf_fg );
  }
}

void GraphPort::pop_bg_buf()
{
  if( buf_bg != 0 )
  {
    gdev->RstrRect( win->x(), win->y(), win->x()+win->w(), win->y()+win->h(), &buf_bg );
  }
}

void GraphPort::pop_fg_buf()
{
  if( buf_fg != 0 )
  {
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
