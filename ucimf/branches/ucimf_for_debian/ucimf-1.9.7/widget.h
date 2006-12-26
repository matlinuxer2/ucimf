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


#ifndef __Widget
#define __Widget

#include "window.h"
#include "graphport.h"
#include "type.h"

#include <vector>
using std::vector;


class Widget
{
  public:
    Window* getWindow();
    GraphPort* getGraphPort();
    virtual void draw()=0;
    void render();
    
  protected:
    GraphPort *gp;
    Window *win;
};

class Status : public Widget{
  public:
    static Status* getInstance();
    
    void draw();
    void clear();
    void set_imf_name( char* );
    void set_im_name( char* );
    void set_lang_name( char* );

  protected:
    Status();

  private:
    static Status* _instance;
   
    ustring imf_name;
    ustring im_name;
    ustring lang_name;
};

class Preedit : public Widget
{
  public:
    static Preedit* getInstance();

    void draw();
    void append( char* s);
    void append( char* s, const char* encoding);
    void clear();
    
  protected: 
    Preedit();

  private:
    static Preedit* _instance;
    ustring buf;
};

class LookupChoice : public Widget
{
  public:
    static LookupChoice* getInstance();

    void draw();
    void append( char* s);
    void append( char* s, const char* encoding);
    void append_next( char* s);
    void append_next( char* s, const char* encoding);
    void clear();
    
  protected: 
    LookupChoice();

  private:
    static LookupChoice* _instance;
    vector<ustring> bufs;
};
/*

class Lkc : public Widget
{
  public:
    Lkc();
    ~Lkc();

    void draw();
    void append( const char* s );
    void clear();

  private:
    vector<char*> title;
};
*/
#endif
