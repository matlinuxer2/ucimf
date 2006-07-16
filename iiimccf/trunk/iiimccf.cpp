#include "iiimccf.h"
#include <iostream>
#include <ctime>
#include <cstring>
#include "widget.h"

using namespace std;

#define CONVERT_BUFSIZE 48


/*
 * Definition of utilities functions.
 */
void mesg( char *str ){
  cout << "msg:" << str << endl;
}

void debug( char *str ){
  cout << "err:" << str << endl;
}


int wchar_to_utf8(wchar_t c, char * outbuf, int bufsize)
{
	unsigned int len = 0;
	int first, i;

	if(c < 0x80) {
		first = 0;
		len = 1;
	}
	else if(c < 0x800) {
		first = 0xc0;
		len = 2;
	}
	else if(c < 0x10000) {
		first = 0xe0;
		len = 3;
	}
	else if(c < 0x200000) {
		first = 0xf0;
		len = 4;
	}
	else if(c < 0x4000000) {
		first = 0xf8;
		len = 5;
	}
	else {
		first = 0xfc;
		len = 6;
	}

	if(len > bufsize)
		return 0;

	if(outbuf) {
		for(i = len - 1; i > 0; --i) {
			outbuf[i] = (c & 0x3f) | 0x80;
			c >>= 6;
		}
		outbuf[0] = c | first;
	}

	return len;
}

bool  get_committed_text(IIIMCF_context context, IIIMCCF& iiimccf)
{
	IIIMF_status st;
	IIIMCF_text text;
	
	iiimcf_get_committed_text( context , &text);
	//iiimcf_get_text_length( text, buf_len );

	char* buf_utf8 = iiimcf_text_to_utf8( text );
	int buf_utf8_len = strlen( buf_utf8 );
	
	return 0;
}


char* iiimcf_text_to_utf8( IIIMCF_text t)
{
	IIIMF_status st;
	char *p, *pr, *pr2;
	int i, j, n, l, rest, nfb, initlen = 256;
	IIIMP_card16 ch;
	const IIIMP_card32 *pids, *pfbs;
	char utf8buf[CONVERT_BUFSIZE] = {0, };

	pr = p = (char *)malloc(sizeof(*p) * initlen);
	rest = initlen - 1;

	st = iiimcf_get_text_length(t, &n);
	if(st != IIIMF_STATUS_SUCCESS)
		return 0;

	for(i = 0; i < n; i++) {
		st = iiimcf_get_char_with_feedback(t, i, &ch, &nfb,
							&pids, &pfbs);
		if(st != IIIMF_STATUS_SUCCESS)
			return 0;

		if(rest <= (16 + 2 + 20 * nfb)) {
			rest += initlen;
			initlen *= 2;
			pr2 = (char *)realloc(pr, sizeof(*p) * initlen);
			p = pr2 + (p - pr);
			pr = pr2;
		}
		if(ch < 128) {
			*p++ = ch;
			rest--;
		}
		else {
			wchar_to_utf8((wchar_t)ch, utf8buf, CONVERT_BUFSIZE);
			l = snprintf(p, CONVERT_BUFSIZE, "%s", utf8buf);
			p += l;
			rest -= l;
		}
	}
	*p = '\0';

	return pr;
}


char* iiimcf_string_to_utf8( const IIIMP_card16 *pu16 )
{
    char *p, *pr, *pr2;
    int i, rest;
    int initlen = 256;

    pr = (char*) malloc(sizeof(*p) * initlen);
    rest = initlen - 1;
    for (p = pr; *pu16; pu16++) {
	if (*pu16 < 128) {
	    if (rest < 1) {
		rest += initlen;
		initlen *= 2;
		pr2 = (char*) realloc(pr, sizeof(*p) * initlen);
		p = pr2 + (p - pr);
		pr = pr2;
	    }
	    *p++ = *pu16;
	    rest--;
	} else {
	    if (rest < 16) {
		rest += initlen;
		initlen *= 2;
		pr2 = (char*) realloc(pr, sizeof(*p) * initlen);
		p = pr2 + (p - pr);
		pr = pr2;
	    }
	    i = snprintf(p, 15, " U+%.4X", *pu16);
	    p += i;
	    rest -= i ;
	}
    }
    *p = '\0';

    return pr;
}


