/*
 * reimplementation of Daniel Bernstein's unix library.
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */
#include <signal.h>
#include "sig.h"

int sig_alarm = SIGALRM;
int sig_child = SIGCHLD;
int sig_cont = SIGCONT;
int sig_hangup = SIGHUP;
int sig_pipe = SIGPIPE;
int sig_term = SIGTERM;
int sig_int = SIGINT;
#ifdef SIGWINCH
int sig_winch = SIGWINCH;
#else
int sig_winch = 0;
#endif

void (*sig_defaulthandler)(int) = SIG_DFL;
void (*sig_ignorehandler)(int) = SIG_IGN;
