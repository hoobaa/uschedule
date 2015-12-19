/*
 * reimplementation of Daniel Bernstein's unix library.
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */
#ifndef NDELAY_H
#define NDELAY_H

extern int ndelay_on(int fd);
extern int ndelay_off(int fd);

#endif