void check( IIIMF_status st )
{
  switch( st )
  {
    case IIIMF_STATUS_FAIL	 :	
      cout << "[st: fail ]" << endl;
      break;
    case IIIMF_STATUS_SUCCESS	 :	
      cout << "[st: sucess ]" << endl;
      break;
    
    case IIIMF_STATUS_MALLOC	 :	
      cout << "[st: malloc ]" << endl;
      break;
    case IIIMF_STATUS_ARGUMENT	 :	
      cout << "[st: argument ]" << endl;
      break;
    case IIIMF_STATUS_PROTOCOL_VERSION :	
      cout << "[st: protocal_version ]" << endl; 
      break;
    
    case IIIMF_STATUS_CONFIG	 :	
      cout << "[st: config ]" << endl;
      break;
    case IIIMF_STATUS_ROLE	 :	
      cout << "[st: role ]" << endl;
      break;
    
    case IIIMF_STATUS_OPCODE	 :	
      cout << "[st: opcode ]" << endl;
      break;
    
    case IIIMF_STATUS_SEQUENCE_REQUEST :	
      cout << "[st: sequence_request ]" << endl;
      break;
    case IIIMF_STATUS_SEQUENCE_REPLY :	
      cout << "[st: sequence reply ]" << endl;
      break;
    case IIIMF_STATUS_SEQUENCE_ROLE :	
      cout << "[st: sequence role ]" << endl;
      break;
    case IIIMF_STATUS_SEQUENCE_STATE :	
      cout << "[st:  sequence state ]" << endl;
      break;
    case IIIMF_STATUS_SEQUENCE_NEST :	
      cout << "[st: sequence_nest ]" << endl;
      break;
    
    case IIIMF_STATUS_IM_INVALID	 :	
      cout << "[st: im invalid ]" << endl;
      break;
    case IIIMF_STATUS_IC_INVALID	 :	
      cout << "[st: ic invalid ]" << endl;
      break;
    
    case IIIMF_STATUS_STREAM	 :	
      cout << "[st:  ]" << endl;
      break;
    case IIIMF_STATUS_STREAM_SEND :	
      cout << "[st:  ]" << endl;
      break;
    case IIIMF_STATUS_STREAM_RECEIVE :	
      cout << "[st:  ]" << endl;
      break;
    case IIIMF_STATUS_PACKET	 :	
      cout << "[st:  ]" << endl;
      break;
    case IIIMF_STATUS_INVALID_ID	 :	
      cout << "[st:  ]" << endl;
      break;
    case IIIMF_STATUS_TIMEOUT	 :	
      cout << "[st:  ]" << endl;
      break;
    case IIIMF_STATUS_CONNECTION_CLOSED :        
      cout << "[st:  ]" << endl;
      break;
    
    case IIIMF_STATUS_IIIMCF_START :	
      cout << "[st: iiimcf_start ]" << endl;
      break;
    case IIIMF_STATUS_NO_ATTR_VALUE :	
      cout << "[st: no_attr_value ]" << endl;
      break;
    case IIIMF_STATUS_NO_TEXT_PROPERTY :	
      cout << "[st: no_text_property ]" << endl;
      break;
    case IIIMF_STATUS_NO_EVENT	 :	
      cout << "[st: no event ]" << endl;
      break;
    case IIIMF_STATUS_NO_PREEDIT	 :	
      cout << "[st: no preedit ]" << endl;
      break;
    case IIIMF_STATUS_NO_LOOKUP_CHOICE :	
      cout << "[st: no lookup choice ]" << endl;
      break;
    case IIIMF_STATUS_NO_STATUS_TEXT :	
      cout << "[st: no status text ]" << endl;
      break;
    case IIIMF_STATUS_NO_COMMITTED_TEXT :	
      cout << "[st: no committed text ]" << endl;
      break;
    case IIIMF_STATUS_CLIENT_RESET_BY_PEER:	
      cout << "[st: client reset by peer ]" << endl;
      break;
    case IIIMF_STATUS_INVALID_EVENT_TYPE :	
      cout << "[st: invalid event type ]" << endl;
      break;
    case IIIMF_STATUS_EVENT_NOT_FORWARDED:	
      cout << "[st: event not forwarded ]" << endl;
      break;
    case IIIMF_STATUS_COMPONENT_DUPLICATED_NAME:	
      cout << "[st: component duplicated ]" << endl;
      break;
    case IIIMF_STATUS_COMPONENT_FAIL :	
      cout << "[st: component_fail ]" << endl;
      break;
    case IIIMF_STATUS_NO_COMPONENT :	
      cout << "[st: no component ]" << endl;
      break;
    case IIIMF_STATUS_STATIC_EVENT_FLOW :	
      cout << "[st: static event flow ]" << endl;
      break;
    case IIIMF_STATUS_FAIL_TO_EVENT_DISPATCH:	
      cout << "[st: fail to event dispatch ]" << endl;
      break;
    case IIIMF_STATUS_NO_AUX	 :	
      cout << "[st: no aux ]" << endl;
      break;
    case IIIMF_STATUS_NOT_TRIGGER_KEY :	
      cout << "[st: not trigger key ]" << endl;
      break;
    case IIIMF_STATUS_COMPONENT_INDIFFERENT:	
      cout << "[st: component indifferent ]" << endl;
      break;
    
    default:
      break;
  }
}

