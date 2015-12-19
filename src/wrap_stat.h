#include "typesize.h"
#include "taia.h"
struct wrap_stat {
    uo_uint64_t dev;          /* inode's device */
    uo_uint64_t ino;          /* inode's number */
    uo_uint64_t mode;         /* inode protection mode */
    uo_uint64_t nlink;        /* number of hard links */
    uo_uint32_t uid;          /* user ID of the file's owner */
    uo_uint32_t gid;          /* group ID of the file's group */
    uo_uint64_t rdev;         /* device type */
    struct taia atime;        /* time of last access */
    struct taia mtime;        /* time of last data modification */
    struct taia ctime;        /* time of last file status change */
    uo_uint64_t size;         /* file size, in bytes */
};
int    wrap_fstat (int, struct wrap_stat *);
int    wrap_stat (const char *, struct wrap_stat *);
int    wrap_lstat (const char *, struct wrap_stat *);
