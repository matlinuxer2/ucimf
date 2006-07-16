#ifdef __cplusplus
extern "C" {
#endif

/* stdin(keyboard) input */
char* ucimf_process_stdin( char** buf, int* ret );

/* cursor position */
void ucimf_cursor_position( int x, int y);


#ifdef __cplusplus
}
#endif
