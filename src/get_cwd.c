/* 
 * (C) 2001 Uwe Ohse, <uwe@ohse.de>.
 * Placed in the public domain.
 */
/* @(#) $Id: get_cwd.c 1.6 01/05/03 20:08:26+00:00 uwe@fjoras.ohse.de $ */
#include "alloc.h"
#include "error.h"
#include "stralloc.h"
#include "byte.h"
#include "str.h"
#include "get_cwd.h"
#include <unistd.h>

char *
get_cwd(void)
{
	stralloc sa=STRALLOC_INIT;
	unsigned int m=256;
	while (m<=8192) { /* ought to be enough */
		if (!stralloc_ready(&sa,m+1)) return 0;
		if (getcwd(sa.s,m)) {
			char *p;
			m=str_len(sa.s)+1;
			p=alloc(m);
			if (!p)
				return sa.s;
			byte_copy(p,m,sa.s);
			stralloc_free(&sa);
			return p;
		}
		if (errno==error_acces)	
			return 0;
		m*=2;
	}
	return 0;
}
