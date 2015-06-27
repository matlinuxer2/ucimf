#include <ucimf.h>
#include <iostream>
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

#define BUF_SIZE 128

using namespace std;

int main()
{
	ucimf_init();
	ucimf_cursor_position( 300, 300);
	ucimf_refresh_begin();
	ucimf_refresh_end();
	struct timeval tv;
	int ret;
	u_char buf[BUF_SIZE+1];

	for (;;) {
		fd_set fds;
		int max = 0;
		tv.tv_sec = 0;
		tv.tv_usec = 100000;	// 100 msec
		FD_ZERO(&fds);
		FD_SET(0,&fds);
		FD_SET(1,&fds);
		if (1 > max) max = 1;
		ret = select(max+1, &fds, NULL, NULL, &tv);
		if (ret == 0 || (ret < 0 && errno == EINTR)) {
			continue;
		}

		if (ret < 0) {
			;//
		}
		if (FD_ISSET(0, &fds)) {
			ret = read(0, buf, BUF_SIZE);

			// ucimf input method framework switch on/off 
			ucimf_switch( buf, &ret );
			ucimf_process_stdin( (char*)buf, &ret );

			if (ret > 0) {
				write(1, buf, ret);
			}

		} 
		//else if (FD_ISSET(1,&fds)) {
		//	ret = read(1, buf, BUF_SIZE);
		//	if (ret > 0) {
		//		/* write(1, buf, ret); */
		//		tvterm_emulate(&(p->vterm), buf, ret);
		//		ucimf_refresh_begin();
		//		tvterm_refresh(&(p->vterm));
		//		ucimf_cursor_position( p->vterm.pen.x * gFontsWidth, p->vterm.pen.y*gFontsHeight);
		//		ucimf_refresh_end();
		//	}
		//}
	}

	ucimf_exit();
}
