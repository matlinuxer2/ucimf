#include "iiimccf-int.h"
#include <iostream>
#define CONVERT_BUFSIZE 48

using namespace std;


/** Local static operations   **/

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

bool  get_committed_text()
{
	IIIMF_status st;
	IIIMCF_text text;

	//char *buf=cmt_buf;
	//int *buf_len = &cmt_len;
	
	//iiimcf_get_committed_text( context , &text);
	//iiimcf_get_text_length( text, buf_len );

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

void show_lookup_choice( IIIMCF_context context )
{
  IIIMF_status st;
  IIIMCF_lookup_choice ilc;

  st = iiimcf_get_lookup_choice( context, &ilc);
  if (st == IIIMF_STATUS_SUCCESS) {
      char *itemstr, *labelstr;
      int i, flag;
      int size, idx_first, idx_last, idx_current;
      IIIMCF_text cand, label;

      iiimcf_get_lookup_choice_size(ilc, &size,
					  &idx_first,
					  &idx_last,
					  &idx_current);

      for (i = 0; i < size; i++) {
	  iiimcf_get_lookup_choice_item(ilc, i, &cand, &label, &flag);
	  if (flag & IIIMCF_LOOKUP_CHOICE_ITEM_ENABLED) {
	      itemstr = iiimcf_text_to_utf8(cand);
	      labelstr = iiimcf_text_to_utf8(label);
	      fprintf(stderr, "LC[%d](%s): %s\n", i, labelstr, itemstr);
	      free(labelstr);
	      free(itemstr);
	  }
      }
  } else if (st == IIIMF_STATUS_NO_LOOKUP_CHOICE) {
      fprintf(stderr, "Lookup choice is disabled.\n");
  } else {
  }
}


void show_preedit_info( IIIMCF_context c )
{
    int cpos;
    IIIMF_status st;
    IIIMCF_text text;
    
    st = iiimcf_get_preedit_text(c, &text, &cpos);
    if (st == IIIMF_STATUS_SUCCESS) {
	char *str;

	str = iiimcf_text_to_utf8(text);
	fprintf(stderr, "Preedit(%d):%s\n", cpos, str);
	free(str);
    } else if (st == IIIMF_STATUS_NO_PREEDIT) {
	fprintf(stderr, "Preedit is disabled.\n");
    } else {
	cout << "unknow err" << st << endl;
    }
}


void show_incoming_event( IIIMCF_event ev )
{
    IIIMF_status st;
    IIIMCF_event_type et;
    iiimcf_get_event_type(ev, &et);
    switch (et) {
      case IIIMCF_EVENT_TYPE_KEYEVENT:
      {
	  IIIMCF_keyevent kev;

	  iiimcf_get_keyevent_value(ev, &kev);
	  cout << "Event[KEY]:" << kev.keycode << ", "
	                        << kev.keychar << ", "
				<< kev.modifier<< ", "
				<< kev.time_stamp << endl;
	  
	  break;
      }
      case IIIMCF_EVENT_TYPE_TRIGGER_NOTIFY:
      {
	  int flag;

	  iiimcf_get_trigger_notify_flag(ev, &flag);
	  cout << "Event[TRIGGER-NOTIFY]:" << flag << endl ;
	  break;
      }
      case IIIMCF_EVENT_TYPE_UI_PREEDIT_START:
       cout << "Event[PREEDIT-START]" << endl;
       break;
      case IIIMCF_EVENT_TYPE_UI_PREEDIT_CHANGE:
       cout << "Event[PREEDIT-CHANGE]" << endl;
       break;
      case IIIMCF_EVENT_TYPE_UI_PREEDIT_DONE:
       cout << "Event[PREEDIT-DONE]" << endl;
       break;

      case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE_START:
       cout << "Event[LOOKUP-CHOICE-START]" << endl;
       break;
      case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE_CHANGE:
       cout << "Event[LOOKUP-CHOICE-CHANGE]" << endl;
       break;
      case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE_DONE:
       cout << "Event[LOOKUP-CHOICE-DONE]" << endl;
       break;

      case IIIMCF_EVENT_TYPE_UI_STATUS_START:
       cout << "Event[STATUS-START]" << endl;
       break;
      case IIIMCF_EVENT_TYPE_UI_STATUS_CHANGE:
       cout << "Event[STATUS-CHANGE]" << endl;
       break;
      case IIIMCF_EVENT_TYPE_UI_STATUS_DONE:
       cout << "Event[STATUS-DONE]" << endl;
       break;

      case IIIMCF_EVENT_TYPE_UI_COMMIT:
       cout << "Event[COMMIT]" << endl;
       break;
       /*
      case IIIMCF_EVENT_TYPE_AUX_START:
       show_aux("AUX-START", ev);
       break;

      case IIIMCF_EVENT_TYPE_AUX_DRAW:
       show_aux("AUX-DRAW", ev);
       break;
      case IIIMCF_EVENT_TYPE_AUX_SETVALUES:
       show_aux("AUX-SETVALUES", ev);
       break;
      case IIIMCF_EVENT_TYPE_AUX_DONE:
       show_aux("AUX-DONE", ev);
       break;
       */


      default:
       cout << "Event[Unknown]:" << et << endl;;
       break;
    }
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
