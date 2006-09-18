
int keyinput_to_keyevent( char* buf, int buf_len, int* p_keycode, int* p_keychar, int* p_modifier );
int setup_keys(void);
void restore_keys(void);

enum
{
  CTRL_SPACE = 203,
  CTRL_SHIFT = 204,
};
