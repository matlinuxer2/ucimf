#include "iiimccf-int.h"
#include "preedit.h"
#include "lookupchoice.h"
#include <iostream>

using namespace std;

extern IIIMCCF* iiimccf;

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
		  iiimccf->prdt = new Prdt( context );
		  iiimccf->prdt->update();
		  iiimccf->prdt->show();
		  break;
		  
	  case IIIMCF_EVENT_TYPE_UI_PREEDIT_CHANGE:
		  mesg("preedit changed");
		  iiimccf->prdt->update();
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
		  iiimccf->lkc = new Lkc( context );
		  iiimccf->lkc->update();
		  iiimccf->lkc->show();
		  break;
	  
	  case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE_CHANGE:
		  debug( "lookup change" );
		  iiimccf->lkc->update();
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

/* 
 * Commit
 */
//void cmt_update(IIIMCF_context);

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
	      cout << " !! " << endl;
	      break;	
	}
	return IIIMF_STATUS_SUCCESS;
    }

    return IIIMF_STATUS_COMPONENT_INDIFFERENT;
}

/*
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
*/


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