vector<IIIMP_card16> text_to_vector( IIIMCF_text text )
{
  int text_len;
  iiimcf_get_text_length( text, &text_len );
  
  vector<IIIMP_card16> output(text_len);
  IIIMP_card16 ch;
  int nfb;
  const IIIMP_card32 *pids, *pfbs;
  for( int i = 0; i< text_len; i++)
  {
    iiimcf_get_char_with_feedback( text, i, &ch, &nfb, &pids, &pfbs );
    output[i] = ch;
  }
  
  return output;
}



/* 
 * Definition of iiimcf_components
 */

IIIMF_status
iiimccf_preedit(
    IIIMCF_context context,
    IIIMCF_event event,
    IIIMCF_component current,
    IIIMCF_component parent
){

  IIIMF_status st;
  IIIMCF_event_type type;
  IIIMCF_text buf0;
  st = iiimcf_get_event_type( event, &type );
  if( st != IIIMF_STATUS_SUCCESS ) return st;
   
  switch( type ){
	  case IIIMCF_EVENT_TYPE_UI_PREEDIT:
		  mesg("preedit");
		  break;
		  
	  case IIIMCF_EVENT_TYPE_UI_PREEDIT_START:
		  mesg("preedit start");
		  iiimccf->prdt = new Prdt( context );
		  iiimccf->prdt->update();
		  iiimccf->prdt->show();
		  break;
		  
	  case IIIMCF_EVENT_TYPE_UI_PREEDIT_CHANGE:
		  mesg("preedit changed");
		  
		  IIIMP_card16* buf1;
		  int buf1_len;
		  vector<IIIMP_card16> buf_utf16(buf1_len);
		  IIIMP_card16 ch;
		  int nfb;
		  const IIIMP_card32 *pids, *pfbs;
		  
		  st = iiimcf_get_preedit_text( context, &buf0 , &cur_pos);
		  if( st != IIIMF_STATUS_SUCCESS )
		  {
		    return false;
		  }

		  st = iiimcf_get_text_utf16string( buf0, ( const IIIMP_card16** ) &buf1 );
		  if( st != IIIMF_STATUS_SUCCESS )
		  {
		    return false;
		  }

		  st = iiimcf_get_text_length( buf0, &buf1_len ); 
		  if( st != IIIMF_STATUS_SUCCESS ) return false;

		  for( int i=0; i < buf1_len; i++ )
		  {
		    st = iiimcf_get_char_with_feedback( buf0, i, &ch, &nfb, &pids, &pfbs );
		    if( st != IIIMF_STATUS_SUCCESS ) return false;
		    buf_utf16[i] = ch;
		  }
		  prdt->append( buf_utf16 );

		  break;
		  
	  case IIIMCF_EVENT_TYPE_UI_PREEDIT_DONE:
		  mesg("preedit done");
		  iiimccf->prdt->hide();
		  iiimccf->prdt->empty();
		  break;
		  
	  case IIIMCF_EVENT_TYPE_UI_PREEDIT_END:
		  mesg("preedit end");
		  break;
		  
	  default:
		  mesg("preedit none");
		  break;
  }
  return IIIMF_STATUS_SUCCESS;
}

