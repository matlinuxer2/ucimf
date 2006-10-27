#include "window.h"
#include "graphport.h"

Window::Window()
{
  visible = false;
  pos_x = pos_y = width = height =0;
  gp = new GraphPort;
}

Window::Window( GraphPort *new_gp )
{
  visible = false;
  pos_x = pos_y = width = height =0;
  gp = new_gp;
  gp->win = this;
}

void Window::show()
{
  if( this != 0)
  {
    visible = true;
    gp->pop_fg_buf();
  }
}

void Window::hide()
{
  if( this !=0 )
  {
    visible = false;
    gp->pop_bg_buf();
  }

}

void Window::x( int new_x )
{
  if( isVisible() )
  {
    hide();
  }
  
  pos_x = new_x; 

  if( isVisible() )
  {
    show();
  }
}

void Window::y( int new_y )
{
  if( isVisible() )
  {
    hide();
  }
  
  pos_y = new_y; 

  if( isVisible() )
  {
    show();
  }

}
void Window::w( int new_width )
{ 
  if( isVisible() )
  {
    hide();
  }
  
  width = new_width; 

  if( isVisible() )
  {
    show();
  }
}

void Window::h( int new_height)
{ 
  if( isVisible() )
  {
    hide();
  }
  
  height = new_height; 

  if( isVisible() )
  {
    show();
  }
}



void Window::measure( int x1, int y1, int x2, int y2 )
{
  if( x1 < pos_x )
    pos_x = x1;

  if( y1 < pos_y )
    pos_y = y1;

  if( x2 > pos_x + width )
    width = x2 - pos_x ;

  if( y2 > pos_y + height )
    height = y2 - pos_y;
}


