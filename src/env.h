/*
 * reimplementation of Daniel Bernstein's unix library.
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */
#ifndef ENV_H
#define ENV_H

extern char **environ;
char *env_get(const char *var);

#endif
