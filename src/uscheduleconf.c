#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#include "svscan_conf.h"
#include "bailout.h"
#include "str.h"
#include "env.h"
#include "scheduled.h"
#include "uogetopt.h"

static const char *dir;
static int rootuid=0;
static int rootgid=0;
static int useruid;
static int usergid;
static int loguid;
static int loggid;

static int o_user_change=1;
static void xowner(uid_t u, gid_t g) { if (o_user_change) owner(u,g); }
static void xsetuidgid(const char *s)
{ if (!o_user_change) return;
  outs("setuidgid "); outs(s); outs(" \\\n");
}

static void mp(const char *s, int mode, uid_t u, gid_t g)
{
	static stralloc d=STRALLOC_INIT;
	if (!stralloc_copys(&d,dir)) oom();
	if (!stralloc_cats(&d,"/")) oom();
	if (!stralloc_cats(&d,s)) oom();
	if (!stralloc_0(&d)) oom();
	make_dir(d.s);
	xowner(u,g);
	perm(mode);
}

static uogetopt2 myopts[]={
{'n',"no-user-change",uogo_flag,UOGO_NOARG, &o_user_change, 0 , 
"Don't change file ownership, don't switch user ids.",
"The default is to make ACCT and LOGACCT the owners "
"of many files, and to switch to ACCT and LOGACCT "
"before running uscheduled and the logging program. "
"With this option the caller will be the owner of "
"all files created and the programs will run with "
"all the rights they inherit.",0},
{0,0,0,0,0,0,0,0,0}
};

static void simple_env(const char *f, const char *val)
{
	start_file(f);
	outs(val); outs("\n");
	finish_file();
	xowner(useruid,usergid);
	perm(0700);
}
static uogetopt_env myoptenv={
"uscheduleconf",PACKAGE,VERSION,
"uscheduleconf DIR ACCT LOGACCT [JOBDIR [LOGDIR]]",
"This program creates an svscan service directory DIR, starting a task "
"to schedule for ACCT. Jobs will be read from JOBDIR or ~ACCT/.uschedule.\n"
"Logging information will be written to LOGDIR or ~ACCT/.uschedule/log, "
"using the account LOGACCT.\n"
"JOBDIR may be `-', in which case ~ACCT/.uschedule is used.\n",
"long",
"Report bugs to uschedule@lists.ohse.de",
4,6,0,0,uogetopt_out,myopts
};


