/*
 * UCIMF - Unicode Console InputMethod Framework
 * Copyright (C) <2006>  Chun-Yu Lee (Mat) <Matlinuxer2@gmail.com> 
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <iostream>
#include <ctime>
#include <cstring>
#include <string>
#include "iiimccf.h"
#include <imf/widget.h>
using namespace std;


/*
 * Definition of utilities functions.
 */
#define CONVERT_BUFSIZE 48

void debug( char *str ){
  cerr << "err:" << str << endl;
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
 * Implementation of IIIMCCF
 */

Imf* IIIMCCF::_instance = 0;

Imf* IIIMCCF::getInstance()
{
  if( _instance == 0 )
    _instance = new IIIMCCF;

  return _instance;
}

//Status* IIIMCCF::stts = Status::getInstance();

IIIMCCF::IIIMCCF()
{
    IIIMF_status st;
    IIIMCF_attr attr;
    IIIMCF_event event;
    Status *stts = Status::getInstance();
    cur_ims_id = 0;
    const IIIMP_card16 *u16idname, *u16hrn, *u16domain;
    char *hrn, *domain;

    st = iiimcf_initialize( IIIMCF_ATTR_NULL );
    st = iiimcf_create_attr(&attr);
    st = iiimcf_attr_put_string_value(attr, IIIMCF_ATTR_CLIENT_TYPE,
					  "IIIM Console Client Framework");
    st = iiimcf_create_handle(attr, &handle );
    st = iiimcf_destroy_attr(attr);


    st = iiimcf_get_supported_input_methods(handle, &num_of_ims, &pims);
    iiimcf_get_input_method_desc(pims[cur_ims_id], &u16idname, &u16hrn, &u16domain);
    cur_id_name = iiimcf_string_to_utf8(u16idname);

    st = iiimcf_create_attr( &attr );
    st = iiimcf_attr_put_string_value( attr, IIIMCF_ATTR_INPUT_METHOD_NAME, cur_id_name );
    
    st = iiimcf_create_context( handle,attr, &context );

    iiimcf_create_trigger_notify_event( 1, &event);
    iiimcf_forward_event( context, event);

    stts->set_im_name(cur_id_name);
	
}


IIIMCCF::~IIIMCCF()
{
	
    IIIMCF_event event;
    iiimcf_create_trigger_notify_event( 0, &event );
    iiimcf_forward_event( context, event );

    iiimcf_destroy_handle( handle );
    iiimcf_finalize();  
}


void IIIMCCF::refresh()
{
  Status *stts = Status::getInstance();
  stts->set_imf_name("IIIMF");
  stts->set_im_name( cur_id_name );
}


void IIIMCCF::switch_lang()
{
  switch_im();
}

void IIIMCCF::switch_im()
{
    Status *stts = Status::getInstance();
    Preedit *prdt = Preedit::getInstance();
    LookupChoice *lkc = LookupChoice::getInstance();

    IIIMF_status st;
    IIIMCF_attr attr;
    IIIMCF_event event;
    const IIIMP_card16 *u16idname, *u16hrn, *u16domain;
    char *hrn, *domain;

    
    if ( cur_ims_id >= (num_of_ims - 1) )
    {  
      cur_ims_id =0;
    }
    else
    {
      cur_ims_id++;
    }

    iiimcf_get_input_method_desc(pims[cur_ims_id], &u16idname, &u16hrn, &u16domain);
    cur_id_name = iiimcf_string_to_utf8(u16idname);
    

    // destruct
    iiimcf_create_trigger_notify_event( 0, &event );
    iiimcf_forward_event( context, event );
    iiimcf_destroy_context( context );
   
    // construct
    st = iiimcf_create_attr( &attr );
    st = iiimcf_attr_put_string_value( attr, IIIMCF_ATTR_INPUT_METHOD_NAME, cur_id_name );
    st = iiimcf_create_context( handle,attr, &context );
    st = iiimcf_create_trigger_notify_event( 1, &event);
    st = iiimcf_forward_event( context, event);
    
    refresh();
    prdt->clear();
    lkc->clear();
}

void IIIMCCF::switch_im_per_lang()
{


}


static const int stdin_to_iiimf_keycode[] = {
  // ascii code 00-0
	IIIMF_KEYCODE_UNDEFINED, 
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_TAB,	 

        // 10-19
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_CLEAR,
	IIIMF_KEYCODE_ENTER,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,

  	// 20-29
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_ESCAPE,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,

        // 30-39
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_SPACE,
	IIIMF_KEYCODE_EXCLAMATION_MARK,
	IIIMF_KEYCODE_QUOTEDBL,
	IIIMF_KEYCODE_NUMBER_SIGN,
	IIIMF_KEYCODE_DOLLAR,
	IIIMF_KEYCODE_UNDEFINED, 		/* % */
	IIIMF_KEYCODE_AMPERSAND,
	IIIMF_KEYCODE_QUOTE,

	// 40-49
	IIIMF_KEYCODE_LEFT_PARENTHESIS,
	IIIMF_KEYCODE_RIGHT_PARENTHESIS,
	IIIMF_KEYCODE_ASTERISK,
	IIIMF_KEYCODE_PLUS,
	IIIMF_KEYCODE_COMMA,
	IIIMF_KEYCODE_MINUS,
	IIIMF_KEYCODE_PERIOD,
	IIIMF_KEYCODE_SLASH,
	IIIMF_KEYCODE_0,
	IIIMF_KEYCODE_1,

	// 50-59
	IIIMF_KEYCODE_2,
	IIIMF_KEYCODE_3,
	IIIMF_KEYCODE_4,
	IIIMF_KEYCODE_5,
	IIIMF_KEYCODE_6,
	IIIMF_KEYCODE_7,
	IIIMF_KEYCODE_8,
	IIIMF_KEYCODE_9,
	IIIMF_KEYCODE_COLON,
	IIIMF_KEYCODE_SEMICOLON,

	// 60-69
	IIIMF_KEYCODE_LESS,
	IIIMF_KEYCODE_EQUALS,
	IIIMF_KEYCODE_GREATER,
	IIIMF_KEYCODE_UNDEFINED,		/* ? */
	IIIMF_KEYCODE_AT,
	IIIMF_KEYCODE_A,
	IIIMF_KEYCODE_B,
	IIIMF_KEYCODE_C,
	IIIMF_KEYCODE_D,
	IIIMF_KEYCODE_E,

	// 70-79
	IIIMF_KEYCODE_F,
	IIIMF_KEYCODE_G,
	IIIMF_KEYCODE_H,
	IIIMF_KEYCODE_I,
	IIIMF_KEYCODE_J,
	IIIMF_KEYCODE_K,
	IIIMF_KEYCODE_L,
	IIIMF_KEYCODE_M,
	IIIMF_KEYCODE_N,
	IIIMF_KEYCODE_O,

	// 80-89
	IIIMF_KEYCODE_P,
	IIIMF_KEYCODE_Q,
	IIIMF_KEYCODE_R,
	IIIMF_KEYCODE_S,
	IIIMF_KEYCODE_T,
	IIIMF_KEYCODE_U,
	IIIMF_KEYCODE_V,
	IIIMF_KEYCODE_W,
	IIIMF_KEYCODE_X,
	IIIMF_KEYCODE_Y,

	// 90-99
	IIIMF_KEYCODE_Z,
	IIIMF_KEYCODE_OPEN_BRACKET,
	IIIMF_KEYCODE_BACK_SLASH,
	IIIMF_KEYCODE_CLOSE_BRACKET,
	IIIMF_KEYCODE_CIRCUMFLEX,
	IIIMF_KEYCODE_UNDERSCORE,
	IIIMF_KEYCODE_BACK_QUOTE,
	IIIMF_KEYCODE_A,
	IIIMF_KEYCODE_B,
	IIIMF_KEYCODE_C,

	// 100-109
	IIIMF_KEYCODE_D,
	IIIMF_KEYCODE_E,
	IIIMF_KEYCODE_F,
	IIIMF_KEYCODE_G,
	IIIMF_KEYCODE_H,
	IIIMF_KEYCODE_I,
	IIIMF_KEYCODE_J,
	IIIMF_KEYCODE_K,
	IIIMF_KEYCODE_L,
	IIIMF_KEYCODE_M,

	// 110-119
	IIIMF_KEYCODE_N,
	IIIMF_KEYCODE_O,
	IIIMF_KEYCODE_P,
	IIIMF_KEYCODE_Q,
	IIIMF_KEYCODE_R,
	IIIMF_KEYCODE_S,
	IIIMF_KEYCODE_T,
	IIIMF_KEYCODE_U,
	IIIMF_KEYCODE_V,
	IIIMF_KEYCODE_W,

	// 120-126
	IIIMF_KEYCODE_X,
	IIIMF_KEYCODE_Y,
	IIIMF_KEYCODE_Z,
	IIIMF_KEYCODE_BRACELEFT,
	IIIMF_KEYCODE_UNDEFINED,		/* | */
	IIIMF_KEYCODE_BRACERIGHT,
	IIIMF_KEYCODE_DELETE,		/* ~ */
	IIIMF_KEYCODE_BACK_SPACE,		/* BACKSPACE */
};

void stdin_to_iiimcf_keyevent( const string& buf, IIIMCF_keyevent& kev )
{
  int buf_len = buf.size();
  int keychar,keycode,modifier;

  if( buf_len == 1 )
  {
    keychar = (int)buf[0];
    keycode = stdin_to_iiimf_keycode[keychar];
    modifier = 0; // default as zero.
  }
  else if( buf_len == 3 && buf[0]==27 && buf[1]==91 )
  {
    switch( buf[2] )
    {
      case 65:
	keychar = 0;
	keycode = IIIMF_KEYCODE_UP;
	modifier = 0; // default as zero.
	break;

      case 66:
	keychar = 0;
	keycode = IIIMF_KEYCODE_DOWN;
	modifier = 0; // default as zero.
	break;
	
      case 67:
	keychar = 0;
	keycode = IIIMF_KEYCODE_RIGHT;
	modifier = 0; // default as zero.
	break;
	
      case 68:
	keychar = 0;
	keycode = IIIMF_KEYCODE_LEFT;
	modifier = 0; // default as zero.
	break;

      default:
	break;
    }
  }
  else if( buf_len == 4 && buf[0]==27 && buf[1]==91 && buf[4]==126 )
  {
    switch( buf[3] )
    {
      case 53:
	keychar = 0;
	keycode = IIIMF_KEYCODE_PAGE_UP;
	modifier = 0; // default as zero.
	break;
	
      case 54:
	keychar = 0;
	keycode = IIIMF_KEYCODE_PAGE_DOWN;
	modifier = 0; // default as zero.
	break;
      default:
	break;
    }
  }
  else
  {
    keychar = 0;
    keycode = 0;
    modifier = 0; // default as zero.
  }
  
  kev.keycode = keycode;
  kev.keychar = keychar;
  kev.modifier = modifier;
  kev.time_stamp = time( NULL );

}


IIIMF_status
IIIMCCF::iiimccf_preedit(
    IIIMCF_context context,
    IIIMCF_event event
){
  Preedit *prdt = Preedit::getInstance();

  IIIMF_status st;
  IIIMCF_event_type type;
  
  IIIMCF_text buf0;
  vector<IIIMP_card16> buf_utf16;
  IIIMP_card16* buf1;
  int buf1_len=0;
  IIIMP_card16 ch;
  int nfb;
  const IIIMP_card32 *pids, *pfbs;

  st = iiimcf_get_event_type( event, &type );
  if( st != IIIMF_STATUS_SUCCESS ) return st;
  
  if( type < IIIMCF_EVENT_TYPE_UI_PREEDIT || type > IIIMCF_EVENT_TYPE_UI_PREEDIT_END )
    return IIIMF_STATUS_COMPONENT_INDIFFERENT;
  
 
  switch( type ){
	  case IIIMCF_EVENT_TYPE_UI_PREEDIT:
		  debug("preedit");
		  break;
		  
	  case IIIMCF_EVENT_TYPE_UI_PREEDIT_START:
		  debug("preedit start");
		  prdt->clear();

		  break;
		  
	  case IIIMCF_EVENT_TYPE_UI_PREEDIT_CHANGE:
		  debug("preedit changed");
		  
		  st = iiimcf_get_preedit_text( context, &buf0 , &cur_pos);
		  if( st != IIIMF_STATUS_SUCCESS ) check(st);
		 
		  prdt->clear();
		  prdt->append( iiimcf_text_to_utf8(buf0) );
		  prdt->render();
		  break;
		  
	  case IIIMCF_EVENT_TYPE_UI_PREEDIT_DONE:
		  debug("preedit done");
		  prdt->clear();
		  break;
		  
	  case IIIMCF_EVENT_TYPE_UI_PREEDIT_END:
		  debug("preedit end");
		  break;
		  
	  default:
		  debug("preedit none");
		  break;
  }
  return IIIMF_STATUS_SUCCESS;
}



IIIMF_status
IIIMCCF::iiimccf_lookup_choice(
    IIIMCF_context context,
    IIIMCF_event event
){

  IIIMF_status st;
  IIIMCF_event_type type;
  IIIMCF_lookup_choice ilc;

  LookupChoice *lkc = LookupChoice::getInstance();
  static int cur_idx=0;

  st = iiimcf_get_event_type( event, &type );
  if( st != IIIMF_STATUS_SUCCESS ) return st;
  
  if( type < IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE || type > IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE_END )
    return IIIMF_STATUS_COMPONENT_INDIFFERENT;
  
  switch( type ){	
	  case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE: 
		  debug( "lookup" );
		  break;
		  
	  case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE_START:
		  debug( "lookup start" );
		  lkc->clear();
		  break;
	  
	  case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE_CHANGE:
		  debug( "lookup change" );
		  lkc->clear();

		  st = iiimcf_get_lookup_choice( context, &ilc );
		  if( st != IIIMF_STATUS_SUCCESS ) check(st);
		  
		  int lkc_size, idx_first, idx_last;
		  st = iiimcf_get_lookup_choice_size( ilc, &lkc_size, &idx_first, &idx_last, &cur_idx );
		  if( st!= IIIMF_STATUS_SUCCESS ) check(st);

		  for( int i=0; i< lkc_size; i++ )
		  { 
		    IIIMCF_text cand, label;
		    int flag;
		    iiimcf_get_lookup_choice_item( ilc, i, &cand, &label, &flag );
		    if( flag & IIIMCF_LOOKUP_CHOICE_ITEM_ENABLED )
		    {
		      string label_utf8 = iiimcf_text_to_utf8( label );
		      string cand_utf8 = iiimcf_text_to_utf8( cand );
		      string item = label_utf8 + cand_utf8;
		      lkc->append_next( (char*) item.c_str() );
		    }
		  }
		  lkc->render();
		  
		  break;
	  
	  case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE_DONE:
		  debug( "lookup done" );
		  lkc->clear();
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
IIIMCCF::iiimccf_commit(
    IIIMCF_context context,
    IIIMCF_event event
){
    IIIMF_status st;
    IIIMCF_event_type type;
    IIIMCF_text text;

    st = iiimcf_get_event_type( event, &type );
    if( st != IIIMF_STATUS_SUCCESS ) return st;

    if ((type >= IIIMCF_EVENT_TYPE_UI_COMMIT)
	    && (type < IIIMCF_EVENT_TYPE_UI_COMMIT_END))
    {
	switch( type ){	
	  case IIIMCF_EVENT_TYPE_UI_COMMIT: 
	      debug("commit");
	      st = iiimcf_get_committed_text( context, &text );
	      if( st == IIIMF_STATUS_SUCCESS )
	      {
		cmt_buf = iiimcf_text_to_utf8( text );
	      }
	      break;
		  
	  case IIIMCF_EVENT_TYPE_UI_COMMIT_END:
	      debug("commit_end");
	      break;
	      
	  default: 
	      debug(" !!commit!! ");
	      break;	
	}
	return IIIMF_STATUS_SUCCESS;
    }

    return IIIMF_STATUS_COMPONENT_INDIFFERENT;
}

IIIMF_status
IIIMCCF::iiimccf_status(
    IIIMCF_context context,
    IIIMCF_event event
){
	IIIMCF_text text; 
	IIIMF_status st;
	IIIMCF_event_type type;
	st = iiimcf_get_event_type( event, &type );
	if( st != IIIMF_STATUS_SUCCESS ) return st;
	
        if ( type < IIIMCF_EVENT_TYPE_UI_COMMIT || type > IIIMCF_EVENT_TYPE_UI_COMMIT_END )
	  return IIIMF_STATUS_COMPONENT_INDIFFERENT;


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


IIIMF_status IIIMCCF::dispatch_event(IIIMCF_context context, IIIMCF_event event)
{
  IIIMF_status st;
  if( iiimccf_preedit( context, event ) == IIIMF_STATUS_SUCCESS )
    return IIIMF_STATUS_SUCCESS;
  if( iiimccf_lookup_choice( context, event ) == IIIMF_STATUS_SUCCESS )
    return IIIMF_STATUS_SUCCESS;
  if( iiimccf_commit( context, event ) == IIIMF_STATUS_SUCCESS )
    return IIIMF_STATUS_SUCCESS;
  if( iiimccf_status( context, event ) == IIIMF_STATUS_SUCCESS )
    return IIIMF_STATUS_SUCCESS;
  
  return iiimcf_dispatch_event( context, event );

}

string IIIMCCF::process_input( const string& input )
{
	IIIMCF_event event,ev ;
	IIIMCF_keyevent kev;
	IIIMCF_event_type event_type;
  	stdin_to_iiimcf_keyevent( input, kev );

	/* sent the keyevent to iiimcf */
        iiimcf_create_keyevent( &kev, &ev );
        iiimcf_forward_event( context, ev );

	/* get the reponse from the iiimcf */	
	while( iiimcf_get_next_event( context, &event ) == IIIMF_STATUS_SUCCESS ){
		
	    IIIMF_status st;
	    st = dispatch_event( context , event );
	    
	    if (st != IIIMF_STATUS_SUCCESS) {
			if (st == IIIMF_STATUS_COMPONENT_FAIL) {
			    debug("at least one component reported failure");
			} else if (st == IIIMF_STATUS_COMPONENT_INDIFFERENT) {
			    IIIMCF_event_type event_type;
			    iiimcf_get_event_type( event, &event_type );
			    debug( "none of the components deal with the event" );
			} else {
			    debug("fail to dispatch");
			}
	    }
	    
	    iiimcf_ignore_event( event );

    	}
	
	string result;
	if( cmt_buf.size() > 0 )
	{
	  result = cmt_buf;
	  cmt_buf.clear();
	}

	return result;
}
