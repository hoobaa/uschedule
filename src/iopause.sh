#! /bin/sh
#
# poll/emulation decision
#
FILE=conftest$$
set -e

cat >$FILE.c <<EOF
#include <sys/types.h>
#include <fcntl.h>
#include <poll.h>

main()
{
	struct pollfd x;

	x.fd = open("$FILE.c",O_RDONLY);
	if (x.fd == -1) _exit(111);
	x.events = POLLIN;
	if (poll(&x,1,10) == -1) _exit(1);
	if (x.revents != POLLIN) _exit(1);

	/* XXX: try to detect and avoid poll() imitation libraries */
	_exit(0);
}
EOF
use=

if ./auto-compile.sh -c $FILE.c  2>/dev/null >/dev/null ; then
  if ./auto-link.sh $FILE $FILE.o ; then
    if ./$FILE ; then
      use=poll
    else
      use=select
    fi
  else
    use=select
  fi
else
  use=select
fi

cat <<EOF
#ifndef AUTO_IOPAUSE_H
#define AUTO_IOPAUSE_H
EOF

# iopause.c needs select in any case. Look for sys/select.h
cat >$FILE.c <<EOF
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
int dummy(void);
EOF
if ./auto-compile.sh -c $FILE.c  2>/dev/null >/dev/null ; then
  cat <<EOF
#define IOPAUSE_SYS_SELECT_H /* systype-info */
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
EOF
else
  cat <<EOF
#define IOPAUSE_UNISTD_H /* systype-info */
#include <unistd.h>
EOF
fi


if test "x$use" = xpoll ; then
  cat <<EOF
#define IOPAUSE_POLL /* systype-info */

#include <poll.h>

typedef struct pollfd iopause_fd;
#define IOPAUSE_READ POLLIN
#define IOPAUSE_WRITE POLLOUT

#endif
EOF
  rm -f $FILE $FILE.o $FILE.c
  exit 0
fi

# have to use select
cat <<EOF
#undef IOPAUSE_POLL /* systype-info */
typedef struct {
  int fd;
  short events;
  short revents;
} iopause_fd;

#define IOPAUSE_READ 1
#define IOPAUSE_WRITE 4

#endif
EOF
rm -f $FILE.c $FILE.o $FILE
