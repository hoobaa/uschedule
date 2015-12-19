/*
 * Copyright (C) 2000-2002 Uwe Ohse, uwe@ohse.de
 * This is free software, licensed under the terms of the GNU Lesser
 * General Public License Version 2.1, of which a copy is stored at:
 *    http://www.ohse.de/uwe/licenses/LGPL-2.1
 * Later versions may or may not apply, see 
 *    http://www.ohse.de/uwe/licenses/
 * for information after a newer version has been published.
 */

/* uogetopt.c: somewhat GNU compatible reimplementation of getopt(_long) */

/*
 * main differences:
 * + new feature: --version 
 * + new feature: --help  (standard help text, one line per option)
 * + new feature: --help OPTION (maybe somewhat more)
 * + new feature: --longhelp (all --help OPTION)
 * o really no support for i18n.
 * o small code, about 65% of GNU C library stuff.
 * o it doesn't do black magic, like that GNU stuff.
 *
 * Note that the following statement from the GNU getopt.c was the cause
 * for reinventing the wheel:
 *    Bash 2.0 puts a special variable in the environment for each
 *    command it runs, specifying which ARGV elements are the results of
 *    file name wildcard expansion and therefore should not be
 *    considered as options.
 * i decided that this wheel is to broken to be reused. Think of that
 * "-i" trick. As time passed by i calmed down, but now uogetopt is
 * better than GNU getopt ...
 * And in any case: uogetopt is shorter.
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "readwrite.h"
#include "str.h"
#include "env.h"
#include "scan.h"
#include "uogetopt.h"
#include "attributes.h"

extern void _exit(int) attribute_noreturn;

static void uogetopt_printver(uogetopt_env *env, int maxlen) 
	attribute_regparm(2);
static void outplus(uogetopt_env *e,const char *s) 
	attribute_regparm(2);
static unsigned int outandcount(uogetopt_env *env, const char *s) 
  attribute_regparm(2);
static void uogetopt_printhelp(uogetopt_env *env, int mode)
	attribute_regparm(2);

int uogo_posixmode=0;

static int dummy;
static uogetopt2 hack_version=
    {0,"version",uogo_flag,UOGO_NOARG,&dummy,0,
		"Show version: ",0,0
	};
static uogetopt2 hack_help=
    {0,"help",   uogo_flag,UOGO_NOARG,&dummy,0,
       /* 12345678901234567890123456789012345678901234567890" */
	 "Show a list of options or the long help on one.",
	 "The use with an argument shows the long help text\n"
	 "of that option, without an argument it will list\n"
	 "all options.","[OPTION-NAME]"
	};
static uogetopt2 hack_longhelp=
    {0,"longhelp",   uogo_flag,UOGO_NOARG,&dummy,0,
	     "Show longer help texts for all or one option.",0
       /* 12345678901234567890123456789012345678901234567890" */
		 ,"[OPTION-NAME]"
	};

static char minus_help[]="--help";
static char minus_version[]="--version";
static char minus_manhelp_synopsis[]="--manhelp-synopsis";
static char minus_manhelp_long_desc[]="--manhelp-long-desc";
static char minus_manhelp_short_desc[]="--manhelp-short-desc";
static char minus_manhelp_tail[]="--manhelp-tail";
static char minus_manhelp_options[]="--manhelp-options";


#define L longname
#define S shortname
#define LLEN 80
#define OFFSET 29 /* keep me < LLEN */

void 
uogetopt_out(uogetopt_env *e, const char *s)
{
  write(e->fd,s,str_len(s));
}

static void
uogetopt_outn (uogetopt_env * e, 
  const char *s, const char *t, const char *u,
  const char *v)
{
  e->out (e,s);
  if (t) e->out (e, t);
  if (u) e->out (e, u);
  if (v) e->out (e, v);
}
#define O2(s,t) do { uogetopt_outn(env,(s),(t),0,0); } while (0)
#define O3(s,t,u) do { uogetopt_outn(env,(s),(t),(u),0); } while (0)
#define O4(s,t,u,v) do { uogetopt_outn(env,(s),(t),(u),(v)); } while (0)
#define E2(s,t) E4(s,t,0,0)
#define E4(s,t,u,v) do { uogetopt_outn(env,(s),(t),(u),(v)); } while (0)

