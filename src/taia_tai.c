/* Reimplementation of Daniel J. Bernsteins taia library.
 * (C) 2001 Uwe Ohse, <uwe@ohse.de>.
 *   Report any bugs to <uwe@ohse.de>.
 * Placed in the public domain.
 */
/* @(#) $Id: taia_tai.c 1.4 02/10/17 14:40:18+00:00 uwe@ranan.ohse.de $ */
#include "taia.h"

/* taia2tai. ugh. tai_taia? */
void
taia_tai (const struct taia *ta, struct tai *t)
{
	*t = ta->sec;
}
