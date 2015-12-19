#ifndef UO_LOCK_H
#define UO_LOCK_H

int uolock_share(int fd);
int uolock_excl(int fd);
int uolock_unlock(int fd);
int uolock_tryshare(int fd);
int uolock_tryexcl(int fd);

#endif