int
main(int argc, char **argv)
{
  const char *username;
  const char *logusername;
  const char *jobdir=0;
  const char *logdir=0;
  static stralloc pw_dir, pw_shell, pw_name;

  bailout_progname(argv[0]);
  flag_bailout_fatal_begin=3;
  uogo_posixmode=1;
  myoptenv.program=flag_bailout_log_name;
  uogetopt_parse(&myoptenv,&argc,argv);

  dir=argv[1];
  username=argv[2];
  logusername=argv[3];
  if (argv[4]) {
    jobdir=argv[4];
    if (argv[5])
      logdir=argv[5];
  }

  /* band-aids: Code below doesn't quote >'<, and it's questionable
   * whether such quoting works correctly everywhere. Better play safe. */
#define BAND_AID(x) \
  if (x[str_chr(x,'\'')]) xbailout(100,0,#x " must not contain >'<: ",x,0,0);
  BAND_AID(dir);
  BAND_AID(username);
  BAND_AID(logusername);
  if (jobdir) BAND_AID(jobdir);
  if (logdir) BAND_AID(logdir);

  /* limit scope of pw. i often misused it below */
  {
    struct passwd *pw; 

    pw = getpwuid(getuid());
    /* setuidgid `random 0 65535` uscheduleconf ..., anyone? */
    if (!pw) xbailout(100,0,"failed to get current user data",0,0,0);

    pw = getpwnam(username);
    if (!pw) xbailout(100,0,"unknown account ",username,0,0);

    useruid=pw->pw_uid;
    usergid=pw->pw_gid;
    if (!stralloc_copys(&pw_shell,pw->pw_shell)) oom();
    if (!stralloc_0(&pw_shell)) oom();
    if (!stralloc_copys(&pw_dir,pw->pw_dir)) oom();
    if (!stralloc_0(&pw_dir)) oom();
    if (!stralloc_copys(&pw_name,pw->pw_name)) oom();
    if (!stralloc_0(&pw_name)) oom();

    pw = getpwnam(logusername);
    if (!pw) xbailout(100,0,"unknown account ",logusername,0,0);
    loguid=pw->pw_uid;
    loggid=pw->pw_gid;
  }

  if (!jobdir || str_equal(jobdir,"-")) {
    static stralloc d;
    if (!stralloc_copys(&d,pw_dir.s)) oom();
    if (!stralloc_cats(&d,"/")) oom();
    if (!stralloc_cats(&d,SCHEDULEDIR)) oom();
    if (!stralloc_0(&d)) oom();
    jobdir=d.s;
    BAND_AID(jobdir);
  }
  if (!logdir) {
    static stralloc d;
    if (!stralloc_copys(&d,jobdir)) oom();
    if (!stralloc_cats(&d,"/log")) oom();
    if (!stralloc_0(&d)) oom();
    logdir=d.s;
    BAND_AID(logdir);
  }
  if (logdir[str_chr(logdir,'\'')])
    /* band-aid */
    xbailout(100,0,"logdir must not contain >'<: ",logdir,0,0);

  /* root space */

  if (o_user_change)
    base_dir(dir,01700, rootuid,rootgid);
  else
    base_dir(dir,01700, useruid,usergid);

  start_file("run");
  outs("#! /bin/sh\n");
  outs("exec 2>&1\n");
  outs("cd '"); outs(jobdir); outs("' || exit 1\n");
  outs("exec \\\n");
  xsetuidgid(username);
  outs("./run\n");
  finish_file();
  xowner(rootuid,rootgid);
  perm(0700);

  mp("log",0700,rootuid,rootgid);
  start_file("log/run");
  outs("#! /bin/sh\n");
  outs("exec 2>&1\n");
  outs("cd '"); outs(logdir); outs("' || exit 1\n");
  outs("exec \\\n");
  outs("softlimit -m 8000000 -o 400 -p 40 \\\n");
  xsetuidgid(logusername);
  outs("./run\n");
  finish_file();
  xowner(rootuid,rootgid);
  perm(0700);

  /* user space */

  make_dir(jobdir);
  xowner(useruid,usergid);
  perm(0700);
  set_dir(jobdir);
  dir=jobdir;

  start_file("run");
  outs("#! /bin/sh\n");
  outs("exec \\\n");
  outs("envdir ./env \\\n");
  outs("uscheduled -d `pwd`\n");
  finish_file();
  xowner(useruid,usergid);
  perm(0700);
  make_fifo("fifo",0600);
  xowner(useruid,usergid);

  mp("env",0700,useruid,usergid);
  start_file("env/PATH");
  outs("/command:/usr/local/bin:/usr/bin:/bin");
  if (0==useruid)
    outs(":/usr/local/sbin:/usr/sbin:/sbin");
  outs("\n");
  finish_file();
  xowner(useruid,usergid);
  perm(0700);

  simple_env("env/HOME",pw_dir.s);
  simple_env("env/SHELL",pw_shell.s);
  simple_env("env/USER",pw_name.s);
  simple_env("env/LOGNAME",pw_name.s);


  mp(IDDIR,0700,useruid,usergid);
  make_dir(logdir);
  xowner(loguid,loggid);
  perm(0700);
  set_dir(logdir);
  {
    static stralloc logrun;
    if (!stralloc_copys(&logrun,logdir)) oom();
    if (!stralloc_cats(&logrun,"/run")) oom();
    if (!stralloc_0(&logrun)) oom();
    start_file(logrun.s);
    outs("#! /bin/sh\n");
    outs("exec \\\n");
    outs("softlimit -m 5000000 -o 100 -p 10 \\\n");
    outs("multilog t \\\n");
    outs("./\n");
    finish_file();
    xowner(loguid,loggid);
    perm(0700);
  }

  return 0;
}