static void 
uogo_umbruch(uogetopt_env *env, const char *p, const char *indent)
{
  unsigned int i;
  unsigned int pos;
  char minbuf[2];
  void (*out)(struct uogetopt_env *,const char *)=env->out;
  unsigned int offset;
  minbuf[1]=0;

  out(env,indent);
  offset=str_len(indent);
  pos=offset;

  i=0;
  while (1) {
    unsigned int j;
  sol: /* at start of line */
    j=i;
    while (p[j]) {
      minbuf[0]=p[j];
      out(env,minbuf);
      pos++;
      if (p[j]=='\n') {
	out(env,indent);
	pos=offset;
	i=j+1;
	goto sol;
      }
      j++;
      if (p[j-1]==' ')
	break;
    }
    i=j;
    /* first word done in any case */
    while (1) {
      if (!p[i])
	break;
      for (j=i;p[j]!=' ' && p[j]!='\n' && p[j];)
	j++;
      if (pos+j-i+1>LLEN && pos!=offset) {
	out(env,"\n");
	out(env,indent);
	pos=offset;
	goto sol;
      }
      while (i<=j) {
	if (!p[i])
	  break;
	minbuf[0]=p[i++];
	pos++;
	out(env,minbuf);
      }
      if (!p[i])	break;
      if (i && p[i-1]=='\n') {
//	  out(env,"\n");
	out(env,indent);
	pos=offset;
	goto sol;
      }
    }
    if (!p[i])
      break;
  }
  if (!p[i]) out(env,"\n");
} // while (EXPECT(*p,1) && EXPECT(p[1],1));

static void attribute_regparm(2)
outplus(uogetopt_env *e, const char *s)
{
  unsigned int l;
  e->out(e,s); 
  l=str_len(s);
  if (s[l-1]!='\n')
    e->out(e,"\n");
}
#define PRINTHELP_MODE_USAGE 0
#define PRINTHELP_MODE_SHORT 1
#define PRINTHELP_MODE_LONG  2
#define PRINTHELP_MODE_OPTIONS  3
static void
uogetopt_describe(uogetopt_env *env,uogetopt2 *opt, int mode)
{
/* 
  -s, --size                 print size of each file, in blocks
  -S                         sort by file size
      --sort=WORD            ctime -c, extension -X, none -U, size -S,
123456789012345678901234567890
I don't really know for what the spaces at the start at the line are
for, but i suppose someone will need them ...
*/
  int l;
  const char *p;
  char buf[LLEN+1];
  char minbuf[3];
  void (*out)(struct uogetopt_env *,const char *)=env->out;
  if (opt->function==uogo_label) {
    if (opt->shorthelp && opt->shorthelp[0])
      outplus(env,opt->shorthelp);
    return;
  }

  for (l=0;l<OFFSET;l++)
    buf[l]=' ';
  buf[l]=0;

  if (mode!=PRINTHELP_MODE_OPTIONS)
    out(env,"  ");
  if (opt->S) { 
    /* -X */
    minbuf[0]='-'; 
    minbuf[1]=opt->S; 
    minbuf[2]=0; 
    out(env,minbuf);
  } else if (mode!=PRINTHELP_MODE_OPTIONS)
    out(env,"  ");

  if (opt->S && opt->L) out(env,", --");
  else {
    if (mode!=PRINTHELP_MODE_OPTIONS) out(env,"  ");
    out(env,"--");
  }

  l=8;

  if (opt->L) l+=outandcount(env,opt->L);

  if (opt->flags & UOGO_OPTARG) {
    const char *x;
    if (opt->L) out(env,"[=");
    else      out(env," [");
    if (opt->paraname) x=opt->paraname;
    else x="ARG";
    l+=outandcount(env,x);
    out(env,"]");
    l+=3;
  } else if ( (opt->flags & UOGO_NOARG) == 0) {
    const char *x;
    if (opt->L) out(env,"=");
    else      out(env," ");
    if (opt->paraname) x=opt->paraname;
    else x="ARG";
    l+=outandcount(env,x);
    l+=1;
  }
  minbuf[1]=0;
  if (mode==PRINTHELP_MODE_OPTIONS) {
    out(env,"\n");
    out(env,"  ");
    out(env,opt->shorthelp);
    if (opt==&hack_version) {
      /* -1 for trailing dot */
      uogetopt_printver(env,LLEN-OFFSET-1-str_len(opt->shorthelp));
      out(env,".\n");
      return;
    }
    out(env,"\n");
    if (opt->longhelp && 0==(opt->flags & UOGO_NOLHD)) {
      unsigned int i;
      int sol=1;
      for (i=0;opt->longhelp[i];i++) {
	if (sol)
	  out(env,"  ");
	sol=0;
	minbuf[0]=opt->longhelp[i];
	out(env,minbuf);
	if (minbuf[0]=='\n')
	  sol=1;
      }
      if (opt->longhelp[i-1]!='\n')
	out(env,"\n");
    }
    return;
  }

  /* fill up with spaces - or start at the next line */
  if (l>=OFFSET) {out(env,"\n");out(env,buf);}
  else out(env,buf+l);

  if (opt==&hack_version) {
    out(env,opt->shorthelp);
    /* -1 for trailing dot */
    uogetopt_printver(env,LLEN-OFFSET-1-str_len(opt->shorthelp));
    out(env,".\n");
    return;
  }

  /* 1. line of help */
  outplus(env,opt->shorthelp); 
  if (mode<PRINTHELP_MODE_LONG) return;

  p=opt->longhelp;
  if (opt->flags & UOGO_UNINDENT) 
    buf[2]=0;
  if (p && 0==(opt->flags & UOGO_NOLHD)) 
    uogo_umbruch(env,p,buf);
}

