#include "shape.h"
#include "graphport.h"
#include "type.h"
#include "graphdev.h"

void Rect::draw( GraphPort* gp)
{
  gp->FillRect( 0, 0, width , height, color );
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

  if( new_str.size() * font_width > width )
  {
    width = new_str.size() * font_width;
  }

}

void Text::append_next( const ustring& ustr )
{
  data.push_back( ustr );
  
  height = font_height * data.size();

  if( ustr.size() * font_width > width )
  {
    width = ustr.size() * font_width;
  }

}

void Text::draw( GraphPort* gp)
{
  for( int i=0; i< data.size() ; i++ )
  {
    for( int j=0; j < data[i].length() ; j++ )
    {
      gp->OutChar( j*font_width, i*font_height, color_fg, color_bg, data[i][j] );
    }
  }
}
