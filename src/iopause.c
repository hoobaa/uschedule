/*
 * reimplementation of Daniel Bernstein's unix library.
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */
#include <string.h> /* FD_ZERO might use memset */
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include "taia.h"
#include "iopause.h"

int iopause_force_select;

static int 
calc_timeout(struct taia *then, struct taia *now)
{
	double d;
	struct taia tmp;
	if (taia_less (then, now))
		return 0;
	taia_sub (&tmp, then, now);
	d = taia_approx (&tmp);
	if (d > 1000.0)
		d = 1000.0; /* hu? */
	return d * 1000.0 + 20.0;
}

#ifdef IOPAUSE_POLL
static void
iopause_poll (iopause_fd * iop, unsigned int n, struct taia *then,
		 struct taia *now)
{
	poll (iop, n, calc_timeout(then,now));
}
#endif

static void
iopause_select (iopause_fd * iop, unsigned int n, struct taia *then,
		 struct taia *now)
{
	struct timeval tv;
	fd_set rfds;
	fd_set wfds;
	int highfd;
	unsigned int i;
	int ms;

	FD_ZERO (&rfds);
	FD_ZERO (&wfds);

	highfd = 0;
	for (i = 0; i < n; i++) {
		iop[i].revents = 0;
		if (iop[i].fd < 0) /* EBADF */
			continue;
		if (iop[i].fd >= (int) (8 * sizeof (fd_set))) /* EBADF */
			continue;
		if (iop[i].fd > highfd)
			highfd=iop[i].fd;
		if (iop[i].events & IOPAUSE_READ)
			FD_SET (iop[i].fd, &rfds);
		if (iop[i].events & IOPAUSE_WRITE)
			FD_SET (iop[i].fd, &wfds);
	}
	ms=calc_timeout(then,now);

	tv.tv_sec = ms / 1000;
	tv.tv_usec = 1000 * (ms % 1000);

	if (select (highfd+1, &rfds, &wfds, 0, &tv) <= 0)
		return;

	for (i = 0; i < n; i++) {
		if (iop[i].fd < 0)
			continue;
		if (iop[i].fd >= (int) (8 * sizeof (fd_set)))
			continue;
		if (iop[i].events & IOPAUSE_READ) {
			if (FD_ISSET (iop[i].fd, &rfds))
				iop[i].revents |= IOPAUSE_READ;
		}
		if (iop[i].events & IOPAUSE_WRITE) {
			if (FD_ISSET (iop[i].fd, &wfds))
				iop[i].revents |= IOPAUSE_WRITE;
		}
	}
}

void
iopause (iopause_fd * iop, unsigned int n, struct taia *then,
		 struct taia *now)
{
#ifdef IOPAUSE_POLL
	if (!iopause_force_select) {
		iopause_poll(iop,n,then,now);
		return;
	}
#endif
	iopause_select(iop,n,then,now);
}
