#include <iiimcf.h>
#include <iiimp-keycode.h>


#include "input.h"
#include <sys/ioctl.h>
#include <linux/input.h>
#include <linux/kd.h>
#include <sys/types.h>
#include <linux/keyboard.h>

/*
 *  Mapping: ASCII keychar --> IIIMF keycode
 */
static const int keychar_to_keycode[] = {
  // ascii code 00-0
	IIIMF_KEYCODE_UNDEFINED, 
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_TAB,	 

        // 10-19
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_CLEAR,
	IIIMF_KEYCODE_ENTER,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,

  	// 20-29
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_ESCAPE,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,

        // 30-39
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_SPACE,
	IIIMF_KEYCODE_EXCLAMATION_MARK,
	IIIMF_KEYCODE_QUOTEDBL,
	IIIMF_KEYCODE_NUMBER_SIGN,
	IIIMF_KEYCODE_DOLLAR,
	IIIMF_KEYCODE_UNDEFINED, 		/* % */
	IIIMF_KEYCODE_AMPERSAND,
	IIIMF_KEYCODE_QUOTE,

	// 40-49
	IIIMF_KEYCODE_LEFT_PARENTHESIS,
	IIIMF_KEYCODE_RIGHT_PARENTHESIS,
	IIIMF_KEYCODE_ASTERISK,
	IIIMF_KEYCODE_PLUS,
	IIIMF_KEYCODE_COMMA,
	IIIMF_KEYCODE_MINUS,
	IIIMF_KEYCODE_PERIOD,
	IIIMF_KEYCODE_SLASH,
	IIIMF_KEYCODE_0,
	IIIMF_KEYCODE_1,

	// 50-59
	IIIMF_KEYCODE_2,
	IIIMF_KEYCODE_3,
	IIIMF_KEYCODE_4,
	IIIMF_KEYCODE_5,
	IIIMF_KEYCODE_6,
	IIIMF_KEYCODE_7,
	IIIMF_KEYCODE_8,
	IIIMF_KEYCODE_9,
	IIIMF_KEYCODE_COLON,
	IIIMF_KEYCODE_SEMICOLON,

	// 60-69
	IIIMF_KEYCODE_LESS,
	IIIMF_KEYCODE_EQUALS,
	IIIMF_KEYCODE_GREATER,
	IIIMF_KEYCODE_UNDEFINED,		/* ? */
	IIIMF_KEYCODE_AT,
	IIIMF_KEYCODE_A,
	IIIMF_KEYCODE_B,
	IIIMF_KEYCODE_C,
	IIIMF_KEYCODE_D,
	IIIMF_KEYCODE_E,

	// 70-79
	IIIMF_KEYCODE_F,
	IIIMF_KEYCODE_G,
	IIIMF_KEYCODE_H,
	IIIMF_KEYCODE_I,
	IIIMF_KEYCODE_J,
	IIIMF_KEYCODE_K,
	IIIMF_KEYCODE_L,
	IIIMF_KEYCODE_M,
	IIIMF_KEYCODE_N,
	IIIMF_KEYCODE_O,

	// 80-89
	IIIMF_KEYCODE_P,
	IIIMF_KEYCODE_Q,
	IIIMF_KEYCODE_R,
	IIIMF_KEYCODE_S,
	IIIMF_KEYCODE_T,
	IIIMF_KEYCODE_U,
	IIIMF_KEYCODE_V,
	IIIMF_KEYCODE_W,
	IIIMF_KEYCODE_X,
	IIIMF_KEYCODE_Y,

	// 90-99
	IIIMF_KEYCODE_Z,
	IIIMF_KEYCODE_OPEN_BRACKET,
	IIIMF_KEYCODE_BACK_SLASH,
	IIIMF_KEYCODE_CLOSE_BRACKET,
	IIIMF_KEYCODE_CIRCUMFLEX,
	IIIMF_KEYCODE_UNDERSCORE,
	IIIMF_KEYCODE_BACK_QUOTE,
	IIIMF_KEYCODE_A,
	IIIMF_KEYCODE_B,
	IIIMF_KEYCODE_C,

	// 100-109
	IIIMF_KEYCODE_D,
	IIIMF_KEYCODE_E,
	IIIMF_KEYCODE_F,
	IIIMF_KEYCODE_G,
	IIIMF_KEYCODE_H,
	IIIMF_KEYCODE_I,
	IIIMF_KEYCODE_J,
	IIIMF_KEYCODE_K,
	IIIMF_KEYCODE_L,
	IIIMF_KEYCODE_M,

	// 110-119
	IIIMF_KEYCODE_N,
	IIIMF_KEYCODE_O,
	IIIMF_KEYCODE_P,
	IIIMF_KEYCODE_Q,
	IIIMF_KEYCODE_R,
	IIIMF_KEYCODE_S,
	IIIMF_KEYCODE_T,
	IIIMF_KEYCODE_U,
	IIIMF_KEYCODE_V,
	IIIMF_KEYCODE_W,

	// 120-126
	IIIMF_KEYCODE_X,
	IIIMF_KEYCODE_Y,
	IIIMF_KEYCODE_Z,
	IIIMF_KEYCODE_BRACELEFT,
	IIIMF_KEYCODE_UNDEFINED,		/* | */
	IIIMF_KEYCODE_BRACERIGHT,
	IIIMF_KEYCODE_DELETE,		/* ~ */
	IIIMF_KEYCODE_BACK_SPACE,		/* BACKSPACE */
};


