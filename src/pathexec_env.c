/*
 * reimplementation of Daniel Bernstein's unix library.
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */
#include "stralloc.h"
#include "alloc.h"
#include "str.h"
#include "byte.h"
#include "env.h"
#include "pathexec.h"

static stralloc plusminus;

int
pathexec_env (const char *name, const char *var)
{
	static stralloc tmp;
	if (!name) return 1;
	if (!stralloc_copys (&tmp, name)) return 0;
	if (var) {
		if (!stralloc_cats (&tmp, "=")) return 0;
		if (!stralloc_cats (&tmp, var)) return 0;
	}
	if (!stralloc_0 (&tmp)) return 0;
	if (!stralloc_cat (&plusminus, &tmp)) return 0;
	return 1;
}

void
pathexec (char *const*argv)
{
	char **ne;
	unsigned int count;
	unsigned int i;

	if (!stralloc_0 (&plusminus)) return;
	for (count=0;environ[count];)
		count++;
	for (i = 0; plusminus.s[i]; i+=str_len(plusminus.s+i)+1)
		count++;

	ne = (char **) alloc ((count + 1) * sizeof (char *));
	if (!ne)
		return;

	for (count=0;environ[count];count++)
		ne[count] = environ[count];
	
	for (i=0;plusminus.s[i];i+=str_len(plusminus.s+i)+1) {
		unsigned int eq;
		unsigned int j;
		eq=str_chr(plusminus.s+i,'=');
		for (j=0;j<count;j++)
			if (byte_equal(ne[j],eq,plusminus.s+i) && ne[j][eq]=='=') {
/*			if (str_start(ne[j],plusminus.s+i) && ne[j][eq]=='=') {*/
				ne[j]=ne[--count];
				break;
			}

		if (plusminus.s[i + eq])
			ne[count++] = plusminus.s + i;
	}
	ne[count] = 0;

	pathexec_run (*argv, argv, ne);
	alloc_free ((char *)ne);
}