IIIMF_status
iiimccf_lookup_choice(
    IIIMCF_context context,
    IIIMCF_event event,
    IIIMCF_component current,
    IIIMCF_component parent
){

  IIIMF_status st;
  IIIMCF_event_type type;
  IIIMCF_lookup_choice ilc;

  st = iiimcf_get_event_type( event, &type );
  if( st != IIIMF_STATUS_SUCCESS ) return st;
  
  switch( type ){	
	  case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE: 
		  debug( "lookup" );
		  break;
		  
	  case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE_START:
		  debug( "lookup start" );
		  iiimccf->lkc = new Lkc( context );
		  iiimccf->lkc->update();
		  iiimccf->lkc->show();
		  break;
	  
	  case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE_CHANGE:
		  debug( "lookup change" );

		  st = iiimcf_get_lookup_choice( context, &ilc );
		  if( st != IIIMF_STATUS_SUCCESS )
		  {
		    // throw error
		    return false;
		  }
		  
		  int lkc_size, idx_first, idx_last;
		  st = iiimcf_get_lookup_choice_size( ilc, &lkc_size, &idx_first, &idx_last, &cur_idx );
		  if( st!= IIIMF_STATUS_SUCCESS ) return false;

		  for( int i=0; i< lkc_size; i++ )
		  { 
		    IIIMCF_text cand, label;
		    int flag;
		    iiimcf_get_lookup_choice_item( ilc, i, &cand, &label, &flag );
		    if( flag & IIIMCF_LOOKUP_CHOICE_ITEM_ENABLED )
		    {
		      lkc->append( text_to_vector(label) + text_to_vector(cand) );
		    }
		  }
		  
		  break;
	  
	  case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE_DONE:
		  debug( "lookup done" );
		  iiimccf->lkc->hide();
		  iiimccf->lkc->empty();
		  break;

	  case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE_END: 
		  debug( "lookup end" );
		  break;
		  
	  default:
		  break;
  }
  
  return IIIMF_STATUS_SUCCESS;
}

IIIMF_status
iiimccf_commit(
    IIIMCF_context context,
    IIIMCF_event event,
    IIIMCF_component current,
    IIIMCF_component parent
){
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
	      //cmt_update(context);
	      IIIMCF_text text;
	      
	      st = iiimcf_get_committed_text( context, &text );
	      
	      if( st != IIIMF_STATUS_SUCCESS )
	      { 
		iiimccf->cmt_buf_len = 0;
	      }
	      else
	      {
		iiimccf->cmt_buf = iiimcf_text_to_utf8( text );
		iiimccf->cmt_buf_len = strlen( iiimccf->cmt_buf );
	      }
	      break;
		  
	  case IIIMCF_EVENT_TYPE_UI_COMMIT_END:
	      debug("commit_end");
	      break;
	      
	  default: 
	      debug(" !!commit!! ");
	      cout << " !! " ;
	      cout.setf( ios_base::hex, ios_base::basefield );
	      cout << type;
	      cout.setf( ios_base::dec, ios_base::basefield );
	      cout << " !! " << endl;
	      break;	
	}
	return IIIMF_STATUS_SUCCESS;
    }

    return IIIMF_STATUS_COMPONENT_INDIFFERENT;
}

