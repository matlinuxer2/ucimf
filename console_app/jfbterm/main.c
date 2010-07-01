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

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/vt.h>
#include <sys/kd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <getopt.h>
#include <locale.h>
#include <langinfo.h>
#include <iconv.h>

#include "pcf.h"
#include "font.h"
#include "fbcommon.h"
#include "term.h"
#include "vterm.h"
#include "message.h"
#include "mytypes.h"
#include "getcap.h"
#include "util.h"
#include "csv.h"
#include "main.h"

#ifdef SYSCONFDIR
#define	FILE_JFBTERM_CONF	SYSCONFDIR"/jfbterm.conf"
#else
#define	FILE_JFBTERM_CONF	"/etc/jfbterm.conf"
#endif

TFrameBufferMemory gFramebuffer;

static void tapp_get_options(TApplication* p, int argc, char *argv[])
{
	static struct option optList[] = {
		{"shell",	1, NULL, 'e'},
		{"exec",	1, NULL, 'e'},
		{"config",	1, NULL, 'f'},
		{"encoding",	1, NULL, 'c'},
		{"charmap",	1, NULL, 'c'},
		{"reset",	2, NULL, 'r'},
		{"quite",	0, NULL, 'q'},
		{"help",	0, NULL, 'h'},
		{NULL,		0, NULL, 0},
	};
	int c;
	int ix;
	int i;
	
	while (1) {
		c = getopt_long(argc, argv, "c:f:e:hqr::", optList, &ix);
		if (c == EOF) {
			break;
		}
		switch (c) {
		case 'e':
			p->gExecShell = optarg;
			goto end;
		case 'f':
			p->gConfFile = optarg;
			break;
		case 'c':
			p->gConfEncoding = optarg;
			break;
		case 'r':
			p->gOptQuiet = TRUE;
			p->gOptReset = optarg;
			if (p->gOptReset == NULL)
				p->gOptReset = "locale";
			break;
		case 'q':
			p->gOptQuiet = TRUE;
			break;
		case 'h':
			p->gOptShowHelpQ = TRUE;
			break;
		default:
			break;
		}
	}
     end:
	p->gExecShellArgv = (char**)calloc(argc + 2 - optind, sizeof(char*));
	p->gExecShellArgv[0] = p->gExecShell;
	for (i = 0 ; i < argc+1 - optind ; i++) {
		p->gExecShellArgv[i+1] = argv[i + optind];
	}
}

void tapp_change_to_original_console(TApplication* p)
{
        int cfd;
	int n = p->gOrigVirtualConsole;

/*
        signal(SIGUSR1, SIG_DFL);
        signal(SIGUSR2, SIG_DFL);
*/

	util_privilege_on();
	cfd = open("/dev/console", O_WRONLY);
	if (cfd < 0 && 
	    (cfd = open("/dev/console", O_RDONLY)) < 0) {
		util_privilege_off();
                print_strerror("/dev/console");
		return;
	}
	if (ioctl(cfd, VT_ACTIVATE, n) != 0) {
		util_privilege_off();
		print_warn("can't activate VC(%d)", n);
	}
	util_privilege_off();
	close(cfd);
}

void tapp_final(TApplication* p)
{
	if (p->gCapsQ) {
		tcaps_final(&(p->gCaps));
	}
	util_free(p->gExecShellArgv);
}

TApplication gApp;

void tapp_final_at_exit(void)
{
	tapp_final(&gApp);
}

void tapp_init(TApplication* p)
{
	static char shell[128];
	
	p->gOrigVirtualConsole	= -1;
	tcaps_init(&(p->gCaps));
	p->gCapsQ	= TRUE;
	p->gOptShowHelpQ = FALSE;
	if (getenv("SHELL")) {
		/* This cause a buffer overflow. */
		memset (shell, '\0', sizeof shell);
		strncpy(shell, getenv("SHELL"), sizeof shell - 1);
		p->gExecShell = shell;
	} else {
		p->gExecShell = "/bin/sh";
	}
	p->gExecShellArgv = NULL;
	p->gConfFile = FILE_JFBTERM_CONF;
	p->gConfEncoding = NULL;
	signal(SIGCHLD, sigchld);
	atexit(tapp_final_at_exit);
}

