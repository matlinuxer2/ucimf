#include <ucimf.h>
#include "imf.h"
#include "openvanilla.h"
#include "iiimccf.h"
#include "subject_observer.h"
#include <sys/ioctl.h>
#include <linux/kd.h>
#include <linux/keyboard.h>
#include <stdlib.h>
#include "widget.h"
#include "cwm.h"

bool prev_focus;
//ConsoleFocus *focus = ConsoleFocus::getInstance();
//CursorPosition *pos = CursorPosition::getInstance();
//CurrentImfStatus *sts = CurrentImfStatus::getInstance();
Status *stts = Status::getInstance();
Cwm *cwm = Cwm::getInstance();

Imf *imf;
char *input,*output;
int input_len, output_len;

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
  prev_focus = false;
  imf = new DummyImf;

  input =  (char*)malloc(sizeof(char)*10);
  output = (char*)malloc(sizeof(char)*10);
  input_len = 10;
  output_len = 10;

}

void ucimf_exit()
{
  restore_keys();
  delete imf;
  free(input);
  free(output);
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
	if( !cwm->get_focus() )
	  cwm->set_focus( true );
	else
	  cwm->set_focus( false );
      }
      else if( buf[0] == 204 )
      {
	if( cwm->get_focus() )
	  imf->switch_im(); 
	else
	  cwm->set_focus( true );
      }
      else if( buf[0] == 205 )
      {
	imf = IIIMCCF::getInstance();
	stts->set_imf_name("IIIMF");
      }
      else if( buf[0] == 206 )
      {
	imf = OVImf::getInstance();
	stts->set_imf_name("OpenVanilla");

      }
      else if( buf[0] == 207 )
      {
	// This is preserved for the future
	//imf = SCIMF::getInstance();
	imf = new DummyImf;
	stts->set_imf_name("Dummy");
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

  if( cwm->get_focus() == false || *p_ret == 0 || imf == 0  ) 
  {
    return buf;
  }

  if( input_len < *p_ret )
  {
    free(input);
    input_len = sizeof(char) * (*p_ret);
    input = (char*) malloc( input_len );
  }

  strcpy( input, buf );
  // clean input buffer
  (*p_ret)=0;
  strcpy(buf,"");

  char *result= imf->process_input( input );
  if( output_len < strlen(result) )
  {
    free(output);
    output_len = sizeof(char) * strlen(result);
    output = (char*) malloc( output_len );
  }
  strcpy( output, result );
  //strcpy( result, "\0");

  return output;
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
  cwm->set_focus( true );
  cwm->set_focus( prev_focus );
}