int
uogo_flag(struct uogetopt_env *e,uogetopt2 *x,char *ignored)
{
  (void) e;
  (void) ignored;
  *((int*)x->var)=x->value;
  return 0;
}
int
uogo_flagor(struct uogetopt_env *e,uogetopt2 *x,char *ignored)
{
  (void) e;
  (void) ignored;
  *((int*)x->var)|=x->value;
  return 0;
}
int
uogo_ulong(struct uogetopt_env *env,uogetopt2 *x,char *s)
{
  unsigned int l;
  l=scan_ulong(s,(unsigned long *)x->var);
  if (l && !s[l]) return 0;
  E4(env->program,": ",s,": not a number.\n");
  return 1;
}
int
uogo_long(struct uogetopt_env *env, uogetopt2 *x,char *s)
{
  unsigned int l;
  l=scan_long(s,(unsigned long *)x->var);
  if (l && !s[l]) return 0;
  E4(env->program,": ",s,": not a number.\n");
  return 1;
}
int
uogo_string(struct uogetopt_env *env, uogetopt2 *x,char *s)
{
  (void) env;
  *((char **)x->var)=s;
  return 0;
}
int
uogo_print_help_as_error(struct uogetopt_env *env, uogetopt2 *x,char *s)
{
  (void) s;
  uogo_umbruch(env,x->longhelp,"");
  return 2;
}
int
uogo_print_help(struct uogetopt_env *env, uogetopt2 *x,char *s)
{
  env->fd=1;
  uogo_print_help_as_error(env,x,s);
  env->fd=2;
  return 0;
}
int
uogo_label(struct uogetopt_env *env, uogetopt2 *x,char *s)
{
  int ofd=env->fd;
  (void)s;
  env->fd=1;
  uogo_umbruch(env,x->longhelp,"");
  env->fd=ofd;
  return 0;
}
int uogo_include(struct uogetopt_env *env, uogetopt2 *x,char *s)
  attribute_noreturn;