void tapp_change_to_new_console(TApplication* p)
{
        struct vt_stat vts;
        int cfd;
        int vfd;
        int vtNum;
        int child;
        char vtty[128];
        int mode;

        cfd = util_privilege_open("/dev/console", O_WRONLY);
        if (cfd < 0 && 
	    (cfd = util_privilege_open("/dev/console", O_RDONLY)) < 0) {
                die("can't open /dev/console");
        }
#if 1
        ioctl(cfd, KDGETMODE, &mode);
        if (mode == KD_TEXT) {
                close(cfd);
                return;
        }
#endif
        ioctl(cfd, VT_GETSTATE, &vts);
	p->gOrigVirtualConsole	= vts.v_active;

        ioctl(cfd, VT_OPENQRY, &vtNum);
        if (vtNum < 0) {
                die("can't get free VC");
        }
	fflush(stdout);
	fflush(stderr);
        if ((child = fork()) == -1) {
                print_strerror_and_exit("fork");
        }
        if (child) {
		/* parent - waiting child process, and reset VC */
                print_message("\r\nJFBTERM> switched to new VC\r\n");
	        if (waitpid(child, NULL, 0) < 0) {
			print_strerror("waitpid");
		}
		tapp_change_to_original_console(p);
                exit(EXIT_SUCCESS);
        }
        setsid();
        sprintf(vtty, "/dev/tty%d", vtNum);
	util_privilege_on();
        if ((vfd = open(vtty, O_RDWR)) < 0) {
		util_privilege_off();
                die("can't open %s", vtty);
        }
        if (ioctl(cfd, VT_ACTIVATE, vtNum) != 0) {
		util_privilege_off();
                die("can't activate VC(%d)", vtNum);
        }
	util_privilege_off();
        close(cfd);
        dup2(vfd, 0);
        dup2(vfd, 1);
        dup2(vfd, 2);
}

void ShowCaps(void)
{
	fprintf(stdout,
		"======== Capabilities =====================================\n"
		"[[ COLOR ]]\n"
#ifdef JFB_VGA16FB
			" VGA16"
#endif
#ifdef JFB_PSEUDOCOLOR
			" Pseudo"
#endif
#ifdef JFB_DIRECTCOLOR
			" Direct"
#endif
#ifdef JFB_TRUECOLOR
			" True"
#endif
		"\n"
		"[[ BITS ]]\n"
#ifdef JFB_1BPP
			" 1bpp"
#endif
#ifdef JFB_2BPP
			" 2bpp"
#endif
#ifdef JFB_4BPP
			" 4bpp"
#endif
#ifdef JFB_VGA16FB
			" 4bpp(VGA)"
#endif
#ifdef JFB_8BPP
			" 8bpp"
#endif
#ifdef JFB_15BPP
			" 15bpp"
#endif
#ifdef JFB_16BPP
			" 16bpp"
#endif
#ifdef JFB_24BPP
			" 24bpp"
#endif
#ifdef JFB_32BPP
			" 32bpp"
#endif
		"\n"
	);
	fprintf(stdout, "[[ FONTSET ]]\n");
	tfont_ary_show_list(stdout);

	fprintf(stdout, "[[ ENCODING ]]\n");
#ifdef JFB_MINI_JFBTERM
	fprintf(stdout, " ISO-2022-JP EUC-JP");
#else
	fprintf(stdout, " ISO-2022-*");
#ifdef JFB_UTF7
	fprintf(stdout, " UTF-7");
#endif
#ifdef JFB_UTF8
	fprintf(stdout, " UTF-8");
#endif
#ifdef JFB_TRON
	fprintf(stdout, " TRON");
#endif	/* JFB_TRON */
#ifdef JFB_OTHER_CODING_SYSTEM
	fprintf(stdout, " OTHER-CODING-SYSTEM");
#endif
#endif	/* JFB_MINI_JFBTERM */
	fprintf(stdout, "\n");
	fprintf(stdout, "[[ MISC ]]\n"
#ifdef DEBUG
			" DEBUG"
#endif
		"\n"
	);

}

