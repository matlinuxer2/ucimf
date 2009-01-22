/*
 * UCIMF - Unicode Console InputMethod Framework                                                    
 *
 * Copyright (c) 2006-2007 Chun-Yu Lee (Mat) and Open RazzmatazZ Laboratory (OrzLab)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <ucimf.h>
#include <sys/ioctl.h>
#include <linux/kd.h>
#include <linux/keyboard.h>
#include <linux/input.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <dirent.h>
#include <ltdl.h>
#include <vector>

#include "imf.h"
#include "widget.h"
#include "cwm.h"
#include "options.h"

using namespace std;

bool prev_focus;
//ConsoleFocus *focus = ConsoleFocus::getInstance();
//CursorPosition *pos = CursorPosition::getInstance();
//CurrentImfStatus *sts = CurrentImfStatus::getInstance();
Status *stts = Status::getInstance();
Preedit *prdt = Preedit::getInstance();
LookupChoice *lkc = LookupChoice::getInstance();

Shift *status_shift = new StatusShift;
Shift *preedit_shift = new PreeditShift;
Shift *lookupchoice_shift = new LookupChoiceShift;

Cwm *cwm = Cwm::getInstance();

Imf *imf;
vector<Imf*> imfs;
size_t current_imf;


/* 紀錄整個鍵盤，看有那些鍵被按下 */
static char key_down[NR_KEYS];
static unsigned char shift_down[NR_SHIFT];
static short shift_state;
static char lock_state;
static int npadch;
static char cr_with_lf, applic_keypad, cursor_esco;

unsigned short keycode_to_keysym(unsigned short keycode, char down);                              
char *keysym_to_term_string(unsigned short keysym, char down);





void scanImf()
{
  current_imf =0;
  imfs.clear();
  createImf_t* create_imf = NULL;
  destroyImf_t* destroy_imf = NULL;

  Options* option= Options::getInstance();
  char* imf_mod_path = option->getOption("IMF_MODULE_DIR");
  cerr << "IMF Modules scan path: " << imf_mod_path << endl;
 
  lt_dlinit();
  lt_dlsetsearchpath( imf_mod_path );

  DIR *dir = opendir( imf_mod_path );
  if( dir )
  {
    struct dirent *d_ent;
    while( ( d_ent = readdir(dir) ) != NULL )
    {
      if( strstr( d_ent->d_name, ".so") )
      {
	  lt_dlhandle handle = lt_dlopen( d_ent->d_name );
	  if( handle == NULL){
	    fprintf(stderr, "lt_dlopen %s failed\n", d_ent->d_name );
	  }
	  else{ 
	    create_imf = (createImf_t*) lt_dlsym( handle, "createImf" );
	    destroy_imf = (destroyImf_t*) lt_dlsym( handle, "destroyImf" );
	  }

	  if( !create_imf || !destroy_imf ){
	     fprintf(stderr, "lt_dlsym %s failed\n", d_ent->d_name );
	  }
	  
	  Imf* i;
	  i=create_imf();
	  if( i!=0 )
	  {
	    imfs.push_back(i);
	    cerr << "  Load Module[" << imfs.size() << "]: " << d_ent->d_name << endl;
	  }

      }
    }
    closedir(dir);
  }

}



Imf* nextImf()
{
  if( imfs.empty() )
  {
    return 0;
  }

  current_imf+=1;

  if( current_imf >= imfs.size() )
  {
    current_imf =0;
  }

  return imfs[current_imf];

}




void ucimf_init()
{
  prev_focus = false;
  imf = 0;
  cwm->attachWindow( stts->getWindow(), status_shift );
  cwm->attachWindow( prdt->getWindow(), preedit_shift );
  cwm->attachWindow( lkc->getWindow(), lookupchoice_shift );
  cwm->set_focus( false );
  scanImf();
  imf = imfs[current_imf];
  cerr << "UCIMF core intialized." << endl;
}

void ucimf_exit()
{
  if( imf !=0 )
  {
    delete imf;
    imf=0;
  }
}

