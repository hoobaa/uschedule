#include "scheduled.h"
#include "uogetopt.h"
#include "byte.h"
#include "bailout.h"
#include "buffer.h"
#include "env.h"
#include "str.h"
#include "attributes.h"
#include "alloc.h"
#include "error.h"
#include "api_dir.h"
#include "mssort.h"
#include "wrap_stat.h"
#include "fmt_tai.h"
#include "scan.h"
#include "open.h"
#include "getln.h"
#include <unistd.h>

static const char *o_dir;
static int o_commands;
static int o_show_command;
static int o_short;
static int o_dot_as_home;
static int exitcode=0;
static const char *o_now=0;
static uogetopt2 myopts[]={
{'.',"dot-as-home",uogo_flag,UOGO_NOARG,&o_dot_as_home, 1 ,
"Use current directory instead of $HOME.",0,0},
{'c',"commands",uogo_flag,UOGO_NOARG,&o_commands, 1 ,
"List command files, not scheduled jobs.",
"The default is to list schedules. This option lists all prepared commands.",
0},
{'d',"dir",uogo_string,0,&o_dir, 0 ,
"Jobdirectory.",
"Jobs will be read from this directory. Note that this does not read from "
"DIR/.uschedule, but from DIR.","DIR"},
{  0,"now",uogo_string,0,&o_now, 0 ,
"Use FAKE-TIME instead of `now'.",
"FAKE-TIME must be given as seconds since epoch. This option is for meant "
"for the self check of the uschedule package only.","FAKE-TIME"},
{'s',"short",uogo_flag,UOGO_NOARG,&o_short, 1 ,
"Create a short listing.",0,0},
{'S',"show-command",uogo_flag,UOGO_NOARG,&o_show_command, 1 ,
"Show the whole command.",
"Lists the full command in addition to the listing output. This may be "
"useful to distinguish between multiple similary named commands.",0},
{0,"examples",uogo_print_help,UOGO_NOARG|UOGO_HIDDEN,0,0,"",
"The following three command lines all show the same result:\n"
"  cd /tmp ; uschedulelist\n"
"  cd ~ ; uschedulelist -.\n"
"  cd /tmp ; uschedulelist -d ~/.uschedule\n"
"but this one does not:\n"
"  cd /tmp ; uschedulelist -d ~\n"
"To list the registered commands use the --commands option:\n"
"  uschedulelist -c\n"
"",0},
{0,"author",uogo_print_help,UOGO_NOARG|UOGO_HIDDEN,0,0,"",
"Uwe Ohse, <uwe@ohse.de>",0},
{0,"copyright",uogo_print_help,UOGO_NOARG|UOGO_HIDDEN,0,0,"",
"This software is published under the TODO...",0},
{0,0,0,0,0,0,0,0,0}
};              
static stralloc jobs;
static struct taia now;
static void die_write(void) attribute_noreturn;
static void die_write(void) {xbailout(111,errno,"failed to write",0,0,0);}

static int 
cmp_ji(const void *a, const void *b)
{
	struct jobinfo *ja;
	struct jobinfo *jb;
	union {const struct jobinfo *c; struct jobinfo *u;} dq;
	unsigned int l;
	int i;
	struct taia ta,tb;
	dq.c=a; ja=dq.u;
	dq.c=b; jb=dq.u;
	/* the next two lines are for the sake of attribute_check_result */
	if (!find_next(ja,&now,&ta)) return -1; 
	if (!find_next(jb,&now,&tb)) return 1;
	if (taia_less(&ta,&tb)) return -1;
	if (taia_less(&tb,&ta)) return  1;
	l=jb->idlen;
	if (ja->idlen<l)
		l=ja->idlen;
	i=byte_diff(ja->id,l,jb->id);
	if (i<0) return -1;
	if (i>0) return  1;
	if (ja->idlen>jb->idlen)
		return -1;
	return 1;
}
static int 
cmp_commands(const void *a, const void *b)
{
	int i;
	i=str_diff(a,b);
	if (i<0) return -1;
	if (i>0) return  1;
	if (str_len(a) > str_len(b))
		return -1;
	return 1;
}
static void
doonestr(unsigned int *l, const char *s)
{
	unsigned int sl=str_len(s);
	if (*l+sl>79) {
		if (-1==buffer_put(buffer_1,"\n  ",2)) die_write();
		*l=2;
	}
	if (-1==buffer_put(buffer_1,s,sl)) die_write();
}

