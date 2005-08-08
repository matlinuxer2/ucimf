#include <stdint.h>
#include <iiimp.h>
#include "layer.h"
#include "graphdev.h"
#include "font.h"

#include <vector>

GraphDev *gdev;
Font font( EFont );


/* Layer implementation */

inline int Layer::x(){ return X;}
inline int Layer::y(){ return Y;}
inline int Layer::w(){ return Width;}
inline int Layer::h(){ return Height;}
inline int Layer::c(){ return Color;}

inline void Layer::x( int new_x ){ X= new_x;} 
inline void Layer::y( int new_y ){ Y = new_y;}
inline void Layer::w( int new_width ){ Width = new_width;}
inline void Layer::h( int new_height ){ Height = new_height;}
inline void Layer::c( int new_color ){ Color = new_color;}

/* Rectangle implementation */
inline Rectangle::Rectangle( int pos_x, int pos_y, int pos_xx, int pos_yy ,int color)
{ x(pos_x); y(pos_y); w( pos_xx - pos_x ); h( pos_yy- pos_y ); c(color);}

void Rectangle::render()
{
  gdev->FillRect( x(), y(), x()+w(), y()+h(), c() );
}


/* String implementation */


String::String( vector<IIIMP_card16> charcodes )
{
  int size = charcodes.size();
  vector<FT_ULong> CharCodes( size );
  for (int i =0; i< size; i++ )
  {
    CharCodes[i] = (FT_ULong) charcodes[i];
  }
}

String::String( int font_width, int font_height, vector<IIIMP_card16> charcodes )
{
  int size = charcodes.size();
  vector<FT_ULong> CharCodes( size );
  for (int i =0; i< size; i++ )
  {
    CharCodes[i] = (FT_ULong) charcodes[i];
  }
  
  fw( font_width );
  fh( font_height );
  w( fw()*(CharCodes.size()) );
  h( fh() );
}

inline int String::fw(){ return FontWidth; }
inline int String::fh(){ return FontHeight; }

inline void String::fw( int new_font_width ){ FontWidth = new_font_width; w( fw()* (CharCodes.size()) ); }
inline void String::fh( int new_font_height ){ FontHeight = new_font_height; h( fh() ); }


void String::render()
{
  int num=CharCodes.size();
 
  for( int i= 0; i < num ; i++ )
  {
    font.load( x()+i*fw(), y() , fw() , FT_ENCODING_UNICODE, CharCodes[i] );
    font.draw();
  }
}

void String::render( int pos_x, int pos_y)
{
  x( pos_x );
  y( pos_y );
  render();
}


/* Text implementation */

Text::Text( vector<String> strings )
{
  Strings = strings;
}

Text::Text( int pos_x, int pos_y, vector<String> strings )
{
  Strings = strings;
  x(pos_x);
  y(pos_y);
  
  int rows = Strings.size();
  int max_width;
  for ( int i = 0; i< rows; i++ )
  {
    if ( max_width < Strings[i].w() )
    {
      max_width = Strings[i].w();
    }
  }
  w( max_width );
  h( Strings[0].fw() * rows );
}

void Text::append( String str )
{
  Strings.push_back( str );
  return ;
}

int Text::fw(){ return Strings[0].fw(); }

int Text::fh(){ return Strings[0].fh(); }

void Text::fw( int font_width )
{
  
  int rows = Strings.size();
  int max_width;
  for ( int i =0; i< rows; i++ )
  {
    Strings[i].fw( font_width );
    if ( max_width < Strings[i].w() )
    {
      max_width = Strings[i].w();
    }
  }
  w(max_width);
}

void Text::fh( int font_height )
{
  int rows = Strings.size();
  for ( int i =0; i< rows; i++ )
  {
    Strings[i].fh( font_height );
  }
  h( fh()*Strings.size() );
}

void Text::render()
{
  for( int i=0; i < Strings.size(); i++ )
  {
    Strings[i].render( x(), i*fh()+y() );
  }
}