void ucimf_switch( unsigned char *buf, int *p_buf_len )
{ 

  if( *p_buf_len !=5 || buf[0] != 27 || buf[1] !=91 || buf[2] != 50 || buf[4] != 126 )
  {
    return;
  }
  else
  {
      if(  buf[3] == 52 ) // F12
      {
	cwm->set_focus( !cwm->get_focus() );
	if(imf!=0)
	{
	   imf->refresh();
	}
      }
      else if( cwm->get_focus() && buf[3] == 51 ) // IM=ON && F11
      {
	if( cwm->get_focus() && imf !=0 )
	  imf->switch_im();
	else
	  cwm->set_focus( true );
      }
      else if( cwm->get_focus() && buf[3] == 49 ) // IM=ON && F10
      {
	prdt->clear();
	lkc->clear();
	stts->clear();

	imf = nextImf();
	if( imf!=0 )
	{
	  imf->refresh();
	}
      }
      else
      {
	return;
      }

      // Clear input buffer
      *p_buf_len = 0;
      buf[0]='\0';
  }


}

void ucimf_switch_raw( char *buf, int *p_buf_len )
{ 
	//char *buf;
	//size_t n = read(fd, buf, sizeof(buf));

	size_t n = (*p_buf_len);
	int kc;

	int i = 0;
	while (i < n) {
		const char *s;
		s = (buf[i] & 0x80) ? "release" : "press";
		char down = !(buf[i] & 0x80);

		if (i+2 < n && (buf[i] & 0x7f) == 0
		&& (buf[i+1] & 0x80) != 0
		&& (buf[i+2] & 0x80) != 0) {
			kc = ((buf[i+1] & 0x7f) << 7) |
			(buf[i+2] & 0x7f);
			i += 3;
		} else {
			kc = (buf[i] & 0x7f);
			i++;
		}
		printf( "keycode %3d %s\n", kc, s);
	}


  if( kc !=67 || kc != 68 || kc != 87 || kc != 88 )
  {
    return;
  }
  else
  {
      if(  kc == 88 ) // F12
      {
	cwm->set_focus( !cwm->get_focus() );
	if(imf!=0)
	{
	   imf->refresh();
	}
      }
      else if( cwm->get_focus() && kc == 87 ) // IM=ON && F11
      {
	if( cwm->get_focus() && imf !=0 )
	  imf->switch_im();
	else
	  cwm->set_focus( true );
      }
      else if( cwm->get_focus() && kc == 68 ) // IM=ON && F10
      {
	prdt->clear();
	lkc->clear();
	stts->clear();

	imf = nextImf();
	if( imf!=0 )
	{
	  imf->refresh();
	}
      }
      else
      {
	return;
      }

      // Clear input buffer
      *p_buf_len = 0;
      buf[0]='\0';
  }


}


char* ucimf_process_stdin( char *buf, int *p_ret )
{
  string input( buf, *p_ret );
  string output;

  if( cwm->get_focus() == false || input.size() == 0 || imf == 0  ) 
  {
    ; // do nothing...
  }
  else
  {
    // clean input buffer
    (*p_ret)=0; 
    strcpy(buf,"");

    output = imf->process_input( input );
    (*p_ret) = output.copy( buf, string::npos ); // need to be checked!
  }

  return buf;
}

char* ucimf_process_raw( char *buf, int *p_ret )
{

	//char *buf;
	//size_t n = read(fd, buf, sizeof(buf));

	size_t n = (*p_ret);
	int kc;
	char down;

	int i = 0;
	while (i < n) {
		const char *s;

		s = (buf[i] & 0x80) ? "release" : "press";
		down = !(buf[i] & 0x80);


		if (i+2 < n && (buf[i] & 0x7f) == 0
		&& (buf[i+1] & 0x80) != 0
		&& (buf[i+2] & 0x80) != 0) {
			kc = ((buf[i+1] & 0x7f) << 7) |
			(buf[i+2] & 0x7f);
			i += 3;
		} else {
			kc = (buf[i] & 0x7f);
			i++;
		}
	}

	if( !down )
	{
		return buf;
	}

	unsigned short sym = keycode_to_keysym( kc, down );
	char *str = keysym_to_term_string(sym, down);


  string input( str );
  string output;

  if( cwm->get_focus() == false || input.size() == 0 || imf == 0  ) 
  {
    ; // do nothing...
  }
  else
  {
    // clean input buffer
    (*p_ret)=0; 
    strcpy(buf,"");

    output = imf->process_input( input );
    (*p_ret) = output.copy( buf, string::npos ); // need to be checked!
  }

  return buf;
}


