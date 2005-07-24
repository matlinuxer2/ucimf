/* System support */
#include <sys/ioctl.h>
#include <linux/input.h>
#include <linux/kd.h>
#include <linux/keyboard.h>
#include <stdio.h>
#include <stdlib.h>
#include <iconv.h>
#include <wchar.h>
#include <stdint.h>

/* Add IIIMCF support. */
#include <iiimcf.h>
#include "iiimccf.h"

/* Add ext key */
#include <sys/types.h>
#include <linux/input.h>
#include <unistd.h>
#include "keycodes.h"

/* Global parameter */
#define CONVERT_BUFSIZE 48
IIIMCCF *iiimccf;
OverSpot* overspot;

/** Registed component functions **/
static IIIMF_status iiimccf_preedit( IIIMCF_context context, IIIMCF_event event, IIIMCF_component current, IIIMCF_component parent );
static IIIMF_status iiimccf_lookup_choice( IIIMCF_context context, IIIMCF_event event, IIIMCF_component current, IIIMCF_component parent );
static IIIMF_status iiimccf_status( IIIMCF_context context, IIIMCF_event event, IIIMCF_component current, IIIMCF_component parent );
static IIIMF_status iiimccf_commit( IIIMCF_context context, IIIMCF_event event, IIIMCF_component current, IIIMCF_component parent );
static IIIMF_status iiimccf_event_key( IIIMCF_context context, IIIMCF_event event, IIIMCF_component current, IIIMCF_component parent );
static IIIMF_status iiimccf_trigger_notify( IIIMCF_context context, IIIMCF_event event, IIIMCF_component current, IIIMCF_component parent );
static IIIMF_status iiimccf_aux( IIIMCF_context context, IIIMCF_event event, IIIMCF_component current, IIIMCF_component parent );


/** Local static operations   **/
void debug( char *str ){
  char *tmp, *tmp2;
  tmp=strcat( tmp, "\t");
  tmp=strcat( tmp, str );
  tmp=strcat( tmp, "\n");
  fprintf( stderr, tmp );
}


int
wchar_to_utf8(wchar_t c, char * outbuf, int bufsize)
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


char *
iiimcf_text_to_utf8( IIIMCF_text t)
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


int get_committed_text()
{
  	extern IIIMCCF *iiimccf;
	IIIMF_status st;
	IIIMCF_text text;

	unsigned char *buf=iiimccf->buf_out ;
	int *buf_len = iiimccf->buf_out_len ;
	
	iiimcf_get_committed_text( iiimccf->context , &text);
	iiimcf_get_text_length( text, buf_len );
	buf = iiimcf_text_to_utf8( text );

	size_t next_max = 4;
	mbstate_t *ps;
	(*buf_len) = mbrlen( buf, next_max, ps );

	return 0;
}

int clr_committed_text()
{
  extern IIIMCCF *iiimccf;
  *(iiimccf->buf_out_len)=0;

}


/*
 * IIIMCCF constructor
 * 
 * return: a pointer of IIIMCCF object.
 * para1 : a pointer to buffer, which store the keyinput information.
 * para2 : a pointer to int, pass the length of the buffer.
 */
IIIMCCF* iiimccf_new( unsigned char* buf_in, int *buf_in_len, unsigned char* buf_out, int *buf_out_len )
{
	// Allocate memory.
	extern IIIMCCF *iiimccf;
	iiimccf = malloc( sizeof(IIIMCCF) );
	
	// Set up the parameter.
	iiimccf->buf_in  = buf_in;
	iiimccf->buf_in_len  = buf_in_len;
	iiimccf->buf_out = buf_out;
	iiimccf->buf_out_len = buf_out_len;
	
	// Assign function section.
	iiimccf->init = to_init;
	iiimccf->exit = to_exit;
	iiimccf->start = to_start;
	iiimccf->stop = to_stop;
	iiimccf->process = to_process;
	
	return iiimccf;
}

int to_init()
{
  	extern IIIMCCF *iiimccf;

	IIIMCF_component parent, child;
	IIIMF_status st;
    	IIIMCF_attr attr;

    	st = iiimcf_initialize( IIIMCF_ATTR_NULL );
    	st = iiimcf_create_attr(&attr);
    	st = iiimcf_attr_put_string_value(attr, IIIMCF_ATTR_CLIENT_TYPE,
	                                      "IIIM Console Client Framework");
    	st = iiimcf_create_handle(attr, &( iiimccf->handle ) );
    	st = iiimcf_destroy_attr(attr);

	
	/*  Register new component */
	     
	st = iiimcf_get_component( iiimccf->handle,"org.OpenI18N.IIIMCF.UI.preedit",&parent);
	st = iiimcf_register_component( iiimccf->handle,"iiimccf-preedit",iiimccf_preedit,parent,&child);
         
	st = iiimcf_get_component( iiimccf->handle,"org.OpenI18N.IIIMCF.UI.status",&parent);
	st = iiimcf_register_component( iiimccf->handle,"iiimccf-status",iiimccf_status,parent,&child);
         
	st = iiimcf_get_component( iiimccf->handle,"org.OpenI18N.IIIMCF.UI.commit",&parent);
	st = iiimcf_register_component( iiimccf->handle,"iiimccf-commit",iiimccf_commit,parent,&child);
         
	st = iiimcf_get_component( iiimccf->handle,"org.OpenI18N.IIIMCF.UI.lookup_choice",&parent);
	st = iiimcf_register_component( iiimccf->handle,"iiimccf-lookup_choice",iiimccf_lookup_choice,parent,&child);
         
	st = iiimcf_get_component( iiimccf->handle,"org.OpenI18N.IIIMCF.trigger_notify",&parent);
	st = iiimcf_register_component( iiimccf->handle,"iiimccf-trigger_notify",iiimccf_trigger_notify,parent,&child);
         
	st = iiimcf_get_component( iiimccf->handle,"org.OpenI18N.IIIMCF.event.key",&parent);
	st = iiimcf_register_component( iiimccf->handle,"iiimccf-event.key",iiimccf_event_key,parent,&child);
         
	st = iiimcf_get_component( iiimccf->handle,"org.OpenI18N.IIIMCF.AUX",&parent);
	st = iiimcf_register_component( iiimccf->handle,"iiimccf-aux",iiimccf_aux,parent,&child);
	
	return 1;
}

