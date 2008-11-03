#include <cstdlib>
#include <iostream>
#include <ft2build.h>
#include FT_FREETYPE_H

using namespace std;

const char* get_ft_error(FT_Error error)
{
	#undef __FTERRORS_H__
	#define FT_ERRORDEF(e,v,s) case e: return s;
	#define FT_ERROR_START_LIST
	#define FT_ERROR_END_LIST
	switch (error) {
		#include FT_ERRORS_H
		default:
		return "unknown";
	}
}



int main( int argc, char* argv[] )
{
	cerr << "Font instance init" << endl;
	FT_Error ft_err = 0;
	FT_Library library;
	FT_Face	face;
	char* font_path=NULL;

	if( argc >=2 )
	{
		font_path = argv[1];
	}
	else
	{
		font_path = getenv( "FONT_PATH" );
	}

	cerr << "pass0" << endl;

	int font_width =0;
	int font_height =0;

	if( font_path == NULL )
	{
		font_path = "";
	}

	cerr << "fp: " << font_path << endl;


	cerr << "pass1" << endl;
	ft_err = FT_Init_FreeType( &library ); 
	if( ft_err ) cerr << "FreeType hurt: "<< get_ft_error( ft_err ) << endl;
	ft_err = FT_New_Face( library, font_path, 0, &face);
	if( ft_err ) cerr << "FreeType hurt: "<< get_ft_error( ft_err ) << endl;

	cerr << "pass2" << endl;
	cerr << font_width << endl;
	cerr << font_height << endl;
	/* get font size info */
	font_width = font_width ? font_width : 16;
	font_height = font_height ? font_height : 16;

	cerr << "pass3" << endl;

	cerr << "fw: "<< font_width << endl;
	cerr << font_path << endl;
	cerr << "fh: "<< font_height << endl;

	//FT_Set_Char_Size( face, font_width*64 , font_height*64, 0, 0 );
	FT_Set_Pixel_Sizes( face, font_width, font_height);
	//FT_Select_Charmap( face, FT_ENCODING_UNICODE );
	cerr << "pass4" << endl;

	if( face->available_sizes !=0 ) // stand for the pcf font.
	{
		cerr << "pass5" << endl;
		ft_err = FT_Set_Charmap( face, face->charmaps[0] );
		if( ft_err ) cerr << "FreeType hurt: "<< get_ft_error( ft_err ) << endl;

		cerr << "pass6" << endl;
		int fh = face->available_sizes->height;
		int fw = face->available_sizes->width;
	}

}
