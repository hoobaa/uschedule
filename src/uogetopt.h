/*
 * Copyright (C) 2000-2002 Uwe Ohse, uwe@ohse.de
 * This is free software, licensed under the terms of the GNU Lesser
 * General Public License Version 2.1, of which a copy is stored at:
 *    http://www.ohse.de/uwe/licenses/LGPL-2.1
 * Later versions may or may not apply, see 
 *    http://www.ohse.de/uwe/licenses/
 * for information after a newer version has been published.
 */
#ifndef UOGETOPT_H
#define UOGETOPT_H

struct uogetopt2;
struct uogetopt_env;
typedef int (*uogo_functype)(struct uogetopt_env *,struct uogetopt2 *, char *);

extern int uogo_flag(struct uogetopt_env *,struct uogetopt2 *, char *);
extern int uogo_flagor(struct uogetopt_env *,struct uogetopt2 *, char *);
extern int uogo_ulong(struct uogetopt_env *,struct uogetopt2 *, char *);
extern int uogo_long(struct uogetopt_env *,struct uogetopt2 *, char *);
extern int uogo_double(struct uogetopt_env *,struct uogetopt2 *, char *);
extern int uogo_float(struct uogetopt_env *,struct uogetopt2 *, char *);
extern int uogo_string(struct uogetopt_env *,struct uogetopt2 *, char *);

extern int uogo_label(struct uogetopt_env *,struct uogetopt2 *, char *);
extern int uogo_include(struct uogetopt_env *,struct uogetopt2 *, char *);
extern int uogo_print_help(struct uogetopt_env *,struct uogetopt2 *, char *);
extern int uogo_print_help_as_error(struct uogetopt_env *,struct uogetopt2 *, 
  char *);

/* flags */
#define UOGO_HIDDEN  0x1 /* don't show this in --help or --longhelp */
#define UOGO_OPTARG  0x2 /* option may or may not have an argument */
#define UOGO_NOARG   0x4 /* option has no argument */
#define UOGO_EXIT    0x8 /* exit if option was given (like --help) */
#define UOGO_UNINDENT 0x10 /* do not indent more then 2 spaces deep*/
#define UOGO_NOLHD    0x20 /* do not show long help by default */

extern int uogo_posixmode; /* defaults to off */
typedef struct uogetopt2 {
	char shortname;
	const char *longname;
	uogo_functype function;
	int flags;
	void *var; /* mandatory */
	unsigned long value;
	const char *shorthelp;
	const char *longhelp;
	const char *paraname;
} uogetopt2;

typedef struct  uogetopt_env {
	const char *program;
	const char *package;
	const char *version;
	const char *synopsis;
	const char *short_desc;
	const char *long_desc;
	const char *tail;
	int minargs;
	int maxargs;
	int fd; /* internal */
	int return_on_error;
	void (*out)(struct uogetopt_env *,const char *);
	uogetopt2 *opts;
} uogetopt_env;
int uogetopt_parse(uogetopt_env *,int *argc, char **argv); /* 1 ok, 0 error */

void uogetopt_out (uogetopt_env *e, const char *s);
uogetopt2 *uogetopt_join(uogetopt2 *);
void uogetopt_free(uogetopt2 *); /* free what join() allocated */
#endif
