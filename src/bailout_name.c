/*
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */
#include "bailout.h"

void 
bailout_progname(const char *s)
{
	unsigned int x;
	unsigned int l=0;
	int flag=0;

	for (x=0;s[x];x++)
		if (s[x]=='/') {
			l=x;
			flag=1;
		}
	if (!flag || !s[l+1])
		flag_bailout_log_name=s;
	else 
		flag_bailout_log_name=s+l+1;
}
