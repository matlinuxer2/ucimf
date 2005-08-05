#include "overspot.h"

inline OverSpot::OverSpot()
{
  visible = false;
  modified = false;
}
inline OverSpot::~OverSpot()
{ 

}

inline void OverSpot::show()
{
  visible = true;
  update();
}

inline void OverSpot::hide()
{
  visible = false;
  update();
}


inline void OverSpot::update()
{
  if( modified )
  {
    draw();
  }
}

inline void OverSpot::draw()
{
  int pos_x = x() - b();
  int pos_y = y() - b();
  Rect.x( pos_x );
  Rect.y( pos_y );
  Title.x( x() );
  Title.y( y() );
  Table.x( x() + b() );
  Table.y( y() + Title.h() );
  
  if( visible )
  {
    Rect.render();
    Title.render();
    Table.render();
  }
}

inline int OverSpot::b(){ return boarder; }
inline void OverSpot::b( int new_boarder ){ boarder = new_boarder; }
inline void OverSpot::pos( int new_x, int new_y )
{
  x( new_x );
  y( new_y );
}
