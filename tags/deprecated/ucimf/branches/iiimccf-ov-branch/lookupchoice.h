#include <iiimcf.h>

class Text;
class Rectangle;
class BitMap;

class Lkc
{
  public:
    Lkc();
    Lkc( IIIMCF_context context );
    ~Lkc(){};

    void info(); // for text-mode checking.

    bool update();
    bool position(int x, int y);
    bool isVisible();
    void setShow();
    void setHide();
    void show();
    void hide();
    bool draw();
    void shift();
    void empty();

  private:

    IIIMCF_context context;
    bool visible;
    int cur_x, cur_y;
    int shift_x, shift_y;
    int cur_idx;
    Text* lkc_text;
    Rectangle* rect;
    BitMap lkc_tmp;
};