void ucimf_cursor_position( int x, int y)
{
  cwm->set_position( x, y );
}

void ucimf_refresh_begin()
{
  prev_focus = cwm->get_focus();
  cwm->set_focus( false );
}

void ucimf_refresh_end()
{
  cwm->set_focus( prev_focus );
}


void init_keycode_state()
{
	npadch = -1;
	shift_state = 0;
	bzero(key_down, sizeof(char) * NR_KEYS);
	bzero(shift_down, sizeof(char) * NR_SHIFT);
	ioctl(STDIN_FILENO, KDGKBLED, &lock_state);
}


void update_term_mode(char crlf, char appkey, char curo)
{
    cr_with_lf = crlf;
    applic_keypad = appkey;
    cursor_esco = curo;
}


/*
 * down 的型別是 char, 但內容只是 1 (Press) 或是 0 (Release)
 */
unsigned short keycode_to_keysym(unsigned short keycode, char down)
{
	if (keycode >= NR_KEYS) return K_HOLE;

	char rep = (down && key_down[keycode]);
	key_down[keycode] = down;

	struct kbentry ke;
	ke.kb_table = shift_state;
	ke.kb_index = keycode;

	if (ioctl(STDIN_FILENO, KDGKBENT, &ke) == -1) return K_HOLE;

	if (KTYP(ke.kb_value) == KT_LETTER && (lock_state & K_CAPSLOCK)) {
		ke.kb_table = shift_state ^ (1 << KG_SHIFT);
		if (ioctl(STDIN_FILENO, KDGKBENT, &ke) == -1) return K_HOLE;
	}

	if (ke.kb_value == K_HOLE || ke.kb_value == K_NOSUCHMAP) return K_HOLE;

	unsigned value = KVAL(ke.kb_value);

	switch (KTYP(ke.kb_value)) {
	case KT_LETTER:
		ke.kb_value = K(KT_LATIN, value);
		break;

	case KT_SPEC:
		switch (ke.kb_value) {
		case K_NUM:
			if (applic_keypad) break;
		case K_BARENUMLOCK:
		case K_CAPS:
		case K_CAPSON:
			if (down && !rep) {
				if (value == KVAL(K_NUM) || value == KVAL(K_BARENUMLOCK)) lock_state ^= K_NUMLOCK;
				else if (value == KVAL(K_CAPS)) lock_state ^= K_CAPSLOCK;
				else if (value == KVAL(K_CAPSON)) lock_state |= K_CAPSLOCK;

				ioctl(STDIN_FILENO, KDSKBLED, lock_state);
			}
			break;

		default:
			break;
		}
		break;

	case KT_SHIFT:
		if (value >= NR_SHIFT || rep) break;

		if (value == KVAL(K_CAPSSHIFT)) {
			value = KVAL(K_SHIFT);

			if (down && (lock_state & K_CAPSLOCK)) {
				lock_state &= ~K_CAPSLOCK;
				ioctl(STDIN_FILENO, KDSKBLED, lock_state);
			}
		}
		
		if (down) shift_down[value]++;
		else if (shift_down[value]) shift_down[value]--;

		if (shift_down[value]) shift_state |= (1 << value);
		else shift_state &= ~(1 << value);

		break;

	case KT_DEAD:
	case KT_LOCK:
	case KT_SLOCK:
	case KT_BRL:
		printf("not support!\n");
		break;

	default:
		break;
	}

	return ke.kb_value;
}

unsigned short keypad_keysym_redirect(unsigned keysym)
{
	if (applic_keypad || KTYP(keysym) != KT_PAD || KVAL(keysym) >= NR_PAD) return keysym;
	
	#define KL(val) K(KT_LATIN, val)
	static const unsigned short num_map[] = {
		KL('0'), KL('1'), KL('2'), KL('3'), KL('4'),
		KL('5'), KL('6'), KL('7'), KL('8'), KL('9'),
		KL('+'), KL('-'), KL('*'), KL('/'), K_ENTER,
		KL(','), KL('.'), KL('?'), KL('('), KL(')'),
		KL('#')
	};

	static const unsigned short fn_map[] = {
		K_INSERT, K_SELECT, K_DOWN, K_PGDN, K_LEFT,
		K_P5, K_RIGHT, K_FIND, K_UP, K_PGUP,
		KL('+'), KL('-'), KL('*'), KL('/'), K_ENTER,
		K_REMOVE, K_REMOVE, KL('?'), KL('('), KL(')'),
		KL('#')
	};
	
	if (lock_state & K_NUMLOCK) return num_map[keysym - K_P0];
	return fn_map[keysym - K_P0];
}


