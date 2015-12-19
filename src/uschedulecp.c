#include "uogetopt.h"
#include "scheduled.h"
#include "open.h"
#include "attributes.h"
#include "stralloc.h"
#include "bailout.h"
#include "buffer.h"
#include "error.h"
#include <unistd.h>
#include <sys/stat.h>

static const char *oldid;
static const char *newid;
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

static void die_write(void) attribute_noreturn;
static void die_write_temp(void) attribute_noreturn;
static void die_fsync(void) attribute_noreturn;
static void die_fchmod(void) attribute_noreturn;

static void die_write(void)  {xbailout(111,errno,"failed to write",0,0,0);}
static void die_write_temp(void)
{ xbailout(111,errno,"failed to write temporary file",0,0,0); }
static void die_fsync(void)
{ xbailout(111,errno,"failed to fsync temporary file",0,0,0); }
static void die_fchmod(void)
{ xbailout(111,errno,"failed to fchmod temporary file",0,0,0); }

static void 
cp1(int rfd, int wfd)
{
	buffer i;
	char ispace[512];
	buffer o;
	char ospace[512];
	
	buffer_init(&i,(buffer_op) read,rfd,ispace,sizeof(ispace)); 
	buffer_init(&o,(buffer_op) write,wfd,ospace,sizeof(ospace)); 
	switch (buffer_copy(&o,&i)) {
	case 0: break;
	case -2: xbailout(111,errno,"failed to read input",0,0,0);
	case -3: die_write_temp();
	default: xbailout(100,errno,"unexpected errorcode from buffer_copy",0,0,0);
	}

	if (-1==buffer_flush(&o)) die_write_temp();
	if (-1==fchmod(wfd,0755)) die_fchmod();
	if (-1==fsync(wfd)) die_fsync();
}

static void
copyit(int rfd, int wfd)
{
	static stralloc sa;

	cp1(rfd,wfd);
	close(wfd);

	if (!stralloc_copys(&sa,oldid)) oom();
	if (!stralloc_cats(&sa,".run")) oom();
	if (!stralloc_0(&sa)) oom();
	
	close(rfd);
	rfd=open_read(sa.s);
	if (-1==rfd) {
		int e=errno;
		if (errno==error_noent)
			return; /* <-------- point of return if no .run file exists */
		unlink(newid);
		xbailout(111,e,"failed to open_read ",sa.s,0,0);
	}
	if (!stralloc_copys(&sa,newid)) oom();
	if (!stralloc_cats(&sa,".run")) oom();
	if (!stralloc_0(&sa)) oom();

	wfd=open_excl(sa.s);
	if (-1==wfd) {
		int e=errno;
		unlink(newid);
		xbailout(111,e,"failed to open_excl ",sa.s,0,0);
	}
	cp1(rfd,wfd);
	close(wfd);

}

static uogetopt_env myoptenv={
"uschedulecp",PACKAGE,VERSION,
"uschedulecp [options] OLD-ID [NEW-ID]",
"This program creates a new uschedule job by copying an old one.\n",
"long",
"Report bugs to uschedule@lists.ohse.de",
2,3,0,0,uogetopt_out,myopts
};


int main(int argc, char **argv) 
{
	struct tai now;
	int wfd=-1;
	int rfd;
	int prt=0;
	tai_now(&now);
	bailout_progname(argv[0]);
	flag_bailout_fatal_begin=3;
	uogo_posixmode=1;
	myoptenv.program=flag_bailout_log_name;
	uogetopt_parse(&myoptenv,&argc,argv);

	change_dir(0,o_dir,o_dot_as_home);
	if (-1==chdir(IDDIR))
		xbailout(111,errno,"failed to chdir to ",IDDIR, " directory",0);
	oldid=argv[1];
	newid=argv[2];

	rfd=open_read(oldid);
	if (-1==rfd) 
		xbailout(111,errno,"failed to open job ",oldid, " for reading",0);
	if (!newid) {
		static stralloc sa;
		wfd=make_id(&sa);
		newid=sa.s;
		prt=1;
	} else {
		check_id(newid);
		wfd=open_excl(newid);
		if (-1==wfd)
			xbailout(111,errno,"failed to open_excl ",newid,0,0);
	}
	copyit(rfd,wfd);
	if (prt) {
		if (-1==buffer_puts(buffer_1,"The id of the new job is ")) die_write();
		if (-1==buffer_puts(buffer_1,newid)) die_write();
		if (-1==buffer_puts(buffer_1,"\n")) die_write();
		if (-1==buffer_flush(buffer_1)) die_write();
	}        
	return 0;
}
