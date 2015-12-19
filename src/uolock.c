#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "uolock.h"

static int
lock_region (int fd, int cmd, int typ, off_t start, int whence, off_t len)
{
	struct flock lock;
	lock.l_type = typ;
	lock.l_start = start;
	lock.l_len = len;
	lock.l_whence = whence;
	return fcntl (fd, cmd, &lock);
}

int uolock_share(int fd)
{ return lock_region(fd,F_SETLKW, F_RDLCK, 0, SEEK_SET, 0); }
int uolock_tryshare(int fd)
{ return lock_region(fd,F_SETLK, F_RDLCK, 0, SEEK_SET, 0); }
int uolock_excl(int fd)
{ return lock_region(fd,F_SETLKW, F_WRLCK, 0, SEEK_SET, 0); }
int uolock_tryexcl(int fd)
{ return lock_region(fd,F_SETLK, F_WRLCK, 0, SEEK_SET, 0); }
int uolock_unlock(int fd)
{ return lock_region(fd,F_SETLK, F_UNLCK, 0, SEEK_SET, 0); }


