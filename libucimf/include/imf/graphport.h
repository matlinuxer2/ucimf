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

#ifndef _GraphPort_
#define _GraphPort_

class GraphDev;
class Window;
class Shape;

class GraphPort 
{
    friend class Window;

  public:
    int OutChar(  int x, int y, int fg, int bg, unsigned int charcode );
    void PutPixel( int x, int y, int color);
    void FillRect( int x, int y, int width, int height, int color);
    void DrawRect( int x, int y, int width, int height, int color);
    void RevRect(  int x, int y, int width, int height);
    
    void draw( int x, int y, Shape* sp );
    void setXY( int x_to_draw, int y_to_draw ){ x_tmp = x_to_draw; y_tmp = y_to_draw; };

    void push_fg_buf();
    void pop_fg_buf();
    void push_bg_buf();
    void pop_bg_buf();
    void setPseudo( bool flag );
    void drop_fg_buf();
    void drop_bg_buf();
    
  protected:
    GraphPort();
    ~GraphPort(){};
    
    int x_tmp, y_tmp;
    bool pseudo;
    char *buf_bg, *buf_fg;

    Window *win;
    GraphDev *gdev;
};

#endif
