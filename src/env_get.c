/*
 * reimplementation of Daniel Bernstein's unix library.
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */
#include "env.h"

extern char **environ;
char *env_get(const char *var)
{
	int e;
	for (e=0;environ[e];e++) {
		char *p=environ[e];
		int i;
		for (i=0;var[i];i++)
			if (p[i]!=var[i])
				break;
		if (p[i]=='=' && var[i]=='\0')
			return p+i+1;
		p+=i;
		while (*p) p++;
		p++;
	}
	return 0;
}
	
