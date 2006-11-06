#include "shape.h"
#include "graphport.h"
#include "type.h"
#include "graphdev.h"

void Rect::draw( GraphPort* gp)
{
  if( be_Filled )
  {
    gp->FillRect( 0, 0, width , height, color );
  }
  else
  {
    gp->DrawRect( 0, 0, width , height, color );
  }
}

Text::Text()
{
  height = width = 0;
  color_fg = color_bg = 0;
  font_width = GraphDev::BlockWidth();
  font_height = GraphDev::BlockHeight();
  data.clear();
}

void Text::append( const ustring& ustr )
{
  ustring orig_str;

  if( !data.empty() )
  {
    orig_str = data.back();
    data.pop_back();
  }
  
  ustring new_str = orig_str + ustr ;
  data.push_back( new_str );
  
  height = font_height * data.size();
  
  int new_length = new_str.length();
  
  if( new_length > width )
  {
    width = new_length;
  }

}

void Text::append_next( const ustring& ustr )
{
  data.push_back( ustr );
  
  height = font_height * data.size();

  int new_length = ustr.length();
  if( new_length > width )
  {
    width = new_length;
  }

}

void Text::draw( GraphPort* gp)
{
  for( int i=0; i< data.size() ; i++ )
  {
    int x = 0;
    int font_h = font_height -1;
    for( int j=0; j < data[i].size() ; j++ )
    {
      x = gp->OutChar( x, i*font_h, color_fg, color_bg, data[i][j] );
    }
  }
}
