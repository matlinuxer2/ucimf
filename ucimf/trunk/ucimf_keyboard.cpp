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
#include <iostream>
using namespace std;

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
  {(1 << KG_CTRL), KEY_F9,        	K_F9 },		/* IMF Load */
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
    cerr << "Get KeyBoard Event..." << endl;
    ret = ioctl (0, KDGKBENT, &entry);
    tmp.kb_value = entry.kb_value;

    entry.kb_value = nkeys[i].new_key;
    cerr << "Set KeyBoard Event..." << endl;
    ret=ioctl (0, KDSKBENT, &entry);
    nkeys[i].new_key = tmp.kb_value;
  }
  return 0;
}

int main()
{
  setup_keys();
  return 0;
}
