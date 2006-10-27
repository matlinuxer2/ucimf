class GraphDev;
class Window;
class Shape;

class GraphPort 
{
  public:
    GraphPort();
    ~GraphPort(){};
    void OutChar(  int x, int y, int fg, int bg, unsigned int charcode );
    void PutPixel( int x, int y, int color);
    void FillRect( int x, int y, int width, int height, int color);
    void RevRect(  int x, int y, int width, int height);
    
    void draw( int x, int y, Shape* sp );
    void setXY( int x_to_draw, int y_to_draw ){ x_tmp = x_to_draw; y_tmp = y_to_draw; };

    void push_fg_buf();
    void pop_fg_buf();
    void push_bg_buf();
    void pop_bg_buf();
    void setPseudo( bool flag ){ pseudo = flag; };
    void drop_fg_buf();
    void drop_bg_buf();
    friend class Window;
    
  protected:
    int x_tmp, y_tmp;
    bool pseudo;
    char *buf_bg, *buf_fg;

    Window *win;
    GraphDev *gdev;
};

