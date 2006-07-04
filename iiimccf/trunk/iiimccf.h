#include "imf.h"
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

class IIIMCCF : public Imf {
  public:
    IIIMCCF();
    ~IIIMCCF();

    int process_input( char* buf );
   
    bool switch_im();
    
    void update_cursor_position( int, int );
   
  protected:
    // IIIMCF Object
    IIIMCF_handle 	handle;
    IIIMCF_context 	context;
    
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

    /* current imput method id */
    int cur_ims_id;
    
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


/** LookupChoice Object **/

/** Utilities functions **/

void check(IIIMF_status st);
void debug( char *str );
void mesg( char *str );
int wchar_to_utf8( wchar_t c, char * outbuf, int bufsize);
bool  get_committed_text( IIIMCF_context, IIIMCCF& );
char* iiimcf_text_to_utf8( IIIMCF_text t);
char* iiimcf_string_to_utf8( const IIIMP_card16 *pu16 );
void show_incoming_event( IIIMCF_event ev );
vector<IIIMP_card16> text_to_vector( IIIMCF_text t );
