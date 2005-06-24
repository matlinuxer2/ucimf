#include <iiimcf.h>

/**
 * @struct IIIMCCF
 * IIIMF console input interface.
 */

class IIIMCCF {
    public:
	IIIMCCF( unsigned char *buffer_in, int *buffer_in_length );
	~IIIMCCF();
	init init();
	init exit();
	init start();
	init stop();
	init process();
    protected:



	IIIMCF_handle handle;
	IIIMCF_context context;
    private:
	unsigned char *buf_in, *buf_out;
	int *buf_in_len, *buf_out_len;
}


/*
 * @component component
 * These are component of iiimccf.
 */
int iiimccf_preedit( IIIMCF_context, IIIMCF_event, IIIMCF_component, IIIMCF_component );
int iiimccf_lookup_choice( IIIMCF_context, IIIMCF_event, IIIMCF_component, IIIMCF_component );
int iiimccf_status( IIIMCF_context, IIIMCF_event, IIIMCF_component, IIIMCF_component );
int iiimccf_commit( IIIMCF_context, IIIMCF_event, IIIMCF_component, IIIMCF_component );
int iiimccf_event_key( IIIMCF_context, IIIMCF_event, IIIMCF_component, IIIMCF_component );
int iiimccf_trigger_notify( IIIMCF_context, IIIMCF_event, IIIMCF_component, IIIMCF_component );
int iiimccf_aux( IIIMCF_context, IIIMCF_event,	IIIMCF_component, IIIMCF_component );



/* local function */
int wchar_to_utf8(wchar_t c, char * outbuf, int bufsize);
char * iiimcf_text_to_utf8(IIIMCF_text t);
char * card16_to_utf8(const IIIMP_card16 * pu16);
void debug_print_event(IIIMCF_event ev);
