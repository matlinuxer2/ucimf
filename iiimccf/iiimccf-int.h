#include "iiimccf.h"
#include "overspot.h"
#include <cstdlib>


class IIIMCCF {
  public:
    IIIMCCF();
    ~IIIMCCF();

    bool on();
    bool off();
    int proc( int, int, int );
    int result( char** );
   
    void lkc_show();

    bool ims_show();
    bool ims_set();
    
    void scrn( int, int ,int );
    void pos( int, int );
    void refresh();
    
  private:

    // IIIMCF Object
    IIIMCF_handle 	handle;
    IIIMCF_context 	context;
   
    /* committed text */
    char* cmt_buf;
    /* current screen infomation */
    int height, width, resolution;
    /* current cursor position */
    int x,y;
 
};


/** Registed component functions **/
IIIMF_status iiimccf_preedit( IIIMCF_context, IIIMCF_event, IIIMCF_component, IIIMCF_component );
IIIMF_status iiimccf_lookup_choice( IIIMCF_context, IIIMCF_event, IIIMCF_component, IIIMCF_component );
IIIMF_status iiimccf_status( IIIMCF_context, IIIMCF_event, IIIMCF_component, IIIMCF_component );
IIIMF_status iiimccf_commit( IIIMCF_context, IIIMCF_event, IIIMCF_component, IIIMCF_component );
IIIMF_status iiimccf_event_key( IIIMCF_context, IIIMCF_event, IIIMCF_component, IIIMCF_component );
IIIMF_status iiimccf_trigger_notify( IIIMCF_context, IIIMCF_event, IIIMCF_component, IIIMCF_component );
IIIMF_status iiimccf_aux( IIIMCF_context, IIIMCF_event, IIIMCF_component, IIIMCF_component );

/** Preedit Object **/
class Prdt
{
  public:
    Prdt( IIIMCF_context context );
    ~Prdt(){};

    void info(); // for text-mode checking.

    void show();
    void hide();
    bool update();
    bool position(int x, int y);
  private:
    bool draw();

    IIIMCF_context context;
    vector<IIIMP_card16> buf_utf16;
    bool visible;
    int cur_x, cur_y;
    int cur_pos;
    Text* prdt_text;
};


/** LookupChoice Object **/
class Lkc
{
  public:
    Lkc( IIIMCF_context context );
    ~Lkc(){};

    void info(); // for text-mode checking.

    void show();
    void hide();
    bool update();
    bool position(int x, int y);
  private:
    bool draw();

    IIIMCF_context context;
    bool visible;
    int cur_x, cur_y;
    int cur_idx;
    Text* lkc_text;
};
/** Utilities functions **/

void check(IIIMF_status st);
void debug( char *str );
void mesg( char *str );
int wchar_to_utf8( wchar_t c, char * outbuf, int bufsize);
bool  get_committed_text();
char* iiimcf_text_to_utf8( IIIMCF_text t);
char* iiimcf_string_to_utf8( const IIIMP_card16 *pu16 );
void show_lookup_choice( IIIMCF_context context );
void show_preedit_info( IIIMCF_context context );
void show_incoming_event( IIIMCF_event ev );
vector<IIIMP_card16> text_to_vector( IIIMCF_text t );