static unsigned to_utf8(unsigned c, char *buf)
{
	unsigned index = 0;

	if (c < 0x80)
		buf[index++] = c;
	else if (c < 0x800) {
		// 110***** 10******
		buf[index++] = 0xc0 | (c >> 6);
		buf[index++] = 0x80 | (c & 0x3f);
	} else if (c < 0x10000) {
		if (c >= 0xD800 && c < 0xE000)
			return index;
		if (c == 0xFFFF)
			return index;
		// 1110**** 10****** 10******
		buf[index++] = 0xe0 | (c >> 12);
		buf[index++] = 0x80 | ((c >> 6) & 0x3f);
		buf[index++] = 0x80 | (c & 0x3f);
	} else if (c < 0x200000) {
		// 11110*** 10****** 10****** 10******
		buf[index++] = 0xf0 | (c >> 18);
		buf[index++] = 0x80 | ((c >> 12) & 0x3f);
		buf[index++] = 0x80 | ((c >> 6) & 0x3f);
		buf[index++] = 0x80 | (c & 0x3f);
	}

	return index;
}


char *keysym_to_term_string(unsigned short keysym, char down)
{
	static struct kbsentry kse;
	char *buf = (char *)kse.kb_string;
	*buf = 0;

	if (KTYP(keysym) != KT_SHIFT && !down) return buf;

	keysym = keypad_keysym_redirect(keysym);
	unsigned index = 0, value = KVAL(keysym);


	switch (KTYP(keysym)) {
	case KT_LATIN:
		index = to_utf8(value, buf);
		break;

	case KT_FN:
		kse.kb_func = value;
		ioctl(STDIN_FILENO, KDGKBSENT, &kse);
		index = strlen(buf);
		break;

	case KT_SPEC:
		if (keysym == K_ENTER) {
			buf[index++] = '\r';
			if (cr_with_lf) buf[index++] = '\n';
		} else if (keysym == K_NUM && applic_keypad) {
			buf[index++] = '\e';
			buf[index++] = 'O';
			buf[index++] = 'P';
		}
		break;

	case KT_PAD:
		if (applic_keypad && !shift_down[KG_SHIFT]) {
			if (value < NR_PAD) {
				static const char app_map[] = "pqrstuvwxylSRQMnnmPQS";

				buf[index++] = '\e';
				buf[index++] = 'O';
				buf[index++] = app_map[value];
			}
		} else if (keysym == K_P5 && !(lock_state & K_NUMLOCK)) {
			buf[index++] = '\e';
			buf[index++] = (applic_keypad ? 'O' : '[');
			buf[index++] = 'G';
		}
		break;

	case KT_CUR:
		if (value < 4) {
			static const char cur_chars[] = "BDCA";

			buf[index++] = '\e';
			buf[index++] = (cursor_esco ? 'O' : '[');
			buf[index++] = cur_chars[value];
		}
		break;

	case KT_META: {
		long flag;
		ioctl(STDIN_FILENO, KDGKBMETA, &flag);

		if (flag == K_METABIT) {
			buf[index++] = 0x80 | value;
		} else {
			buf[index++] = '\e';
			buf[index++] = value;
		}
		break;
	}

	case KT_SHIFT:
		if (!down && npadch != -1) {
			index = to_utf8(npadch, buf);
			npadch = -1;
		}
		break;
	
	case KT_ASCII:
		if (value < NR_ASCII) {
			int base = 10;

			if (value >= KVAL(K_HEX0)) {
				base = 16;
				value -= KVAL(K_HEX0);
			}
			
			if (npadch == -1) npadch = value;
			else npadch = npadch * base + value;
		}
		break;

	default:
		break;
	}

	buf[index] = 0;
	return buf;
}
