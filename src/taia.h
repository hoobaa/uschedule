/* Reimplementation of Daniel J. Bernsteins taia library.
 * (C) 2001 Uwe Ohse, <uwe@ohse.de>.
 *   Report any bugs to <uwe@ohse.de>.
 * Placed in the public domain.
 */
/* @(#) $Id: taia.h 1.4 02/10/17 14:40:18+00:00 uwe@ranan.ohse.de $ */
#ifndef TAIA_H
#define TAIA_H

#include "tai.h"

struct taia {
  struct tai sec;
  unsigned long nano; /* 0...999999999 */
  unsigned long atto; /* 0...999999999 */
} ;

extern void taia_now(struct taia *);
extern void taia_uint(struct taia *,unsigned int);
extern void taia_add(struct taia *,const struct taia *,const struct taia *);
extern void taia_sub(struct taia *,const struct taia *,const struct taia *);
extern void taia_half(struct taia *to,const struct taia *src);

extern void taia_tai(const struct taia *from,struct tai *to); /* taia to tai */
extern int taia_less(const struct taia *,const struct taia *);
extern double taia_approx(const struct taia *); 
extern double taia_frac(const struct taia *);

#define TAIA_FMTFRAC 19
extern unsigned int taia_fmtfrac(char *to,const struct taia *src);

#define TAIA_PACK 16
extern void taia_pack(char *to,const struct taia *src);
extern void taia_unpack(const char *src,struct taia *to);

#endif
