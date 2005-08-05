#include "layer.h"

class OverSpot:Layer
{
  public:
    OverSpot();
    ~OverSpot();
  
    void show();
    void hide();
    void update();
    void draw();
    void title( const char* );
    void table( const char** );
    int b();
    void b(int new_boarder );
    void pos( int new_x, int new_y );
    
  private:
    bool      visible;
    bool      modified;
   
    int       boarder;
    
    Rectangle Rect;
    Text      Title;
    Text      Table;
};