IIIMF_status
iiimccf_status(
    IIIMCF_context context,
    IIIMCF_event event,
    IIIMCF_component current,
    IIIMCF_component parent
){
	IIIMCF_text text; 
	IIIMF_status st;
	IIIMCF_event_type type;
	st = iiimcf_get_event_type( event, &type );
	if( st != IIIMF_STATUS_SUCCESS ) return st;
	
	switch( type ){	
		case IIIMCF_EVENT_TYPE_UI_STATUS:
		  break;
		  
		case IIIMCF_EVENT_TYPE_UI_STATUS_START:
		  break;
		  
		case IIIMCF_EVENT_TYPE_UI_STATUS_CHANGE:
		  
		  st = iiimcf_get_status_text(context, &text);
		  if (st == IIIMF_STATUS_SUCCESS) {
		      char* str = iiimcf_text_to_utf8(text);
		      debug("Status:");
		      debug( str );

		  } else if (st == IIIMF_STATUS_NO_STATUS_TEXT) {
		      debug( "Status is disabled." );
		  } else {
		      debug( "Status error." );
		  }

		  break;
		  
		case IIIMCF_EVENT_TYPE_UI_STATUS_DONE:
		  break;
		  
		case IIIMCF_EVENT_TYPE_UI_STATUS_END:
		  break;
		  
		default: 
		  debug(" !!status2!! "); 
		  
		  st = iiimcf_get_status_text(context, &text);
		  if (st == IIIMF_STATUS_SUCCESS) {
		      char* str = iiimcf_text_to_utf8(text);
		      debug("Status:");
		      debug( str );

		  } else if (st == IIIMF_STATUS_NO_STATUS_TEXT) {
		      debug( "Status is disabled." );
		  } else {
		      debug( "Status error." );
		  }

		  break; 
	}
	return IIIMF_STATUS_SUCCESS;
}




IIIMF_status regist_component( IIIMCF_handle handle )
{
	IIIMCF_component parent, child;
	IIIMF_status st;

	/*  Register new component */
	st = iiimcf_get_component( handle,"org.OpenI18N.IIIMCF.UI.preedit",&parent);
	if( st != IIIMF_STATUS_SUCCESS ) return st;
	st = iiimcf_register_component( handle,"iiimccf-preedit",iiimccf_preedit,parent,&child);
	if( st != IIIMF_STATUS_SUCCESS ) return st;
         
	st = iiimcf_get_component( handle,"org.OpenI18N.IIIMCF.UI.status",&parent);
	if( st != IIIMF_STATUS_SUCCESS ) return st;
	st = iiimcf_register_component( handle,"iiimccf-status",iiimccf_status,parent,&child);
	if( st != IIIMF_STATUS_SUCCESS ) return st;
        
	st = iiimcf_get_component( handle,"org.OpenI18N.IIIMCF.UI.lookup_choice",&parent);
	if( st != IIIMF_STATUS_SUCCESS ) return st;
	st = iiimcf_register_component( handle,"iiimccf-lookup_choice",iiimccf_lookup_choice,parent,&child);
	if( st != IIIMF_STATUS_SUCCESS ) return st;

	// st = iiimcf_get_component( handle,"org.OpenI18N.IIIMCF.UI.commit",&parent);
	st = iiimcf_get_component( handle,"org.OpenI18N.IIIMCF.UI",&parent);
	if( st != IIIMF_STATUS_SUCCESS ) return st;
	st = iiimcf_register_component( handle,"iiimccf-commit",iiimccf_commit,parent,&child);
	if( st != IIIMF_STATUS_SUCCESS ) return st;

}



/*
 * Implementation of IIIMCCF
 */
