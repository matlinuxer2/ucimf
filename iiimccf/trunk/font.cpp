#include "font.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <libxml++/libxml++.h>
#include <libxml++/parsers/textreader.h>
using namespace std;
using namespace xmlpp;

GraphDev* gdev;

Font::Font()
{
  try
  {
        string homedir = getenv("HOME");
	Glib::ustring conf=static_cast<Glib::ustring>( homedir +"/iiimccf.conf.xml" );
	cout << " Configure file is : " << conf << endl;
	xmlpp::TextReader reader( conf );
	while( reader.read() )
	{
	  if( reader.get_name() == "fontpath" && reader.get_node_type() != TextReader::EndElement )
	  {
	    reader.read();
	    fontpath= static_cast<string>( reader.get_value().data() ) ;
	    cout << "FontPatch fetch successfully...[" << fontpath << "]"<< endl;
	  } 
	}
	
	font_width = 0;
	font_height = 0;
	font_color = 3;
	charcode = 0;
	
	cout << "fontpath is: " << fontpath << endl;

	FT_Init_FreeType( &library ); 
	FT_New_Face( library, fontpath.c_str(), 0, &face);
	slot = face->glyph;

	GraphDev::Open();
	gdev = GraphDev::mpGraphDev;    
  
  }
  catch( const std::exception& e)
  {
    cout << "Exception caught: " << e.what() << endl;
  }
  
}

Font::Font( string fpath )
{
  fontpath = fpath;
  font_width = 0;
  font_height = 0;
  font_color = 3;
  charcode = 0;
  
  cout << "fontpath is: " << fontpath << endl;

  FT_Init_FreeType( &library ); 
  FT_New_Face( library, fontpath.c_str(), 0, &face);
  slot = face->glyph;

  GraphDev::Open();
  gdev = GraphDev::mpGraphDev;    
}

Font::~Font()
{

}

void Font::status()
{
  cout << "fontpath: " << fontpath << endl;
  cout << "encoding: " << "UTF-16" << endl;
  PCF_Public_Face pcfface = (PCF_Public_Face)face ;
  cout << "encoding: " << pcfface->charset_encoding << endl;
  cout << "registry: " << pcfface->charset_registry << endl;

}

void Font::info()
{
  cout << "--Font info--" << endl;
  cout.setf(ios_base::hex, ios_base::basefield );
  cout << "charcode: [" << charcode << "] in UTF-16" << endl;
  cout.setf(ios_base::dec, ios_base::basefield );
  cout << "font_width: " << font_width << endl;
  cout << "font_height: " << font_height << endl;
  cout << "font_color: " << font_color << endl;
  cout << "--info end---" << endl;
}

void Font::load( int code, int x, int y, int fw, int fh, int c )
{
  pos_x = x;
  pos_y = y;
  font_width = fw;
  font_height = fh;
  //FT_Set_Char_Size( face, font_width*64 , font_height*64, 0, 0 );
  encoding = FT_ENCODING_UNICODE;
  charcode = (FT_ULong) code;
  font_color = c;
}
  
void Font::draw()
{
    //error = FT_Load_Char( face, charcode, FT_LOAD_RENDER );
    error = FT_Load_Char( face, charcode, FT_LOAD_DEFAULT );
    unsigned char* tmp = slot->bitmap.buffer;
    int pos_left = pos_x + slot->bitmap_left;
    int pos_top = pos_y;// + slot->bitmap_top;
    int color = font_color;

    for (int i=0; i< slot->bitmap.rows; i++ )
    {
      for( int j=0; j< slot->bitmap.width; j++ )
      {
	if ( tmp[ i * slot->bitmap.pitch + j/8] & (1 << ( 7- j%8)) )
          gdev->PutPixel(pos_left+j, pos_top+i, color );
      }
    }
}
    

