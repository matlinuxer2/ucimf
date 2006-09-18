/*
 * implementation of Subject
 */

void Subject::notify()
{
  for(int i=0; i< observers.size(); i++ )
  {
    observers[i]->update();
  }
}

void Subject::attach( Observer* obsr )
{
  observers.push_back( obsr );
}


void Subject::detach( Observer* obsr )
{
  observers.erase( find( observers.begin(), observers.end(), obsr) );
}




/*
 * implementation of TrackPoing.
 */

TrackPoint::TrackPoint()
{
  x = 0;
  y = 0;
}

TrackPoint::~TrackPoint(){}

void TrackPoint::get_position( int& old_x, int& old_y)
{
  old_x = x;
  old_y = y;
}

void TrackPoint::set_position( int new_x, int new_y)
{
  if( x != new_x || y != new_y )
  {
    x = new_x;
    y = new_y;
    this->notify();
  }
}
