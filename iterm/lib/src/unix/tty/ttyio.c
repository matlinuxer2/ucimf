/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html
*/
#include "../../../config.h"

#include "iterm/unix/ttyio.h"
#include "iterm/io.h"
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#define __USE_XOPEN
#include <stdlib.h>
#undef __USE_XOPEN
#include <unistd.h>
#include <stropts.h>

#include <string.h>
#include <strings.h>
#include <signal.h>
#include <errno.h>
#include <utmp.h>
#include <utmpx.h>
#include <pwd.h>
#include <time.h>

#if defined(HAVE_LIBUTEMPTER)
#include <utempter.h>
#endif

/*
 * ConcreteIO definitin, which is for pseudo tty base communication
 */
struct Concrete_IO
{
  int fd;
      /* file descriptor of this object */
  char slave_name[20];
      /* slave tty name */

  pid_t child;
      /* child process id */
  unsigned closed:1;
      /* flag if this file descriptor is closed or not */
};

#if defined(HAVE_LIBUTEMPTER)
#define write_utmp(tty_name,fd) addToUtmp(tty_name,NULL,fd);
#define delete_utmp(tty_name) removeFromUtmp();
#else
static void write_utmp(char *tty_name)
{
  struct utmpx entry;
  struct passwd *pw;
  char id[4];
  char *p;
  char *line;
  char *host;

  p = rindex(tty_name,'/') + 1;
  if( '0' <= *p && *p <= '9')
  {
        /* /dev/pty/? */
    id[0] = 'p';
    strncpy(id+1,p,2);
    line = tty_name + strlen("/dev/");
  }
  else
  {
        /* /dev/ttyp? */
    strncpy(id,p,3);
    line = p;
  }
  id[3] = '\0';
  memset(&entry,0,sizeof(entry));
  entry.ut_type = DEAD_PROCESS;
  strncpy(entry.ut_id,id,sizeof(id));
  setutent();
  getutxid(&entry);
  pw = getpwuid(getuid());
  entry.ut_type = USER_PROCESS;
  entry.ut_pid = getpid();
  strncpy(entry.ut_line, line, sizeof(entry.ut_line));
  strncpy(entry.ut_user, pw->pw_name, sizeof(entry.ut_user));

  host = getenv("DISPLAY");
  if(host)
    strncpy(entry.ut_host, host, sizeof(entry.ut_host));

  entry.ut_tv.tv_sec = time(NULL);
  entry.ut_tv.tv_usec = 0;
  pututxline(&entry);
  
  endutxent();
}
static void delete_utmp(char *tty_name)
{
  struct utmpx entry;
  struct utmpx *utp;
  char id[3];
  char *p;
  char *line;

  p = rindex(tty_name,'/') + 1;
  if( '0' <= *p && *p <= '9')
  {
        /* /dev/pty/? */
    id[0] = 'p';
    strncpy(id+1,p,2);
    line = rindex(p,'/') + 1;
  }
  else
  {
        /* /dev/ttyp? */
    strncpy(id,p,3);
    line = p;
  }
  memset(&entry,0,sizeof(entry));
  entry.ut_type = USER_PROCESS;
  strncpy(entry.ut_id,id,sizeof(id));
  setutxent();
  utp = getutxid(&entry);
  if(utp)
  {
    utp->ut_type = DEAD_PROCESS;
    memset(utp->ut_user,0,sizeof(entry.ut_user));
    entry.ut_tv.tv_sec = time(NULL);
    entry.ut_tv.tv_usec = 0;
    
    pututxline(utp);
  }
  endutxent();
}
#endif

static int open_master(char *slave_name)
{
  char *ptys;
  int fd;

#ifdef HAVE_DEV_PTC
  if ( (fd = open("/dev/ptc",O_RDWR)) < 0 )
    return -1;

  if ( (ptys = ttyname(fd)) == NULL)
  {
    close(fd);
    return -1;
  }
#else
#ifdef HAVE_DEV_PTMX
  if( (fd = open("/dev/ptmx",O_RDWR)) < 0 )
      return -1;

  if( grantpt(fd) < 0 )
  {
    close(fd);
    return -1;
  }

  if( unlockpt(fd) < 0 )
  {
    close(fd);
    return -1;
  }

  if ( (ptys = (char *)ptsname(fd)) == NULL )
  {
    close(fd);
    return -1;
  }
#endif /* PTMX */
#endif /* PTC  */

  strcpy(slave_name,ptys);
  return fd;
}

static int open_slave(int master_fd, char *slave_name)
{
  int fd;

  if( (fd = open(slave_name,O_RDWR)) < 0 )
  {
    close(master_fd);
    return -1;
  }

  if ( ioctl(fd,I_FIND,"ptem") == 0)
      if ( ioctl(fd,I_PUSH,"ptem") < 0 )
      {
        close(master_fd);
        close(fd);
        return -1;
      }
  
  if ( ioctl(fd,I_FIND,"ldterm") == 0)
      if ( ioctl(fd,I_PUSH,"ldterm") < 0 )
      {
        close(master_fd);
        close(fd);
        return -1;
      }

  if ( ioctl(fd,I_FIND,"ttcompat") == 0)
      if ( ioctl(fd,I_PUSH,"ttcompat") < 0 )
      {
        close(master_fd);
        close(fd);
        return -1;
      }
  close(master_fd);
  return fd;
}

