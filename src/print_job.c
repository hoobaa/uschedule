#include "scheduled.h"
#include "bailout.h"
#include "buffer.h"
#include "fmt.h"
#include "str.h"
#include "error.h"
#include "attributes.h"
#include "fmt_tai.h"

static void die_write(void) attribute_noreturn;
static void die_write(void)  {xbailout(111,errno,"failed to write",0,0,0);}

static void outb(const char *s, unsigned int len)
{ if (-1==buffer_put(buffer_1,s,len)) die_write(); }
#define PRTFIELD(x) do { \
	    if (si.x) outb(si.x,si.x##len); else outs("*"); \
    } while(0)
static void outnum(unsigned long ul) 
{ char nb[FMT_ULONG]; outb(nb,fmt_ulong(nb,ul)); }
static void outs(const char *s)
{ outb(s,str_len(s)); }
static void 
print_months(const char *s, unsigned int l)
{
  unsigned long ul=0;
  unsigned int ul_valid=0;
  unsigned int i;
  for (i=0;i<l;i++) {
    unsigned char c=s[i];
    if (c<'0'||c>'9') {
      if (ul_valid)
	outnum(ul+1);
      ul_valid=0;
      ul=0;
      outb((const char *)&c,1);
    }  else {
      ul*=10;
      ul_valid++;
      ul+=c-'0';
    }
  }
  if (ul_valid)
    outnum(ul+1);
}

static void
print_schedule(const char *str, unsigned int len)
{
	struct schedinfo si;

	if (!preparse_schedspec(&si,str,len)) {
		outs("unable to parse ");
		outb(str,len);
		return;
	}
	PRTFIELD(Y);  outs("-");
	if (si.M)
	  print_months(si.M,si.Mlen);
	else
	  outs("*");
	outs("-");
	PRTFIELD(D);  outs(" ");
	PRTFIELD(h);  outs(":");
	PRTFIELD(m);  outs(":");
	PRTFIELD(s);  outs("");
	if (si.W) {
		const char *se="";
		outs(" Only on ");
		if (si.W & 1) {outs("Sun");se=",";}
		if (si.W & 2) {outs(se); outs("Mon");se=",";}
		if (si.W & 4) {outs(se); outs("Tue");se=",";}
		if (si.W & 8) {outs(se); outs("Wed");se=",";}
		if (si.W & 16) {outs(se); outs("Thu");se=",";}
		if (si.W & 32) {outs(se); outs("Fri");se=",";}
		if (si.W & 64) {outs(se); outs("Sat");se=",";}
	}
}

void
print_job(struct jobinfo *j, struct taia *now)
{
	struct taia next;
	uint64 ui64;

	outb(j->id,j->idlen);
	outs("\n");

	outs("  schedule: ");
	print_schedule(j->cronspec, j->cronlen);
	outs("\n");

	if (j->commentlen) {
		outs("  comment: ");
		outb(j->comment, j->commentlen);
		outs("\n");
	}

	ui64=HACK_TAIA_SEC(&j->lastrun);
	if (0==ui64) {
		outs("  last: never");
	} else {
		char buf[128];

		outs("  last: ");
		if (0==fmt_tai(buf,sizeof(buf),&j->lastrun.sec))
			outs(buf);
		else
			outs("cannot convert date");
	}
	outs("\n");

	if (!find_next(j,now,&next)) {
		outs("  next: never");
	} else {
		char buf[128];
		outs("  next: ");
		if (0==fmt_tai(buf,sizeof(buf),&next.sec))
			outs(buf);
		else
			outs("cannot convert date");
	}
	outs("\n");

	outs("  grace: ");
	if (0==j->late) {
		outs("none. Must not start late.");
	} else {
		outs("may start up to ");
		outnum(j->late);
		outs(" seconds late.");
	}
	outs("\n");
	if (j->every) {
		unsigned long ul=j->every;
		outs("  repeat: ");
#define DO(x,y) if (ul>(x)) {outnum(ul/(x)); outs(y);ul%=x;if (ul) outs(", ");}
		DO(7*86400," weeks")
		DO(86400," days")
		DO(3600," hours ")
		DO(60," minutes")
                if (ul) {outnum(ul); outs(" seconds");}
		outs("\n");
#undef DO
	}

	if (j->repeats==1) {
		outs("  count: will run job once.");
		outs("\n");
	} else if (j->repeats) {
		outs("  count: will run up to ");
		outnum(j->repeats);
		outs(" times, as long as the time specification matches.");
		outs("\n");
	}

	if (j->null1)
		outs("  standard output is redirected to /dev/null.\n");

	if (j->null2)
		outs("  standard error output is redirected to /dev/null.\n");
	if (j->fromlen) {
	  outs("  from: ");
	  print_schedule(j->fromspec, j->fromlen);
	  outs("\n");
	}
	if (j->tolen) {
	  outs("  to: ");
	  print_schedule(j->tospec, j->tolen);
	  outs("\n");
	}
	if (-1==buffer_flush(buffer_1)) die_write();
}
