/*
 * reimplementation of Daniel Bernstein's byte library.
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */
#include "scan.h"

unsigned int
scan_plusminus (const char *str, int *sign)
{
	if (*str == '-') {
		*sign = -1;
		return 1;
	}
	*sign = 1;
	return (*str == '+');
}
