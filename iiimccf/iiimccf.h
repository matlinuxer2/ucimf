#include <iiimcf.h>
#include <unistd.h>
#include <stdio.h>

/**
 * @struct IIIMCCF
 * IIIMF input interface.
 */
typedef struct _IIIMCCF{
  
  	// IIIMCF Object
	IIIMCF_handle 	handle;
	IIIMCF_context 	context;
	
	// Input and Output buffer
	const unsigned char *buf_in, *buf_out;
	int *buf_in_len, *buf_out_len;

	// Tag
	int available;
	int lookup_done;
	
	int (*init)();
	int (*exit)();
	int (*start)();
	int (*stop)();
	int (*process)();
} IIIMCCF;

/* API to console */
IIIMCCF* iiimccf_new( unsigned char* buf_in, int *buf_in_len, unsigned char* buf_out, int *buf_out_len ); 
int iiimccf_dispatch();

/* IIIMCCF member functions*/
int to_init();
int to_exit();
int to_start();
int to_stop();
int to_process( int keycode, int keychar, int modifier );



/**
 * @struct Preedit
 * Preedit display module
 */
typedef struct _Preedit{

} Preedit;


/**
 * @struct LookupChoice 
 * lookup choice display module
 */
typedef struct _LookupChoice{

} LookupChoice;

/**
 * @struct Aux
 * Aux display module
 */
typedef struct _Aux{

} Aux;



/**
 * @struct Display
 * Input method display module
 */
typedef struct _Display{

  int (*update)();
  int (*hide)();
  int (*show)();
  int (*vtdeltasize)();
  int (*visible)();

  Preedit *predt;
  LookupChoice *lkpchc;
  Aux *ax;
  
} Display;



/* Local utilities function */
int wchar_to_utf8(wchar_t c, char * outbuf, int bufsize);
char * iiimcf_text_to_utf8(IIIMCF_text t);
char * card16_to_utf8(const IIIMP_card16 * pu16);
void debug_print_event(IIIMCF_event ev);
