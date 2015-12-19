#include "typesize.h"
#include "taia.h"
#include "stralloc.h"
#include "attributes.h"
#define SCHEDULEDIR ".uschedule"
#define IDDIR "commands"

#define HACK_TAIA_SEC(taiap) ((taiap)->sec.x)
#define TAI_UTC_OFFSET 4611686018427387914ULL

struct schedinfo {
	const char *Y; unsigned int Ylen;
	const char *M; unsigned int Mlen;
	const char *D; unsigned int Dlen;
	const char *h; unsigned int hlen;
	const char *m; unsigned int mlen;
	const char *s; unsigned int slen;
	unsigned int W;
};
int preparse_schedspec(struct schedinfo *, const char *, unsigned int len)
  attribute_check_result;

void load_jobs(const char *d, stralloc *jobs);
struct jobinfo {
	struct taia lastrun;
	unsigned long late;
	const char *id;
	unsigned int idlen;
	const char *cronspec;
	unsigned int cronlen;
	unsigned long every;

	const char *fromspec;
	unsigned int fromlen;
	const char *tospec;
	unsigned int tolen;

	int null1;
	int null2;
	unsigned long repeats;
	const char *comment;
	unsigned int commentlen;
};
/* 0: cannot parse. 1: ok. */
int parse_job(const char *s, struct jobinfo *);
void parse_timespec(stralloc *sa, char *date);
void fill_timespec(stralloc *sa, char *date);
void timespec_from_now(stralloc *sa, const char *date);
void make_name(stralloc *sa, struct jobinfo *j) attribute_all_nonnull;
void notice(void);
void print_job(struct jobinfo *j, struct taia *now) attribute_all_nonnull;
void change_dir(stralloc *sa, const char *opt, int flag_dot_as_home);
int make_id(stralloc *id) attribute_check_result attribute_all_nonnull;
void check_id(const char *id) attribute_all_nonnull;


/* 0: failed. 
 * 1: ok.
 */
int find_next(struct jobinfo *, struct taia *now, struct taia *then) 
  attribute_check_result;
