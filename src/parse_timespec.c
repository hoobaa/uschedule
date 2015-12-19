#include "scheduled.h"
#include "uogetopt.h"
#include "bailout.h"
#include "attributes.h"
#include "buffer.h"
#include "str.h"
#include "scan.h"
#include "case.h"
#include "uotime.h"
#include <time.h>

static void die_scan(const char *s,const char *t) attribute_noreturn;
static void die_scan(const char *s,const char *t)
{ xbailout(100,0,"unable to parse date: ",s,t ? " at " : "" ,t); }

static unsigned int
scan_part(const char *s, const unsigned char sep,
	const char what, stralloc *sa, unsigned int base, 
	unsigned int min, unsigned int max)
{
	unsigned int len=0;
	unsigned int i=0;
	unsigned int oldlen=sa->len;
	unsigned long num=0;
	int needcomma=0;
	int gotstar=0;
	if (!stralloc_catb(sa,&what,1)) oom();
	while (1) {
		if (sep==s[i]) { /* sep may be \0, so this has to come first */
			if (gotstar)
				sa->len=oldlen;
			if (sa->len==oldlen+1)
				sa->len=oldlen;
			return i+1;
		}
		if (!s[i]) 
			die_scan("unexpected end of date",0);
		switch(s[i]) {
		case '*':
			i++;
			gotstar=1;
			break;
		case ',':
			if (!len)
				die_scan("unexpected , ",s+i);
			needcomma=1;
			len=0;
			num=0;
			i++;
			break;
		case '+': 
		case '/': 
			{
				unsigned int l;
				unsigned int j;
				unsigned long num2;
				if (!len)
					die_scan("unexpected / or +",s+i);
				i++;
				l=scan_ulong(s+i,&num2);
				if (!l)
					die_scan("bad number after / or +",s+i);
				num+=num2;
				for (j=0;num<=max;j++) {
					if (!stralloc_catb(sa,",",1)) oom();
					if (!stralloc_catuint0(sa,num,0)) oom();
					num+=num2;
				}
				len=0; /* guard against x/y/z */
				i+=l;
			}
			break;
		default:
			if (len)
				die_scan("unexpected number",s+i);
			len=scan_ulong(s+i,&num);
			if (!len)
				die_scan("bad number",s+i);
			if (base && num<base)
				die_scan("number too low",s+i);
			if (needcomma)
				if (!stralloc_catb(sa,",",1)) oom();
			num-=base;
			if (num>max)
				die_scan("number too high",s+i);
			if (num<min)
				die_scan("number too low",s+i);
			if (!stralloc_catuint0(sa,num,0)) oom();
			i+=len;
			break;
		}
	}
}

static int iswd(const char *s, const char *t)
{
	unsigned int l;
	if (!case_starts(s,t))
		return 0;
	l=str_len(t);
	if (','==s[l] || ' '==s[l])
		return 1;
	return 0;
}

static unsigned int 
parse_weekday(char *s, unsigned int len)
{
	unsigned int wd=0;
	unsigned int i;
	i=0;
	while (i<len) {
		unsigned int comma;
		comma=str_chr(s+i,',');
		if (!comma)
			die_scan("bad weekday",s+i);
		if      (iswd(s+i,"Sun") || iswd(s+i,"Sunday"))    wd|=1;
		else if (iswd(s+i,"Mon") || iswd(s+i,"Monday"))    wd|=2;
		else if (iswd(s+i,"Tue") || iswd(s+i,"Tuesday"))   wd|=4;
		else if (iswd(s+i,"Wed") || iswd(s+i,"Wednesday")) wd|=8;
		else if (iswd(s+i,"Thu") || iswd(s+i,"Thursday"))  wd|=16;
		else if (iswd(s+i,"Fri") || iswd(s+i,"Friday"))    wd|=32;
		else if (iswd(s+i,"Sat") || iswd(s+i,"Saturday"))  wd|=64;
		else
			die_scan("bad weekday",s+i);
		if (!s[i+comma])
			break;
		i+=comma+1;
	}
	return wd;
	
}

void
parse_timespec(stralloc *sa, char *date)
{
	unsigned int i;
	unsigned int spc;
	if (!stralloc_copys(sa,"")) oom();
	/* Sat,Tue 2001-01-* 16:00:00 */
	/* 2001-01-* 16:00:00 */
	for (i=0;date[i];i++)
		if (date[i]=='\t')
			date[i]=' ';
	while (*date==' ') date++;
	spc=str_chr(date,' ');
	if (!date[spc])
		die_scan("bad date",0);
	if (date[spc+1+str_chr(date+spc+1,' ')]) {
		unsigned int w;
		w=parse_weekday(date,spc);
		if (127!=w && 0!=w) {
			if (!stralloc_cats(sa,"W")) oom();
			if (!stralloc_catuint0(sa,w,0)) oom();
		}
		date+=spc;
		while (*date==' ') date++;
	}
	i=0;
	/* XXX 2100 is a bad maximum year ... */
	i+=scan_part(date+i,'-','Y',sa,0,1970,2100);
	i+=scan_part(date+i,'-','M',sa,1,0,11);
	i+=scan_part(date+i,' ','D',sa,0,1,31);
	while (date[i]==' ') date++;
	i+=scan_part(date+i,':','h',sa,0,0,23);
	i+=scan_part(date+i,':','m',sa,0,0,59);
	i+=scan_part(date+i,  0,'s',sa,0,0,59);
}

