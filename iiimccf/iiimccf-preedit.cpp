#include "iiimccf-int.h"
#include <iostream>

Prdt::Prdt( IIIMCF_context new_context)
{
  context = new_context;
}

void Prdt::info()
{
    int cur_pos;
    IIIMF_status st;
    
    st = iiimcf_get_preedit_text (context, &prdt_buf, &cur_pos);
    if (st == IIIMF_STATUS_SUCCESS) {
	char *str = iiimcf_text_to_utf8( prdt_buf );
	cout << "  Preedit(" << cur_pos << "):" << str << endl; 
	delete str;
    } else if (st == IIIMF_STATUS_NO_PREEDIT) {
	cout << "Preedit is disabled" << endl;
    } else {
	cout << "unknow err" << st << endl;
    }
}

void Prdt::draw(){}
void Prdt::show(){}
void Prdt::hide(){}
void Prdt::update(){}
