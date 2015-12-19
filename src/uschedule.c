#include "uogetopt.h"
#include "scheduled.h"
#include "bailout.h"
#include "str.h"
#include "error.h"
#include "scan.h"
#include "fmt.h"
#include <unistd.h>

static unsigned long o_repeats; 
static unsigned long o_late=3600;
static unsigned long o_every=0;
static const char *o_every_s;
static const char *o_comment;
static const char *o_dir;
static char *o_from;
static char *o_to;
static int o_verbose=0;
static int o_null1=0;
static int o_null2=0;
static int o_dot_as_home;
static uogetopt2 myopts[]={
{'.',"dot-as-home",uogo_flag,UOGO_NOARG,&o_dot_as_home, 1 ,
"Use current directory instead of $HOME.",0,0},
{'1',"null1",uogo_flag,UOGO_NOARG,&o_null1, 1 ,
"Redirect stdout to /dev/null.",
"Redirect the standard output of the job to "
"/dev/null. The default is to print it into the "
"same log the scheduled daemon uses.",0},
{'2',"null2",uogo_flag,UOGO_NOARG,&o_null2, 1 ,
"Redirect stderr to /dev/null.",
"Redirect the standard error output of the job to "
"/dev/null. The default is to print it into the "
"same log the scheduled daemon uses.",0},
{'c',"count",uogo_ulong,0,&o_repeats, 0 ,
"Repeat job N times.",
"The jobs will be deleted after it was executed "
"or or tried to be executed the given number of "
"times. The default is to repeat it forever.","N"},
{'d',"dir",uogo_string,0,&o_dir, 0 ,
"Jobdirectory.",
"Jobs will be written to this directory.","DIR"},
{'D',"description",uogo_string,0,&o_comment, 0 ,
"Short job description.",
"Less than 70 characters, no colons.", "STRING"},
{'e',"every",uogo_string,0,&o_every_s, 0 ,
"Repeat every NUMBER time units (default: seconds).",
"To change the time unit append these characters to the NUMBER: "
"`m' (minutes), `h' hours, `d' days and `w' weeks.\n"
"This option is implemented in such a way that the NUMBER is added once "
"at the start of a search. Then all other restrictions (late, from, to, "
"TIMESPEC) will be applied and the next matching time will be searched "
"for.", "NUMBER"},
{'f',"from",uogo_string,0,&o_from, 0 ,
"Run job only after STARTSPEC.",
"STARTSPEC is a TIMESPEC. Jobs will only be started "
"after STARTSPEC is reached. This may be used "
"together with --to.\n"
"Note: leading wildcards are OK, trailing wildcards "
"are not! See the manual page.", "STARTSPEC"},
{'l',"late",uogo_ulong,0,&o_late, 0 ,
"Allow job to start up to SECONDS later.",
"The default is to allow it to be executed up to one hour later.","SECONDS"},
{'t',"to",uogo_string,0,&o_to, 0 ,
"Run job only before ENDSPEC.",
"ENDSPEC is a TIMESPEC. Jobs will only be started "
"until ENDSPEC is reached. This may be used together "
"with --from.\n"
"Note: leading wildcards are OK, trailing wildcards "
"are not! See the manual page.", "ENDSPEC"},
{'v',"verbose",uogo_flag,UOGO_NOARG,&o_verbose, 1 ,
"Print success messages.",0,0},
{0,0,0,0,0,0,0,0,0}
};              

static void
create_job(struct jobinfo *j)
{
	static stralloc fn;
	static stralloc fn2;

	make_name(&fn2,j);

	if (!stralloc_copys(&fn,IDDIR)) oom();
	if (!stralloc_cats(&fn,"/")) oom();
	if (!stralloc_catb(&fn,j->id,j->idlen)) oom();
	if (!stralloc_0(&fn)) oom();
	if (-1==link(fn.s,fn2.s))
		xbailout(111,errno,"failed to link ",fn.s, " to ", fn2.s);
}
/* the matching alghorithm for the from/to specifications does 
 * very unintuitive things with tailing wildcards. That is, 
 * a wildcard follows a fixed value. */
