#include "scheduled.h"
#include "bailout.h"
#include "error.h"
#include "open.h"
#include "ndelay.h"
#include <unistd.h>

void
notice(void)
{
  int fd=open_write("fifo");
  if (-1==fd) {
    warning(errno,"fatal: failed to open_write fifo",0,0,0);
    xbailout(111,0,"is the uscheduled daemon running?",0,0,0);
  }
  ndelay_on(fd);
  if (-1==write(fd,"",1)) 
    warning(errno,"failed to write into daemon fifo",0,0,0);
  close(fd);
}
