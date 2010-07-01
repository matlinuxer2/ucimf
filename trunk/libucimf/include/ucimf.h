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

#ifdef __cplusplus
extern "C" {
#endif

/* initialize & finalize */
void ucimf_init();
void ucimf_exit();

/* on/off switch */
void ucimf_switch( unsigned char* buf, int* ptr_buf_len );
void ucimf_switch_raw( char* buf, int* ptr_buf_len );
  
/* stdin(keyboard) input */
char* ucimf_process_stdin( char* buf, int* ptr_buf_len );
char* ucimf_process_raw( char* buf, int* ptr_buf_len );

/* cursor position */
void ucimf_cursor_position( int x, int y);

/* console refresh */
void ucimf_refresh_begin();
void ucimf_refresh_end();

#ifdef __cplusplus
}
#endif
