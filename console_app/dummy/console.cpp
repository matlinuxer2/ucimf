#include "widget.h"

int main( int argc, char* argv[])
{
  // Rect rect;
  // rect.w(100);
  // rect.h(100);
  // rect.c(3);

  // ustring str("UTF-8", "test char drawing...");
  // ustring str2( "UTF-8", "我可以打中文嗎?");
  // 
  // Text txt;
  // txt.append( str );
  // txt.append( str2 );
  // txt.bgColor( 3 );
  // txt.fgColor( 5 );


  Status *stts = Status::getInstance();
  // GraphPort gp;
  // Window win(&gp);
  stts->set_imf_name("iiimf");
  stts->set_im_name("newpy");
  stts->set_lang_name("zh");
  stts->render();

  stts->set_imf_name("scim");
  stts->render();


} 
