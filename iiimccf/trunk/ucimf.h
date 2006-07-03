#include <stdint.h>
#include <iiimcf.h>

#ifdef __cplusplus
extern "C" {
#endif

enum IMF_NAME{ 
  IIIMF, 
  OVIME, 
  SCIM 
};

/* status switching */
int ucimf_focus_on();
int ucimf_focus_off();
int ucimf_change_im();
int ucimf_switch_imf( IMF_NAME );

/* keyboard input */
char* ucimf_process_key( int keycode, int keychar, int modifier );

/* cursor position */
void ucimf_cursor_position( int x, int y);


#ifdef __cplusplus
}
#endif
