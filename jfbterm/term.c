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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <dlfcn.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pwd.h>
#include <pty.h>
#include <utmp.h>
#include <grp.h>

#include "term.h"
#include "vterm.h"
#include "fbcommon.h"
#include "font.h"
#include "message.h"
#include "main.h"
#include "util.h"

#include "config.h"

int gChildProcessId = 0;

TTerm gTerm;

void tterm_wakeup_shell(TTerm* p, const char* tn);
void tterm_final(TTerm* p);

static void tterm_set_utmp(TTerm* p);
static void tterm_reset_utmp(TTerm* p);

void send_hangup(
	int closure)
{
	if (gChildProcessId) {
		kill(gChildProcessId, SIGHUP);
	}
}

void sigchld(sig) int sig; {
	int st;
	int ret;
	ret = wait(&st);
	if (ret == gChildProcessId || ret == ECHILD) {
		tvterm_unregister_signal();
		tterm_final(&gTerm);
		exit(EXIT_SUCCESS);
	}
	signal(SIGCHLD, sigchld);
}


void tterm_init(TTerm* p, const char* en)
{
	p->ptyfd = -1;
	p->ttyfd = -1;
	p->name[0] = '\0';
	tcgetattr(0, &(p->ttysave));
	tvterm_init(&(p->vterm), p,
		    gFramebuffer.width/gFontsWidth,
		    gFramebuffer.height/gFontsHeight, 
		    &(gApp.gCaps), en);
}

void tterm_final(TTerm* p)
{
	tterm_reset_utmp(p);
	tvterm_final(&(p->vterm));
}

void application_final(void)
{
	TTerm* p = &gTerm;
/*
	write(1, "\x1B[?25h", 6);
*/
	tcsetattr(0, TCSAFLUSH, &(p->ttysave));
	tterm_final(p);

	tfbm_close(&gFramebuffer);
	tfont_ary_final();
}
	

int tterm_get_ptytty(TTerm* p)
{
	if (openpty(&p->ptyfd, &p->ttyfd, p->name, NULL, NULL) < 0) {
	    print_strerror("openpty");
	    return 0;
	}
	return 1;
}

#define BUF_SIZE 1024
void tterm_start(TTerm* p, const char* tn, const char* en)
{
	struct termios ntio;

	int ret;
	struct timeval tv;
	u_char buf[BUF_SIZE+1];
#ifdef JFB_ENABLE_DIMMER
	u_int idle_time = 0;
	u_int blank = 0;
	int tfbm_set_blank(int, int);
#  define DIMMER_TIMEOUT (3 * 60 * 10)        /* 3 min */
#endif

	tterm_init(p, en);
	if (!tterm_get_ptytty(p)) {
		die("Cannot get free pty-tty.\n");
	}

	ntio = p->ttysave;
	ntio.c_lflag &= ~(ECHO|ISIG|ICANON|XCASE);
        ntio.c_iflag = 0;
        ntio.c_oflag &= ~OPOST;
        ntio.c_cc[VMIN] = 1;
        ntio.c_cc[VTIME] = 0;
	ntio.c_cflag |= CS8;
        ntio.c_line = 0;
	tcsetattr(0, TCSAFLUSH, &ntio);
/*
	write(1, "\x1B[?25l", 6);
*/

	tvterm_start(&(p->vterm));
	fflush(stdout);
	gChildProcessId = fork();
	if (gChildProcessId == 0) {
	    /* child */
	    tterm_wakeup_shell(p, tn);
	    exit(1);
	} else if (gChildProcessId < 0) {
	    print_strerror("fork");
	    exit(1);
	}
	/* parent */
	tterm_set_utmp(p);
	signal(SIGCHLD, sigchld);
	atexit(application_final);

	/* not available
	 * VtInit();
	 * VtStart();
	 */
	for (;;) {
		fd_set fds;
		int max = 0;
		tv.tv_sec = 0;
		tv.tv_usec = 100000;	// 100 msec
		FD_ZERO(&fds);
		FD_SET(0,&fds);
		FD_SET(p->ptyfd,&fds);
		if (p->ptyfd > max) max = p->ptyfd;
		ret = select(max+1, &fds, NULL, NULL, &tv);
                if (ret == 0 || (ret < 0 && errno == EINTR)) {
#ifdef JFB_ENABLE_DIMMER
			if (!blank && ++idle_time > DIMMER_TIMEOUT) {
				// Goto blank
				idle_time = 0;
				if (tfbm_set_blank(gFramebuffer.fh, 1))
					blank = 1;
			}
#endif
			continue;
		}

		if (ret < 0) {
			print_strerror_and_exit("select");
		}
		if (FD_ISSET(0, &fds)) {
			ret = read(0, buf, BUF_SIZE);
#ifdef JFB_ENABLE_DIMMER
			idle_time = 0;
			if (blank) {
				// Wakeup console
				blank = 0;
				tfbm_set_blank(gFramebuffer.fh, 0);
			}
#endif
			if (ret > 0) {
				write(p->ptyfd, buf, ret);
			}
		} else if (FD_ISSET(p->ptyfd,&fds)) {
			ret = read(p->ptyfd, buf, BUF_SIZE);
			if (ret > 0) {
				/* write(1, buf, ret); */
				tvterm_emulate(&(p->vterm), buf, ret);
				tvterm_refresh(&(p->vterm));
			}
		}
	}
}

void tterm_wakeup_shell(TTerm* p, const char* tn)
{
	setenv("TERM", tn, 1);
	close(p->ptyfd);
	login_tty(p->ttyfd);
	tcsetattr(0, TCSANOW, &(p->ttysave));
	setgid(getgid());
	setuid(getuid());
	sleep(1); /* XXX: wait vt swtich completed? */
	execvp(gApp.gExecShell, gApp.gExecShellArgv);
	exit(1);
}


void	tterm_set_utmp(TTerm* p)
{
	struct utmp	utmp;
	struct passwd	*pw;
	char	*tn;

	pw = getpwuid(util_getuid());
	tn = rindex(p->name, '/') + 1;
	memset((char *)&utmp, 0, sizeof(utmp));
	strncpy(utmp.ut_id, tn + 3, sizeof(utmp.ut_id));
	utmp.ut_type = DEAD_PROCESS;
	setutent();
	getutid(&utmp);
	utmp.ut_type = USER_PROCESS;
	utmp.ut_pid = getpid();
	if (strncmp("/dev/", p->name, 5) == 0)
	    tn = p->name + 5;
	strncpy(utmp.ut_line, tn, sizeof(utmp.ut_line));
	strncpy(utmp.ut_user, pw->pw_name, sizeof(utmp.ut_user));
	time(&(utmp.ut_time));
	pututline(&utmp);
	endutent();
}

void	tterm_reset_utmp(TTerm* p)
{
	struct utmp	utmp, *utp;
	char	*tn;

	tn = rindex(p->name, '/') + 4;
	memset((char *)&utmp, 0, sizeof(utmp));
	strncpy(utmp.ut_id, tn, sizeof(utmp.ut_id));
	utmp.ut_type = USER_PROCESS;
	setutent();
	utp = getutid(&utmp);
	utp->ut_type = DEAD_PROCESS;
	memset(utp->ut_user, 0, sizeof(utmp.ut_user));
	utp->ut_type = DEAD_PROCESS;
	time(&(utp->ut_time));
	pututline(utp);
	endutent();
}


