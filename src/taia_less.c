/* Reimplementation of Daniel J. Bernsteins taia library.
 * (C) 2001 Uwe Ohse, <uwe@ohse.de>.
 *   Report any bugs to <uwe@ohse.de>.
 * Placed in the public domain.
 */
/* @(#) $Id: taia_less.c 1.4 02/10/17 14:40:18+00:00 uwe@ranan.ohse.de $ */
#include "taia.h"

int taia_less(const struct taia *a,const struct taia *b)
{
/* if (taia_approx(&a) < taia_approx(&b)) return 1; return 0; */
	if (tai_less(&a->sec,&b->sec)) return 1;
	if (tai_less(&b->sec,&a->sec)) return 0;
	if (a->nano < b->nano) return 1;
	if (a->nano > b->nano) return 0;
	return a->atto < b->atto;
}
