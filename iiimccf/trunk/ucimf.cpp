#include <ucimf.h>
#include "imf.h"
#include "openvanilla.h"
#include "iiimccf.h"
#include "subject_observer.h"
#include <sys/ioctl.h>
#include <linux/kd.h>
#include <linux/keyboard.h>


int focus,prev_focus;
TrackPoint *cursor_position;
Imf *imf;

enum
{
  CTRL_SPACE = 203,
  CTRL_SHIFT = 204,
  CTRL_F5 = 205,
  CTRL_F6 = 206,
  CTRL_F7 = 207,
};

typedef struct _new_key
{
  int table;
  int key;
  int new_key;
}
new_key;

static new_key nkeys[] = {
  /* [table]        [key]         [new_key] */
  {(1 << KG_CTRL), KEY_SPACE, K(KT_LATIN, CTRL_SPACE)},	/* Ctrl + Space */
  {(1 << KG_CTRL), KEY_LEFTSHIFT, K(KT_LATIN, CTRL_SHIFT)},	/* Ctrl + Shift */
  {(1 << KG_CTRL), KEY_F5, K(KT_LATIN, CTRL_F5)},	/* Ctrl + F5 */
  {(1 << KG_CTRL), KEY_F6, K(KT_LATIN, CTRL_F6)},	/* Ctrl + F6 */
  {(1 << KG_CTRL), KEY_F7, K(KT_LATIN, CTRL_F7)},	/* Ctrl + F7 */
};

int setup_keys()
{
  int i;
  for (i = 0; i < sizeof (nkeys) / sizeof (new_key); i++)
  {
    struct kbentry entry;
    struct kbentry tmp;
    entry.kb_table = nkeys[i].table;
    entry.kb_index = nkeys[i].key;

        /* save current value */
    ioctl (0, KDGKBENT, &entry);
    tmp.kb_value = entry.kb_value;

    entry.kb_value = nkeys[i].new_key;
    ioctl (0, KDSKBENT, &entry);

        /* save current value */
    nkeys[i].new_key = tmp.kb_value;
  }
  return 0;
}

void restore_keys()
{
  int i;
  for (i = 0; i < sizeof (nkeys) / sizeof (new_key); i++)
  {
    struct kbentry entry;
    entry.kb_table = nkeys[i].table;
    entry.kb_index = nkeys[i].key;
    entry.kb_value = nkeys[i].new_key;
    ioctl (0, KDSKBENT, &entry);
  }

}

void ucimf_init()
{
  setup_keys();
  focus = 0;
  prev_focus =0;
  cursor_position = new TrackPoint;
  imf = new DummyImf;

}

void ucimf_exit()
{
  restore_keys();
  delete cursor_position;
  delete imf;
}

void ucimf_switch( unsigned char *buf, int *p_buf_len )
{ 

  if(  *p_buf_len !=1  )
  {
    return ;
  }
  else
  {
      if(  buf[0] == 203 )
      {
	if( focus == 0 )
	  focus =1;
	else
	  focus =0;
      }
      else if( buf[0] == 204 )
      {
	if( focus == 1)
	  imf->switch_im(); 
	else
	  focus = 1;
      }
      else if( buf[0] == 205 )
      {
	imf = IIIMCCF::getInstance();
      }
      else if( buf[0] == 206 )
      {
	imf = OVImf::getInstance();

      }
      else if( buf[0] == 207 )
      {
	// This is preserved for the future
	//imf = SCIMF::getInstance();
	imf = new DummyImf;
      }
      else
      {
	return;
      }

      *p_buf_len = 0;
      buf[0]='\0';
  }


}


char* ucimf_process_stdin( char *buf, int *p_ret )
{
  buf[*p_ret]='\0';

  if( focus == 0 || *p_ret == 0 || imf == 0) 
  {
    return buf;
  }

  char *result= imf->process_input( buf );

  // clean input buffer
  *p_ret=0;
  strcpy(buf,"\0");

  return result;
}


void ucimf_cursor_position( int x, int y)
{
  if( cursor_position == 0 )
    TrackPoint *cursor_position = new TrackPoint;

  cursor_position->set_position( x, y );
}

void ucimf_refresh_begin()
{
  prev_focus = focus;
  focus = 0;
  // notify to push
}

void ucimf_refresh_end()
{
  focus = prev_focus;
  // notify to pop
}
