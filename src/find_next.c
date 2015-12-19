#include <sys/types.h>
#include <time.h>
#include "scheduled.h"
#include "stralloc.h"
#include "bailout.h"
#include "scan.h"
#include "fmt.h"

#define OUTOFRANGE 32767

/* find the next allowed value in `s', starting from 'val'. Min and max
 * are minimum and maximum allowed values. rset is set if val is wrapped
 * around. rset is initialized to 0 in the outer level functions.
 * Note: this function assumes what "s" is terminated by something 
 * different from a number.
 */
static int 
find_next_sub(const char *s, unsigned int len, int *val, int min, 
	int max, int *rset)
{
	unsigned int i,j;
	short next=OUTOFRANGE;
	short low=OUTOFRANGE;
	if (*val > max) {
		/* 32. of month, month has 31 days ... */
		*rset=1;
		if (*val==max+1)  {
			/* overflow of lower level unit */
			*val=min;
			return 1;
		}
		*val=min;
		return 0;
	}
	i=0;
	while (i!=len) {
		long num;
		if (','==s[i]) {
			i++;
			continue;
		}
		j=scan_long(s+i,&num);
		if (!j)
			break;

		if (num<min) 
			num=min; /* just in case, although it's a bug if that happens */
		if (num<=max) {
			if (num==*val)
				return 1;
			if (num>*val && num<next)
				next=num;
			if (num<low)
				low=num;
		} else {
			/* *-*-29 in a non-leapyear february, for example */
		}
		i+=j;
	}
	if (OUTOFRANGE==next) {
		if (OUTOFRANGE==low) {
			/* *-*-29 in a non-leapyear february ... */
			return 0;
		}
		*val=low;
		*rset=1;
		return 1;
	}
	*val=next;
	return 1;
}

static short
get_monlen(int m, int y)
{
	switch(m) {
	case 0: case 2: case 4: case 6: 
	case 7: case 9: case 11: 
		return 31;
	case 1: /* february */
		if (0!= (y % 4))
			return 28;
		else if (0==(y %400))
			return 29;
		else if (0!=(y %100))
			return 29;
		return 28;
	default:
		return 30; 
	}
}

static int
find_next1(struct taia *from, struct taia *target,
	const char *spec, unsigned int speclen)
{
	time_t s70;
	struct tm dt, *tm;
	int old;
	int ret;
	short dummy;
	int rset;
	struct schedinfo si;
	uint64 ui64;

	if (!preparse_schedspec(&si, spec,speclen))
		return 0;

	ui64=HACK_TAIA_SEC(from);
	if (ui64)
		s70=ui64-TAI_UTC_OFFSET;
	else
		s70=0;
	s70++; /* "this" second already handled */
	tm=localtime(&s70);
	if (!tm) 
		return 0;
	dt=*tm;

#define NEXT(what,val,nextone,low,high) \
	old=val; rset=0; \
	if (si.what==0) ret=1; \
	else ret=find_next_sub(si.what,si.what##len,&val,low,high,&rset); \
	if (val<old) nextone++;

	while (1) {
		short monlen;

		NEXT(s,dt.tm_sec,dt.tm_min,0,59)
		if (!ret) return 0; /* second out of range */

		NEXT(m,dt.tm_min,dt.tm_hour,0,59)
		if (!ret) return 0; /* minute out of range */
		if (rset) {dt.tm_sec=0;continue; }

		NEXT(h,dt.tm_hour,dt.tm_mday,0,23)
		if (!ret) return 0; /* hour out of range */
		if (rset) {dt.tm_sec=dt.tm_min=0;continue; }

		monlen=get_monlen(dt.tm_mon,dt.tm_year+1900);
		NEXT(D,dt.tm_mday,dt.tm_mon,1,monlen)
		if (!ret) {
			/* *-*-29, but also *-*-32, or 2001/4-02-29 */
			if (dt.tm_mday>31) return 0;
			rset=1;
			dt.tm_mday=1;
			dt.tm_mon++;
			/* still possible to waste processor power using bad dates. */
		}
		if (rset) {dt.tm_sec=dt.tm_min=dt.tm_hour=0;continue; }

		NEXT(M,dt.tm_mon,dt.tm_year,0,11)
		if (!ret) return 0; /* month out of range */
		if (rset) {dt.tm_sec=dt.tm_min=dt.tm_hour=0;dt.tm_mday=1;continue; }

		dt.tm_year+=1900;
		NEXT(Y,dt.tm_year,dummy,1970,OUTOFRANGE-1)
		dt.tm_year-=1900;
		if (!ret) return 0; /* year out of range */
		if (rset) {return 0;}

		monlen=get_monlen(dt.tm_mon,dt.tm_year+1900);
		if (dt.tm_mday>monlen) {
			dt.tm_mday=1;
			dt.tm_mon++;
			dt.tm_hour=dt.tm_sec=dt.tm_min=0;
			continue;
		}
		s70=mktime(&dt);
		if (0==si.W)
			break;
		if (si.W & (1<<dt.tm_wday))
			break;
		dt.tm_mday++;
		dt.tm_sec=dt.tm_min=dt.tm_hour=0;
	}
	target->atto=0;
	target->nano=0;
	tai_unix(&target->sec,s70);
	return 1;
}

int
find_next(struct jobinfo *j, struct taia *now, struct taia *target)
{
	struct taia late;
	struct taia to;

	taia_uint(&late,j->late);
	taia_sub(&late,now,&late);
	if (taia_less(&late,&j->lastrun))
	  late=j->lastrun;
	if (j->every) {
	  struct taia x;
	  taia_uint(&x,j->every-1);
	  taia_add(&late,&late,&x);
	}
	while (1) {
		if (j->fromlen || j->tolen) {
			struct taia from;
			if (j->fromlen)
				if (!find_next1(&late,&from,j->fromspec,j->fromlen))
					return 0;
			if (j->tolen) {
				if (!find_next1(&late,&to,j->tospec,j->tolen))
					return 0;
				if (j->fromlen && taia_less(&from,&to)) {
					/* we are outside period */
					late=from;
					if (!find_next1(&late,&to,j->tospec,j->tolen))
						return 0;
				}
			} else 
				late=from;
		}

		if (!find_next1(&late,target,j->cronspec,j->cronlen))
			return 0;
		if (taia_less(target,&late))
			return 0;
		if (!j->tolen)
			return 1;	
		if (!taia_less(&to,target))
			return 1;
		late=*target;
	}
}