IIIMCCF::IIIMCCF()
{
	IIIMF_status st;
    	IIIMCF_attr attr;

    	st = iiimcf_initialize( IIIMCF_ATTR_NULL );
    	st = iiimcf_create_attr(&attr);
    	st = iiimcf_attr_put_string_value(attr, IIIMCF_ATTR_CLIENT_TYPE,
	                                      "IIIM Console Client Framework");
	st = iiimcf_create_handle(attr, &handle );
    	st = iiimcf_destroy_attr(attr);

	check( regist_component(handle) );
        
	/*
	cur_ims_id = 0;
    
	IIIMCF_input_method *pims;
	const IIIMP_card16 *u16idname, *u16hrn, *u16domain;
	char *idname, *hrn, *domain;
	int num_of_ims;

	st = iiimcf_get_supported_input_methods(handle, &num_of_ims, &pims);
	iiimcf_get_input_method_desc(pims[cur_ims_id], &u16idname, &u16hrn, &u16domain);
	idname = iiimcf_string_to_utf8(u16idname);
	*/

	st = iiimcf_create_attr( &attr );
	st = iiimcf_attr_put_string_value( attr, IIIMCF_ATTR_INPUT_METHOD_NAME, idname );
	
	st = iiimcf_create_context( handle,attr, &context );
    
	IIIMCF_event event;
	iiimcf_create_trigger_notify_event( 1, &event);
	iiimcf_forward_event( context, event);
	
	trkpt = new TrackPoint;
	stts = new Stts;
	prdt = new Prdt;
	lkc = new Lkc;
	trkpt->attach( stts );
}

IIIMCCF::~IIIMCCF()
{
  delete trkpt;
  delete stts;
	IIIMCF_event event;
	iiimcf_create_trigger_notify_event( 0, &event );
	iiimcf_forward_event( context, event );

	iiimcf_destroy_handle( handle );
	iiimcf_finalize();  
}

bool IIIMCCF::on()
{
    IIIMF_status st;
	
    // IIIMCF_event event;
    // iiimcf_create_trigger_notify_event( 1, &event);
    // iiimcf_forward_event( context, event);
    
    stts->show();
    prdt->show();
    lkc->show();
    
    return true;
}

bool IIIMCCF::off()
{
        stts->hide();
	prdt->hide();
	lkc->hide();

	// IIIMCF_event event;
	// iiimcf_create_trigger_notify_event( 0, &event );
	// iiimcf_forward_event( context, event );

	return true;
}

void stdin_to_iiimcf_keyevent( char* buf, IIIMCF_keyevent& kev );
//int IIIMCCF::proc( int keycode, int keychar, int modifier )
int IIIMCCF::proc( char* buf_input )
{
	IIIMCF_event event,ev ;
	IIIMCF_keyevent kev;
	IIIMCF_event_type event_type;
  	stdin_to_iiimcf_keyevent( buf_input, kev );

	/* sent the keyevent to iiimcf */
	kev.keycode = keycode;
    	kev.keychar = keychar;
        kev.modifier = modifier;
    	kev.time_stamp = time( NULL );
        iiimcf_create_keyevent( &kev, &ev );
        iiimcf_forward_event( context, ev );

	/* get the reponse from the iiimcf */	
	while( iiimcf_get_next_event( context, &event ) == IIIMF_STATUS_SUCCESS ){
		
	    IIIMF_status st;
	    st = iiimcf_dispatch_event( context , event );
	    
	    if (st != IIIMF_STATUS_SUCCESS) {
			if (st == IIIMF_STATUS_COMPONENT_FAIL) {
			    debug("at least one component reported failure");
			} else if (st == IIIMF_STATUS_COMPONENT_INDIFFERENT) {
			    //debug("none of the components deal with the event");
			    IIIMCF_event_type event_type;
			    iiimcf_get_event_type( event, &event_type );
			    fprintf( stderr, "none of the components deal with the event: [[ %x ]]\n", event_type );
			} else {
			    debug("fail to dispatch");
			}
	    }
	    
	    iiimcf_ignore_event( event );

    	}
}

