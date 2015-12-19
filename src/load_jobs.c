#include "scheduled.h"
#include "bailout.h"
#include "error.h"
#include "str.h"
#include "byte.h"
#include "api_dir.h"

/* load all the jobs from directory dn.
 * fill "jobs" array with job file names, each one terminated by \0.
 * add an additional \0 to the end of the list.
 */
void
load_jobs(const char *dn,stralloc *jobs)
{
	int count;
	unsigned int i,j;
	count=api_dir_read(jobs,dn);
	if (count==-1)
		xbailout(111,errno,"failed to read ",dn,0,0);

	/* XXX breaks api_dir encapsulation */
	for (i=0,j=0;i<jobs->len;) {
		struct jobinfo ji;
		unsigned int l;
		char *s;
		s=jobs->s+i;
		l=str_len(s)+1;
		if (parse_job(s,&ji)) {
			if (i!=j)
				byte_copy(jobs->s+j,l,s);
			j+=l;
		}
		i+=l;
	}
	jobs->len=j;
	if (!stralloc_0(jobs)) oom();
}
