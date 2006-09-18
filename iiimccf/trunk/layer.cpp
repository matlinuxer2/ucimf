#include "layer.h"
#include "font.h"
#include "graphdev.h"
#include <iostream>


/*
 * Window impementation.
 */

bool Window::position(int x, int y )
{
  cur_x = x; 
  cur_y =y;
}

void shift( int s_x, int s_y )
{
  shift_x = (cur_x + w()) - X_MAX ;
  if ( shift_x < -8 )
  {
    shift_x = 0 ;
  }
  else
  {
    shift_x = shift_x + 8 ; // shift back
  }

  shift_y = (cur_y + h() ) - Y_MAX ;
  if ( shift_y < -3*h() )
  {
    shift_y = -3*fh() - 9 ;
 }
  else
  {
    shift_y = 3*fh() + h() + 9;

  }
}

bool isVisible()
{
  return visible;
}

void setShow()
{
  visible = true;
}

void setHide()
{
  visible = false;
}

void show()
{
  push();
}

void hide()
{
  pop();
}

/* 
 *
 * String implementation 
 *
 * */

int String::size(){ 
  return CharCodes.size(); 
}

void String::clear()
{
  CharCodes.clear();
}

void String::push_char( const Word& ich )
{
  CharCodes.push_back( ich );
  return;
}

Word String::operator[](const int& i)
{ 
  return CharCodes[i]; 
}

String& String::operator+( const String& a, const String& b)
{
  String* result = new String;
  result.assign( a.begin(), a.end() );
  for( i = 0; i < b.size(); i++ )
  {
    result.push_back( b[i] );
  }
  return result ;
}

void String::info()
{
  int num = CharCodes.size();
  
  cout << "<p><b>String::info()</b></p><ul>";
  
  for( int i=0; i < num; i++ )
  {
    cout.setf( ios_base::hex, ios_base::basefield);
    cout << "<li>" <<  CharCodes[i] << "</li>" << endl;
    cout.setf( ios_base::dec, ios_base::basefield);
  }

  cout << "</ul>" << endl;
}


