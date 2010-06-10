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

#include <stdlib.h>
#include "graphdev.h"
#include "cwm.h"
#include "options.h"
#include <iostream>
#include "debug.h"
using namespace std;

Cwm* Cwm::_instance = 0;

Cwm* Cwm::getInstance()
{
  if( _instance == 0 )
  {
	 UrDEBUG( "Cwm new() \n");
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
  if( new_focus != _focus && !cwmmap.empty() )
  {
      _focus = new_focus;
      map<Window*, Shift*>::iterator iter;
      for( iter = cwmmap.begin(); iter != cwmmap.end(); iter++ )
      {
	Window *w = (*iter).first;
	if( new_focus == true )	
	{
	  w->show();
	}
	else
	{
	  w->hide();
	}
      }
  }
}

void Cwm::update( Window* cw )
{
 UrDEBUG("void Cwm::update( Window* cw )\n");
  Shift *s = cwmmap[cw];
if( s->cwm != 0 )
{
  s->update( cw );
}
 UrDEBUG("void Cwm::update( Window* cw ) END\n");
}

void Cwm::attachWindow( Window* new_win, Shift* new_shift)
{
  cwmmap[new_win]=new_shift;
  new_win->wm = this;
}


StatusShift::StatusShift( int x_shift, int y_shift )
{
  cwm = 0;
  Options* option = Options::getInstance();
  x_gap = x_shift; //atoi( option->getOption("STATUS_SHIFT_X") ); 
  y_gap = y_shift; //atoi( option->getOption("STATUS_SHIFT_Y") ); 
}

void StatusShift::update( Window* win )
{
 UrDEBUG("void StatusShift::update( Window* win )\n"); 
  cwm = Cwm::getInstance();

  int Xres= cwm->xres() - 16;
  int Yres= cwm->yres() - 16 ;
  int x = cwm->cursor_x();
  int y = cwm->cursor_y();
  int height = win->h();
  int width = win->w();
  
  int over_x = ( x + width ) - Xres;
  int over_y = ( y + height ) - Yres;
  int shift_x = x_gap;
  int shift_y = y_gap;
 
  if( over_x + shift_x > 0  )
  {
      shift_x = -1 * over_x ;
  }

  if ( over_y + shift_y > 0 )
  {
    shift_y *= -1;
    shift_y -= height;
  }

  win->x( x + shift_x );
  win->y( y + shift_y );
 UrDEBUG("void StatusShift::update( Window* win ) END\n");

}

PreeditShift::PreeditShift( int x_shift, int y_shift )
{
  cwm = 0;
  Options* option = Options::getInstance();
  x_gap = x_shift; //atoi( option->getOption("PREEDIT_SHIFT_X") ); 
  y_gap = y_shift; //atoi( option->getOption("PREEDIT_SHIFT_Y") ); 
}


void PreeditShift::update( Window* win )
{
 UrDEBUG("void PreeditShift::update( Window* win )\n");
  cwm = Cwm::getInstance();
  int Xres= cwm->xres() -16;
  int Yres= cwm->yres() -16;
  int x = cwm->cursor_x();
  int y = cwm->cursor_y();
  
  
  int height = win->h();
  int width = win->w();

  int over_x = ( x + width ) - Xres;
  int over_y = ( y + height ) - Yres;
  int shift_x = x_gap;
  int shift_y = y_gap;
 
  if( over_x + shift_x > 0  )
  {
    shift_x = -1 * over_x ;
  }

  if ( over_y + shift_y > 0 )
  {
    shift_y *= -1;
    shift_y -= height;
  }

  win->x( x + shift_x );
  win->y( y + shift_y );
  
 UrDEBUG("void PreeditShift::update( Window* win ) END\n");
}

LookupChoiceShift::LookupChoiceShift( int x_shift, int y_shift )
{
  cwm = 0;
  Options* option = Options::getInstance();
  x_gap = x_shift; //atoi( option->getOption("LOOKUPCHOICE_SHIFT_X") ); 
  y_gap = y_shift; //atoi( option->getOption("LOOKUPCHOICE_SHIFT_Y") ); 
}

void LookupChoiceShift::update( Window* win )
{
 UrDEBUG("void LookupChoiceShift::update( Window* win )\n");
  cwm = Cwm::getInstance();
  int Xres= cwm->xres()-16;
  int Yres= cwm->yres()-16;
  int x = cwm->cursor_x();
  int y = cwm->cursor_y();
  int height = win->h();
  int width = win->w();
  
  int over_x = ( x + width ) - Xres;
  int over_y = ( y + height ) - Yres;
  int shift_x = x_gap;
  int shift_y = y_gap;
 
  if( over_x + shift_x > 0  )
  {
    shift_x = -1 * over_x ;
  }

  if ( over_y + shift_y > 0 )
  {
    shift_y *= -1;
    shift_y -= height;
  }

  win->x( x + shift_x );
  win->y( y + shift_y );
 UrDEBUG("void LookupChoiceShift::update( Window* win ) END\n");

}
