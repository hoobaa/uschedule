#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "scheduled.h"
#include "bailout.h"
#include "error.h"
#include "env.h"
#include "get_cwd.h"

void
change_dir(stralloc *sa, const char *opt, int flag_dot_as_home)
{
	const char *home;
	stralloc absprefix=STRALLOC_INIT;
	char *cwd=get_cwd();
	if (!cwd) 
		xbailout(111,errno,"failed to find current directory",0,0,0);
	if (flag_dot_as_home) 
	  home=cwd;
	else
	  home=env_get("HOME");
	if (!home)
		xbailout(100,0,"$HOME is not set",0,0,0);
	/* keep things simple */
	if ('/'!=home[0]) 
		xbailout(100,0,"$HOME is not absolute: ",home,0,0);
	/* i want this to exist ... and i want to daemon to print 
	 * errors very early. */
	if (!flag_dot_as_home && -1==chdir(home)) 
		xbailout(111,errno,"failed to chdir to ",home,0,0);

	if (opt) {
		if (opt[0]=='/') {
			if (!stralloc_copys(&absprefix,opt)) oom();
		} else {
			if (!stralloc_copys(&absprefix,cwd)) oom();
			if (!stralloc_cats(&absprefix,"/")) oom();
			if (!stralloc_cats(&absprefix,opt)) oom();
			if (-1==chdir(cwd)) 
				xbailout(111,errno,"failed to chdir to ",cwd,0,0);
		}
		if (-1==chdir(opt))
			xbailout(111,errno,"failed to chdir to ",opt,0,0);
	} else {
		if (!stralloc_copys(&absprefix,home)) oom();
		if (!stralloc_cats(&absprefix,"/")) oom();
		if (!stralloc_cats(&absprefix,SCHEDULEDIR)) oom();
		if (-1==chdir(home)) 
			xbailout(111,errno,"failed to chdir to ",home,0,0);
		if (-1==chdir(SCHEDULEDIR))
			xbailout(111,errno,"failed to chdir to ",SCHEDULEDIR,0,0);
	}
	if (sa) 
		*sa=absprefix;
	else
		stralloc_free(&absprefix);
}
