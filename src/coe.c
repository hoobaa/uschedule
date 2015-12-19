/*
 * reimplementation of Daniel Bernstein's unix library.
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */
#include <sys/types.h>
#include <fcntl.h>
#include "coe.h"

/* love compatability */
#ifndef FD_CLOEXEC
#define FD_CLOEXEC 1
#endif

int
coe(int fd)
{
	int st;
	st=fcntl(fd,F_GETFD);
	if (st==-1)
		return -1;
	return fcntl(fd,F_SETFD, st | FD_CLOEXEC);
}
