#include "iiimccf-int.h"
#include <ctime>

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
	st = iiimcf_register_component( handle,"iiimccf-preedit",iiimccf_preedit,parent,&child);
         
	st = iiimcf_get_component( handle,"org.OpenI18N.IIIMCF.UI.status",&parent);
	st = iiimcf_register_component( handle,"iiimccf-status",iiimccf_status,parent,&child);
         
	st = iiimcf_get_component( handle,"org.OpenI18N.IIIMCF.UI.commit",&parent);
	st = iiimcf_register_component( handle,"iiimccf-commit",iiimccf_commit,parent,&child);
         
	st = iiimcf_get_component( handle,"org.OpenI18N.IIIMCF.UI.lookup_choice",&parent);
	st = iiimcf_register_component( handle,"iiimccf-lookup_choice",iiimccf_lookup_choice,parent,&child);
         
	st = iiimcf_get_component( handle,"org.OpenI18N.IIIMCF.trigger_notify",&parent);
	st = iiimcf_register_component( handle,"iiimccf-trigger_notify",iiimccf_trigger_notify,parent,&child);
         
	st = iiimcf_get_component( handle,"org.OpenI18N.IIIMCF.event.key",&parent);
	st = iiimcf_register_component( handle,"iiimccf-event.key",iiimccf_event_key,parent,&child);
         
	st = iiimcf_get_component( handle,"org.OpenI18N.IIIMCF.AUX",&parent);
	st = iiimcf_register_component( handle,"iiimccf-aux",iiimccf_aux,parent,&child);
}

IIIMCCF::~IIIMCCF()
{
	iiimcf_destroy_handle( handle );
	iiimcf_finalize();  
}

bool IIIMCCF::on()
{
	iiimcf_create_context( handle, IIIMCF_ATTR_NULL, &context );
	
	IIIMCF_event event;
	iiimcf_create_trigger_notify_event( 1, &event);
	iiimcf_forward_event( context, event);
	return true;
}

bool IIIMCCF::off()
{
	IIIMCF_event event;
	iiimcf_create_trigger_notify_event( 0, &event );
	iiimcf_forward_event( context, event );

	iiimcf_destroy_context( context );
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
		
	    IIIMF_status st;
	    st = iiimcf_dispatch_event( context , event );
	    if (st != IIIMF_STATUS_SUCCESS) {
			if (st == IIIMF_STATUS_COMPONENT_FAIL) {
			    debug("at least one component reported failure");
			} else if (st == IIIMF_STATUS_COMPONENT_INDIFFERENT) {
			    debug("none of the components deal with the event");
				IIIMCF_event_type event_type;
				iiimcf_get_event_type( event, &event_type );
				fprintf( stderr, "\t\t %x\n", event_type );
			} else {
			    debug("fail to dispatch");
			}
	    }
		
		iiimcf_ignore_event( event );
    	}
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
	 free(idname);
	 free(hrn);
	 free(domain);
    }
    return true;
}

bool IIIMCCF::ims_set ( )
{
  return true;
}






