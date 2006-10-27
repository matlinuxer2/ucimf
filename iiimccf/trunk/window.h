#ifndef __Window
#define __Window

class GraphPort;

class Window
{
  public:
    Window();
    Window( GraphPort *new_gp );
    int x(){ return pos_x; };
    int y(){ return pos_y; };
    int w(){ return width; };
    int h(){ return height; };
    void x( int new_x );
    void y( int new_y );
    void w( int new_width );
    void h( int new_height );
    void measure( int x1, int y1, int x2, int y2 );

    bool isVisible(){ return visible; };
    void show();
    void hide();
    GraphPort* getGraphPort(){ return gp; };
    
  private:
    bool visible;
    int pos_x, pos_y, width, height;
    GraphPort *gp;
};

#endif