int to_exit()
{
  	extern IIIMCCF *iiimccf;
	iiimcf_destroy_handle( iiimccf->handle );
	iiimcf_finalize();  
	
	return 1;
}

int to_start()
{
 	extern IIIMCCF *iiimccf; 
	iiimcf_create_context( iiimccf->handle, IIIMCF_ATTR_NULL, &(iiimccf->context) );
	
	IIIMCF_event event;
	iiimcf_create_trigger_notify_event( 1, &event);
	iiimcf_forward_event( iiimccf->context, event);
	
	iiimccf->lookup_done = 0; 
	
}

int to_stop()
{
  	extern IIIMCCF *iiimccf;
	IIIMCF_event event;
	iiimcf_create_trigger_notify_event( 0, &event );
	iiimcf_forward_event( iiimccf->context, event );

	iiimcf_destroy_context( iiimccf->context );
	
}

int to_process( int keycode, int keychar, int modifier )
{
  	extern IIIMCCF *iiimccf;

	IIIMCF_event event,ev ;
	IIIMCF_keyevent kev;
	IIIMCF_event_type event_type;

	/* sent the keyevent to iiimcf */
	kev.keycode = keycode;
    	kev.keychar = keychar;
        kev.modifier = modifier;
    	kev.time_stamp = time( NULL );
        iiimcf_create_keyevent( &kev, &ev );
        iiimcf_forward_event( iiimccf->context, ev );

	/* get the reponse from the iiimcf */	
	while( iiimcf_get_next_event( iiimccf->context, &event ) == IIIMF_STATUS_SUCCESS ){
		
	    IIIMF_status st;
	    st = iiimcf_dispatch_event( iiimccf->context , event );
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

void to_show_ims()
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

       idname = test_format_iiimcf_string(u16idname);
       hrn = test_format_iiimcf_string(u16hrn);
       domain = test_format_iiimcf_string(u16domain);
       fprintf(stderr,
                "Input Method %d ----------------------------------\n"
                "  idname: %s\n"
                "  HRN: %s\n"
                "  domain: %s\n"
                "  langs: ",
                i, idname, hrn, domain);

       		int j;
	        for (j = 0; j < nlangs; j++) {
	              iiimcf_get_language_id(plangs[j], &langid);
	              fprintf(stderr, "%s, ", langid);
	        }
	 free(idname);
	 free(hrn);
	 free(domain);
    }
}

int to_change_ims( int ){

}

int show_input_style()
{

}

int change_input_style( int )
{


}


/** Registered components **/

static IIIMF_status
iiimccf_preedit(
    IIIMCF_context context,
    IIIMCF_event event,
    IIIMCF_component current,
    IIIMCF_component parent
){
	IIIMCF_event_type event_type;
	iiimcf_get_event_type( event, &event_type);
	switch( event_type ){
		case IIIMCF_EVENT_TYPE_UI_PREEDIT:
			debug("preedit");
			break;
			
		case IIIMCF_EVENT_TYPE_UI_PREEDIT_START:
			debug("preedit start");
			//dsply->preedit->update();
			//disply->preedit->show();
			break;
			
		case IIIMCF_EVENT_TYPE_UI_PREEDIT_CHANGE:
			debug(" preedit changed");
			//dsply->preedit->update();
			//dsply->preedit->show();
			break;
			
		case IIIMCF_EVENT_TYPE_UI_PREEDIT_DONE:
			debug("preedit done");
			//dsply->preedit->update();
			//dsply->preedit->hide();
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

static IIIMF_status
iiimccf_lookup_choice(
    IIIMCF_context context,
    IIIMCF_event event,
    IIIMCF_component current,
    IIIMCF_component parent
){
	extern IIIMCCF *iiimccf;
	IIIMCF_event_type event_type;
	iiimcf_get_event_type( event, &event_type );
	
	switch( event_type ){
		case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE: 
			debug( "lookup" );
			break;
			
		case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE_START:
			debug( "lookup start" );
			overspot->start();
			break;
		
		case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE_CHANGE:
			debug( "lookup change" );
			overspot->update();
			overspot->redraw();
			get_lookup_choice()
			break;
		
		case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE_DONE:
			overspot->update();
			overspot->redraw();
			get_committed_text();
			debug( "lookup done" );
			break;
		case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE_END: 
			overspot->hide();
			debug( "lookup end" );
			break;
			
		default: break;
	}
	
	return IIIMF_STATUS_SUCCESS;
}

static IIIMF_status
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

static IIIMF_status
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
		    clr_committed_text();
		    break;
		    
		default: break;
	}
	return IIIMF_STATUS_SUCCESS;

}

static IIIMF_status
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

static IIIMF_status
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

static IIIMF_status
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



