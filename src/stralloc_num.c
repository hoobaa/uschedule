/*
 * reimplementation of Daniel Bernstein's unix library.
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */
#include "stralloc.h"
#include "fmt.h"

int
stralloc_catulong0 (stralloc * sa, unsigned long num, unsigned int tolen)
{
	unsigned int numlen;
	unsigned int pad;

	numlen=fmt_ulong(0,num);
	pad=0;
	if (numlen < tolen)
		pad = tolen-numlen;

	if (!stralloc_readyplus (sa, numlen+pad))
		return 0;

	while (pad--)
		sa->s[sa->len++]='0';

	sa->len+=fmt_ulong(sa->s+sa->len,num);
	return 1;
}

int
stralloc_catlong0 (stralloc * sa, long num, unsigned int tolen)
{
	if (num < 0) {
		if (!stralloc_append (sa, "-"))
			return 0;
		num = -num;
	}
	return stralloc_catulong0 (sa, num, tolen);
}

#if 0
int main(void)
{
	static stralloc sa;
	if (!stralloc_catulong0(&sa,3000000000,8)) _exit(1);
	if (!stralloc_append(&sa,"-")) _exit(1);
	if (!stralloc_catulong0(&sa,42,8)) _exit(1);
	if (!stralloc_append(&sa,"-")) _exit(1);
	if (!stralloc_catlong0(&sa,-42,8)) _exit(1);
	if (!stralloc_append(&sa,"-")) _exit(1);
	if (!stralloc_catlong0(&sa,-1000000000,8)) _exit(1);
	if (!stralloc_append(&sa,"-")) _exit(1);
	if (!stralloc_catulong0(&sa,4000,2)) _exit(1);
	if (!stralloc_append(&sa,"-")) _exit(1);
	if (!stralloc_catulong0(&sa,88,2)) _exit(1);
	if (!stralloc_append(&sa,"-")) _exit(1);
	if (!stralloc_catlong0(&sa,-88,2)) _exit(1);
	if (!stralloc_append(&sa,"-")) _exit(1);
	if (!stralloc_catlong0(&sa,-88,3)) _exit(1);
	write(1,sa.s,sa.len);
}
#endif
