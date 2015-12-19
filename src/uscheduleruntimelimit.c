#include "scheduled.h"
#include "scan.h"
#include "fmt.h"
#include "sig.h"
#include "attributes.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

static void mywrite(const char *s)
{
  int l;
  for (l=0;s[l];l++) ; /* nothing */
  while (l) {
    int x=write(2,s,l);
    if (x>0) {
      s+=x;
      l-=x;
      continue;
    }
    if (-1==x && errno==EINTR) continue;
    _exit(1);
  }
}
static void die(const char *s, const char *t, const char *u, const char *v)
  attribute_noreturn;

static void 
die(const char *s, const char *t, const char *u, const char *v)
{
  mywrite("uscheduleruntimelimit: ");
  if (s) mywrite(s);
  if (t) mywrite(t);
  if (u) mywrite(u);
  if (v) mywrite(v);
  _exit(1);
}

static int pid;
static int state;
static void catcher(int signo)
{
  (void) signo;
  state++;
}

static void doit(unsigned long long timeout, char **argv) attribute_noreturn;
static void doit(unsigned long long timeout, char **argv)
{
  int code;
  int ret;
  char nb[FMT_ULONG];
  pid=fork();
  if (-1==pid)
    die("failed to fork: ",strerror(errno),0,0);
  if (0==pid) {
    execvp(argv[0],argv);
    die("failed to execute ",argv[0],": ",strerror(errno));
  }
  sig_catch(SIGALRM,catcher);
  alarm(timeout);
  while (1) {
    ret = waitpid(pid,&code,0);
    if (ret!=-1) break;
    if (state==1) {
      /* got first alarm */
      state++;
      sig_catch(SIGALRM,catcher);
      kill(pid,SIGTERM);
      alarm(5);
    }
    if (state==3) {
      /* got second alarm */
      state++;
      kill(pid,SIGKILL);
    }
  }
  if (-1==ret) die("failed to wait for ",argv[0],": ",strerror(errno));
  if (0==ret) die("wait_pid returned 0",0,0,0); /* ECANTHAPPEN */
#if defined(WIFEXITED) && defined(WEXITSTATUS) \
 && defined(WIFSIGNALED) &&  defined(WTERMSIG)
  if (WIFEXITED(code))
    _exit(WEXITSTATUS(code));
  if (WIFSIGNALED(code)) {
    kill(getpid(),WTERMSIG(code));
    sleep(5);
    _exit(code);
  }
#endif
  if (0==code) _exit(0);
  nb[fmt_ulong(nb,code)]=0;
  die(argv[0], " exited with code ",nb,0);
}



int main(int argc, char **argv) 
{
  int x;
  unsigned long ul;
  unsigned long mul=1;
  if (argc<3) {
    mywrite("usage: uscheduleruntimelimit NUMBER[dhm] CHILD\n");
    mywrite("  kills CHILD after that NUMBER seconds (default), days, hours or minutes\n");
    _exit(2);
  }
  x=scan_ulong(argv[1],&ul);
  switch (argv[1][x]) {
  case 'd': mul=86400; break;
  case 'h': mul=3600; break;
  case 'm': mul=60; break;
  case 0: break;
  default: die("cannot parse ",argv[1],0,0);
  }
  doit(mul*ul,argv+2);
}
