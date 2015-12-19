/* Reimplementation of Daniel J. Bernsteins tai library.
 * (C) 2001 Uwe Ohse, <uwe@ohse.de>.
 *   Report any bugs to <uwe@ohse.de>.
 * Placed in the public domain.
 */
/* @(#) $Id: tai_add.c 1.4 02/10/17 14:42:07+00:00 uwe@ranan.ohse.de $ */
#include "tai.h"

void
tai_add (struct tai *target, const struct tai *s1, const struct tai *s2)
{
	target->x = s1->x + s2->x;
}
