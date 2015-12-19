#include "scheduled.h"
#include "stralloc.h"
#include "sig.h"
#include "iopause.h"
#include "byte.h"
#include "str.h"
#include "uogetopt.h"
#include "open.h"
#include "pathexec.h"
#include "bailout.h"
#include "error.h"
#include "wait.h"
#include "fmt.h"
#include "coe.h"
#include "env.h"
#include "uolock.h"
#include "ndelay.h"
#include "wrap_stat.h"
#include <unistd.h>

/* he who decided that rename should be placed in stdio.h should be ashamed */
extern int rename(const char *oldpath, const char *newpath);

static int got_sigterm;
static int got_sigchld;
static int got_sighup;
static void sigterm_handler(int st) {got_sigterm=st; }
static void sighup_handler(int st) {got_sighup=st; }
static void sigchld_handler(int st) {got_sigchld=st; }

static const char *home;
static stralloc absprefix;
static stralloc jobs;

static void 
delete(const char *s, struct jobinfo *j)
{
	static stralloc id;

	if (!stralloc_copyb(&id,j->id,j->idlen)) oom();
	if (!stralloc_0(&id)) oom();
	if (-1==unlink(s) && errno!=error_noent) 
		warning(errno,"failed to unlink stamp of job",id.s,0,0);
	else
		warning(0,"deleted job ",id.s,0,0);
}

static int 
reschedule(const char *s,struct jobinfo *j)
{
	static stralloc sa;
	struct taia now, next;

	taia_now(&now);

	if (1==j->repeats) {
		delete(s,j);
		return 0;
	}
	if (j->repeats)
		j->repeats--;

	j->lastrun=now;

	if (!find_next(j,&now,&next)) {
		delete(s,j);
		return 0;
	}

	make_name(&sa,j);

	if (0==rename(s,sa.s)) {
		warning(0,"rescheduled ", s," to ", sa.s);
		return 1;
	}
	warning(errno,"failed to rename ", s , " to ", sa.s);
	return 0;
}

static void 
execute(const char *s, struct jobinfo *ji)
{
	int pid;
	pid=fork();
	if (-1==pid) {
		warning(errno,"failed to fork, job delayed",0,0,0);
		sleep(1);
		return;
	}
	if (0==pid) {
		/* child */
		static stralloc c;
		char *av[2];
		int fd;
		if (!stralloc_copy(&c,&absprefix)) oom();
		if (!stralloc_cats(&c,"/")) oom();
		if (!stralloc_cats(&c,IDDIR)) oom();
		if (!stralloc_cats(&c,"/")) oom();
		if (!stralloc_catb(&c,ji->id,ji->idlen)) oom();
		if (!stralloc_0(&c)) oom();
		if (!pathexec_env("SCHEDULED_COMMAND_FILE",c.s)) oom();
		if (ji->null1) {
			close(1);
			if (-1==open_write("/dev/null"))
				xbailout(111,errno,"failed to open /dev/null",0,0,0);
		}
		if (ji->null2) {
			close(2);
			if (-1==open_write("/dev/null"))
				xbailout(111,errno,"failed to open /dev/null",0,0,0);
		}
		av[0]=c.s;
		av[1]=0;
		if (-1==chdir(home))
			xbailout(111,errno,"failed to chdir to ",home,0,0);
		fd=open_read(c.s);
		if (-1==fd)
			xbailout(111,errno,"failed to open_read ",c.s,0,0);
		coe(fd);
		/* to allow editing of command file */
		if (-1==uolock_share(fd))
			xbailout(111,errno,"failed to lock ",c.s,0,0);
		pathexec(av);
		if (errno==error_noent) {
			struct wrap_stat st;
			int e=errno;
			/* don't ever trust kernels errno after execv ... */
			if (-1==wrap_stat(av[0],&st) && errno==error_noent) {
				/* there's an entry in the schedule directory, but no
				 * matching entry in the command directory. This can
				 * happen if someone used rm or if schedulerm lost the
				 * race.
				 */
				warning(errno,"delete entry ", s,0,0);
				delete(s,ji);
				_exit(1);
			}
			errno=e;
		}
		warning(errno,"failed to execute ", av[0],0,0);
		_exit(1);
	} else {
		/* parent */
		char nb[FMT_ULONG];
		nb[fmt_ulong(nb,pid)]=0;
		warning(0,"started pid ",nb, " for job ",s);
		reschedule(s,ji);
	}
}