int keyinput_to_keyevent( char* buf, int buf_len, int* p_keycode, int* p_keychar, int* p_modifier )
{
  if( buf_len== 1 )
  {
    int ich = (int) buf[0];
    *p_keychar = ich;
    *p_keycode = keychar_to_keycode[ ich ];
    *p_modifier = 0; // default as zero.
  }
  else 
    if( buf_len == 3 && buf[0]==27 && buf[1]==91 )
  {
    switch( buf[2] )
    {
      case 65:
	*p_keychar = 0;
	*p_keycode = IIIMF_KEYCODE_UP;
	*p_modifier = 0; // default as zero.
	break;

      case 66:
	*p_keychar = 0;
	*p_keycode = IIIMF_KEYCODE_DOWN;
	*p_modifier = 0; // default as zero.
	break;
	
      case 67:
	*p_keychar = 0;
	*p_keycode = IIIMF_KEYCODE_RIGHT;
	*p_modifier = 0; // default as zero.
	break;
	
      case 68:
	*p_keychar = 0;
	*p_keycode = IIIMF_KEYCODE_LEFT;
	*p_modifier = 0; // default as zero.
	break;

      default:
	break;
    }
  }
  else 
    if( buf_len == 4 && buf[0]==27 && buf[1]==91 && buf[4]==126 )
  {
    switch( buf[3] )
    {
      case 53:
	*p_keychar = 0;
	*p_keycode = IIIMF_KEYCODE_PAGE_UP;
	*p_modifier = 0; // default as zero.
	break;
	
      case 54:
	*p_keychar = 0;
	*p_keycode = IIIMF_KEYCODE_PAGE_DOWN;
	*p_modifier = 0; // default as zero.
	break;
      default:
	break;
    }
  }
  else
  {
    *p_keychar = 0;
    *p_keycode = 0;
    *p_modifier = 0; // default as zero.
  }
  
  return 0;
}

typedef struct _new_key
{
  int table;
  int key;
  int new_key;
}
new_key;


static new_key nkeys[] = {
  /* [table]        [key]         [new_key] */
  {(1 << KG_CTRL), KEY_SPACE, K (KT_LATIN, CTRL_SPACE)},	/* Ctrl + Space */
  {(1 << KG_CTRL), KEY_LEFTSHIFT, K (KT_LATIN, CTRL_SHIFT)},	/* Ctrl + Shift */
};

int setup_keys(void)
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

void restore_keys(void)
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
