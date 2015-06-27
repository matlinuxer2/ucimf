#include "widget.h"
#include "window.h"
#include "graphport.h"
#include "shape.h"

int main( int argc, char* argv[])
{
  Rect rect;
  rect.w(100);
  rect.h(100);
  rect.c(3);

  ustring str("UTF-8", "test char drawing...");
  ustring str2( "UTF-8", "我可以打中文嗎?");
  
  Text txt;
  txt.append( str );
  txt.append( str2 );
  txt.bgColor( 3 );
  txt.fgColor( 5 );

  Widget* wd;
  Window* win = wd->getWindow();
  GraphPort* gp = wd->getGraphPort();

  gp->setPseudo(true);
  gp->draw( 30, 30, &rect );
  gp->draw( 40, 40, &txt );
 
  gp->push_bg_buf();
  gp->setPseudo(false);
  gp->draw( 30, 30, &rect );
  gp->draw( 40, 40, &txt );
  gp->push_fg_buf();
  gp->pop_bg_buf();

  win->x(400);
  win->y(300);

  win->show();
  
  win->x(200);
  win->y(100);

  win->show();
  
  // Status *stts = Status::getInstance();
  // stts->set_imf_name("iiimf");
  // stts->set_im_name("newpy");
  // stts->set_lang_name("zh");
  // stts->render();
  // stts->set_imf_name("scim");
  // stts->render();

} 