static const char *o_dir;
static uogetopt2 myopts[]={
{'d',"dir",uogo_string,0,&o_dir, 0 ,
"Jobdirectory.",
"Jobs will be read from this directory.","DIR"},
{0,0,0,0,0,0,0,0,0}
};
static uogetopt_env myoptenv={
  "uscheduled",PACKAGE,VERSION,
  "uscheduled [-d DIR]",
  "  This program schedules the content of DIR or ~/.uschedule.",
  "long",
  "  Report bugs to uschedule@lists.ohse.de",
  1,1,
  0,0,uogetopt_out,myopts
};

static iopause_fd iop[1];
static void fifo_open(void) 
{
	iop[0].fd=open_read("fifo");
	if (-1==iop[0].fd)
		xbailout(111,errno,"failed to open fifo",0,0,0);
	ndelay_on(iop[0].fd);
	iop[0].events=IOPAUSE_READ;
}
static void fifo_read(void)
{
	while (1) {
		int r;
		char c;
		r=read(iop[0].fd,&c,1);
		if (-1==r)
			break;
		if (0==r)
			break;
	}
	close(iop[0].fd);
	fifo_open();
}
static void reap(void)
{
	do {
		int st;
		int pid;
		char nb1[FMT_ULONG];
		char nb2[FMT_ULONG];
		pid=wait_nohang(&st);
		if (-1==pid || 0==pid)
			break;
		nb1[fmt_ulong(nb1,pid)]=0;
		nb2[fmt_ulong(nb2,st)]=0;
		warning(0,"processs ",nb1, " terminated with code ",nb2);
	} while (1);
}

static void 
calc_next_run(struct taia *now, struct taia *then)
{
  unsigned int i;
  taia_uint(then,1800); /* sleep no more than 30 minutes */
  taia_add(then,now,then);
  for (i=0;jobs.s[i];i+=str_len(jobs.s+i)+1) {
    struct jobinfo ji;
    struct taia nextrun;
    /* load_jobs did the checks on the job */
    parse_job(jobs.s+i,&ji);
    if (find_next(&ji,now, &nextrun))
      if (taia_less(&nextrun,then))
	*then=nextrun;
    /* care about timed-out jobs below */
  }
  if (taia_less(then,now))
	  *then=*now;
}

static void 
do_executes(struct taia *now)
{
  unsigned int i;
  for (i=0;jobs.s[i];i+=str_len(jobs.s+i)+1) {
    struct taia nextrun;
    struct jobinfo ji;
    parse_job(jobs.s+i,&ji); /* load_jobs did the checks on the job */
    if (!find_next(&ji,now,&nextrun)) {
      warning(0,"no next run, deleting ",jobs.s+i,0,0);
      delete(jobs.s+i,&ji);
      continue;
    }
    if (taia_less(now,&nextrun))
      continue;
    execute(jobs.s+i,&ji);
  }
}

int 
main(int argc, char **argv)
{
	bailout_progname(argv[0]);
	flag_bailout_fatal_begin=3;
	flag_bailout_log_pid=1;
	uogo_posixmode=1;
	myoptenv.program=flag_bailout_log_name;
	uogetopt_parse(&myoptenv,&argc,argv);
	change_dir(&absprefix, o_dir,0);
	home=env_get("HOME"); /* change_dir checks for existance */

	warning(0,"starting",0,0,0);

	sig_catch(sig_term,sigterm_handler);
	sig_catch(sig_hangup,sighup_handler);
	sig_catch(sig_child,sigchld_handler);
	sig_ignore(sig_pipe);
	setsid();
	close(0);
	if (-1==open_read("/dev/null"))
		xbailout(111,errno,"failed to open /dev/null",0,0,0);
	fifo_open();

	while (1) {
		struct taia now,then;
		if (got_sigterm) break;
		if (got_sigchld) {
			reap();
			got_sigchld=0;
		}
		if (got_sighup) {
			warning(0,"reloading",0,0,0);
			got_sighup=0;
		}
		load_jobs(".",&jobs); 

		taia_now(&now);

		calc_next_run(&now,&then);

		iopause(iop,1,&then,&now);
		if (iop[0].revents)
			fifo_read();

		taia_now(&now);

		if (taia_less(&now,&then))
			continue; /* signal */

		do_executes(&now);
	}
	warning(0,"exiting",0,0,0);
	_exit(0);
}
