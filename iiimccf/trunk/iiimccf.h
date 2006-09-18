#include <iiimcf.h>
#include "imf.h"


class IIIMCCF : public Imf {
  public:
    IIIMCCF();
    ~IIIMCCF();

    char* process_input( char* buf );
    bool switch_im();
   
  protected:
    // IIIMCF Object
    IIIMCF_handle 	handle;
    IIIMCF_context 	context;
    
    /* committed text */
    char* cmt_buf;
    int cmt_buf_len;

    /* current imput method id */
    int cur_ims_id;
    
};

