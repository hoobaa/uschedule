/* Reimplementation of Daniel J. Bernsteins taia library.
 * (C) 2001 Uwe Ohse, <uwe@ohse.de>.
 *   Report any bugs to <uwe@ohse.de>.
 * Placed in the public domain.
 */
/* @(#) $Id: taia_approx.c 1.4 02/10/17 14:40:18+00:00 uwe@ranan.ohse.de $ */
#include "taia.h"

double
taia_approx (const struct taia *t)
{
	return tai_approx (&t->sec) + taia_frac (t);
}
