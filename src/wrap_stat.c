#include <sys/types.h>
#include <sys/stat.h>
#include "wrap_stat.h"
static void do_wrap_stat(struct stat *st, struct wrap_stat *w)
{
	w->dev=st->st_dev;
	w->ino=st->st_ino;
	w->mode=st->st_mode;
	w->nlink=st->st_nlink;
	w->uid=st->st_uid;
	w->gid=st->st_gid;
	w->rdev=st->st_rdev;
	w->size=st->st_size;
	w->atime.atto=w->ctime.atto=w->mtime.atto=0;
	w->atime.nano=w->ctime.nano=w->mtime.nano=0;
	tai_unix(&w->atime.sec,st->st_atime);
	tai_unix(&w->mtime.sec,st->st_mtime);
	tai_unix(&w->ctime.sec,st->st_ctime);
}

int    wrap_fstat (int a, struct wrap_stat *b)
{ struct stat st; int x=fstat(a,&st); if (x==-1) return x;
  do_wrap_stat(&st,b); return x; }
int    wrap_stat (const char *a, struct wrap_stat *b)
{ struct stat st; int x=stat(a,&st); if (x==-1) return x;
  do_wrap_stat(&st,b); return x; }
int    wrap_lstat (const char *a, struct wrap_stat *b)
{ struct stat st; int x=lstat(a,&st); if (x==-1) return x;
  do_wrap_stat(&st,b); return x; }
