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


