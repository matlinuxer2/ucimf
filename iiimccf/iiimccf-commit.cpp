#include "iiimccf-int.h"
#include <iostream>
using namespace std;

/* 
 * Commit
 */
void cmt_update(IIIMCF_context);

IIIMF_status
iiimccf_commit(
    IIIMCF_context context,
    IIIMCF_event event,
    IIIMCF_component current,
    IIIMCF_component parent
){
    //Cmt cmt( context );
    IIIMF_status st;
    IIIMCF_event_type type;
    st = iiimcf_get_event_type( event, &type );
    if( st != IIIMF_STATUS_SUCCESS ) return st;

    if ((type >= IIIMCF_EVENT_TYPE_UI_COMMIT)
	    && (type < IIIMCF_EVENT_TYPE_UI_COMMIT_END))
    {
	switch( type ){	
	  case IIIMCF_EVENT_TYPE_UI_COMMIT: 
	      debug("commit");
	      //cmt.update();
	      cmt_update(context);
	      break;
		  
	  case IIIMCF_EVENT_TYPE_UI_COMMIT_END:
	      debug("commit_end");
	      break;
	      
	  default: 
	      debug(" !!commit!! ");
	      cout << " !! " ;
	      cout.setf( ios_base::hex, ios_base::basefield );
	      cout << type;
	      cout << " !! " << endl;
	      break;	
	}
	return IIIMF_STATUS_SUCCESS;
    }

    return IIIMF_STATUS_COMPONENT_INDIFFERENT;
}

Cmt::Cmt( IIIMCF_context c)
{
  context = c;
}

//void Cmt::update()
void cmt_update( IIIMCF_context ctx )
{

  IIIMF_status st;
  IIIMCF_text text;
  
  st = iiimcf_get_committed_text( ctx, &text );
  
  cout << "=====================================================pppp===" << endl;
  if( st != IIIMF_STATUS_SUCCESS )
  { 
    iiimccf->cmt_buf_len = 0;
  }
  else
  {
    iiimccf->cmt_buf = iiimcf_text_to_utf8( text );
    iiimccf->cmt_buf_len = strlen( iiimccf->cmt_buf );
    cout << "=====================================================yyyy===" << endl;
  }

}

