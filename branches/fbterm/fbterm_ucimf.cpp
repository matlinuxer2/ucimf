#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "imapi.h"
#include "ucimf.h"
#include "imf/widget.h"
//#include "debug.h"

static char raw_mode = 1;
static char first_show = 1;
//int LogFd=-1;

static void im_active()
{
	if (raw_mode) {
		extern void init_keycode_state();
		init_keycode_state();
	}
	//UrDEBUG("im active\n");

	/* patch */
	int buf_len = 5;
	unsigned char *buf = (unsigned char*)malloc( sizeof(unsigned char)*( buf_len+1) );
	
	buf[0]=27;
	buf[1]=91;
	buf[2]=50;
	buf[3]=52;
	buf[4]=126;
	buf[5]=0;

	ucimf_switch(  buf, &buf_len );                                       
	/* patch */
}

static void im_deactive()
{
	/* patch */
	int buf_len = 5;
	unsigned char *buf = (unsigned char*)malloc( sizeof(unsigned char)*( buf_len+1) );
	
	buf[0]=27;
	buf[1]=91;
	buf[2]=50;
	buf[3]=52;
	buf[4]=126;
	buf[5]=0;

	ucimf_switch(  buf, &buf_len );                                       
	/* patch */

	//UrDEBUG("im deactive\n");
	//set_im_windows(0, 0);
	first_show = 1;
}

static void process_raw_key(char *buf, unsigned int len) 
{

    if( len <= 0 ) { return; }

    char* result = ucimf_process_raw( buf, (int*)&len ); 

    if( len <= 0 ) { 
	    return; 
    }
    else
    {
	    unsigned short result_len = len;
	    //UrDEBUG("ucimf: return text, '%s', length:%d \n", result, len );
	    put_im_text( result, result_len );
    }

}

static void cursor_pos_changed(unsigned x, unsigned y)
{
	//UrDEBUG("cursor, %d, %d\n", x, y);

	ucimf_cursor_position( x, y);
}

static ImCallbacks cbs = {
	im_active, // .active
	im_deactive, // .deactive
	0,
	0,
	process_raw_key, // .send_key
	cursor_pos_changed, // .cursor_position
	0, // .fbterm_info
	0 // .term_mode
};

int main()
{
	/* patch */
	ucimf_init();
	/* patch */

	register_im_callbacks(cbs);
	connect_fbterm(raw_mode);
	while (check_im_message()) ;

	//UrDEBUG("im exit normally\n");
	return 0;
}
