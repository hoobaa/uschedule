/*
 * reimplementation of Daniel Bernstein's unix library.
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */
#ifndef SIG_H
#define SIG_H

extern int sig_alarm;
extern int sig_child;
extern int sig_cont;
extern int sig_hangup;
extern int sig_pipe;
extern int sig_term;
extern int sig_int;
extern int sig_winch;

extern void (*sig_defaulthandler)(int);
extern void (*sig_ignorehandler)(int);

extern void sig_catch(int,void (*func)(int));
#define sig_ignore(sig) (sig_catch((sig),sig_ignorehandler))
#define sig_uncatch(sig) (sig_catch((sig),sig_defaulthandler))

extern void sig_block(int);
extern void sig_unblock(int);
extern void sig_blocknone(void);
extern void sig_pause(void);

extern void sig_dfl(int); /* not impl. by uo */

#endif
