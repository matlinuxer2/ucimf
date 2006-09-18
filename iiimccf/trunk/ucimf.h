#ifdef __cplusplus
extern "C" {
#endif

/* initialize & finalize */
void ucimf_init();
void ucimf_exit();

/* on/off switch */
void ucimf_switch();
  
/* stdin(keyboard) input */
char* ucimf_process_stdin( char** buf, int* buf_len );

/* cursor position */
void ucimf_cursor_position( int x, int y);

/* console refresh */
void ucimf_refresh_begin();
void ucimf_refresh_end();

#ifdef __cplusplus
}
#endif
