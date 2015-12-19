#include "buffer.h"
void base_dir(const char *d, int mode, int uid, int gid);
void set_dir(const char *d);
void log_dir(const char *user, const char *dir, int uid, int gid);
void owner(int uid,int gid);
void perm(int mode);
void finish_file(void);
void outs(const char *s);
void outb(char *s,unsigned int len);
void make_dir(const char *d);
void start_file(const char *f);
void make_fifo(const char *f,int mode);
void copyfrom(buffer *b);

