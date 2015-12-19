#include "stralloc.h"
#include "scheduled.h"
#include "bailout.h"
#include "fmt.h"

/* put a schedule file name into sa */
void 
make_name(stralloc *sa, struct jobinfo *j)
{
	int needcomma=0;
	uint64 ui64;
	unsigned int l;
	char nb[FMT_ULONG];

	if (!stralloc_copys(sa,"@")) oom();
	ui64=HACK_TAIA_SEC(&j->lastrun);
	l=fmt_xlong(nb,ui64>>32);
	if (8!=l)
		if (!stralloc_catb(sa,"00000000",8-l)) oom();
	if (!stralloc_catb(sa,nb,l)) oom();
	l=fmt_xlong(nb,ui64 & 0xffffffff);
	if (8!=l)
		if (!stralloc_catb(sa,"00000000",8-l)) oom();
	if (!stralloc_catb(sa,nb,l)) oom();
	if (!stralloc_cats(sa,":")) oom();
	/* tai finished */
	if (!stralloc_catb(sa,j->id,j->idlen)) oom();
	if (!stralloc_cats(sa,":")) oom();

	nb[fmt_ulong(nb,j->late)]=0;
	if (!stralloc_cats(sa,nb)) oom();
	if (!stralloc_cats(sa,":")) oom();

	nb[fmt_ulong(nb,j->repeats)]=0;
	if (!stralloc_cats(sa,nb)) oom();
	if (!stralloc_cats(sa,":")) oom();

	if (!stralloc_catb(sa,j->cronspec,j->cronlen)) oom();
	if (!stralloc_cats(sa,":")) oom();
	if (!stralloc_catb(sa,j->comment,j->commentlen)) oom();
	if (!stralloc_cats(sa,":")) oom();

	if (j->null1) {
		if (needcomma)
			if (!stralloc_cats(sa,",")) oom();
		if (!stralloc_cats(sa,"null1")) oom();
		needcomma=1;
	}
	if (j->null2) {
		if (needcomma)
			if (!stralloc_cats(sa,",")) oom();
		if (!stralloc_cats(sa,"null2")) oom();
		needcomma=1;
	}
	if (!stralloc_cats(sa,":")) oom();
	if (!stralloc_catb(sa,j->fromspec,j->fromlen)) oom();
	if (!stralloc_cats(sa,":")) oom();
	if (!stralloc_catb(sa,j->tospec,j->tolen)) oom();
	if (!stralloc_cats(sa,":")) oom();
	nb[fmt_ulong(nb,j->every)]=0;
	if (!stralloc_cats(sa,nb)) oom();
	if (!stralloc_0(sa)) oom();
}