static void 
show_command(const char *fname)
{
  stralloc saline=STRALLOC_INIT;
  int fd;
  char bi[4096];
  buffer i;

  fd=open_read(fname);
  if (-1==fd) { warning(errno,"failed to open_read ",fname,0,0); return;}
  buffer_init(&i,(buffer_op)read,fd,bi,sizeof(bi));
  while (1)  {
    int gotlf;
    if (-1==getln(&i,&saline,&gotlf,'\n')) 
      xbailout(111,errno,"failed to read ",fname,0,0);
    if (!saline.len) break;
    if (-1==buffer_put(buffer_1,"    ",4)) die_write();
    if (-1==buffer_put(buffer_1,saline.s,saline.len)) die_write();
  }
  close(fd);
  stralloc_free(&saline);
  if (-1==buffer_flush(buffer_1)) die_write();
}
static void
print_short_job(unsigned int maxlen, struct jobinfo *j)
{
  static stralloc sa;
  struct taia next;
  uint64 ui64;
  unsigned int l;
  if (-1==buffer_put(buffer_1,j->id,j->idlen)) die_write();
  l=j->idlen;
  while (l<maxlen) {
    if (-1==buffer_puts(buffer_1," ")) die_write();
    l++;
  }
  if (-1==buffer_puts(buffer_1," (")) die_write();
  l+=2;

  doonestr(&l,"next: ");
  if (!find_next(j,&now,&next)) {
    doonestr(&l,"never");
  } else {
    char buf[128];
    if (!fmt_tai(buf,sizeof(buf),&next.sec)) 
      doonestr(&l,buf);
    else
      doonestr(&l,"???");
  }

  doonestr(&l,", last: ");

  ui64=HACK_TAIA_SEC(&j->lastrun);       
  if (0==ui64) {
    doonestr(&l,"never");
  } else {
    char buf[128];
    if (!fmt_tai(buf,sizeof(buf),&j->lastrun.sec)) 
      doonestr(&l,buf);
    else
      doonestr(&l,"???");
  }

  if (j->repeats) {
    sa.len=0;
    doonestr(&l,", runs left: ");
    if (!stralloc_catuint0(&sa,j->repeats,0)) oom();
    if (!stralloc_0(&sa)) oom();
    doonestr(&l,sa.s);
  }
  if (j->every) {
    sa.len=0;
    doonestr(&l,", repeat every ");
    if (!stralloc_catuint0(&sa,j->every,0)) oom();
    if (!stralloc_0(&sa)) oom();
    doonestr(&l," seconds");
    doonestr(&l,sa.s);
  }
  doonestr(&l,")\n");
  if (-1==buffer_flush(buffer_1)) die_write();
}

static void 
list_schedules(char **filenames, int *flag)
{
  unsigned int i;
  unsigned int count=0;
  unsigned int maxlen=0;
  struct jobinfo *j;
  load_jobs(".",&jobs);

  for (i=0;jobs.s[i];i+=str_len(jobs.s+i)+1)
    count++;
  j=(struct jobinfo *)alloc(count*sizeof(*j));
  if (!j)
    oom();
  count=0;
  for (i=0;jobs.s[i];i+=str_len(jobs.s+i)+1) {
    parse_job(jobs.s+i,&j[count]); /* load_jobs ensures this is OK */
    if (j[count].idlen>maxlen)
      maxlen=j[count].idlen;
    count++;
  }
  mssort((char *)j,count,sizeof(*j),cmp_ji);
  for (i=0;i<count;i++) {
    if (filenames[0]) {
      unsigned int k;
      for (k=0;filenames[k];k++) {
	unsigned int l=str_len(filenames[k]);
	if (l!=j[i].idlen)
	  continue;
	if (!byte_equal(j[i].id,l,filenames[k]))
	  continue;
	break;
      }
      if (!filenames[k])
	continue;
      flag[k]=1;
    }
    if (o_short)
      print_short_job(maxlen,&j[i]);
    else
      print_job(&j[i],&now);
    if (o_show_command) {
      stralloc sa;
      make_name(&sa,&j[i]); 
      show_command(sa.s);
      stralloc_free(&sa);
    }
  }
}
static void
print_command(const char *s)
{
  struct wrap_stat st;
  static stralloc sa;
  char buf[128];

  if (-1==buffer_puts(buffer_1,s)) die_write();
  if (-1==buffer_puts(buffer_1,"\n")) die_write();

  if (-1==wrap_stat(s,&st)) {
    int e=errno;
    warning(e,"failed to stat ",s,0,0);
    exitcode=1;
    if (-1==buffer_puts(buffer_1,"  failed to stat command file: "))
	    die_write();
    if (-1==buffer_puts(buffer_1,error_str(e))) die_write();
    if (-1==buffer_puts(buffer_1,"\n")) die_write();
    if (-1==buffer_flush(buffer_1)) die_write();
    return;
  }
  if (!stralloc_copys(&sa,"  size: ")) oom();
  if (!stralloc_catuint0(&sa,st.size,0)) oom();
  if (!stralloc_cats(&sa,"\n  links: ")) oom();
  if (!stralloc_catuint0(&sa,st.nlink,0)) oom();
  if (!stralloc_cats(&sa,"\n  modification: ")) oom();

  if (!fmt_tai(buf,sizeof(buf),&st.mtime.sec)) 
  if (!stralloc_cats(&sa,buf)) oom();
  if (!stralloc_cats(&sa,"\n")) oom();

  if (-1==buffer_put(buffer_1,sa.s,sa.len)) die_write();
  if (-1==buffer_flush(buffer_1)) die_write();
}

