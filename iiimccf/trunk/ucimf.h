#include <stdint.h>
#include <iiimcf.h>

#ifdef __cplusplus
extern "C" {
#endif

/* initialization */
int ucimf_init();
int ucimf_exit();

/* status switching */
int ucimf_focus_on();
int ucimf_focus_off();

/* keyboard input */
char* ucimf_process_key( int keycode, int keychar, int modifier );

/* switch input mehtod frameworks */
enum IMF{ IIIMF, OVIME, SCIM };

int ucimf_switch_imf( IMF );

/* show input methods */
void ucimf_change_im();

/* current position */
void ucimf_pos( int x, int y);


#ifdef __cplusplus
}
#endif
