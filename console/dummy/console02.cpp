#include "font01.h"
#include <iostream>
using namespace std;

Font* font = Font::getInstance();

int main()
{
	cerr << "start" << endl;
	Font* font2 = Font::getInstance();
	cerr << "end" << endl;
	return 0;
}
