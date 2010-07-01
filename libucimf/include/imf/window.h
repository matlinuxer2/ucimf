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

#ifndef __Window
#define __Window

class GraphPort;
class Widget;
class Window;

class Wm
{
  public:
    virtual void update( Window* changed_window )=0;
};

class Window
{
    friend class Widget;

  public:
    int x(){ return pos_x; };
    int y(){ return pos_y; };
    int w(){ return width; };
    int h(){ return height; };
    void x( int new_x );
    void y( int new_y );
    void w( int new_width );
    void h( int new_height );
    void measure( int max_x, int max_y );

    bool isVisible(){ return visible; };
    void show();
    void hide();
    GraphPort* getGraphPort(){ return gp; };
    
    
    Wm *wm;
    
  protected:
    Window();
    bool visible;
    int pos_x, pos_y, width, height;
    GraphPort *gp;
    Widget *wd;
};

#endif
