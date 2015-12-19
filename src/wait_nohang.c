/*
 * reimplementation of Daniel Bernstein's unix library.
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */
#include <sys/types.h>
#include <sys/wait.h>
#include "wait.h"

int wait_nohang(int *wstat)
{
  return waitpid(-1,wstat,WNOHANG);
}
