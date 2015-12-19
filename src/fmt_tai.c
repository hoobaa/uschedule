#include "fmt_tai.h"
#include "scheduled.h"
#include <time.h>



int 
fmt_tai(char *buf, unsigned int size, struct tai *t)
{
	struct tm *tm;
	time_t tt;
	tt=t->x-TAI_UTC_OFFSET;
	tm=localtime(&tt);
	if (!tm)
		return -1;
	if (!strftime(buf,size,"%Y-%m-%d %H:%M:%S",tm))
		return -1;
	return 0;
}
