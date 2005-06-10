/* System support */
#include <sys/ioctl.h>
#include <linux/input.h>
#include <linux/kd.h>
#include <linux/keyboard.h>
#include <stdio.h>
#include <stdlib.h>
#include <iconv.h>
#include <wchar.h>

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


/** Global interface functions **/

/*
 * IIIMCCF constructor
 * 
 * return: a pointer of IIIMCCF object.
 * para1 : a pointer to buffer, which store the keyinput information.
 * para2 : a pointer to int, pass the length of the buffer.
 */
IIIMCCF* iiimccf_new( unsigned char* buf_in, int *buf_in_len )
{
	// Allocate memory.
	IIIMCCF *iiimccf;
	iiimccf = malloc( sizeof(IIIMCCF) );
	
	// Set up the parameter.
	iiimccf->buf_in = buf_in;
	iiimccf->buf_in_len = buf_in_len;
	
	// Assign function section.
	iiimccf->init = iiimccf_init;
	iiimccf->exit = iiimccf_exit;
	iiimccf->start = iiimccf_start;
	iiimccf->stop = iiimccf_stop;
	iiimccf->handling = iiimccf_handle;
	
	return iiimccf;
}'

int iiimccf_init( IIIMCCF* iiimccf )
{
	// Register new component
	IIIMCF_component parent, child;
	IIIMF_status st;

	st = iiimcf_initialize( IIIMCF_ATTR_NULL );
	st = iiimcf_create_handle( IIIMCF_ATTR_NULL, &( iiimccf->handle) );
	     
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

int iiimccf_exit()
{
	iiimcf_destroy_handle( vtinput->handle );
	iiimcf_finalize();  
	
	return 1;
}

int iiimccf_start()
{
	iiimcf_create_context( vtinput->handle, IIIMCF_ATTR_NULL, &(vtinput->context) );
	
	IIIMCF_event event;
    iiimcf_create_trigger_notify_event( 1, &event);
    iiimcf_forward_event( vtinput->context, event);
	
	vtinput->lookup_done = 0; 
	
}

int iiimccf_stop()
{
	IIIMCF_event event;
    iiimcf_create_trigger_notify_event( 0, &event );
    iiimcf_forward_event( vtinput->context, event );

	iiimcf_destroy_context( vtinput->context );
	
}

int iiimccf_handle()
{
	IIIMCF_event event;
	IIIMCF_event_type event_type;
	
	while( iiimcf_get_next_event( vtinput->context, &event ) == IIIMF_STATUS_SUCCESS ){
		
		IIIMF_status st;
	    st = iiimcf_dispatch_event( vtinput->context , event );
	    if (st != IIIMF_STATUS_SUCCESS) {
			if (st == IIIMF_STATUS_COMPONENT_FAIL) {
			    fprintf(stderr, "at least one component reported failure.\n");
			} else if (st == IIIMF_STATUS_COMPONENT_INDIFFERENT) {
			    fprintf(stderr, "none of the components deal with the event.\n");
				IIIMCF_event_type event_type;
				iiimcf_get_event_type( event, &event_type );
				fprintf( stderr, "\t\t %x\n", event_type );
			} else {
			    fprintf(stderr, "fail to dispatch");
			}
	    }
		
		iiimcf_ignore_event( event );
    	}
		
	
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
		fprintf(stderr,"\t preedit--\n");
		break;
		case IIIMCF_EVENT_TYPE_UI_PREEDIT_START:
		fprintf(stderr,"\t preedit start--\n");
		break;
		case IIIMCF_EVENT_TYPE_UI_PREEDIT_CHANGE:
		fprintf(stderr,"\t preedit changed--\n");
		break;
		case IIIMCF_EVENT_TYPE_UI_PREEDIT_DONE:
		fprintf(stderr,"\t preedit done--\n");
		break;
		case IIIMCF_EVENT_TYPE_UI_PREEDIT_END:
		fprintf(stderr,"\t preedit end--\n");
		break;
		default:
		fprintf(stderr,"\t preedit none--\n");
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
	IIIMCF_event_type event_type;
	iiimcf_get_event_type( event, &event_type );
	extern VTInput *pInput;
	
	switch( event_type ){
		case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE: 
		fprintf( stderr, "\t\t lookup \n" );
		break;
		case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE_START:
		// lookup->start();
		fprintf( stderr, "\t\t lookup start \n" );
		break;
		case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE_CHANGE:
		// lookup->show();
		fprintf( stderr, "\t\t lookup change \n" );
		break;
		case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE_DONE:
			pInput->lookup_done = 1;
			get_committed_text( pInput );
		    fprintf( stderr, "\t\t lookup done \n" );
		    break;
		case IIIMCF_EVENT_TYPE_UI_LOOKUP_CHOICE_END: 
		fprintf( stderr, "\t\t lookup end \n" );
		break;
		default:
		break;
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
		default:
		break;
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
			break;
		case IIIMCF_EVENT_TYPE_UI_COMMIT_END:
		    break;
		default: 
		break;
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
		default: 
		break;
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
		default:
		break;
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
		default:
		break;
	}	
	return IIIMF_STATUS_SUCCESS;
}



/** Local static operations   **/
IIIMF_status 
VTInput_send_key(
    VTInput *vtinput,
    int keycode,
    int keychar,
    int modifier
){
	IIIMCF_event ev;
    IIIMCF_keyevent kev;

    kev.keycode = keycode;
    kev.keychar = keychar;
    kev.modifier = modifier;
    kev.time_stamp = time( NULL );
    iiimcf_create_keyevent( &kev, &ev );
    iiimcf_forward_event( vtinput->context, ev );
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
iiimcf_text_to_utf8(IIIMCF_text t)
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


int get_committed_text( VTInput *vtinput )
{
	IIIMF_status st;
	IIIMCF_text text;
	size_t n=3;
	mbstate_t *ps;
	unsigned char *buffer;
	int  length;
	
	
    iiimcf_get_committed_text( vtinput->context , &text);
	iiimcf_get_text_length( text, &length );
	buffer = iiimcf_text_to_utf8( text );

	vtinput->mbbuffer  = buffer;
	length= mbrlen( buffer,  n, ps);
	vtinput->mb_length = length;

	return 0;
}
