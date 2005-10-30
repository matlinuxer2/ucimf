#include "iiimccf-int.h"
#include <iostream>
using namespace std;
/*
 * Preedit
 */

IIIMF_status
iiimccf_preedit(
    IIIMCF_context context,
    IIIMCF_event event,
    IIIMCF_component current,
    IIIMCF_component parent
){

  Prdt prdt( context );
  IIIMF_status st;
  IIIMCF_event_type type;
  st = iiimcf_get_event_type( event, &type );
  if( st != IIIMF_STATUS_SUCCESS ) return st;
   
  switch( type ){
	  case IIIMCF_EVENT_TYPE_UI_PREEDIT:
		  mesg("preedit");
		  break;
		  
	  case IIIMCF_EVENT_TYPE_UI_PREEDIT_START:
		  mesg("preedit start");
		  prdt.show();
		  break;
		  
	  case IIIMCF_EVENT_TYPE_UI_PREEDIT_CHANGE:
		  mesg("preedit changed");
		  prdt.info();
		  prdt.update();
		  break;
		  
	  case IIIMCF_EVENT_TYPE_UI_PREEDIT_DONE:
		  mesg("preedit done");
		  prdt.update();
		  break;
		  
	  case IIIMCF_EVENT_TYPE_UI_PREEDIT_END:
		  mesg("preedit end");
		  prdt.hide();
		  break;
		  
	  default:
		  mesg("preedit none");
		  break;
  }
  return IIIMF_STATUS_SUCCESS;
}



/* 
 * Lookup Choice 
 * */
IIIMF_status
iiimccf_lookup_choice(
    IIIMCF_context context,
    IIIMCF_event event,
    IIIMCF_component current,
    IIIMCF_component parent
){

  Lkc lkc( context );

  IIIMF_status st;
  IIIMCF_event_type type;
  st = iiimcf_get_event_type( event, &type );
  if( st != IIIMF_STATUS_SUCCESS ) return st;
  
  switch( type ){	
	  case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE: 
		  debug( "lookup" );
		  break;
		  
	  case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE_START:
		  debug( "lookup start" );
		  lkc.show();
		  break;
	  
	  case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE_CHANGE:
		  debug( "lookup change" );
		  // show_lookup_choice( context );
		  lkc.update();
		  break;
	  
	  case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE_DONE:
		  lkc.update();
		  debug( "lookup done" );
		  break;
	  case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE_END: 
		  lkc.hide();
		  debug( "lookup end" );
		  break;
		  
	  default:
		  break;
  }
  
  return IIIMF_STATUS_SUCCESS;
}



/*
 * Status
 */
IIIMF_status
iiimccf_status(
    IIIMCF_context context,
    IIIMCF_event event,
    IIIMCF_component current,
    IIIMCF_component parent
){
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
		  break;
		  
		case IIIMCF_EVENT_TYPE_UI_STATUS_DONE:
		  break;
		  
		case IIIMCF_EVENT_TYPE_UI_STATUS_END:
		  break;
		  
		default: 
		  debug(" !!status!! "); 
		  break; 
	}
	return IIIMF_STATUS_SUCCESS;
}



/* 
 * Event Key
 */
IIIMF_status
iiimccf_event_key(
    IIIMCF_context context,
    IIIMCF_event event,
    IIIMCF_component current,
    IIIMCF_component parent
){
	IIIMF_status st;
	IIIMCF_event_type type;
	st = iiimcf_get_event_type( event, &type );
	if( st != IIIMF_STATUS_SUCCESS ) return st;
	
	switch( type ){	
		case IIIMCF_EVENT_TYPE_EVENTLIKE: 
		  break;
		  
		case IIIMCF_EVENT_TYPE_KEYEVENT:
		  break;
		  
		case IIIMCF_EVENT_TYPE_KEYEVENT_END: 
		  break;
		  
		default: 
		  debug(" !!event_key!! ");
		  break;
	}

	return IIIMF_STATUS_SUCCESS;
}



/* 
 * Trigger Notify
 */
IIIMF_status
iiimccf_trigger_notify(
    IIIMCF_context context,
    IIIMCF_event event,
    IIIMCF_component current,
    IIIMCF_component parent
){
	IIIMF_status st;
	IIIMCF_event_type type;
	st = iiimcf_get_event_type( event, &type );
	if( st != IIIMF_STATUS_SUCCESS ) return st;
	
	switch( type ){
		//case IIIMCF_EVENT_TYPE_TRIGGER_NOTIFY_START:
		case IIIMCF_EVENT_TYPE_TRIGGER_NOTIFY:
		  break;
		  
		case IIIMCF_EVENT_TYPE_TRIGGER_NOTIFY_END:
		  break;
		  
		case IIIMCF_EVENT_TYPE_OPERATION:
		  break;
		  
		case IIIMCF_EVENT_TYPE_SETICFOCUS:
		  break;
		  
		case IIIMCF_EVENT_TYPE_UNSETICFOCUS:
		  break;
		  
		case IIIMCF_EVENT_TYPE_EVENTLIKE_END:
		  break;
		  
		default: 
		  debug(" !!trigger_notify!! ");

	          cout << " !! " ;
		  cout.setf( ios_base::hex, ios_base::basefield );
		  cout << type;
		  cout << " !! " << endl;
		  break;
		
	}
	return IIIMF_STATUS_SUCCESS;
}



/* 
 * Aux
 */
IIIMF_status
iiimccf_aux(
    IIIMCF_context context,
    IIIMCF_event event,
    IIIMCF_component current,
    IIIMCF_component parent
){
	IIIMF_status st;
	IIIMCF_event_type type;
	st = iiimcf_get_event_type( event, &type );
	if( st != IIIMF_STATUS_SUCCESS ) return st;
	
	switch( type ){	
		case IIIMCF_EVENT_TYPE_AUX: 
		  break; 
		case IIIMCF_EVENT_TYPE_AUX_START:
		  break; 
		case IIIMCF_EVENT_TYPE_AUX_DRAW:
		  break; 
		case IIIMCF_EVENT_TYPE_AUX_SETVALUES:
		  break; 
		case IIIMCF_EVENT_TYPE_AUX_DONE:
		  break; 
		case IIIMCF_EVENT_TYPE_AUX_GETVALUES:
		  break;
		case IIIMCF_EVENT_TYPE_AUX_END:
		  break; 
		default: 
		  debug(" !!aux!! ");
		  break;
	}	
	return IIIMF_STATUS_SUCCESS;
}
