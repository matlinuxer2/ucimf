#include <iiimcf.h>

/**
 * @struct IIIMCCF
 * IIIMF input interface.
 */
typedef struct _IIIMCCF{
	IIIMCF_handle 	handle;
	IIIMCF_context 	context;
	
	unsigned char *buf_in, *buf_out;
	int *buf_in_len, *buf_out_len;
	
	int (*init)();
	int (*exit)();
	int (*start)();
	int (*stop)();
	int (*handling)();
} IIIMCCF;


IIIMCCF* iiimccf_new( unsigned char *buf_in, int *buf_in_len ); 

typedef struct _VTInput{	
	IIIMCF_handle handle;
	IIIMCF_context context;
	int available;
	
	/* Read_in buffer */
	unsigned char* buffer;
	int *length;
	
	/* Write_out buffer */
	unsigned char* mbbuffer;
	int mb_length;
	int *fd;
	
	/* Tag */
	int lookup_done;
	
} VTInput;

VTInput *VTInput_new( unsigned char* buffer, int *length, int *fd);
int VTInput_isAvailable( VTInput *vtinput );
int VTInput_init( VTInput *vtinput );
int VTInput_exit( VTInput *vtinput );
int VTInput_start( VTInput *vtinput );
int VTInput_stop( VTInput *vtinput );
int VTInput_read_in( VTInput *vtinput );
int VTInput_handle( VTInput *vtinput );
int VTInput_reset( VTInput *vtinput );
int VTInput_write_out( VTInput *vtinput );

int iiimccf_init();
int iiimccf_exit();
int iiimccf_start();
int iiimccf_stop();
int iiimccf_handle();
int iiimccf_preedit( IIIMCF_context context,	IIIMCF_event event,	IIIMCF_component current, IIIMCF_component parent );
int iiimccf_lookup_choice( IIIMCF_context context,	IIIMCF_event event,	IIIMCF_component current, IIIMCF_component parent );
int iiimccf_status( IIIMCF_context context,	IIIMCF_event event,	IIIMCF_component current, IIIMCF_component parent );
int iiimccf_commit( IIIMCF_context context,	IIIMCF_event event,	IIIMCF_component current, IIIMCF_component parent );
int iiimccf_event_key( IIIMCF_context context,	IIIMCF_event event,	IIIMCF_component current, IIIMCF_component parent );
int iiimccf_trigger_notify( IIIMCF_context context,	IIIMCF_event event,	IIIMCF_component current, IIIMCF_component parent );
int iiimccf_aux( IIIMCF_context context,	IIIMCF_event event,	IIIMCF_component current, IIIMCF_component parent );

int wchar_to_utf8(wchar_t c, char * outbuf, int bufsize);
char * iiimcf_text_to_utf8(IIIMCF_text t);
char * card16_to_utf8(const IIIMP_card16 * pu16);
void debug_print_event(IIIMCF_event ev);
