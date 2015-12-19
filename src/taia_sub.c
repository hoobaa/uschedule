/* Reimplementation of Daniel J. Bernsteins taia library.
 * (C) 2001 Uwe Ohse, <uwe@ohse.de>.
 *   Report any bugs to <uwe@ohse.de>.
 * Placed in the public domain.
 */
/* @(#) $Id: taia_sub.c 1.4 02/10/17 14:40:18+00:00 uwe@ranan.ohse.de $ */
#include "taia.h"

/* XXX: breaks tai encapsulation */
#define G 1000000000UL

void
taia_sub (struct taia *to, const struct taia *src1, const struct taia *src2)
{
	unsigned int carry = 0;
	tai_sub (&to->sec, &src1->sec, &src2->sec);

	if (src1->atto >= src2->atto)
		to->atto = src1->atto - src2->atto;
	else {
		to->atto = G + src1->atto - src2->atto;
		carry = 1;
	}
	if (src1->nano >= src2->nano + carry)
		to->nano = src1->nano - (src2->nano + carry);
	else {
		to->nano = G + src1->nano - (src2->nano + carry);
		/* tai_uint(&tmp,1); tai_sub(&to->sec,&to->sec,&tmp); */
		to->sec.x--;
	}
}
