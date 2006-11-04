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
  
  cwmmap.clear();
}


void Cwm::set_position( int new_x, int new_y )
{
  if( new_x != _cursor_x || new_y != _cursor_y )
  {
    bool p_focus = _focus;
    
    _cursor_x = new_x;
    _cursor_y = new_y;

    if( !cwmmap.empty() )
    {
      map<Window*, Shift*>::iterator iter;
      for( iter = cwmmap.begin(); iter != cwmmap.end(); iter++ )
      {
	Window *w = (*iter).first;
	Shift *s = (*iter).second;
	s->update( w );
      }
    }

  }
}


void Cwm::set_focus( bool new_focus )
{
  if( new_focus )
  {
    _focus = true;

    if( !cwmmap.empty() )
    {
      map<Window*, Shift*>::iterator iter;
      for( iter = cwmmap.begin(); iter != cwmmap.end(); iter++ )
      {
	Window *w = (*iter).first;
	w->show();
      }
    }
  }
  else
  {
    _focus = false;

    if( !cwmmap.empty() )
    {
      map<Window*, Shift*>::iterator iter;
      for( iter = cwmmap.begin(); iter != cwmmap.end(); iter++ )
      {
	Window *w = (*iter).first;
	w->hide();
      }
    }
  }
}

void Cwm::update( Window* cw )
{
  Shift *s = cwmmap[cw];
  s->update( cw );
}

void Cwm::attachWindow( Window* new_win, Shift* new_shift)
{
  cwmmap[new_win]=new_shift;
  new_win->wm = this;
}



void StatusShift::update( Window* win )
{
  cwm = Cwm::getInstance();

  int Xres= cwm->xres();
  int Yres= cwm->yres();
  int x = cwm->cursor_x();
  int y = cwm->cursor_y();
  int height = win->h();
  int width = win->w();
  
  int shift_x = ( x + width ) - Xres;
  int shift_y = ( y + height ) - Yres;
 
  if( shift_x >= 0  )
  {
    shift_x = 0 - shift_x;
    shift_x -= 16;
  }
  else
  {
    shift_x = 0;
  }

  if ( shift_y >= 0 )
  {
    shift_y  = 0 - shift_y;
    shift_y -= height ;
  }
  else
  {
    if( y >= height )
    {
      shift_y = (-1)*height;
    }
    else{
      shift_y = height;
    }
  }
  x+=shift_x;
  y+=shift_y;

  win->x(x);
  win->y(y);

}

void PreeditShift::update( Window* win )
{
  cwm = Cwm::getInstance();
  int Xres= cwm->xres();
  int Yres= cwm->yres();
  int x = cwm->cursor_x();
  int y = cwm->cursor_y();
  
  
  int height = win->h();
  int width = win->w();
  
  
  int shift_x = ( x + width ) - Xres;
  int shift_y = ( y + height ) - Yres;
 
  if( shift_x >= -16 )
  {
    shift_x = 0 - shift_x - 16;
  }
  else
  {
    shift_x = 16 ;
  }

  if ( shift_y  >= 0 )
  {
    shift_y = 0 - shift_y -1 ;
  }
  else
  {
    shift_y = 0;
  }

  x+=shift_x;
  y+=shift_y;

  win->x(x);
  win->y(y);

}
