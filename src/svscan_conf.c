#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "buffer.h"
#include "svscan_conf.h"
#include "bailout.h"
#include "str.h"
#include "open.h"
#include "fifo.h"
#include "error.h"
#include "attributes.h"
static const char *dir;
static const char *fname;

static void do_error(const char *s) attribute_noreturn;
static void do_error(const char *s)
{
	if (fname && (fname[0]=='/' || !dir))
		xbailout(111,errno,s,fname,0,0);
	else if (fname)
		xbailout(111,errno,s,dir,"/",fname);
	else
		xbailout(111,errno,s,dir,0,0);
}

void make_dir(const char *d)
{
	fname = d;
	if (mkdir(d,0700) == -1) 
		do_error("failed to create ");
}
void set_dir(const char *d)
{
	dir = d;
	fname=0;
	umask(022);
	if (chdir(dir) == -1)
		do_error("failed to chdir to ");
}

void 
base_dir(const char *d, int permission, int uid, int gid)
{
	dir=0;
	make_dir(d); /* sets fname */
	owner(uid,gid);
	perm(permission); /* needs fname */
	set_dir(d); /* chdir to d */
	fname=0;
}

static int fd;
static char bufspace[1024];
static buffer buf;

void make_fifo(const char *f,int mode)
{
	fname=f;
	if (-1==mkfifo(f,mode))
		do_error("failed to create fifo ");
}

void start_file(const char *f)
{
	fname=f;
	fd=open_trunc(f);
	if (-1==fd)
		do_error("failed to create ");
	buffer_init(&buf,(buffer_op)write,fd,bufspace,sizeof(bufspace));
}
void outs(const char *s)
{
	if (buffer_puts(&buf,s) == -1) 
		do_error("failed to write to ");
}

void outb(char *s,unsigned int len)
{
	if (buffer_put(&buf,s,len) == -1)
		do_error("failed to write to ");
}

void finish_file(void)
{
	if (buffer_flush(&buf) == -1) 
		do_error("failed to write to ");
	if (fsync(fd) == -1) 
		do_error("failed to fsync ");
	close(fd);
}

void perm(int mode)
{
	if (chmod(fname,mode) == -1)
		do_error("failed to change mode of ");
}

void owner(int uid,int gid)
{
	if (chown(fname,uid,gid) == -1)
		do_error("failed to change owner of ");
}

void
log_dir(const char *user, const char *ldir, int uid, int gid)
{
	make_dir("log");
	perm(02755);

	if (ldir) make_dir(ldir);
	else make_dir("log/main");
	owner(uid,gid);
	perm(02755);

	start_file("log/status");
	finish_file();
	owner(uid,gid);
	perm(0644);

	start_file("log/run");
	outs("#!/bin/sh\n");
	outs("umask 077\n");
	outs("exec");
	if (user && *user)
		outs(" setuidgid "); outs(user);
	outs(" multilog t ");
	if (ldir) outs(ldir);
	else outs("./main");
	outs("\n");
	finish_file();
	perm(0755);
}


void copyfrom(buffer *b)
{
	if (buffer_copy(&buf,b) < 0)
		do_error("failed to write to ");
}

