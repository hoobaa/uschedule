#include "uogetopt.h"
#include "scheduled.h"
#include "open.h"
#include "attributes.h"
#include "stralloc.h"
#include "bailout.h"
#include "buffer.h"
#include "env.h"
#include "str.h"
#include "scan.h"
#include "error.h"
#include <unistd.h> /* read, write, fsync, chdir */
#include <sys/stat.h> /* fchmod */

static const char *o_id;
static const char *o_dir;
static int o_dot_as_home;
static int o_env=0;

static uogetopt2 myopts[]={
{'.',"dot-as-home",uogo_flag,UOGO_NOARG,&o_dot_as_home, 1 ,
"Use current directory instead of $HOME.",0,0},
{'d',"dir",uogo_string,0,&o_dir, 0 ,
"Jobdirectory.",
"Jobs will be read from this directory.","DIR"},
{'e',"copy-environment",uogo_flag,UOGO_NOARG,&o_env, 1 ,
"Clone current environment for job.",
"The default is to start new jobs in the daemons environment. This option "
"causes the current environment to be copied for the new process.",0},
{'i',"id",uogo_string,0,&o_id, 0 ,
"Unique ID for the new job.",
"An identifier must not be re-used and must not contain colons, slashes and "
"dots. If this option is not used then a unique ID will be given.", "ID"},
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

extern char **environ;
#define W(b,s,l) do { \
 if (-1==buffer_put(b,s,l)) die_write_temp(); \
} while (0)

static void
copy_env(buffer *o)
{
	unsigned int i;
	if (-1==buffer_puts(o,"env - \\\n")) die_write_temp();
	for (i=0;environ[i];i++) {
		char *e=environ[i];
		unsigned int j;
		j=str_chr(e,'=');
		if (!e[j])
			continue;
		W(o,e,j+1);
		W(o,"'",1);
		j++;
		while (e[j]) {
			if (e[j]=='\'')
				W(o,"'\\'",3);
			W(o,e+j,1);
			j++;
		}
		W(o,"' \\\n",4);
	}
}


static void
create_job(int fd, char **args)
{
	buffer i;
	char ispace[512];
	buffer o;
	char ospace[512];

	buffer_init(&o,(buffer_op) write,fd,ospace,sizeof(ospace)); 
	if (-1==buffer_puts(&o,"#! /bin/sh\n")) die_write_temp();
	if (o_env) {
		static stralloc fn;
		if (!stralloc_copys(&fn,o_id)) oom();
		if (!stralloc_cats(&fn,".run")) oom();
		if (!stralloc_0(&fn)) oom();

		if (-1==buffer_puts(&o,"exec \\\n")) die_write_temp();
		copy_env(&o);
		if (-1==buffer_puts(&o,"/bin/sh -c ")) die_write_temp();
		if (-1==buffer_puts(&o,"\"$SCHEDULED_COMMAND_FILE\"/'")) 
			die_write_temp();
		if (-1==buffer_puts(&o,fn.s)) die_write_temp();
		if (-1==buffer_puts(&o,"'\n")) die_write_temp();
		if (-1==buffer_flush(&o)) die_write_temp();
		if (-1==fchmod(fd,0755)) die_fchmod();
		if (-1==fsync(fd)) die_fsync();
		if (-1==close(fd)) die_write_temp();
		fd=open_excl(fn.s);
		if (fd==-1)
			xbailout(111,errno,"failed to open_excl ",fn.s,0,0);
		buffer_init(&o,(buffer_op) write,fd,ospace,sizeof(ospace)); 
		if (-1==buffer_puts(&o,"#! /bin/sh\n")) die_write_temp();
	} 

	if (args) {
		unsigned int j;
		const char *spc="";
		for (j=0;args[j];j++) {
			if (-1==buffer_puts(&o,spc)) die_write_temp();
			if (-1==buffer_puts(&o,args[j])) die_write_temp();
			spc=" ";
		}
	} else {
		buffer_init(&i,(buffer_op) read,0,ispace,sizeof(ispace)); 
		switch (buffer_copy(&o,&i)) {
		case 0: break;
		case -2: xbailout(111,errno,"failed to read input",0,0,0);
		case -3: die_write_temp();
		default: xbailout(100,errno,"unexpected errorcode from buffer_copy",0,0,0);
		}
	}
	if (-1==buffer_puts(&o,"\n")) die_write_temp();
	if (-1==buffer_flush(&o)) die_write_temp();
	if (-1==fchmod(fd,0755)) die_fchmod();
	if (-1==fsync(fd)) die_fsync();
	if (-1==close(fd)) die_write_temp();
}
static uogetopt_env myoptenv={
"uschedulecmd",PACKAGE,VERSION,
"uschedulecmd [options] [COMMAND ...]",
"This program creates a schedule command executing COMMAND or, if that is "
"not given, a command read from the standard input.",
"long",
"Report bugs to uschedule@lists.ohse.de",
0,0,0,0,uogetopt_out,myopts
};


int main(int argc, char **argv) 
{
	int fd=-1; /* keep gcc quiet */
	int prt=0;

	bailout_progname(argv[0]);
	flag_bailout_fatal_begin=3;
	uogo_posixmode=1;
	myoptenv.program=flag_bailout_log_name;
	uogetopt_parse(&myoptenv,&argc,argv);

	change_dir(0,o_dir,o_dot_as_home);
	if (-1==chdir(IDDIR)) 
		xbailout(111,errno,"failed to chdir to ",IDDIR, " directory",0);

	if (!o_id) {
		static stralloc sat;
		fd=make_id(&sat);
		o_id=sat.s;
		prt=1;
	} else {
		check_id(o_id);
		fd=open_excl(o_id);
		if (-1==fd)
			xbailout(111,errno,"failed to open_excl ",o_id,0,0);
	}

	create_job(fd,argv[1] ? argv+1 : 0);
	if (prt) {
		if (-1==buffer_puts(buffer_1,"The id of the new job is ")) die_write();
		if (-1==buffer_puts(buffer_1,o_id)) die_write();
		if (-1==buffer_puts(buffer_1,"\n")) die_write();      
		if (-1==buffer_flush(buffer_1)) die_write();      
	}
	return 0;
}
