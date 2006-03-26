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


