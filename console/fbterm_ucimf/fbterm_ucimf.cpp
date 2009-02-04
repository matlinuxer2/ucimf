#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "imapi.h"
#include "keycode.h"
#include "font.h"
#include "screen.h"
#include "ucimf.h"
#include "imf/widget.h"
#include "debug.h"

static char raw_mode = 1;
static char first_show = 1;

static void im_active()
{
	if (raw_mode) {
		init_keycode_state();
	}
	UrINFO("im active\n");

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

	UrINFO("im deactive\n");
	set_im_windows(0, 0);
	first_show = 1;
}

static void process_raw_key(char *buf, int len) 
{
    unsigned short i;
    for (i = 0; i < len; i++) {
	    char down = !(buf[i] & 0x80);
	    short code = buf[i] & 0x7f;

	    if (!code) {
		    if (i + 2 >= len) break;

		    code = (buf[++i] & 0x7f) << 7;
		    code |= buf[++i] & 0x7f;
		    if (!(buf[i] & 0x80) || !(buf[i - 1] & 0x80)) continue;
	    }

	    unsigned short keysym = keycode_to_keysym(code, down);
	    char *str = keysym_to_term_string(keysym, down);

	    UrDEBUG( "getkey, down:%d, keysym:0x%x, term string:%s\n", down, keysym, str);
	    //put_im_text(str, strlen(str));
    }

    char* result = ucimf_process_raw( buf, &len ); 
    unsigned short result_len = len;
    UrDEBUG("ucimf: return text, %s\n", result );
    put_im_text( result, result_len );

}

static void process_key(char *keys, unsigned short len)
{
	if (raw_mode) {
		process_raw_key(keys, len);
	} else {
		UrDEBUG("getkey, (%c, 0x%x)\n", *keys, *keys);

		/* patch */
		int buf_len=len;
		char *buf = (char*)malloc( sizeof(char)*(buf_len+1) );
		for( int i=0; i<len ; i++ )
		{
			buf[i]=keys[i];
		}

		char* result = ucimf_process_stdin( buf, &buf_len ); 

		unsigned short result_len = buf_len;
		/* patch */

		put_im_text( result, result_len);
	}
}

static void cursor_pos_changed(unsigned x, unsigned y)
{
	UrDEBUG("cursor, %d, %d\n", x, y);

	ucimf_cursor_position( x, y);

	Status *stts = Status::getInstance();
	Preedit *prdt = Preedit::getInstance();
	LookupChoice *lkc = LookupChoice::getInstance();

	int x1 = stts->getWindow()->x();
	int y1 = stts->getWindow()->y();
	int w1 = stts->getWindow()->w() +1;
	int h1 = stts->getWindow()->h() +1;

	int x2 = prdt->getWindow()->x();
	int y2 = prdt->getWindow()->y();
	int w2 = prdt->getWindow()->w() +1;
	int h2 = prdt->getWindow()->h() +1;

	int x3 = lkc->getWindow()->x();
	int y3 = lkc->getWindow()->y();
	int w3 = lkc->getWindow()->w() +1;
	int h3 = lkc->getWindow()->h() +1;

	ImWin wins[] = {
		{ x1 , y1 , w1, h1 },
		{ x2 , y2 , w2, h2 },
		{ x3 , y3 , w3, h3 },
	};

	set_im_windows(wins, 3);

	/*
	static const char str[] = "a IM example";
	#define NSTR (sizeof(str) -1)
	
	ImWin wins[] = {
		{ x + 10, y + 10, 40, 20 },
		{ x + 10, y + 40, W(NSTR) + 10, H(1) + 10 }
	};
	set_im_windows(wins, 2);
	
	if (first_show) {
		first_show = 0;

		// should call this function once when IM begins to reshow UI after hiding UI last time
		Screen::instance()->updateYOffset();
	}

	Screen::instance()->fillRect(wins[0].x, wins[0].y, wins[0].w, wins[0].h, 7);

	// the better way is only filling margins
	Screen::instance()->fillRect(wins[1].x, wins[1].y, wins[1].w, wins[1].h, 7);
	
	unsigned short unistr[NSTR];
	bool dws[NSTR]; // double width flags
	
	for (int i = 0; i < NSTR; i++) {
		unistr[i] = str[i];
		dws[i] = false;
	}
	Screen::instance()->drawText(wins[1].x + 5, wins[1].y + 5, 0, 7, NSTR, unistr, dws);
	*/
}

static void update_fbterm_info(Info *info)
{
	Font::setFontInfo(info->fontName, info->fontSize);
	Screen::setRotateType((RotateType)info->rotate);
	if (!Screen::instance()) {
		exit(1);
	}
}

static ImCallbacks cbs = {
	im_active, // .active
	im_deactive, // .deactive
	process_key, // .send_key
	cursor_pos_changed, // .cursor_position
	update_fbterm_info, // .fbterm_info
	update_term_mode // .term_mode
};

int main()
{
	/* patch */
	ucimf_init();
	/* patch */

	register_im_callbacks(cbs);
	connect_fbterm(raw_mode);
	while (check_im_message()) ;

	Screen::uninstance();
	UrINFO("im exit normally\n");
	return 0;
}