static void
print_short_command(unsigned int maxlen, const char *s)
{
  struct wrap_stat st;
  static stralloc sa;
  char buf[128];
  unsigned int l;

  if (-1==buffer_puts(buffer_1,s)) die_write();
  l=str_len(s);
  while (l<maxlen) {
    if (-1==buffer_puts(buffer_1," ")) die_write();
    l++;
  }
  if (-1==buffer_puts(buffer_1," (")) die_write();
  l+=2;

  if (-1==wrap_stat(s,&st)) {
    int e=errno;
    warning(e,"failed to stat ",s,0,0);
    exitcode=1;
    if (-1==buffer_puts(buffer_1,error_str(e))) die_write();
    if (-1==buffer_puts(buffer_1,")\n")) die_write();
    if (-1==buffer_flush(buffer_1)) die_write();
    return;
  }
  sa.len=0;
  if (!stralloc_catuint0(&sa,st.size,0)) oom();
  if (!stralloc_0(&sa)) oom();
  doonestr(&l,sa.s);
  doonestr(&l," B, ");

  sa.len=0;
  if (!stralloc_catuint0(&sa,st.nlink,0)) oom();
  if (!stralloc_0(&sa)) oom();
  doonestr(&l,sa.s);
  doonestr(&l," L, ");
  if (!fmt_tai(buf,sizeof(buf),&st.mtime.sec)) 
  doonestr(&l,buf);
  doonestr(&l,")\n");
  if (-1==buffer_flush(buffer_1)) die_write();
}

static void 
list_commands(char **filenames, int *flag)
{
  unsigned int j;
  int count=0;
  unsigned int maxlen=0;
  static stralloc sa;
  const char *s;
  unsigned int dirflag;

  if (-1==chdir(IDDIR))
    xbailout(111,errno,"failed to chdir to ", IDDIR,  0,0);
  count=api_dir_read(&sa,".");
  if (-1==count)
    xbailout(111,errno,"failed to read .",0,0,0);
  if (!count)
    return;

  if (-1==api_dir_sort(&sa,cmp_commands)) oom();

  for (s=api_dir_walkstart(&sa,&dirflag);s;s=api_dir_walknext(&sa,&dirflag)) {
    if (filenames[0]) {
      for (j=0;filenames[j];j++) {
	if (str_equal(s,filenames[j]))
	  break;
      }
      if (!filenames[j])
	continue;
      flag[j]=1;
    }
    if (o_short)
      print_short_command(maxlen,s);
    else
      print_command(s);
    if (o_show_command)
      show_command(s);
  }
}
uogetopt_env myoptenv={	
"uschedulelist",PACKAGE,VERSION,
"uschedulelist [JOB-ID ...]",
"This program list the scheduled jobs.",
"This program by default lists the contents of the ~/.uschedule directory. "
"If any JOB-ID is given then only these jobs are listed.\n"
"The directory the jobs are read from can be changed by use of the "
"--dir (-d) or -. options.\n",
"Report bugs to uschedule@lists.ohse.de",
0,0,0,0,uogetopt_out,myopts
};

int 
main(int argc, char **argv)
{
	int *flag;
	taia_now(&now);
	bailout_progname(argv[0]);
	flag_bailout_fatal_begin=3;
	uogo_posixmode=1;
	myoptenv.program=flag_bailout_log_name;
	uogetopt_parse(&myoptenv,&argc,argv);
	if (o_now) {
	  unsigned long ul;
	  int l;
	  l=scan_ulong(o_now,&ul);
	  if (!l|o_now[l])
	    xbailout(2,0,"cannot understand `now' value `",o_now,"'",0);
	  taia_uint(&now,0);
	  tai_unix(&now.sec,ul);
	}
	change_dir(0,o_dir,o_dot_as_home);
	if (argc==1)
		flag=0;
	else {
		int i;
		flag=(int *)alloc((argc-1)*sizeof(int));
		if (!flag) oom();
		for (i=0;i<argc-1;i++)
			flag[i]=0;
	}
	if (o_commands)
		list_commands(argv+1,flag);
	else
		list_schedules(argv+1,flag);
	if (flag) {	
		int i;
		for (i=0;i<argc-1;i++) 
			if (!flag[i]) {
				warning(0,argv[i+1],": not found",0,0);
				exitcode=1;
			}
	}
	return exitcode;
}
