#include "window.h"
#include "graphport.h"


void Window::show()
{
  visible = true;
  gp->pop_fg_buf();
}

void Window::hide()
{
  gp->pop_bg_buf();
  visible = false;
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





