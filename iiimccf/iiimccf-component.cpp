#include "iiimccf-int.h"
#include "overspot.h"

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
 
  OverSpot prdt;
  IIIMCF_event_type event_type;
  iiimcf_get_event_type( event, &event_type);
  switch( event_type ){
	  case IIIMCF_EVENT_TYPE_UI_PREEDIT:
		  debug("preedit");
		  break;
		  
	  case IIIMCF_EVENT_TYPE_UI_PREEDIT_START:
		  debug("preedit start");
		  prdt.show();
		  break;
		  
	  case IIIMCF_EVENT_TYPE_UI_PREEDIT_CHANGE:
		  debug(" preedit changed");
		  show_preedit_info( context );
		  prdt.update();
		  break;
		  
	  case IIIMCF_EVENT_TYPE_UI_PREEDIT_DONE:
		  debug("preedit done");
		  prdt.update();
		  break;
		  
	  case IIIMCF_EVENT_TYPE_UI_PREEDIT_END:
		  debug("preedit end");
		  prdt.hide();
		  break;
		  
	  default:
		  debug("preedit none");
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
  static OverSpot lkc;
  IIIMCF_event_type event_type;
  iiimcf_get_event_type( event, &event_type );
  IIIMF_status st;
   
  switch( event_type ){
	  case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE: 
		  debug( "lookup" );
		  break;
		  
	  case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE_START:
		  debug( "lookup start" );
		  lkc.show();
		  break;
	  
	  case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE_CHANGE:
		  debug( "lookup change" );
		  show_lookup_choice( context );
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
		  
	  default: break;
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
	IIIMCF_event_type event_type;
	iiimcf_get_event_type( event, &event_type );
	switch( event_type ){	
		case IIIMCF_EVENT_TYPE_UI_STATUS:
		  
		case IIIMCF_EVENT_TYPE_UI_STATUS_START:
		  
		case IIIMCF_EVENT_TYPE_UI_STATUS_CHANGE:
		  
		case IIIMCF_EVENT_TYPE_UI_STATUS_DONE:
		  
		case IIIMCF_EVENT_TYPE_UI_STATUS_END:
		  
		default: break;
	}
	return IIIMF_STATUS_SUCCESS;
}


/* 
 * Commit
 */
IIIMF_status
iiimccf_commit(
    IIIMCF_context context,
    IIIMCF_event event,
    IIIMCF_component current,
    IIIMCF_component parent
){
	IIIMF_status st;
	IIIMCF_event_type event_type;
	st = iiimcf_get_event_type( event, &event_type );
	
	switch( event_type ){	
		case IIIMCF_EVENT_TYPE_UI_COMMIT: 
		    debug("commit");
		    get_committed_text();
		    break;
			
		case IIIMCF_EVENT_TYPE_UI_COMMIT_END:
		    debug("commit_end");
		    break;
		    
		default: break;
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
	IIIMCF_event_type event_type;
	iiimcf_get_event_type( event, &event_type );
	
	switch( event_type ){	
		case IIIMCF_EVENT_TYPE_EVENTLIKE: 
		  
		case IIIMCF_EVENT_TYPE_KEYEVENT:
		  
		case IIIMCF_EVENT_TYPE_KEYEVENT_END: 
		  
		default: break;
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
	IIIMCF_event_type event_type;
	iiimcf_get_event_type( event, &event_type );

	switch( event_type ){
		//case IIIMCF_EVENT_TYPE_TRIGGER_NOTIFY_START:
		case IIIMCF_EVENT_TYPE_TRIGGER_NOTIFY:
		  
		case IIIMCF_EVENT_TYPE_TRIGGER_NOTIFY_END:
		  
		case IIIMCF_EVENT_TYPE_OPERATION:
		  
		case IIIMCF_EVENT_TYPE_SETICFOCUS:
		  
		case IIIMCF_EVENT_TYPE_UNSETICFOCUS:
		  
		case IIIMCF_EVENT_TYPE_EVENTLIKE_END:
		  
		default: break;
		
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
	IIIMCF_event_type event_type;
	iiimcf_get_event_type( event, &event_type );
	
	switch( event_type ){	
		case IIIMCF_EVENT_TYPE_AUX: 
		  
		case IIIMCF_EVENT_TYPE_AUX_START:
		  
		case IIIMCF_EVENT_TYPE_AUX_DRAW:
		  
		case IIIMCF_EVENT_TYPE_AUX_SETVALUES:
		  
		case IIIMCF_EVENT_TYPE_AUX_DONE:
		  
		case IIIMCF_EVENT_TYPE_AUX_GETVALUES:
		  
		case IIIMCF_EVENT_TYPE_AUX_END:
		  
		default: break;
	}	
	return IIIMF_STATUS_SUCCESS;
}
