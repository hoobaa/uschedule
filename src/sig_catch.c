/*
 * reimplementation of Daniel Bernstein's unix library.
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */
#include <signal.h>
#include "sig.h"

/* djb supports systems without sigaction here, but i don't */

void
sig_catch (int sig, void (*fn) (int))
{
	struct sigaction sa;
	sa.sa_handler = fn;
	sa.sa_flags = 0; /* note: no SA_RESTART! */
	sigemptyset (&sa.sa_mask);
	sigaction (sig, &sa, 0);
}
