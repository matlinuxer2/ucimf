#include <iiimcf.h>
#include "imf.h"
#include <string>
using std::string;


class IIIMCCF : public Imf {
  public:
    static Imf* getInstance();

    string process_input( const string& input );
    void switch_lang();
    void switch_im();
    void switch_im_per_lang();

  protected:
    IIIMF_status IIIMCCF::dispatch_event(IIIMCF_context context, IIIMCF_event event);
    IIIMF_status IIIMCCF::iiimccf_preedit(IIIMCF_context context, IIIMCF_event event);
    IIIMF_status IIIMCCF::iiimccf_lookup_choice(IIIMCF_context context, IIIMCF_event event);
    IIIMF_status IIIMCCF::iiimccf_commit(IIIMCF_context context, IIIMCF_event event);
    IIIMF_status IIIMCCF::iiimccf_status(IIIMCF_context context, IIIMCF_event event);
    
  protected:
    IIIMCCF();
    ~IIIMCCF();
    static Imf* _instance;
    
    // IIIMCF Object
    IIIMCF_handle 	handle;
    IIIMCF_context 	context;
    
    /* committed text */
    string cmt_buf;

    /* for input methods */
    int cur_ims_id; // current imput method id 
    int cur_pos; // current preedit position 
    int cur_idx; // current lookupchoice index 
    int num_of_ims; // number of ims

    IIIMCF_input_method *pims; // set of input methods.
    
};

