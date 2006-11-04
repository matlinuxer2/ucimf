#include "window.h"
#include "graphport.h"
#include "type.h"

#include <vector>
using std::vector;


class Widget
{
  public:
    Window* getWindow();
    GraphPort* getGraphPort();
    virtual void draw()=0;
    void render();
    
  protected:
    GraphPort *gp;
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

class Preedit : public Widget
{
  public:
    static Preedit* getInstance();

    void draw();
    void append( char* s);
    void append( char* s, const char* encoding);
    void clear();
    
  protected: 
    Preedit();

  private:
    static Preedit* _instance;
    ustring buf;
};

/*

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