int
uogo_include(struct uogetopt_env *env, uogetopt2 *x,char *s)
{	
  (void) x;
  (void) s;
  env->out(env,"bad programming: uogo_include found in uogetopt_parse\n");
  _exit(100);
}
static unsigned int attribute_regparm(2)
outandcount(uogetopt_env *env, const char *s)
{
  if (!s) return 0;
  env->out(env,s);
  return str_len(s);
}


static void attribute_regparm(2)
uogetopt_printver(uogetopt_env *env, int maxlen)
{
  int l;
  maxlen-=str_len(env->version);
  l=str_len(env->program)+1;
  if (l <= maxlen) {
    O2(env->program," ");
    maxlen-=l;
  }
  if (env->package) {
    l=str_len(env->package)+3;
    if (l <= maxlen)
      O3("(",env->package,") ");
  }
  env->out(env,env->version);
}

static void  attribute_regparm(2)
uogetopt_printhelp(uogetopt_env *env, int mode)
{
  int i;
  uogetopt2 *opts=env->opts;
  /* uogetopt_printver(out,prog,package,version); 
   * is against the GNU standards */
  if (mode!=PRINTHELP_MODE_OPTIONS) {
    if (env->synopsis) outplus(env,env->synopsis); 
    else { O3("usage: ",env->program," [options]\n"); }
    if (mode==PRINTHELP_MODE_USAGE)
      return;
    env->out(env,"\n");
    if (env->short_desc)
      outplus(env,env->short_desc); 
    if (mode>PRINTHELP_MODE_SHORT) {
      if (env->short_desc && env->long_desc)
	env->out(env,"\n");
      if (env->long_desc) {
	uogo_umbruch(env,env->long_desc,"");
	/* outplus(env,env->long_desc);  */
	env->out(env,"\n");
      }
    }
  }
  if (mode!=PRINTHELP_MODE_USAGE) {
    for (i=0;opts[i].S || opts[i].L;i++)
      if (!(opts[i].flags & UOGO_HIDDEN))
	uogetopt_describe(env,&opts[i],mode);
    uogetopt_describe(env,&hack_version,mode);
    uogetopt_describe(env,&hack_help,mode);
    uogetopt_describe(env,&hack_longhelp,mode);
    if (mode!=PRINTHELP_MODE_OPTIONS)
      if (env->tail) 
	outplus(env,env->tail);
  }
}

