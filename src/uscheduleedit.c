#include "uogetopt.h"
#include "scheduled.h"
#include "open.h"
#include "uogetopt.h"
#include "stralloc.h"
#include "wait.h"
#include "bailout.h"
#include "pathexec.h"
#include "env.h"
#include "error.h"
#include "uolock.h"
#include "fmt.h"
#include "wrap_stat.h"
#include <unistd.h>

static const char *o_dir;
static int o_dot_as_home;

static uogetopt2 myopts[]={
{'.',"dot-as-home",uogo_flag,UOGO_NOARG,&o_dot_as_home, 1 ,
"Use current directory instead of $HOME.",0,0},
{'d',"dir",uogo_string,0,&o_dir, 0 ,
"Jobdirectory.",
"Jobs will be read from this directory.","DIR"},
{0,0,0,0,0,0,0,0,0}
};              

static int doit(char *vi, char *file1, char *file2)
{
	int pid;
	int code;
	int ret;
	char nb[FMT_ULONG];
	pid=fork();
	if (-1==pid)
		xbailout(111,errno,"failed to fork",0,0,0);
	if (0==pid) {
		char *av[4];
		av[0]=vi;
		av[1]=file1;
		av[2]=file2;
		av[3]=0;
		pathexec(av);
		xbailout(111,errno,"failed to execute ",av[0],0,0);
	}
	ret=wait_pid(&code,pid);
	if (-1==ret) xbailout(111,errno,"failed to wait for ",vi," process",0);
	if (0==ret) xbailout(111,0,"wait_pid returned 0",0,0,0); /* ECANTHAPPEN */
	if (0==code)
		return 0;
	nb[fmt_ulong(nb,code)]=0;
	warning(0,vi, " exited with code ",nb,0);
	return (code & 127) ? 1: code >> 8;
}

static uogetopt_env myoptenv={
"uscheduleedit",PACKAGE,VERSION,
"uscheduleedit [options] ID",
"This program allows you to edit the schedulejob ID.",
"long",
"Report bugs to uschedule@lists.ohse.de",
2,2,0,0,uogetopt_out,myopts
};

int main(int argc, char **argv) 
{
	static stralloc fn;
	static stralloc fn2;
	struct wrap_stat st;
	int fd;
	char *e;

	bailout_progname(argv[0]);
	flag_bailout_fatal_begin=3;
	uogo_posixmode=1;
	myoptenv.program=flag_bailout_log_name;
	uogetopt_parse(&myoptenv,&argc,argv);

	change_dir(0,o_dir,o_dot_as_home);
	check_id(argv[1]);
	if (!stralloc_copys(&fn,IDDIR)) oom();
	if (!stralloc_cats(&fn,"/")) oom();
	if (!stralloc_cats(&fn,argv[1])) oom();
	if (!stralloc_copy(&fn2,&fn)) oom();
	if (!stralloc_0(&fn)) oom();
	if (!stralloc_cats(&fn2,".run")) oom();
	if (!stralloc_0(&fn2)) oom();

	fd=open_write(fn.s);
	if (-1==fd) 
		xbailout(111,errno,"failed to open_write ",fn.s,0,0);
	if (-1==wrap_fstat(fd,&st)) 
		xbailout(111,errno,"failed to fstat ",fn.s,0,0);
	if (-1==uolock_tryexcl(fd))
		xbailout(111,errno,"failed to lock ",fn.s,0,0);

	e=env_get("VISUAL");
	if (!e)
		e=env_get("EDITOR");
	if (!e) {
		static char vi[]="vi";
		e=vi;
	}
	if (-1==wrap_stat(fn2.s,&st)) {
		if (errno!=error_noent)
			xbailout(111,errno,"failed to stat ",fn2.s,0,0);
		doit(e,fn.s,0);
	} else
		doit(e,fn.s,fn2.s);
	return 0;
}
