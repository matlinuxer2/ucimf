/*
 * JFBTERM -
 * Copyright (c) 2003 Fumitoshi UKAI <ukai@debian.or.jp>
 * Copyright (C) 1999  Noritoshi MASUICHI (nmasu@ma3.justnet.ne.jp)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY NORITOSHI MASUICHI ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL NORITOSHI MASUICHI BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include "config.h"

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_IOPERM
#include <sys/io.h>
#endif
#include <fcntl.h>

#include "util.h"

static uid_t owner_uid;
static uid_t runner_uid;

void
util_privilege_init()
{
    runner_uid = getuid();
    owner_uid = geteuid();	/* if setuid, owner_uid is root */
    /* swap real and effective */
    setreuid(owner_uid, runner_uid);
}

void
util_privilege_on()
{
    setreuid(runner_uid, owner_uid);
}

void
util_privilege_off()
{
    setreuid(owner_uid, runner_uid);
}

int
util_privilege_open(char *pathname, int flags)
{
    int fd;
    setreuid(runner_uid, owner_uid);
    fd = open(pathname, flags);
    setreuid(owner_uid, runner_uid);
    return fd;
}

#ifdef HAVE_IOPERM
int
util_privilege_ioperm(unsigned long from, unsigned int num, int turn_on)
{
    int r;
    setreuid(runner_uid, owner_uid);
    r = ioperm(from, num, turn_on);
    setreuid(owner_uid, runner_uid);
    return r;
}
#endif

uid_t
util_getuid()
{
    return runner_uid;
}

void
util_privilege_drop()
{
    setreuid(runner_uid, runner_uid);
}

void util_euc_to_sjis(u_char* ch, u_char* cl)
{
    u_char  nh, nl;
    
    nh = ((*ch - 0x21) >> 1) + 0x81;
    if (nh > 0x9F) nh += 0x40;
    if (*ch & 1) {
	nl = *cl + 0x1F;
	if (*cl > 0x5F)
	    nl ++;
    } else nl = *cl + 0x7E;
    *cl = nl;
    *ch = nh;
}

void util_sjis_to_jis(u_char* ch, u_char* cl)
{
    u_char  nh = *ch;
    u_char  nl = *cl;
	
    nh -= (nh > 0x9F) ? 0xB1: 0x71;
    nh = nh * 2 + 1;
    if (nl > 0x9E) {
        nl = nl - 0x7E;
        nh ++;
    } else {
        if (nl > 0x7E) nl --;
        nl -= 0x1F;
    }
    *cl = nl;
    *ch = nh;
}

int util_search_string(const char* s, const char** array)
{
	int i;
	for (i = 0 ; array[i] ; i++) {
		if (strcmp(array[i], s) == 0) {
			return i;
		}
	}
	return -1;	
}

