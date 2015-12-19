/*
 * reimplementation of Daniel Bernstein's byte library.
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */
#include "str.h"

unsigned int 
str_chr(const char *s,int c)
{
  char ch;
  unsigned int l=0;

  ch = c;
  for (;;) {
  	if (!s[l]) break;
	if (s[l]==ch) break;
	l++;

  	if (!s[l]) break;
	if (s[l]==ch) break;
	l++;

  	if (!s[l]) break;
	if (s[l]==ch) break;
	l++;

  	if (!s[l]) break;
	if (s[l]==ch) break;
	l++;
  }
  return l;
}
