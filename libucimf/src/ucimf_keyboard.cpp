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

#include <sys/ioctl.h>
#include <linux/kd.h>
#include <linux/keyboard.h>
#include <linux/input.h>
#include <stdlib.h>
#include <iostream>
#include <fcntl.h>
#include "debug.h"
using namespace std;

int LogFd = -1;

/*
* IM  Toggle  -> F12
* IM  Switch  -> F11 ( When Imf Toggle On )
* IMF Switch  -> F10 ( When Imf Toggle On )
* IMF Load    -> F9 ( When Imf Toggle On )
*/


typedef struct _new_key
{
  int table;
  int key;
  int new_key;
}
new_key;

static new_key nkeys[] = {
  /* [table]        [key]         [new_key] */
  {(1 << KG_CTRL), KEY_SPACE,     	K_F12 },	/* IM Toggle */
  {(1 << KG_CTRL), KEY_LEFTSHIFT, 	K_F11 },	/* IM Switch */
  {(1 << KG_CTRL), KEY_RIGHTSHIFT,      K_F10 },	/* IMF Switch */
};

int setup_keys()
{
  for (size_t i = 0; i < sizeof (nkeys) / sizeof (new_key); i++)
  {
    struct kbentry entry;
    struct kbentry tmp;
    entry.kb_table = nkeys[i].table;
    entry.kb_index = nkeys[i].key;

        /* save current value */
    int ret;
    UrINFO( "Getting KeyBoard Event...\n" );
    ret = ioctl (0, KDGKBENT, &entry);
    if( ret == -1 )
    {
       UrINFO( "Get KeyBoard Event Failed\n" );
    }
    tmp.kb_value = entry.kb_value;

    entry.kb_value = nkeys[i].new_key;
    UrINFO("Setting KeyBoard Event...\n" );
    ret=ioctl (0, KDSKBENT, &entry);
    if( ret == -1 )
    {
       UrINFO( "Set KeyBoard Event Failed\n" );
    }
    nkeys[i].new_key = tmp.kb_value;
  }
  return 0;
}

int main()
{
	char name[64];
	snprintf(name, sizeof(name), "%s/%s", getenv("HOME"), ".ucimf-log");

	extern int LogFd;
	LogFd = open(name, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
	if ( LogFd == -1){
		printf("open log file error");
	}  

	setup_keys();

	extern int LogFd;
	if( LogFd >=0 )
	{
		close(LogFd);
	}
	return 0;
}
