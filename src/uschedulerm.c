#include "attributes.h"
#include "scheduled.h"
#include "uogetopt.h"
#include "byte.h"
#include "bailout.h"
#include "iopause.h"
#include "buffer.h"
#include "str.h"
#include "fmt.h"
#include "error.h"
#include "open.h"
#include "wrap_stat.h"
#include "fmt_tai.h"
#include <unistd.h>

static const char *o_dir;
static int o_verbose;
static int o_cmdfile;
static int o_interactive=-1;
static int o_dot_as_home;

static uogetopt2 myopts[]={
{'.',"dot-as-home",uogo_flag,UOGO_NOARG,&o_dot_as_home, 1 ,
"Use current directory instead of $HOME.",0,0},
{'c',"command",uogo_flag,UOGO_NOARG,&o_cmdfile, 1 ,
"Delete command file, not schedule files.",
"The default is to delete scheduled jobs, leaving\n"
"the registered command files alone.",0},
{'d',"dir",uogo_string,0,&o_dir, 0 ,
"Jobdirectory.",
"Jobs will be read from this directory.","DIR"},
{'i',"interactive",uogo_flag,UOGO_NOARG,&o_interactive, 1 ,
"Ask before deleting.",0,0},
{'N',"non-interactive",uogo_flag,UOGO_NOARG,&o_interactive, 0 ,
"Do not ask before deleting.",0,0},
{'v',"verbose",uogo_flag,UOGO_NOARG,&o_verbose, 1 ,
"Print success messages.",0,0},
{0,0,0,0,0,0,0,0,0}
};              
static void die_write(void) attribute_noreturn;

static void die_write(void)  {xbailout(111,errno,"failed to write",0,0,0);}

static void outs(const char *s) 
{ if (-1==buffer_puts(buffer_1,s)) die_write();}
static void outnum(unsigned long ul)
{ char nb[FMT_ULONG]; 
  if (-1==buffer_put(buffer_1,nb,fmt_ulong(nb,ul))) die_write();}

static int ask(void)
{
	/* eat typeahead */
	iopause_fd x;
	struct taia now,then;
	x.fd=0;
	x.events=IOPAUSE_READ;
	while (1) {
		char c;
		int r;
		taia_now(&now);
		then=now;
		iopause(&x,1,&now,&then);
		if (!x.revents) 	
			break;
		r=read(0,&c,1);
		if (-1==r) {
			if (errno==error_intr || errno==error_again 
				|| errno==error_wouldblock)
					continue;
			xbailout(111,errno,"failed to read typeahead",0,0,0);
		}
	}

	while (1) {
		int flag=-1;
		outs("delete? [y/n, default n]:");
		if (-1==buffer_flush(buffer_1)) die_write();
		/* read one line, char by char */
		while (1) {
			int r;
			char c;
			r=read(0,&c,1);
			if (-1==r) {
				if (errno==error_intr || errno==error_again 
					|| errno==error_wouldblock)
						continue;
				xbailout(111,errno,"failed to read answer",0,0,0);
			}
			if (!r)
				xbailout(111,0,"failed to read answer: end of file",0,0,0);
			if ('\n'==c)
				break;
			if (-1==flag) {
				switch(c) {
				case 'Y': case 'y':  flag=1; break;
				case 'N': case 'n':  flag=0; break;
				case ' ': case '\t': break;
				default: flag=2; break;
				}
			}
		}
		if (1==flag) return 1;
		if (0==flag) return 0;
		if (-1==flag) return 0; /* default no */
		/* flag==2 -> continue to ask */
	}
}

static int do_cmdfile(char **argv)
{
	int i;
	int exitcode=0;
	int olddir=open_read(".");
	if (-1==olddir) xbailout(111,errno,"failed to open_read `.'",0,0,0);
	if (-1==chdir(IDDIR)) xbailout(111,errno,"failed to chdir to ",IDDIR,0,0);
	for (i=1;argv[i];i++) {
		struct wrap_stat st;
		if (-1==wrap_stat(argv[i],&st)) {
			warning(errno,"failed to stat ",argv[i],0,0);
			exitcode=1;
			continue;
		}
		if (o_interactive) {
			char buf[128];
			outs(argv[i]); outs("\n");
			outs("  links: "); outnum(st.nlink); outs("\n");
			outs("  modified: "); 
			fmt_tai(buf,sizeof(buf),&st.mtime.sec);
			outs(buf); outs("\n");
			if (st.nlink>1) {
				outs(" not deleted: still in use\n");
				continue;
			} else {
				if (!ask()) 
					continue;
			}
		} else if (st.nlink>1) {
			outs(" not deleted: still in use\n");
			continue;
		}
		if (-1==unlink(argv[i])) {
			warning(errno,"failed to remove ",argv[i],0,0);
			exitcode=1;
		} else {	
			if (o_verbose) {
				outs("removed command with id ");
				outs(argv[i]);
				outs("\n");
			}
		}
	}
	if (-1==fchdir(olddir))
		xbailout(111,errno,"failed to fchdir back from ", IDDIR,0,0);
	close(olddir);
	return exitcode;
}
static int
do_schedulefile(char **argv)
{
  static stralloc jobs;
  int i;
  int exitcode=0;
  struct taia now;
  taia_now(&now);
  load_jobs(".",&jobs);
  for (i=1;argv[i];i++) {
    unsigned int k;
    int done=0;
    for (k=0;jobs.s[k];k+=str_len(jobs.s+k)+1) {
      struct jobinfo j;
      parse_job(jobs.s+k,&j); /* load_jobs did the checks on the job */
      if (j.idlen!=str_len(argv[i]))
	continue;
      if (!byte_equal(j.id,j.idlen,argv[i]))
	continue;
      if (o_interactive) {
	print_job(&j,&now);
	if (!ask()) 
	  continue;
      }
      done=1;
      if (-1==unlink(jobs.s+k)) {
	warning(errno,"failed to remove ",argv[i],0,0);
	exitcode=1;
      } else {	
	if (o_verbose) {
	  outs("removed scheduled job with id ");
	  outs(argv[i]);
	  outs("\n");
	}
      }
    }
    if (!done) {
      warning(0,"No job deleted for ",argv[i],0,0);
      exitcode=1;
    }
  }
  return exitcode;
}

uogetopt_env myoptenv={
"uschedulerm", PACKAGE,VERSION,
"uschedulelist [options] ID1 [...]",
"This program deletes jobs with the identifiers ID1 ... from ~/.uschedule.",
"long",
"Report bugs to uschedule@lists.ohse.de",
2,0,
0,0,
uogetopt_out,
myopts
};



int 
main(int argc, char **argv)
{
	int exitcode=0;

	bailout_progname(argv[0]);
	flag_bailout_fatal_begin=3;
	uogo_posixmode=1;
	myoptenv.program=flag_bailout_log_name;
	uogetopt_parse(&myoptenv,&argc,argv);
	change_dir(0,o_dir,o_dot_as_home);

	if (-1==o_interactive) {
		if (isatty(0) && isatty(1))
			o_interactive=0;
		else
			o_interactive=1;
	}
	if (o_cmdfile)
		exitcode=do_cmdfile(argv);
	else
		exitcode=do_schedulefile(argv);
	if (-1==buffer_flush(buffer_1)) die_write();
	notice();

	return exitcode;
}