void
fill_timespec(stralloc *sa, char *date)
{
  /* Sat,Mon 16:17:18 -> Sat,Mon *-*-* 16:17:18 */
  /* Sat,Mon 1-2-3 -> Sat,Mon 1-2-3 0:0:0 */
  /* 16:17:18 -> *-*-* 16:17:18 */
  /* 18 -> *-*-* *:*:18 */

  /* create trailing zeros, otherwise wildcards */
  unsigned int i;
  unsigned int dashs;
  unsigned int colons;
  if (!stralloc_copys(sa,"")) oom();
  for (i=0;date[i];i++) {
    if (date[i]==' ') break;
    if (date[i]=='*') break;
    if (date[i]=='-') break;
    if (date[i]==':') break;
    if (date[i]>='0' && date[i]<='9') break;
  }
  if (date[i]==' ') {
    if (parse_weekday(date, i)) {
      if (!stralloc_catb(sa,date,i+1)) oom();
      date+=i+1;
    }
  }
  dashs=0;
  colons=0;
  for (i=0;date[i];i++) {
    if (date[i]=='-') dashs++;
    if (date[i]==':') colons++;
    if (date[i]==' ') break;
  }
  if (colons) {
    if (!stralloc_cats(sa,"*-*-* ")) oom();
  } else if (dashs==2) {
    if (!stralloc_catb(sa,date,i)) oom();
    date+=i;
  } else if (dashs==1) {
    if (!stralloc_cats(sa,"*-")) oom();
    if (!stralloc_catb(sa,date,i)) oom();
    date+=i;
  } else if (date[i]) { /* a single word */
    if (!stralloc_cats(sa,"*-*-")) oom();
    if (!stralloc_catb(sa,date,i)) oom();
    date+=i;
  } else {
    if (!stralloc_cats(sa,"*-*-* ")) oom();
  }
  if (date[0]==' ' || !date[0]) {
    if (!stralloc_catb(sa," ",1)) oom();
    if (date[0]) date++;
  }
  i=0;
  if (!date[i]) {
    if (!stralloc_cats(sa,"0:0:0")) oom();
  } else {
    colons=0;
    for (i=0;date[i];i++)
      if (date[i]==':')
	colons++;
    if (colons==2) {
      if (!stralloc_cats(sa,date)) oom();
    } else if (colons==1) {
      if (!stralloc_cats(sa,"*:")) oom();
      if (!stralloc_cats(sa,date)) oom();
    } else {
      if (!stralloc_cats(sa,"*:*:")) oom();
      if (!stralloc_cats(sa,date)) oom();
    }
  }
  if (!stralloc_0(sa)) oom();
}

void
timespec_from_now(stralloc *target, const char *s)
{	
  int i=0;
  const char *os=s;
  unsigned long add=0;
  uo_sec70_t s70;
  uo_datetime_t d;
  while (*s) {
    unsigned long ul;
    unsigned int l=scan_ulong(s,&ul);
    if (s[l] && s[l]!=':') die_scan(os,s);
    if (!l) die_scan(os,s);
    if (i==1) add*=60; /* sec -> min */
    else if (i==2) add*=60; /* min->hour */
    else if (i==3) add*=60; /* hour->day */
    else if (i) die_scan(os,0);
    i++;
    add+=ul;
    s+=l;
    if (s[0]==':')
      s++;
  }
  s70=uo_now();
  s70+=add;
  uo_sec702dt(&d,&s70);
  if (!stralloc_copys(target,"")) oom();
  if (!stralloc_cats(target,"Y")) oom();
  if (!stralloc_catuint0(target,d.year,0)) oom();
  if (!stralloc_cats(target,"M")) oom();
  if (!stralloc_catuint0(target,d.mon,0)) oom();
  if (!stralloc_cats(target,"D")) oom();
  if (!stralloc_catuint0(target,d.day,0)) oom();
  if (!stralloc_cats(target,"h")) oom();
  if (!stralloc_catuint0(target,d.hour,0)) oom();
  if (!stralloc_cats(target,"m")) oom();
  if (!stralloc_catuint0(target,d.min,0)) oom();
  if (!stralloc_cats(target,"s")) oom();
  if (!stralloc_catuint0(target,d.sec,0)) oom();
}
