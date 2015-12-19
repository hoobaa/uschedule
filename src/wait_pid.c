/*
 * reimplementation of Daniel Bernstein's unix library.
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */
#include <sys/types.h>
#include <sys/wait.h>
#include "error.h"
#include "wait.h"

int wait_pid(int *wstat,int pid)
{
  int r;

  do
    r = waitpid(pid,wstat,0);
  while ((r == -1) && (errno == error_intr));
  return r;
}
