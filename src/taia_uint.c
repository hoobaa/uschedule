/* Reimplementation of Daniel J. Bernsteins taia library.
 * (C) 2001 Uwe Ohse, <uwe@ohse.de>.
 *   Report any bugs to <uwe@ohse.de>.
 * Placed in the public domain.
 */
/* @(#) $Id: taia_uint.c 1.3 01/05/03 06:21:07+00:00 uwe@fjoras.ohse.de $ */
#include "taia.h"

void
taia_uint (struct taia *to, unsigned int sec)
{
	tai_uint(&to->sec,sec);
	to->nano = to->atto = 0;
}
