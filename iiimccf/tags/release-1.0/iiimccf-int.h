#include "iiimcf.h"
#include "graphdev.h"
#include <cstdlib>
#include <vector>
using namespace std;

class Prdt;
class Lkc;
class TrackPoint;
class Stts;
class Text;
class Rectangle;
class BitMap;

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
    bool ims_chg();
    
    void pos( int, int );
    void refresh();
    
    /* committed text */
    char* cmt_buf;
    int cmt_buf_len;

    /* current cursor position */
    int x,y;
    TrackPoint* trkpt;
    

    /* pointers to overspot elements */
    Stts *stts;
    Prdt *prdt;
    Lkc  *lkc;
    
    /* bitmap block for overspot elements */

//  private:

    // IIIMCF Object
    IIIMCF_handle 	handle;
    IIIMCF_context 	context;
   
    /* current imput method id */
    int cur_ims_id;
    
    /* current screen infomation */
    int height, width, resolution;
 
};

extern IIIMCCF* iiimccf;

/** Registed component functions **/
IIIMF_status iiimccf_preedit( IIIMCF_context, IIIMCF_event, IIIMCF_component, IIIMCF_component );
IIIMF_status iiimccf_lookup_choice( IIIMCF_context, IIIMCF_event, IIIMCF_component, IIIMCF_component );
IIIMF_status iiimccf_status( IIIMCF_context, IIIMCF_event, IIIMCF_component, IIIMCF_component );
IIIMF_status iiimccf_commit( IIIMCF_context, IIIMCF_event, IIIMCF_component, IIIMCF_component );
IIIMF_status iiimccf_event_key( IIIMCF_context, IIIMCF_event, IIIMCF_component, IIIMCF_component );
IIIMF_status iiimccf_trigger_notify( IIIMCF_context, IIIMCF_event, IIIMCF_component, IIIMCF_component );
IIIMF_status iiimccf_aux( IIIMCF_context, IIIMCF_event, IIIMCF_component, IIIMCF_component );

/** Preedit Object **/


/** LookupChoice Object **/

/** Utilities functions **/

void check(IIIMF_status st);
void debug( char *str );
void mesg( char *str );
int wchar_to_utf8( wchar_t c, char * outbuf, int bufsize);
bool  get_committed_text( IIIMCF_context, IIIMCCF& );
char* iiimcf_text_to_utf8( IIIMCF_text t);
char* iiimcf_string_to_utf8( const IIIMP_card16 *pu16 );
void show_lookup_choice( IIIMCF_context context );
void show_preedit_info( IIIMCF_context context );
void show_incoming_event( IIIMCF_event ev );
vector<IIIMP_card16> text_to_vector( IIIMCF_text t );
