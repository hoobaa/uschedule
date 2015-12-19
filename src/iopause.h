#ifndef IOPAUSE_H
#define IOPAUSE_H
#include <sys/types.h>
#include <sys/time.h>

#include "auto-iopause.h"

#include "taia.h"

extern int iopause_force_select; /* set to 1 to enforce usage of select() */
extern void iopause(iopause_fd *,unsigned int,struct taia *,struct taia *);

#endif
