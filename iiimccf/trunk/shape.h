#ifndef __Shape
#define __Shape

#include "type.h"

class GraphPort;

class Shape
{
  public:
    virtual int x_max()=0;
    virtual int y_max()=0;
    virtual void draw( GraphPort* gp )=0;
};

class Rect : public Shape
{
  public:
    void draw( GraphPort* gp );

    int x_max(){ return w(); }
    int y_max(){ return h(); }

    int w(){ return width; };
    int h(){ return height; };
    void w( int new_width ){ width = new_width; };
    void h( int new_height ){ height = new_height; };
    void c( int new_color ){ color = new_color; };
    void toFill( bool flag ){ be_Filled = flag; };
  private:
    int width, height;
    int color;
    bool be_Filled;
};

class Text : public Shape
{
  public:
    Text();
    void draw( GraphPort* gp );
    void append( const ustring& ustr);
    void append_next( const ustring& ustr);
    void bgColor( int new_bg_color ){ color_bg = new_bg_color; };
    void fgColor( int new_fg_color ){ color_fg = new_fg_color; }; 

    int x_max(){ return width; };
    int y_max(){ return height; };

  private:
    int width, height;
    int font_width, font_height;
    int color_fg, color_bg;
    
    vector<ustring> data;
};


#endif
