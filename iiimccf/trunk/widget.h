#include "window.h"
#include "graphport.h"
#include "type.h"

#include <vector>
using std::vector;


class Widget
{
  public:
    virtual void draw()=0;
    void render();
    
  protected:
    GraphPort *gp;

  private:
    Window *win;
};

class Status : public Widget{
  public:
    static Status* getInstance();
    
    void draw();
    void set_imf_name( char* );
    void set_im_name( char* );
    void set_lang_name( char* );

  protected:
    Status();

  private:
    static Status* _instance;
   
    ustring imf_name;
    ustring im_name;
    ustring lang_name;
};


/*
class Prdt : public Widget
{
  public:
    Prdt();
    ~Prdt();

    void draw();
    void append( const char* s);
    void clear();
    
  private:
    char* title;
};


class Lkc : public Widget
{
  public:
    Lkc();
    ~Lkc();

    void draw();
    void append( const char* s );
    void clear();

  private:
    vector<char*> title;
};
*/