int IIIMCCF::result( char* *buf_out )
{
  int buf_len=0;
  if( cmt_buf_len > 0 )
  {
    (*buf_out) = cmt_buf ;
    buf_len=strlen( (*buf_out) );
    cmt_buf_len = 0;
    cmt_buf = 0 ;
  }

  return buf_len;
}


bool IIIMCCF::ims_chg()
{

    IIIMF_status st;
    IIIMCF_input_method *pims;
    const IIIMP_card16 *u16idname, *u16hrn, *u16domain;
    char *idname, *hrn, *domain;
    int num_of_ims;

    st = iiimcf_get_supported_input_methods(handle, &num_of_ims, &pims);
    
    if ( cur_ims_id == (num_of_ims - 1) )
    {  
      cur_ims_id =0;
    }
    else
    {
      cur_ims_id++;
    }

    iiimcf_get_input_method_desc(pims[cur_ims_id], &u16idname, &u16hrn, &u16domain);
    idname = iiimcf_string_to_utf8(u16idname);
    
    IIIMCF_attr attr;

    //off();
    IIIMCF_event event;
    iiimcf_create_trigger_notify_event( 0, &event );
    iiimcf_forward_event( context, event );
    iiimcf_destroy_context( context );
    
    st = iiimcf_create_attr( &attr );
    st = iiimcf_attr_put_string_value( attr, IIIMCF_ATTR_INPUT_METHOD_NAME, idname );
    st = iiimcf_create_context( handle,attr, &context );
    
    st = iiimcf_create_trigger_notify_event( 1, &event);
    st = iiimcf_forward_event( context, event);
    
    stts->update();

    return true;
}

bool IIIMCCF::ims_show()
{
    IIIMF_status st;
    IIIMCF_input_method *pims;
    IIIMCF_language *plangs;
    const IIIMP_card16 *u16idname, *u16hrn, *u16domain;
    char *idname, *hrn, *domain;
    const char *langid;
    int num_of_ims, num_of_langs;

    st = iiimcf_get_supported_input_methods(handle, &num_of_ims, &pims);

    int i;
    for (i = 0; i < num_of_ims ; i++) {
       iiimcf_get_input_method_desc(pims[i], &u16idname, &u16hrn, &u16domain);
       iiimcf_get_input_method_languages(pims[i], &num_of_langs, &plangs);

       idname = iiimcf_string_to_utf8(u16idname);
       hrn = iiimcf_string_to_utf8(u16hrn);
       domain = iiimcf_string_to_utf8(u16domain);
       fprintf(stderr,
                "Input Method %d ----------------------------------\n"
                "  idname: %s\n"
                "  HRN: %s\n"
                "  domain: %s\n"
                "  langs: ",
                i, idname, hrn, domain);

       		int j;
	        for (j = 0; j < num_of_langs; j++) {
	              iiimcf_get_language_id(plangs[j], &langid);
	              fprintf(stderr, "%s, ", langid);
	        }
	 fprintf( stderr, "\n" );
	 free(idname);
	 free(hrn);
	 free(domain);
    }
    return true;
}

bool IIIMCCF::ims_set ( )
{
  IIIMF_status st;
  IIIMCF_attr attr;

  st = iiimcf_create_attr( &attr );
  
  st = iiimcf_attr_put_string_value( attr, IIIMCF_ATTR_INPUT_METHOD_NAME, "newpy");
  /* libiiimcf.a doesn't define the iiimcf_icsetvalues function*/
  //st = iiimcf_seticvalues( context, attr );
  
  st = iiimcf_destroy_attr( attr );
  
  return true;
}

void IIIMCCF::update_cursor_position( int new_x, int new_y )
{
  cout << "change position " << endl;
  x = new_x;
  y = new_y;
  trkpt->set_position( new_x, new_y );
}

