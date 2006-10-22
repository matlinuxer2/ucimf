#include "subject_observer.h"
#include <iterator>

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

/*
 * Implementation of CursorPosition
 */
CursorPosition* CursorPosition::_instance = 0;

CursorPosition* CursorPosition::getInstance()
{
  if( _instance == 0 )
  {
    _instance = new CursorPosition;
  }
  
  return _instance;

}

CursorPosition::CursorPosition()
{
  x=0;
  y=0;
}

void CursorPosition::get_position( int& new_x, int& new_y)
{
  new_x = x;
  new_y = y;
}

void CursorPosition::set_position( int new_x, int new_y )
{
  if ( x != new_x || y != new_y )
  {
    x = new_x;
    y = new_y;
    notify();
  }
}

/*
 * Implementation of ConsoleFocus
 */
ConsoleFocus* ConsoleFocus::_instance = 0;

ConsoleFocus* ConsoleFocus::getInstance()
{
  if( _instance == 0 )
  {
    _instance = new ConsoleFocus;
  }
  
  return _instance;

}

ConsoleFocus::ConsoleFocus()
{
  focus = false;
}

bool ConsoleFocus::get_focus()
{
  return focus;
}

void ConsoleFocus::set_focus( bool new_focus )
{
  if ( focus != new_focus )
  {
    focus = new_focus;
    notify();
  }
}
