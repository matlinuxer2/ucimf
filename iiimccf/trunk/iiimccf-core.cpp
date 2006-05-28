#include "iiimccf-int.h"
#include <iostream>
#include <ctime>
#include <cstring>
#include "observer.h"
#include "preedit.h"
#include "lookupchoice.h"
using namespace std;



IIIMCCF::IIIMCCF()
{
	IIIMCF_component parent, child;
	IIIMF_status st;
    	IIIMCF_attr attr;

    	st = iiimcf_initialize( IIIMCF_ATTR_NULL );
    	st = iiimcf_create_attr(&attr);
    	st = iiimcf_attr_put_string_value(attr, IIIMCF_ATTR_CLIENT_TYPE,
	                                      "IIIM Console Client Framework");
	st = iiimcf_create_handle(attr, &handle );
    	st = iiimcf_destroy_attr(attr);

	
	/*  Register new component */
	st = iiimcf_get_component( handle,"org.OpenI18N.IIIMCF.UI.preedit",&parent);
	if( st != IIIMF_STATUS_SUCCESS ) check(st);
	st = iiimcf_register_component( handle,"iiimccf-preedit",iiimccf_preedit,parent,&child);
	if( st != IIIMF_STATUS_SUCCESS ) check(st);
         
	st = iiimcf_get_component( handle,"org.OpenI18N.IIIMCF.UI.status",&parent);
	if( st != IIIMF_STATUS_SUCCESS ) check(st);
	st = iiimcf_register_component( handle,"iiimccf-status",iiimccf_status,parent,&child);
	if( st != IIIMF_STATUS_SUCCESS ) check(st);
        
	st = iiimcf_get_component( handle,"org.OpenI18N.IIIMCF.UI.lookup_choice",&parent);
	if( st != IIIMF_STATUS_SUCCESS ) check(st);
	st = iiimcf_register_component( handle,"iiimccf-lookup_choice",iiimccf_lookup_choice,parent,&child);
	if( st != IIIMF_STATUS_SUCCESS ) check(st);

	// st = iiimcf_get_component( handle,"org.OpenI18N.IIIMCF.UI.commit",&parent);
	st = iiimcf_get_component( handle,"org.OpenI18N.IIIMCF.UI",&parent);
	if( st != IIIMF_STATUS_SUCCESS ) check(st);
	st = iiimcf_register_component( handle,"iiimccf-commit",iiimccf_commit,parent,&child);
	if( st != IIIMF_STATUS_SUCCESS ) check(st);
         
	st = iiimcf_get_component( handle,"org.OpenI18N.IIIMCF.event.trigger_notify",&parent);
	if( st != IIIMF_STATUS_SUCCESS ) check(st);
	st = iiimcf_register_component( handle,"iiimccf-trigger_notify",iiimccf_trigger_notify,parent,&child);
	if( st != IIIMF_STATUS_SUCCESS ) check(st);
         
	st = iiimcf_get_component( handle,"org.OpenI18N.IIIMCF.event.key",&parent);
	if( st != IIIMF_STATUS_SUCCESS ) check(st);
	st = iiimcf_register_component( handle,"iiimccf-event.key",iiimccf_event_key,parent,&child);
	if( st != IIIMF_STATUS_SUCCESS ) check(st);
         
	st = iiimcf_get_component( handle,"org.OpenI18N.IIIMCF.AUX",&parent);
	if( st != IIIMF_STATUS_SUCCESS ) check(st);
	st = iiimcf_register_component( handle,"iiimccf-aux",iiimccf_aux,parent,&child);
	if( st != IIIMF_STATUS_SUCCESS ) check(st);
        
	cur_ims_id = 0;
    
	IIIMCF_input_method *pims;
	const IIIMP_card16 *u16idname, *u16hrn, *u16domain;
	char *idname, *hrn, *domain;
	int num_of_ims;

	st = iiimcf_get_supported_input_methods(handle, &num_of_ims, &pims);
	iiimcf_get_input_method_desc(pims[cur_ims_id], &u16idname, &u16hrn, &u16domain);
	idname = iiimcf_string_to_utf8(u16idname);
	
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

int IIIMCCF::proc( int keycode, int keychar, int modifier )
{
	IIIMCF_event event,ev ;
	IIIMCF_keyevent kev;
	IIIMCF_event_type event_type;

	/* sent the keyevent to iiimcf */
	kev.keycode = keycode;
    	kev.keychar = keychar;
        kev.modifier = modifier;
    	kev.time_stamp = time( NULL );
        iiimcf_create_keyevent( &kev, &ev );
        iiimcf_forward_event( context, ev );

	/* get the reponse from the iiimcf */	
	while( iiimcf_get_next_event( context, &event ) == IIIMF_STATUS_SUCCESS ){
		
	    show_incoming_event( event );  
	  
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

void IIIMCCF::lkc_show()
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
  /* libiiimccf.a doesn't define the iiimcf_icsetvalues function*/
  //st = iiimcf_seticvalues( context, attr );
  
  st = iiimcf_destroy_attr( attr );
  
  return true;
}

void IIIMCCF::pos( int new_x, int new_y )
{
  cout << "change position " << endl;
  x = new_x;
  y = new_y;
  trkpt->set_position( new_x, new_y );
}

void IIIMCCF::refresh()
{

}



/* 
 * Definition of iiimcf_components
 */

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
	      cout.setf( ios_base::dec, ios_base::basefield );
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
	          cout.setf( ios_base::dec, ios_base::basefield );
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
