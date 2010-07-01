#include "font.h"
#include <iostream>

using namespace std;

int main()
{
	Font::instance()->setInfo("細明體", 16, 0 ); // 字型取自 fc-list 的列表
	//Font::instance()->showInfo(true);

	u16 code = 0x4e2d; // 0x4e2d => "中"
	Font::Glyph *glyph = (Font::Glyph *)Font::instance()->getGlyph(code);

	cout << "glyph->pitch: " << glyph->pitch << endl;
	cout << "glyph->width: " << glyph->width << endl;
	cout << "glyph->height: " << glyph->height << endl;
	cout << "glyph->left: " << glyph->left << endl;
	cout << "glyph->top: " << glyph->top << endl;

	for ( int y = glyph->height; y > 0 ; y-- )
	{
		for ( int x = 0; x < glyph->width ; x++ )
		{
			// Dump bitwise data
			/*
			int n = glyph->pixmap[ (glyph->height - y) * glyph->pitch + x ];
			for (int i=8; i>=0; i--) {                                                                  
				int bit = ((n >> i) & 1);
				cout << bit;
			}  
			*/

			if ( glyph->pixmap[ (glyph->height - y) * glyph->pitch + x ] ) {
				cout << "O" ;
			}
			else {
				cout << "_" ;

			}
		}
		cout << endl;
	}

	return 0;
}
