#include "imf.h"

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
    
    /* current imput method id */
    int cur_ims_id;
    
};

