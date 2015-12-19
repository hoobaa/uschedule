/* Reimplementation of Daniel J. Bernsteins taia library.
 * (C) 2001 Uwe Ohse, <uwe@ohse.de>.
 *   Report any bugs to <uwe@ohse.de>.
 * Placed in the public domain.
 */
/* @(#) $Id: taia_now.c 1.3 01/05/03 06:21:06+00:00 uwe@fjoras.ohse.de $ */
#include <sys/types.h>
#include <sys/time.h>
#include "taia.h"

void
taia_now (struct taia *t)
{
	struct timeval tv;
	gettimeofday (&tv, (void *)0);
	tai_unix (&t->sec, tv.tv_sec);
	t->atto = 0;
	t->nano = 1000 * tv.tv_usec + 500;
}
