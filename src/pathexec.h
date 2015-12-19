/*
 * reimplementation of Daniel Bernstein's unix library.
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */
#ifndef PATHEXEC_H
#define PATHEXEC_H

void pathexec_run(const char *,char *const*,char *const*);
int pathexec_env(const char *varname,const char *value);
void pathexec(char *const *av);

#endif
