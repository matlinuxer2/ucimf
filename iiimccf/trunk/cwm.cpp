#include "graphdev.h"
#include "cwm.h"

Cwm* Cwm::_instance = 0;

Cwm* Cwm::getInstance()
{
  if( _instance == 0 )
  {
    _instance = new Cwm;
  }

  return _instance;
}

Cwm::Cwm()
{
  _cursor_x = _cursor_y =0;
  _focus = false;
  GraphDev::Open();
  GraphDev* gdev=GraphDev::mpGraphDev;
  _xres= gdev->Width();
  _yres= gdev->Height();

  wins.clear();
  cwmmap.clear();
}


void Cwm::set_position( int new_x, int new_y )
{
  if( new_x != _cursor_x || new_y != _cursor_y )
  {
    _cursor_x = new_x;
    _cursor_y = new_y;

    for( int i=0; i< wins.size(); i++ )
    {
      Shift *s = cwmmap[ wins[i] ];
      s->update( wins[i] );
    }
  }
}


void Cwm::set_focus( bool new_focus )
{
  if( new_focus )
  {
    for( int i=0; i< wins.size(); i++ )
    {
      wins[i]->show();
    }
    _focus = true;
  }
  else
  {
    for( int i=0; i< wins.size(); i++ )
    {
      wins[i]->hide();
    }
    _focus = false;
  }
}

void Cwm::windowChanged( Window* cw )
{
  Shift *s=cwmmap[cw];
  s->update( cw );
}

void Cwm::attachWindow( Window* new_win, Shift* new_shift )
{
  wins.push_back( new_win );
  cwmmap[new_win]=new_shift;
}



void StatusShift::update( Window* win )
{
  if( cwm->get_focus() == false )
  {
    win->hide();
  }
  else
  {
    win->show();
  }

  int Xres= cwm->xres();
  int Yres= cwm->yres();
  int x = cwm->cursor_x();
  int y = cwm->cursor_y();
  int height = win->h();
  int width = win->w();
  
  int shift_x = ( x + width ) - Xres;
  int shift_y = ( y + height ) - Yres;
 
  if( shift_x < 0  )
  {
    shift_x = 0;
  }

  if ( shift_y  < 0 )
  {
    shift_y = 0;
  }
  else
  {
    shift_y = (-1)*height;
  }
  x+=shift_x;
  y+=shift_y;

  win->x(x);
  win->y(y);
}

/*
void PreeditShift::update( Window* win )
{
  int Xres= cwm->xres();
  int Yres= cwm->yres();
  int x = cwm->cursor_x();
  int y = cwm->cursor_y();
  
  
  int height = win->h();
  int width = win->w();
  
  
  int shift_x = ( x + width ) - Xres;
  int shift_y = ( y + height ) - Yres;
 
  if( shift_x + font_width < 0 )
  {
    shift_x = 0;
  }
  else
  {
    shift_x += font_width;
  }

  if ( shift_y + 2*font_height < 0 )
  {
    shift_y = -2 * font_height ;
  }
  else
  {
    shift_y = font_height + height;
  }

  x+=shift_x;
  y+=shift_y;

  win->x(x);
  win->y(y);

}
*/
