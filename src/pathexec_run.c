/*
 * reimplementation of Daniel Bernstein's unix library.
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */
#include <unistd.h> /* execve */
#include "error.h"
#include "stralloc.h"
#include "str.h"
#include "env.h"
#include "pathexec.h"

void
pathexec_run (const char *file, char *const *argv, char *const *envp)
{
	const char *path;
	int savederrno;
	unsigned int slash;
	unsigned int i;

	slash=str_chr(file,'/');
	if (file[slash]) {
		execve (file, argv, envp);
		return;
	}

	path = env_get ("PATH");
	if (!path)
		path = "/bin:/usr/bin";

	savederrno = 0;
	i=0;
	while (1) {
		unsigned int colon;
		static stralloc cmd;
		colon = str_chr (path+i, ':');
		if (!colon) {
			if (!stralloc_copyb (&cmd, ".",1)) return;
		} else {
			if (!stralloc_copyb (&cmd, path+i, colon)) return;
		}
		if (!stralloc_cats (&cmd, "/")) return;
		if (!stralloc_cats (&cmd, file)) return;
		if (!stralloc_0 (&cmd)) return;

		execve (cmd.s, argv, envp);
		if (errno != error_noent) {
			savederrno = errno;
			if ((errno != error_acces) && (errno != error_perm)
				&& (errno != error_isdir))
				return;
		}

		if (!path[i+colon]) {
			if (savederrno)
				errno = savederrno;
			return;
		}
		i+=colon;
		i++;
	}
}
