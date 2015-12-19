#include "scheduled.h"
#include "bailout.h"
#include "str.h"

void check_id(const char *id)
{
	if (id[str_chr(id,':')])
		xbailout(100,0,"ID contains a colon",0,0,0);
	else if (id[str_chr(id,'/')])
		xbailout(100,0,"ID contains a slash",0,0,0);
	else if (id[str_chr(id,'.')])
		xbailout(100,0,"ID contains a dot",0,0,0);
}