char *
tapp_setup_encoding(char *en)
{
	/* if quoted, remove it */
	if (en && ((en[0] == '"' && en[strlen(en)-1] == '"') 
		   || (en[0] == '\'' && en[strlen(en)-1] == '\''))) {
		en = en + 1;
		en[strlen(en)-1] = '\0';
	}
	if (en == NULL || strcmp(en, "locale") == 0) {
		setlocale(LC_ALL, "");
		en = nl_langinfo(CODESET);
		print_message("ENCODING: locale = %s\n", en);
	}
	if (en && (strchr(en, ',') == NULL)) {
		char *encname = en;
		en = tcaps_find_entry(&(gApp.gCaps), "encoding.", encname);
		if (!en) {
			/* not found */
			iconv_t cd = iconv_open("UCS-2BE", encname);
			if (cd != (iconv_t)(-1)) {
				iconv_close(cd);
				en = (char *)malloc(strlen("other,iconv,UTF-8")+1+strlen(encname));
				if (en == NULL)
					die("not enough memory: encode");
				sprintf(en, "other,%s,iconv,UTF-8", encname);
			} else {
				print_error("%s not found, fallback to default\n", encname);
			}
		}
	}
	if (!en) {
		en = "G0,G1,ansix3.4-1968,ansix3.4-1968,iso8859.1-1987,ansix3.4-1968";
	}
	return en;
}

int main(int argc, char *argv[])
{
	TCapability* fcap;
	char* cp;
	const char* tn;
	char* en;

	util_privilege_init();
	tapp_init(&gApp);
	tapp_get_options(&gApp, argc, argv);

	print_message(
		"%s - Kanji on framebuffer console Version %s\n"
		"       Copyright (C) 2003 Fumitoshi UKAI\n"
		"	Copyright (C) 1999-2000 Noritoshi Masuichi\n"
		"This program is based on KON2\n"
		"	Copyright (C) 1992-1996 Takashi MANABE\n\n",
		PACKAGE, VERSION
	);

	if (gApp.gOptShowHelpQ) {
		ShowCaps();
		exit(EXIT_SUCCESS);
	}

	tcaps_read(&(gApp.gCaps), gApp.gConfFile);
	if (gApp.gOptReset) {
		en = tapp_setup_encoding(gApp.gOptReset);
		FILE *tf = fopen("/dev/tty", "w");
		if (tf == NULL)
			tf = stderr; /* XXX */
		tvterm_show_sequence(tf, &(gApp.gCaps), en);
		fclose(tf);
		printf("ENCODING: %s\n", en);
		exit(0);
	}

	en = tcaps_find_first(&(gApp.gCaps), "encoding");
	if (gApp.gConfEncoding) {
		en = gApp.gConfEncoding;
	}
	en = tapp_setup_encoding(en);

	fcap = tcaps_find(&(gApp.gCaps), "fontset"); 
	if (!fcap || !(fcap->values)) {
		print_error("no font specified.\n");
		exit(EXIT_FAILURE);
	}

	if ((cp = tcaps_find_first(&(gApp.gCaps), "color.gamma")) != NULL) {
		fbgamma = (float)atof(cp);
	}
	tfont_setup_fontlist(fcap->values);
	print_message("encoding : %s\n", en);
	print_message("exec : %s ", gApp.gExecShell);
	{
		int i;
		for (i = 1; gApp.gExecShellArgv[i]; i++)
			print_message("%s ", gApp.gExecShellArgv[i]);
	}

	tapp_change_to_new_console(&gApp);
	tfbm_init(&gFramebuffer);
	tfbm_open(&gFramebuffer);

	util_privilege_drop();
	tn = tcaps_find_first(&(gApp.gCaps), "term");
	if (!tn) {
		tn = "jfbterm";
	}
	
	tterm_start(&gTerm, tn, en);
	
	tfbm_close(&gFramebuffer);
	tfont_ary_final();
	exit(EXIT_SUCCESS);
}