static int 
wildcardcheck(stralloc *sa,const char *name)
{
  unsigned int i;
  unsigned int bits=0;
  unsigned int expect=0;
  for (i=0;i<sa->len;i++) {
    switch(sa->s[i]) {
    case 'Y': bits|=32; expect=63; break;
    case 'M': bits|=16; if (!expect) expect=31; break;
    case 'D': bits|=8; if (!expect) expect=15; break;
    case 'h': bits|=4; if (!expect) expect=7; break;
    case 'm': bits|=2; if (!expect) expect=3; break;
    case 's': bits|=1; if (!expect) expect=1; break;
    case 'W': 
      warning(0,"Weekdays in ",name," specifications are unlikely to "
	"work as you expect.",0);
    }
  }
  if (expect!=bits)
    xbailout(2,0,name," specification contains wildcards after "
      "fixed values.",0,0);
  return bits;
}
static uogetopt_env myoptenv={
"uschedule",PACKAGE,VERSION,
"uschedule [options] ID TIMESPEC [...]",
"This program schedules the command ID to be run at TIMESPEC.",
"long",
"Report bugs to uschedule@lists.ohse.de",
3,0,0,0,uogetopt_out,myopts
};

int main(int argc, char **argv) 
{
	const char *id;
	unsigned int i;
	stralloc sa_from=STRALLOC_INIT;
	stralloc sa_to=STRALLOC_INIT;
	bailout_progname(argv[0]);
	flag_bailout_fatal_begin=3;
	uogo_posixmode=1;
	myoptenv.program=flag_bailout_log_name;
	uogetopt_parse(&myoptenv,&argc,argv);
	if (o_every_s) {
	  unsigned int x;
	  x=scan_ulong(o_every_s,&o_every);
	  switch (o_every_s[x]) {
	  case 's': o_every*=1; break; /* be friendly */
	  case 'm': o_every*=60; break;
	  case 'h': o_every*=360; break;
	  case 'd': o_every*=86400; break;
	  case 'w': o_every*=86400*7; break;
	  case 0: break;
	  default:
	    xbailout(100,0,"failed to parse --every argument: ",o_every_s,0,0);
	  }
	}

	if (o_comment)
		check_id(o_comment);
	if (o_to) {
		int bits;
		parse_timespec(&sa_to,o_to);
		bits=wildcardcheck(&sa_to,"to");
		if (bits!=63 && !o_from)
		  xbailout(2,0,"--to specifications with wildcards "
		                "need a --from specification, too",0,0,0);
	}
	if (o_from)  {
		parse_timespec(&sa_from,o_from);
		wildcardcheck(&sa_from,"from");
	}

	change_dir(0,o_dir,o_dot_as_home);
	id=argv[1];
	for (i=2;argv[i];i++) {
		struct jobinfo j;
		static stralloc sa;
		static stralloc sa2;

		HACK_TAIA_SEC(&j.lastrun)=0;
		j.lastrun.nano=0;
		j.lastrun.atto=0;
		j.late=o_late;
		j.id=id;
		j.idlen=str_len(id);
		j.cronspec=0; /* dito */
		j.cronlen=0; /* dito */
		j.null1=o_null1;
		j.null2=o_null1;
		j.comment=o_comment;
		if (o_comment)
			j.commentlen=str_len(o_comment);
		else
			j.commentlen=0;
		if ('+'==argv[i][0]) {
		  timespec_from_now(&sa,argv[i]+1);
		  j.repeats=1;
		} else {
		  fill_timespec(&sa2,argv[i]); /* incomplete -> complete */
		  parse_timespec(&sa,sa2.s);
		  j.repeats=o_repeats;
		}
		j.cronspec=sa.s;
		j.cronlen=sa.len;
		/* the following work since the sa_* are initialized to 0 */
		j.fromlen=sa_from.len; 
		j.fromspec=sa_from.s;
		j.tolen=sa_to.len; 
		j.tospec=sa_to.s;
		j.every=o_every;
		  
		create_job(&j);
	}
	notice();
	return 0;
}
