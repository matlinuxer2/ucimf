#include "overspot.h"

OverSpot::OverSpot()
{
  visible = false;
  modified = false;
}

OverSpot::~OverSpot()
{ 

}

void OverSpot::show()
{
  visible = true;
  update();
}

void OverSpot::hide()
{
  visible = false;
  update();
}


void OverSpot::update()
{
  if( modified )
  {
    draw();
  }
}

void OverSpot::draw()
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

int OverSpot::b(){ return boarder; }
void OverSpot::b( int new_boarder ){ boarder = new_boarder; }
void OverSpot::pos( int new_x, int new_y )
{
  x( new_x );
  y( new_y );
}
