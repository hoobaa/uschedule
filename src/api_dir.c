/*
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */

/* this only has the purpose of not having to polute the namespace with
 * these headers:
 */
#include <sys/stat.h>
#include <dirent.h>

#include "stralloc.h"
#include "str.h"
#include "api_dir.h"

int 
api_dir_read(stralloc *sa, const char *dn)
{
	while (1) {
		DIR *d;
		struct stat st1,st2;
		int cnt=0;
		sa->len=0;
		if (-1==stat(dn,&st1)) return -1;
		d=opendir(dn);
		if (!d) return -1;
		while (1) {
			struct dirent *e;
			e=readdir(d);
			if (!e)
				break;
			if (e->d_name[0]=='.') {
				if (e->d_name[1]=='.' && e->d_name[2]=='\0') continue;
				if (e->d_name[1]=='\0') continue;
			}
			if (!stralloc_cats(sa,e->d_name)) return -1;
			if (!stralloc_0(sa)) return -1;
			cnt++;
		}
		if (-1==closedir(d)) return -1;

		if (-1==stat(dn,&st2)) return -1;
		if (st1.st_mtime == st2.st_mtime
			&& st1.st_size == st2.st_size)
				return cnt;
	}
}

const char * 
api_dir_walkstart(stralloc *sa, unsigned int *flag)
{
	char *p;
	if (!sa->len) {
		*flag=0;
		return 0;
	}
	p=sa->s;
	*flag=str_len(p)+1;
	return p;
}

const char * 
api_dir_walknext(stralloc *sa, unsigned int *flag)
{
	char *p;
	if (*flag==sa->len)
		return 0;
	p=sa->s+*flag;
	*flag+=str_len(p)+1;
	return p;
}

void api_dir_free(stralloc *sa)
{ stralloc_free(sa); }
