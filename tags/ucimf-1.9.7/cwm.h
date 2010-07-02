/*
 * UCIMF - Unicode Console InputMethod Framework
 * Copyright (C) <2006>  Chun-Yu Lee (Mat) <Matlinuxer2@gmail.com> 
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
#include <map>
using namespace std;

class Shift;

class Cwm : Wm
{
  public:
    static Cwm* getInstance();

    void set_position( int new_x, int new_y );
    void set_focus( bool new_focus );
    bool get_focus(){ return _focus; };
    void update( Window* changed_win );

    void attachWindow( Window* new_win, Shift* new_shift);

    int xres(){ return _xres; };
    int yres(){ return _yres; };
    int cursor_x(){ return _cursor_x; };
    int cursor_y(){ return _cursor_y; };

  protected:
    Cwm();
    
  private:
    static Cwm* _instance;

    int _xres, _yres;
    int _cursor_x, _cursor_y;
    bool _focus;
    
    map<Window*,Shift*> cwmmap;
};


class Shift
{
  public:
    virtual void update( Window* )=0;

  protected:
    Cwm* cwm;
};

class StatusShift : public Shift
{
  public:
    void update( Window* );

};

class PreeditShift : public Shift
{
  public:
    void update( Window* );

};

class LookupChoiceShift : public Shift
{
  public:
    void update( Window* );

};