int 
uogetopt_parse(uogetopt_env *env, int *argc, char **argv)
{
  int i;
  int posix;
  int newargc;
  int ocount;
  int is_longhelp;
  int h_used=0;
  int v_used=0;
  int ques_used=0;
  int V_used=0;
  uogetopt2 *copyright=0;
  uogetopt2 *caveat=0;
  uogetopt2 *opts=env->opts;

  if (!env->program)
    env->program="???";

  env->fd=2;
#define OUT(s) do {env->out(env,s);} while(0)

  for (i=0;opts[i].S || opts[i].L;i++) {
    if (opts[i].function == uogo_include)
      uogo_include(env,&opts[i],0);
#if 0
    if (!opts[i].var && !(opts[i].argtype&UOGO_HIDDEN)) {
      int at;
      at=opts[i].argtype & ~(FLAGS);
      /* no use to waste code for detailed error messages, this only
       * happens during development.
       */
      if (at!=UOGO_PRINT && at != UOGO_TEXT) {
	out(1,"NULL variable address in uogetopt().\n");
	_exit(1); /* programmers fault, terminate program */
      }
    }
#endif
    if (opts[i].L && str_equal(opts[i].L,"copyright")) copyright=&opts[i];
    if (opts[i].L && str_equal(opts[i].L,"caveat")) caveat=&opts[i];
    switch (opts[i].S) {
    case 'v': v_used=1; break;
    case 'h': h_used=1; break;
    case 'V': V_used=1; break;
    case '?': ques_used=1; break;
    }
  }
  if (!argv[1]) {
    newargc=1;
    goto checkrest;
  }

  /* try to map -?, -h to --help, -V, -v to --version */
  if (!argv[2] && argv[1][0]=='-') {
    if (argv[1][1]=='h' && !h_used) argv[1]=minus_help;
    if (argv[1][1]=='?' && !ques_used) argv[1]=minus_help;
    if (argv[1][1]=='v' && !v_used) argv[1]=minus_version;
    if (argv[1][1]=='V' && !V_used) argv[1]=minus_version;
  }
  ocount=i;
  if (argv[1] && str_equal(argv[1],minus_manhelp_synopsis)) {
    env->fd=1; outplus(env,env->synopsis); env->fd=2; goto exithelpversion;
  }
  if (argv[1] && str_equal(argv[1],minus_manhelp_long_desc)) {
    env->fd=1; outplus(env,env->long_desc); env->fd=2; goto exithelpversion;
  }
  if (argv[1] && str_equal(argv[1],minus_manhelp_short_desc)) {
    env->fd=1; outplus(env,env->short_desc); env->fd=2; goto exithelpversion;
  }
  if (argv[1] && str_equal(argv[1],minus_manhelp_tail)) {
    env->fd=1; outplus(env,env->tail); env->fd=2; goto exithelpversion;
  }
  if (argv[1] && str_equal(argv[1],minus_manhelp_options)) {
    env->fd=1;
    uogetopt_printhelp(env,PRINTHELP_MODE_OPTIONS);
    env->fd=2;
    goto exithelpversion;
  }
  if (argv[1] && str_equal(argv[1],minus_version)) {
    env->fd=1;
    uogetopt_printver(env,LLEN);
    OUT("\n");
    if (copyright && copyright->longhelp) {
      OUT("\n");
      outplus(env,copyright->longhelp);
    }
    if (caveat && caveat->longhelp) {
      OUT("\n");
      outplus(env,caveat->longhelp);
    }
    env->fd=2;
    goto exithelpversion;
  }
  is_longhelp=(str_equal(argv[1],"--longhelp"));
  if (argv[1] && (is_longhelp || str_equal(argv[1],minus_help))) {
    env->fd=1;
    if (argv[2]) { 
      uogetopt2 *u=0;
      if (argv[2][0]=='-') argv[2]++;
      if (argv[2][0]=='-' && argv[2][1]) argv[2]++;
      for (i=0;i<ocount;i++) {
	if (opts[i].L && str_equal(opts[i].L,argv[2])) break;
	if (opts[i].S && !argv[2][1] && argv[2][0]==opts[i].S) break;
      }
      if (i!=ocount) u=&opts[i]; 
      else if (str_equal(argv[2],"longhelp")) u=&hack_longhelp;
      else if (str_equal(argv[2],"help")) u=&hack_help;
      else if (str_equal(argv[2],"version")) u=&hack_version;
      if (!u) { OUT("no such option\n"); goto exiterr; }
      if (!u->shorthelp) { OUT("no help available\n"); goto exiterr; }
      uogetopt_describe(env,u,PRINTHELP_MODE_LONG);
    } else
      uogetopt_printhelp(env,PRINTHELP_MODE_SHORT+is_longhelp);
    env->fd=2;
    goto exithelpversion;
  }

  if (uogo_posixmode)
    posix=1;
  else
    posix=!!env_get("POSIXLY_CORRECT");
  newargc=1;
  for (i=1;i<*argc;i++) {
    if (*argv[i]!='-' || !argv[i][1]) {
      if (posix) { 
      copyrest:
	while (argv[i]) argv[newargc++]=argv[i++];
	argv[newargc]=0;
	*argc=newargc;
	goto checkrest;
      }
      argv[newargc++]=argv[i];
      continue;
    }
    if (argv[i][1]=='-') { 
      int j;
      int ioff;
      char *para;
      uogetopt2 *o;
      unsigned int paraoff;
      int r;

      if (!argv[i][2]) { i++; goto copyrest; } /* -- */

      o=opts;

      /* --x=y */
      paraoff=str_chr(argv[i],'=');
      if (argv[i][paraoff]) { 
	para=argv[i]+paraoff; 
	*para++=0;
	ioff=0;
      } else {
	ioff=1;
	para=0;
      }

      for (j=0;j<ocount;o++,j++)
	if (o->L && str_equal(o->L,argv[i]+2)) 
	  break;
      if (j==ocount) {
	E4(env->program,": illegal option -- ",argv[i],"\n");
	goto exiterr;
      }
      if (ioff==1) {
	if (o->flags & UOGO_OPTARG) {
	  if (argv[i+1] && argv[i+1][0]!='-') 
	    para=argv[i+1];
	  if (!para) {
	    char one[]="1";
	    para=one;
	    ioff=0;
	  }
	} else if (o->flags & UOGO_NOARG)
	  ioff=0;
	else
	  para=argv[i+1];
      }
      if ((o->flags & UOGO_NOARG) && para) {
	E4(env->program,": option doesn't allow an argument -- ",argv[i],"\n");
	goto exiterr;
      }
      if ((o->flags & UOGO_NOARG)==0 && !para) {
	E4(env->program,": option requires an argument -- ",argv[i]+2,"\n");
	goto exiterr;
      }
      r=o->function(env,o,para);
      if (r==1)
	goto exiterr;
      if (r==2)
	goto exithelpversion;
      if (o->flags & UOGO_EXIT) 
	goto exithelpversion;

      i+=ioff;
      continue;
    } else { 
      int j;
      for (j=1;argv[i][j];j++) { /* all chars */
	char c=argv[i][j];
	int k;
	char *p=0;
	int r;
	char optstr[2];
	int nexti=i;
	int was_flag=0;
	uogetopt2 *o;
	optstr[0]=c;
	optstr[1]=0;
	o=opts;
	for (k=0;k<ocount;k++,o++)
	  if (o->S && o->S==c) 
	    break;
	if (k==ocount) {
	  E4(env->program,": illegal option -- ",optstr,"\n");
	  goto exiterr;
	}

	if (o->flags & UOGO_NOARG)
	  was_flag=1;
	else {
	  /* options with arguments, first get arg */
	  p=argv[i]+j+1; 
	  if (!*p) {
	    if (argv[i+1]) { 
	      p=argv[i+1];
	      nexti=i+1;
	    } else
	      p=0;
	  }
	  if (o->flags & UOGO_OPTARG) {
	    if (p && *p=='-') {
	      p=0;
	      nexti=i;
	    }
	    if (!p) {
	      char one[]="1";
	      was_flag=1;
	      p=one;
	    }
	  }
	}
	if (!p && !was_flag) {
	  E4(env->program,": option requires an argument -- ",
		  optstr,"\n");
	  goto exiterr;
	}

	r=o->function(env,o,p);
	if (r==1)
	  goto exiterr;
	if (r==2)
	  goto exithelpversion;
	if (o->flags & UOGO_EXIT) 
	  goto exithelpversion;
	/* i == nexti means we have to honor the other letters in the
	 * string, so do not "break". */
	if (i!=nexti) {
	  i=nexti;
	  break;
	}
	if (!was_flag) 
	  break;
      }
    }
  }
  *argc=newargc;
  argv[newargc]=0;
  checkrest:
    if (env->minargs && newargc < env->minargs) {
	  E2(env->program,": need more");
	  goto finish_argcount;
	}
    if (env->maxargs && newargc > env->maxargs) {
	  E2(env->program,": too many");
   finish_argcount:
	  env->out(env,
	    " arguments. Use the --help option for more information.\n");
	  uogetopt_printhelp(env,PRINTHELP_MODE_USAGE);
	  goto exiterr;
	}
	return 1;
  exithelpversion:
    if (env->return_on_error) return 2;
    _exit(0);
  exiterr:
    if (env->return_on_error) return 1;
    _exit(1);
}
