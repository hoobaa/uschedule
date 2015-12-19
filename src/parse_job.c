#include "scheduled.h"
#include "bailout.h"
#include "scan.h"
#include "error.h"
#include "str.h"
#include "byte.h"

static int
match_flag_name(const char *s, unsigned int l, const char *t)
{
	if (str_len(t)!=l)
		return 0;
	return byte_equal(s,l,t);
}

/* return 0 if `s' is no valid schedule job file name.
 * return 1 if it is.
 * *j may be changed in any case and contains the job information
 * if the function returns 1.
 */
int 
parse_job(const char *s, struct jobinfo *j)
{
	unsigned int i;
	uint64 x;
	if ('@'!=*s)
		return 0;
	j->null1=0;
	j->null2=0;

	/* @tai:ID:late:repeat:cronspec:flag,flag,...[:fromspec:tospec]
	 * ^ */
	i=1+scan_xint64(s+1,&x);
	if (17!=i) /* 17='@' + 16 (length of tai) */
		goto warn;
	HACK_TAIA_SEC(&(j->lastrun))=x; /* XXX breaks tai encapsulation */
	j->lastrun.atto=0;
	j->lastrun.nano=0;
	if (':'!=s[i])
		goto warn;

	i++; /* skip : */
	/* @tai:ID:late:repeat:cronspec:flag,flag,...[:fromspec:tospec]
	 * -----^ */
	j->id=s+i;
	j->idlen=str_chr(s+i,':');
	i+=j->idlen;
	if (!s[i])	
		goto warn;
	if (!j->idlen)
		goto warn;

	i++; /* skip : */
	/* @tai:ID:late:repeat:cronspec:flag,flag,...[:fromspec:tospec]
	 * --------^ */
	i+=scan_ulong(s+i,&j->late);
	if (':'!=s[i])
		goto warn;

	i++; /* skip : */
	/* @tai:ID:late:repeat:cronspec:flag,flag,...[:fromspec:tospec]
	 * -------------^ */
	i+=scan_ulong(s+i,&j->repeats);
	if (':'!=s[i])
		goto warn;

	i++; /* skip : */
	/* @tai:ID:late:repeat:cronspec:flag,flag,...[:fromspec:tospec]
	 * --------------------^ */
	j->cronspec=s+i;
	j->cronlen=str_chr(s+i,':');
	i+=j->cronlen;
	if (':'!=s[i])
		goto warn;

	i++; /* skip : */
	/* @tai:ID:late:repeat:cronspec:flag,flag,...[:fromspec:tospec]
	 * -----------------------------^ */
	j->comment=s+i;
	j->commentlen=str_chr(s+i,':');
	i+=j->commentlen;
	if (':'!=s[i])
		goto warn;

	i++; /* skip : */
	/* @tai:ID:late:repeat:cronspec:flag,flag,...[:fromspec:tospec]
	 * -------------------------------------^ */
	while (s[i] && s[i]!=':') {
		unsigned int e;
		for (e=i;s[e] && s[e]!=',' && s[e]!=':';)
			e++;
		if (i==e)
			goto warn;
		if (match_flag_name(s+i,e-i,"null1"))
			j->null1=1;
		else if (match_flag_name(s+i,e-i,"null2"))
			j->null2=1;
		else
			goto warn;
		i=e;
		if (s[i] == ',')
			i++; /* skip comma */
	}
	j->fromspec=j->tospec=0;
	j->fromlen=j->tolen=0;
	j->every=0;
	if (s[i]!=':')
		return 1;
	i++; /* skip colon */
	/* @tai:ID:late:repeat:cronspec:flag,flag,...[:fromspec:tospec]
	 * .......-------------------------------------^ */
	j->fromspec=s+i;
	j->fromlen=str_chr(s+i,':');
	i+=j->fromlen;
	if (':'!=s[i])
		goto warn;

	i++; /* skip colon */
	/* @tai:ID:late:repeat:cronspec:flag,flag,...[:fromspec:tospec]
	 * ................-------------------------------------^ */
	j->tospec=s+i;
	j->tolen=str_chr(s+i,':');
	i+=j->tolen;
	if (':'!=s[i] && s[i])
		goto warn;
	if (!s[i]) return 1;

	i++; /* skip colon */
	/* @tai:ID:late:repeat:cronspec:flag,flag,...[:fromspec:tospec][:every]
	 * ................---------------------------------------------^ */
	i+=scan_ulong(s+i,&j->every);
	if (':'!=s[i] && s[i])
		goto warn;
	return 1;
  warn: 
  	warning(0,"cannot parse ",s, " at ",s+i);
	return 0;
}

