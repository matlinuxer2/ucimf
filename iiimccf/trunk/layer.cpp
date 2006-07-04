#include "layer.h"
#include "font.h"
#include "graphdev.h"
#include <iostream>
#include <ft2build.h> 
#include FT_FREETYPE_H 

//Font font(KFont);
Font* font2=NULL;

/*
 * implementation of Subject, Observer
 */
Subject::Subject(){}
Subject::~Subject(){}

void Subject::notify()
{
  cout << "enter notify" << endl;
  for(int i=0; i< observers.size(); i++ )
  {
    cout << "enter observer 1" << endl;
    observers[i]->update();
    cout << "enter observer 2" << endl;
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

Observer::Observer(){}
Observer::~Observer(){}

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


/* 
 * 
 * Layer implementation 
 * 
 * */


int Layer::x(){ return X;}
int Layer::y(){ return Y;}
int Layer::w(){ return Width;}
int Layer::h(){ return Height;}
int Layer::c(){ return Color;}

void Layer::x( int new_x ){ X= new_x;} 
void Layer::y( int new_y ){ Y = new_y;}
void Layer::w( int new_width ){ Width = new_width;}
void Layer::h( int new_height ){ Height = new_height;}
void Layer::c( int new_color ){ Color = new_color;}

/* 
 *
 * Rectangle implementation 
 *
 * */
Rectangle::Rectangle( int pos_x, int pos_y, int pos_xx, int pos_yy ,int color)
{ x(pos_x); y(pos_y); w( pos_xx - pos_x ); h( pos_yy- pos_y ); c(color);}

void Rectangle::update( int pos_x, int pos_y, int pos_xx, int pos_yy ,int color)
{ x(pos_x); y(pos_y); w( pos_xx - pos_x ); h( pos_yy- pos_y ); c(color);}

void Rectangle::render()
{
  GraphDev *gdev;
  GraphDev::Open();
  gdev = GraphDev::mpGraphDev;
  gdev->FillRect( x(), y(), x()+w(), y()+h(), c() );
}

void Rectangle::push( CharBitMap& tmp )
{
  if( w()!=0 || h()!=0 )
  {
    GraphDev *gdev;
    GraphDev::Open();
    gdev = GraphDev::mpGraphDev;
    gdev->SaveRect( x(), y(), x()+w(), y()+h(), tmp );

  }
}


void Rectangle::pop( CharBitMap& tmp )
{
  if( w()!=0 || h()!=0 )
  {
    GraphDev *gdev;
    GraphDev::Open();
    gdev = GraphDev::mpGraphDev;
    gdev->RstrRect( x(), y(), x()+w(), y()+h(), tmp );

  }
}

/* 
 *
 * String implementation 
 *
 * */


int String::size(){ 
  return CharCodes.size(); 
}

void String::push_char( const IIIMP_card16& ich )
{
  CharCodes.push_back( ich );
  return;
}

void String::push_string( String str )
{
  int num = str.size();
  for(int i=0;i< num ; i++)
  {
    CharCodes.push_back( str[i] );
  }
  return;
}

IIIMP_card16 String::operator[](const int& i)
{ 
  return CharCodes[i]; 
}

String::String( const vector<IIIMP_card16>& charcodes )
{
  CharCodes = charcodes;
}

void String::info()
{
  int num = CharCodes.size();
  
  cout << "====String info====" << endl;
  for( int i=0; i < num; i++ )
  {
    cout << "Char[" << i << "]: " ; 
    cout.setf( ios_base::hex, ios_base::basefield);
    cout << CharCodes[i] << endl;
    cout.setf( ios_base::dec, ios_base::basefield);
  }
  cout << "==================" << endl;
}

void String::render( int x, int y, int fw, int fh, int color )
{
  int num=CharCodes.size();
 
  if( font2 == NULL )
  font2=new Font();
  //font2->status();

  for( int i= 0; i < num ; i++ )
  { 
    font2->load( CharCodes[i], x+i*fw, y , fw , fh, color );
    font2->draw();
  }
}

/* 
 *
 * Text implementation 
 *
 * */

Text::Text()
{
  x(0);
  y(0);
  w(0);
  h(0);
  fw(0);
  fh(0);
  fc(3);
  
}

Text::Text( vector<String> strings )
{
  Strings = strings;
}

Text::Text( int pos_x, int pos_y, int font_height, int font_width, int font_color )
{
  x(pos_x);
  y(pos_y);
  FontWidth = font_width;
  FontHeight = font_height;
  FontColor = font_color;
}


void Text::append( String str )
{
  Strings.push_back( str );
  
  /* refresh the x,y of Text */
  fw( FontWidth );
  fh( FontHeight );
  
  return ;
}

void Text::info()
{
  cout << "Text Info::[" 
       << "Font HxW:"  << fw() << "x" << fh() 
       << " (X,Y)_WxH:" << "(" << x() << "," << y() <<")_"<< w() << "x" << h()  
       << " Number of Rows:" << Strings.size() 
       << " ]" << endl;
}
  
int Text::fw(){ return FontWidth; }
int Text::fh(){ return FontHeight; }
int Text::fc(){ return FontColor; }
void Text::fw( int font_width )
{ 
  FontWidth = font_width; 
  
  int num = Strings.size();
  for( int i=0; i< num; i++ )
  {
    if( w() < font_width*Strings[i].size() )
    {
      w( font_width*Strings[i].size() );
    }
  }
}

void Text::fh( int font_height )
{
  FontHeight = font_height; 
 
  h( font_height*Strings.size() );
}

void Text::fc( int font_color )
{ 
  FontColor = font_color; 
}


void Text::render()
{
  for( int i=0; i < Strings.size(); i++ )
  {
    Strings[i].render( x(), i*fh()+y(), fw(), fh(), fc() );
  }
}
