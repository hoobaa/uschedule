#include "scheduled.h"
#include "open.h"
#include "stralloc.h"
#include "bailout.h"
#include "scan.h"
#include "error.h"
#include "api_dir.h"

int make_id(stralloc *sa)
{
	unsigned long high=0;
	int fd;
	const char *s;
	unsigned int flag;
	if (-1==api_dir_read(sa,"."))
		xbailout(111,errno,"failed to read `.'",0,0,0);
	for (s=api_dir_walkstart(sa,&flag);s;s=api_dir_walknext(sa,&flag)) {
		unsigned long ul;
		unsigned int numlen;
		numlen=scan_ulong(s,&ul);
		if (!numlen) continue;
		if (ul>high)
			high=ul;
	}
	api_dir_free(sa); /* oh, well */

	fd=-1;
	while (-1==fd) {
		if (!stralloc_copys(sa,"")) oom();
		if (!stralloc_catuint0(sa,high+1,0)) oom();
		if (!stralloc_0(sa)) oom();
		fd=open_excl(sa->s);
		if (-1==fd) {
			if (errno!=error_exist) 
				xbailout(100,errno,"failed to open_excl ",sa->s,0,0);
			high++;
		}
	}
	return fd;
}

