/*
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */
int api_dir_read(stralloc *sa, const char *dn);
const char * api_dir_walkstart(stralloc *sa, unsigned int *flag);
const char * api_dir_walknext(stralloc *sa, unsigned int *flag);
int api_dir_sort(stralloc *sa, int (*fn)(const void *, const void *));
void api_dir_free(stralloc *sa);
