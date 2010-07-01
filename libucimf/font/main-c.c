#include "font.h"
#include <stdio.h>

int main()
{
	call_Font_setInfo("細明體", 16, 0 ); // 字型取自 fc-list 的列表
	call_Font_showInfo();

	u16 code = 0x4e2d; // 0x4e2d => "中"
	struct_Font_Glyph *glyph = call_Font_getGlyph(code);

	//cout << "glyph->pitch: " << glyph->pitch << endl;
	//cout << "glyph->width: " << glyph->width << endl;
	//cout << "glyph->height: " << glyph->height << endl;
	//cout << "glyph->left: " << glyph->left << endl;
	//cout << "glyph->top: " << glyph->top << endl;

	int x,y;
	for ( y = glyph->height; y > 0 ; y-- )
	{
		for ( x = 0; x < glyph->width ; x++ )
		{
			if ( glyph->pixmap[ (glyph->height - y) * glyph->pitch + x ] ) {
				printf( "O" );
			}
			else {
				printf( "_" );
			}
		}
		printf( "\n" );
	}

	return 0;
}