static int get_slave_fd(int master_fd, char *slave_name)
{
  int fd;
  
  if (setsid() < 0)
      return -1;

  if( (fd = open_slave(master_fd,slave_name)) < 0 )
      return -1;

  return fd;
}

static int set_window_size(int fd, int width, int height)
{
  struct winsize wsize;

  wsize.ws_col = width;
  wsize.ws_row = height;
  
  if(ioctl(fd,TIOCSWINSZ,&wsize))
      return -1;

  return 0;
}

static int dup_fds(int slave_fd)
{

  if (dup2(slave_fd,STDIN_FILENO) != STDIN_FILENO)
      return -1;
  
  if (dup2(slave_fd,STDOUT_FILENO) != STDOUT_FILENO)
      return -1;
  
  if (dup2(slave_fd,STDERR_FILENO) != STDERR_FILENO)
      return -1;

  if (slave_fd > STDERR_FILENO)
      close(slave_fd);
  
  return 0;
}

static pid_t child; /* child process id */
static char *tty_name; 
static int tty_read(TerminalIO *tio,void *buf,int size)
{
  int ret ;

  if(child == 0 || tio == NULL ||
     tio->concrete_io == NULL || tio->concrete_io->closed)
      return -1;
  ret = read(tio->concrete_io->fd,buf,size);
  if (ret < 0)
      tio->concrete_io->closed = 1;
  return ret;
}

static int tty_write(TerminalIO *tio,const void *buf,int size)
{
  int ret;

  if(child == 0 || tio == NULL ||
     tio->concrete_io == NULL || tio->concrete_io->closed)
      return -1;
  ret = write(tio->concrete_io->fd,buf,size);
  if (ret < 0)
      tio->concrete_io->closed = 1;
  return ret;
}

static int tty_tell_window_size(TerminalIO *tio,int width, int height)
{
  if(tio == NULL || tio->concrete_io == NULL)
      return -1;
  return set_window_size(tio->concrete_io->fd,width,height);
}

  /* to kill child process propery, need to catch signal ;-) */
static void finish_child(int signal)
{
  pid_t pid;
  pid = wait(NULL);

  do {
    if(pid == child || pid == ECHILD)
    {
      child = 0;
      delete_utmp(tty_name);
      break;
    }
  } while((pid = waitpid(child,NULL,WNOHANG)) > 0);
}

/*
 * TTYIO  Constructor.
 */
TerminalIO* TtyTerminalIO_new(int width,int height,
                                char *program_name, char *argv[])
{
  pid_t pid;
  int master_fd;
  char slave_name[20];
  struct Concrete_IO *cio;
  TerminalIO *tio;

#ifdef SIGTTOU
      /* so that TIOCSWINSZ || TIOCSIZE doesn't block */
  signal(SIGTTOU,SIG_IGN);
#endif
  if ( (master_fd = open_master(slave_name)) < 0 )
      return NULL;

  if ( (tio = malloc(sizeof(TerminalIO))) == NULL )
      return NULL;
  
  if ( (cio = malloc(sizeof(struct Concrete_IO))) == NULL )
      return NULL;

  strcpy(cio->slave_name,slave_name);
  tty_name = cio->slave_name;
  
  signal(SIGCHLD,finish_child);
  if ( (pid = fork()) < 0 )
      return NULL;

  if(pid)
  {
    child = cio->child = pid;
    cio->fd = master_fd;
    tio->concrete_io = cio;
    tio->read = tty_read;
    tio->write = tty_write;
    tio->tell_window_size = tty_tell_window_size;
    tio->concrete_io->closed = 0;
    return tio;
  }
  else /* child */
  {
    int fd;

#if defined(HAVE_LIBUTEMPTER)    
    write_utmp(cio->slave_name,master_fd);
#else
    write_utmp(cio->slave_name);
#endif
    if ( (fd = get_slave_fd(master_fd,slave_name)) < 0)
      exit(1);

    if ( set_window_size(fd,width,height) < 0 )
      exit(1);

    if ( dup_fds(fd) < 0 )
      exit(1);

    chown(slave_name,getuid(),getgid());
    setgid(getgid());
    setuid(getuid());
    signal(SIGINT,SIG_DFL);
    signal(SIGQUIT,SIG_DFL);
    signal(SIGCHLD,SIG_DFL);
    execvp(program_name,argv);

    printf("can't exec: %s\n",program_name);
    {
       char *default_shell = "/bin/sh";
       char *args[2];

       args[0] = default_shell;
       args[1] = NULL;

       execvp(args[0],args);
    }
  }
  return NULL;
}

/*
 * TTYIO Destructor
 */
void TtyTerminalIO_destroy(TerminalIO *tio)
{
  if(tio != NULL)
  {
    if(!tio->concrete_io->closed)
        delete_utmp(tio->concrete_io->slave_name);
    if(tio->concrete_io != NULL)
        free(tio->concrete_io);
    free(tio);
  }
}

/*
 * Obtain associated file descriptor
 */
int TtyTerminalIO_get_associated_fd(TerminalIO *tio)
{
  return tio->concrete_io->fd;
}
