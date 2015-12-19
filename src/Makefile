VERSION=0.7.1
PACKAGE=uschedule
CLASS=admin


CC=gcc
L=./auto-link.sh
C=./auto-compile.sh
W=./auto-compilew.sh
MYMAKE=make
PACKAGE_VERSION=$(PACKAGE)-$(VERSION)
PREMAKE_DEFS=-DPACKAGE_VERSION=\"$(PACKAGE)-$(VERSION)\" \
 -DPACKAGE=\"$(PACKAGE)\" -DVERSION=\"$(VERSION)\" -DPACKAGE_CLASS=\"$(CLASS)\"

TARGETS=$(EXTRA_TARGETS) $(PREMAKE_TARGETS)
CLEANFILES=$(EXTRA_CLEANFILES) $(PREMAKE_CLEANFILES)
MAINTAINER_CLEANFILES=$(EXTRA_MAINTAINER_CLEANFILES) $(PREMAKE_MAINTAINER_CLEANFILES)
PREMAKE_DIST=$(PACKAGE_VERSION).tar.gz
#
#
# the dependency of socket_ux on ipv6 is here because premake2 can't 
# handle it otherwise.














PREMAKE_TARGETS = uscheduled uschedulerm uschedulelist uschedulecmd \
uschedulecp uscheduleedit uschedule uscheduleruntimelimit \
uscheduleconf uschedule.1 uschedule_intro.7 uschedulecmd.1 \
uscheduleconf.1 uschedulecp.1 uscheduled.8 uscheduleedit.1 \
uschedulelist.1 uschedulerm.1 uscheduleruntimelimit.1
 
PREMAKE_CLEANFILES = check.out t.* conftest* auto-version.tmac \
install.html.t uschedule.html.t intro.html.t uschedulecmd.html.t \
uscheduleconf.html.t uschedulecp.html.t uscheduled.html.t \
uscheduleedit.html.t uschedulelist.html.t uschedulerm.html.t \
uscheduleruntimelimit.html.t INSTALL.t uschedule.1 uschedule.1.t \
uschedule_intro.7 uschedule_intro.7.t uschedulecmd.1 uschedulecmd.1.t \
uscheduleconf.1 uscheduleconf.1.t uschedulecp.1 uschedulecp.1.t \
uscheduled.8 uscheduled.8.t uscheduleedit.1 uscheduleedit.1.t \
uschedulelist.1 uschedulelist.1.t uschedulerm.1 uschedulerm.1.t \
uscheduleruntimelimit.1 uscheduleruntimelimit.1.t auto-typesize.h \
auto-typesize.stamp *.stamp
 
PREMAKE_MAINTAINER_CLEANFILES = install.html uschedule.html intro.html \
uschedulecmd.html uscheduleconf.html uschedulecp.html uscheduled.html \
uscheduleedit.html uschedulelist.html uschedulerm.html \
uscheduleruntimelimit.html INSTALL
 

libbyte_a_OBJECTS=byte_chr.o byte_copy.o byte_cr.o byte_diff.o \
case_lwrtab.o case_starts.o fmt_ulong.o fmt_xlong.o scan_long.o \
scan_sign.o scan_ulong.o scan_xint64.o str_chr.o str_diff.o str_len.o \
str_rchr.o str_start.o
 
libtai_a_OBJECTS=tai_add.o tai_now.o tai_sub.o tai_uint.o taia_add.o \
taia_approx.o taia_frac.o taia_less.o taia_now.o taia_sub.o taia_tai.o \
taia_uint.o
 
libunix_a_OBJECTS=alloc.o alloc_re.o buffer.o buffer_0.o buffer_1.o \
buffer_2.o buffer_copy.o buffer_get.o buffer_put.o coe.o env_get.o \
error.o error_str.o gen_alloc_ady.o gen_alloc_end.o gen_alloc_lus.o \
get_cwd.o getln.o iopause.o ndelay_on.o open_excl.o open_read.o \
open_readwri.o open_trunc.o open_write.o pathexec_env.o pathexec_run.o \
sig.o sig_catch.o stralloc_cat.o stralloc_catb.o stralloc_cats.o \
stralloc_copy.o stralloc_eady.o stralloc_free.o stralloc_num.o \
stralloc_opyb.o stralloc_opys.o stralloc_pend.o stralloc_plus.o \
wait_nohang.o wait_pid.o
 
libuomisc_a_OBJECTS=api_dir.o api_dir_sort.o bailout.o bailout_name.o \
mssort.o svscan_conf.o uogetopt.o uolock.o wrap_stat.o
 
libuotime_a_OBJECTS=uo_now.o uo_sec702dt.o
 
PREMAKE_EXTRA_SYSTYPE =  auto-typesize.h auto-iopause.h
 
all:  auto-systype $(TARGETS) $(EXTRA_TARGETS)
 
uscheduled:  uscheduled.o load_jobs.o parse_job.o make_name.o find_next.o change_dir.o parse_sched.o libuomisc.a libunix.a libtai.a libbyte.a auto-link.sh
	$L uscheduled  uscheduled.o load_jobs.o parse_job.o make_name.o find_next.o change_dir.o parse_sched.o libuomisc.a libunix.a libtai.a libbyte.a 
uschedulerm:  uschedulerm.o load_jobs.o parse_job.o notice.o find_next.o print_job.o change_dir.o parse_sched.o fmt_tai.o libuomisc.a libunix.a libtai.a libbyte.a auto-link.sh
	$L uschedulerm  uschedulerm.o load_jobs.o parse_job.o notice.o find_next.o print_job.o change_dir.o parse_sched.o fmt_tai.o libuomisc.a libunix.a libtai.a libbyte.a 
uschedulelist:  uschedulelist.o load_jobs.o parse_job.o find_next.o print_job.o change_dir.o parse_sched.o fmt_tai.o make_name.o libuomisc.a libunix.a libtai.a libbyte.a auto-link.sh
	$L uschedulelist  uschedulelist.o load_jobs.o parse_job.o find_next.o print_job.o change_dir.o parse_sched.o fmt_tai.o make_name.o libuomisc.a libunix.a libtai.a libbyte.a 
uschedulecmd:  uschedulecmd.o change_dir.o make_id.o check_id.o libuomisc.a libunix.a libtai.a libbyte.a auto-link.sh
	$L uschedulecmd  uschedulecmd.o change_dir.o make_id.o check_id.o libuomisc.a libunix.a libtai.a libbyte.a 
uschedulecp:  uschedulecp.o change_dir.o make_id.o check_id.o libuomisc.a libunix.a libtai.a libbyte.a auto-link.sh
	$L uschedulecp  uschedulecp.o change_dir.o make_id.o check_id.o libuomisc.a libunix.a libtai.a libbyte.a 
uscheduleedit:  uscheduleedit.o load_jobs.o parse_job.o make_name.o change_dir.o check_id.o libuomisc.a libunix.a libtai.a libbyte.a auto-link.sh
	$L uscheduleedit  uscheduleedit.o load_jobs.o parse_job.o make_name.o change_dir.o check_id.o libuomisc.a libunix.a libtai.a libbyte.a 
uschedule:  uschedule.o load_jobs.o parse_job.o notice.o make_name.o parse_timespec.o find_next.o change_dir.o parse_sched.o check_id.o libuomisc.a libuotime.a libunix.a libtai.a libbyte.a auto-link.sh
	$L uschedule  uschedule.o load_jobs.o parse_job.o notice.o make_name.o parse_timespec.o find_next.o change_dir.o parse_sched.o check_id.o libuomisc.a libuotime.a libunix.a libtai.a libbyte.a 
uscheduleruntimelimit:  uscheduleruntimelimit.o libunix.a libtai.a libbyte.a auto-link.sh
	$L uscheduleruntimelimit  uscheduleruntimelimit.o libunix.a libtai.a libbyte.a 
uscheduleconf:  uscheduleconf.o  libuomisc.a libunix.a libtai.a libbyte.a auto-link.sh
	$L uscheduleconf  uscheduleconf.o  libuomisc.a libunix.a libtai.a libbyte.a 
auto-typesize.h: auto-typesize.stamp
auto-typesize.stamp: typesize.sh auto-compile.sh auto-link.sh
	sh typesize.sh >typesize.t
	@if cmp typesize.t auto-typesize.h 2>/dev/null >/dev/null ; then : ; else mv typesize.t auto-typesize.h ; fi
	rm -f typesize.t
	touch $@
auto-iopause.h: iopause.sh auto-compile.sh auto-link.sh
	sh iopause.sh >iopause.t && mv iopause.t $@
libbyte.a: $(libbyte_a_OBJECTS) auto-makelib.sh
	./auto-makelib.sh $@ $(libbyte_a_OBJECTS)
libtai.a: $(libtai_a_OBJECTS) auto-makelib.sh
	./auto-makelib.sh $@ $(libtai_a_OBJECTS)
libunix.a: $(libunix_a_OBJECTS) auto-makelib.sh
	./auto-makelib.sh $@ $(libunix_a_OBJECTS)
libuomisc.a: $(libuomisc_a_OBJECTS) auto-makelib.sh
	./auto-makelib.sh $@ $(libuomisc_a_OBJECTS)
libuotime.a: $(libuotime_a_OBJECTS) auto-makelib.sh
	./auto-makelib.sh $@ $(libuotime_a_OBJECTS)
install.html: install.0.in groff.tmac auto-version.tmac
	groff -mwww -Thtml -P-l groff.tmac auto-version.tmac install.0.in \
>install.html.t && mv install.html.t install.html
uschedule.html: uschedule.1.in groff.tmac auto-version.tmac
	groff -mwww -Thtml -P-l groff.tmac auto-version.tmac uschedule.1.in \
>uschedule.html.t && mv uschedule.html.t uschedule.html
intro.html: uschedule_intro.7.in groff.tmac auto-version.tmac
	groff -mwww -Thtml -P-l groff.tmac auto-version.tmac \
uschedule_intro.7.in >intro.html.t && mv intro.html.t \
intro.html
uschedulecmd.html: uschedulecmd.1.in groff.tmac \
auto-version.tmac
	groff -mwww -Thtml -P-l groff.tmac auto-version.tmac uschedulecmd.1.in \
>uschedulecmd.html.t && mv uschedulecmd.html.t \
uschedulecmd.html
uscheduleconf.html: uscheduleconf.1.in groff.tmac \
auto-version.tmac
	groff -mwww -Thtml -P-l groff.tmac auto-version.tmac \
uscheduleconf.1.in >uscheduleconf.html.t && mv uscheduleconf.html.t \
uscheduleconf.html
uschedulecp.html: uschedulecp.1.in groff.tmac auto-version.tmac
	groff -mwww -Thtml -P-l groff.tmac auto-version.tmac uschedulecp.1.in \
>uschedulecp.html.t && mv uschedulecp.html.t \
uschedulecp.html
uscheduled.html: uscheduled.8.in groff.tmac auto-version.tmac
	groff -mwww -Thtml -P-l groff.tmac auto-version.tmac uscheduled.8.in \
>uscheduled.html.t && mv uscheduled.html.t \
uscheduled.html
uscheduleedit.html: uscheduleedit.1.in groff.tmac \
auto-version.tmac
	groff -mwww -Thtml -P-l groff.tmac auto-version.tmac \
uscheduleedit.1.in >uscheduleedit.html.t && mv uscheduleedit.html.t \
uscheduleedit.html
uschedulelist.html: uschedulelist.1.in groff.tmac \
auto-version.tmac
	groff -mwww -Thtml -P-l groff.tmac auto-version.tmac \
uschedulelist.1.in >uschedulelist.html.t && mv uschedulelist.html.t \
uschedulelist.html
uschedulerm.html: uschedulerm.1.in groff.tmac auto-version.tmac
	groff -mwww -Thtml -P-l groff.tmac auto-version.tmac uschedulerm.1.in \
>uschedulerm.html.t && mv uschedulerm.html.t \
uschedulerm.html
uscheduleruntimelimit.html: uscheduleruntimelimit.1.in groff.tmac \
auto-version.tmac
	groff -mwww -Thtml -P-l groff.tmac auto-version.tmac \
uscheduleruntimelimit.1.in >uscheduleruntimelimit.html.t && mv \
uscheduleruntimelimit.html.t uscheduleruntimelimit.html
INSTALL: install.0.in groff.tmac auto-version.tmac
	groff -man -Tascii groff.tmac auto-version.tmac install.0.in |sed \
's/\o33\[[0-9]*m//g ' |col -b >INSTALL.t && mv INSTALL.t \
INSTALL
uschedule.1: uschedule.1.in groff.tmac auto-version.tmac
	cat groff.tmac auto-version.tmac uschedule.1.in \
 | sed 's/^.section/.SH/' >uschedule.1.t \
 && mv uschedule.1.t uschedule.1
uschedule_intro.7: uschedule_intro.7.in groff.tmac \
auto-version.tmac
	cat groff.tmac auto-version.tmac uschedule_intro.7.in \
 | sed 's/^.section/.SH/' >uschedule_intro.7.t \
 && mv uschedule_intro.7.t uschedule_intro.7
uschedulecmd.1: uschedulecmd.1.in groff.tmac auto-version.tmac
	cat groff.tmac auto-version.tmac uschedulecmd.1.in \
 | sed 's/^.section/.SH/' >uschedulecmd.1.t \
 && mv uschedulecmd.1.t uschedulecmd.1
uscheduleconf.1: uscheduleconf.1.in groff.tmac auto-version.tmac
	cat groff.tmac auto-version.tmac uscheduleconf.1.in \
 | sed 's/^.section/.SH/' >uscheduleconf.1.t \
 && mv uscheduleconf.1.t uscheduleconf.1
uschedulecp.1: uschedulecp.1.in groff.tmac auto-version.tmac
	cat groff.tmac auto-version.tmac uschedulecp.1.in \
 | sed 's/^.section/.SH/' >uschedulecp.1.t \
 && mv uschedulecp.1.t uschedulecp.1
uscheduled.8: uscheduled.8.in groff.tmac auto-version.tmac
	cat groff.tmac auto-version.tmac uscheduled.8.in \
 | sed 's/^.section/.SH/' >uscheduled.8.t \
 && mv uscheduled.8.t uscheduled.8
uscheduleedit.1: uscheduleedit.1.in groff.tmac auto-version.tmac
	cat groff.tmac auto-version.tmac uscheduleedit.1.in \
 | sed 's/^.section/.SH/' >uscheduleedit.1.t \
 && mv uscheduleedit.1.t uscheduleedit.1
uschedulelist.1: uschedulelist.1.in groff.tmac auto-version.tmac
	cat groff.tmac auto-version.tmac uschedulelist.1.in \
 | sed 's/^.section/.SH/' >uschedulelist.1.t \
 && mv uschedulelist.1.t uschedulelist.1
uschedulerm.1: uschedulerm.1.in groff.tmac auto-version.tmac
	cat groff.tmac auto-version.tmac uschedulerm.1.in \
 | sed 's/^.section/.SH/' >uschedulerm.1.t \
 && mv uschedulerm.1.t uschedulerm.1
uscheduleruntimelimit.1: uscheduleruntimelimit.1.in groff.tmac \
auto-version.tmac
	cat groff.tmac auto-version.tmac uscheduleruntimelimit.1.in \
 | sed 's/^.section/.SH/' >uscheduleruntimelimit.1.t \
 && mv uscheduleruntimelimit.1.t \
uscheduleruntimelimit.1
auto-version.tmac: version.tmac.stamp
version.tmac.stamp: Makefile
	(echo .ds PA $(PACKAGE) ;\
	 echo .ds VE $(VERSION) ;\
	 echo .ds CL $(CLASS) ;\
	) >auto-version.tmac.t
	cmp auto-version.tmac auto-version.tmac.t \
	 || cp auto-version.tmac.t auto-version.tmac
	touch $@ && rm auto-version.tmac.t
html: install.html uschedule.html intro.html uschedulecmd.html \
uscheduleconf.html uschedulecp.html uscheduled.html uscheduleedit.html \
uschedulelist.html uschedulerm.html uscheduleruntimelimit.html \
INSTALL

 
alloc.o: alloc.c alloc.h error.h auto-compilew.sh
	$W  -c alloc.c
alloc_re.o: alloc_re.c alloc.h byte.h auto-compilew.sh
	$W  -c alloc_re.c
api_dir.o: api_dir.c stralloc.h gen_alloc.h str.h api_dir.h auto-compilew.sh
	$W  -c api_dir.c
api_dir_sort.o: api_dir_sort.c stralloc.h gen_alloc.h alloc.h str.h \
 api_dir.h mssort.h uint32.h typesize.h auto-typesize.h auto-compilew.sh
	$W  -c api_dir_sort.c
bailout.o: bailout.c error.h buffer.h bailout.h fmt.h auto-compilew.sh
	$W  -c bailout.c
bailout_name.o: bailout_name.c bailout.h buffer.h auto-compilew.sh
	$W  -c bailout_name.c
buffer.o: buffer.c buffer.h auto-compile.sh
	$C  -c buffer.c
buffer_0.o: buffer_0.c readwrite.h buffer.h auto-compile.sh
	$C  -c buffer_0.c
buffer_1.o: buffer_1.c readwrite.h buffer.h auto-compile.sh
	$C  -c buffer_1.c
buffer_2.o: buffer_2.c readwrite.h buffer.h auto-compile.sh
	$C  -c buffer_2.c
buffer_copy.o: buffer_copy.c buffer.h auto-compile.sh
	$C  -c buffer_copy.c
buffer_get.o: buffer_get.c buffer.h byte.h error.h auto-compile.sh
	$C  -c buffer_get.c
buffer_put.o: buffer_put.c buffer.h str.h byte.h error.h auto-compile.sh
	$C  -c buffer_put.c
byte_chr.o: byte_chr.c byte.h auto-compilew.sh
	$W  -c byte_chr.c
byte_copy.o: byte_copy.c byte.h auto-compilew.sh
	$W  -c byte_copy.c
byte_cr.o: byte_cr.c byte.h auto-compilew.sh
	$W  -c byte_cr.c
byte_diff.o: byte_diff.c byte.h auto-compilew.sh
	$W  -c byte_diff.c
case_lwrtab.o: case_lwrtab.c case.h auto-compilew.sh
	$W  -c case_lwrtab.c
case_starts.o: case_starts.c case.h auto-compilew.sh
	$W  -c case_starts.c
change_dir.o: change_dir.c scheduled.h typesize.h auto-typesize.h \
 taia.h tai.h uint64.h stralloc.h gen_alloc.h attributes.h bailout.h \
 buffer.h error.h env.h get_cwd.h auto-compilew.sh
	$W  -c change_dir.c
check_id.o: check_id.c scheduled.h typesize.h auto-typesize.h taia.h \
 tai.h uint64.h stralloc.h gen_alloc.h attributes.h bailout.h buffer.h \
 str.h auto-compilew.sh
	$W  -c check_id.c
coe.o: coe.c coe.h auto-compilew.sh
	$W  -c coe.c
env_get.o: env_get.c env.h auto-compilew.sh
	$W  -c env_get.c
error.o: error.c error.h auto-compilew.sh
	$W  -c error.c
error_str.o: error_str.c error.h auto-compilew.sh
	$W  -c error_str.c
find_next.o: find_next.c scheduled.h typesize.h auto-typesize.h taia.h \
 tai.h uint64.h stralloc.h gen_alloc.h attributes.h bailout.h buffer.h \
 scan.h fmt.h auto-compilew.sh
	$W  -c find_next.c
fmt_tai.o: fmt_tai.c fmt_tai.h tai.h uint64.h typesize.h \
 auto-typesize.h scheduled.h taia.h stralloc.h gen_alloc.h \
 attributes.h auto-compilew.sh
	$W  -c fmt_tai.c
fmt_ulong.o: fmt_ulong.c gen_fmt.h fmt.h auto-compilew.sh
	$W  -c fmt_ulong.c
fmt_xlong.o: fmt_xlong.c gen_fmt.h fmt.h auto-compilew.sh
	$W  -c fmt_xlong.c
gen_alloc_ady.o: gen_alloc_ady.c alloc.h gen_alloci.h auto-compilew.sh
	$W  -c gen_alloc_ady.c
gen_alloc_end.o: gen_alloc_end.c gen_alloci.h byte.h auto-compilew.sh
	$W  -c gen_alloc_end.c
gen_alloc_lus.o: gen_alloc_lus.c gen_alloci.h auto-compilew.sh
	$W  -c gen_alloc_lus.c
get_cwd.o: get_cwd.c alloc.h error.h stralloc.h gen_alloc.h byte.h \
 str.h get_cwd.h auto-compilew.sh
	$W  -c get_cwd.c
getln.o: getln.c byte.h getln.h buffer.h stralloc.h gen_alloc.h auto-compilew.sh
	$W  -c getln.c
iopause.o: iopause.c taia.h tai.h uint64.h typesize.h auto-typesize.h \
 iopause.h auto-iopause.h auto-compilew.sh
	$W  -c iopause.c
load_jobs.o: load_jobs.c scheduled.h typesize.h auto-typesize.h taia.h \
 tai.h uint64.h stralloc.h gen_alloc.h attributes.h bailout.h buffer.h \
 error.h str.h byte.h api_dir.h auto-compilew.sh
	$W  -c load_jobs.c
make_id.o: make_id.c scheduled.h typesize.h auto-typesize.h taia.h \
 tai.h uint64.h stralloc.h gen_alloc.h attributes.h open.h bailout.h \
 buffer.h scan.h error.h api_dir.h auto-compilew.sh
	$W  -c make_id.c
make_name.o: make_name.c stralloc.h gen_alloc.h scheduled.h typesize.h \
 auto-typesize.h taia.h tai.h uint64.h attributes.h bailout.h buffer.h \
 fmt.h auto-compilew.sh
	$W  -c make_name.c
mssort.o: mssort.c mssort.h uint32.h typesize.h auto-typesize.h \
 alloc.h byte.h auto-compilew.sh
	$W  -c mssort.c
ndelay_on.o: ndelay_on.c ndelay.h auto-compilew.sh
	$W  -c ndelay_on.c
notice.o: notice.c scheduled.h typesize.h auto-typesize.h taia.h tai.h \
 uint64.h stralloc.h gen_alloc.h attributes.h bailout.h buffer.h \
 error.h open.h ndelay.h auto-compilew.sh
	$W  -c notice.c
open_excl.o: open_excl.c open.h auto-compilew.sh
	$W  -c open_excl.c
open_read.o: open_read.c open.h auto-compilew.sh
	$W  -c open_read.c
open_readwri.o: open_readwri.c open.h auto-compilew.sh
	$W  -c open_readwri.c
open_trunc.o: open_trunc.c open.h auto-compilew.sh
	$W  -c open_trunc.c
open_write.o: open_write.c open.h auto-compilew.sh
	$W  -c open_write.c
parse_job.o: parse_job.c scheduled.h typesize.h auto-typesize.h taia.h \
 tai.h uint64.h stralloc.h gen_alloc.h attributes.h bailout.h buffer.h \
 scan.h error.h str.h byte.h auto-compilew.sh
	$W  -c parse_job.c
parse_sched.o: parse_sched.c scheduled.h typesize.h auto-typesize.h \
 taia.h tai.h uint64.h stralloc.h gen_alloc.h attributes.h byte.h \
 scan.h auto-compilew.sh
	$W  -c parse_sched.c
parse_timespec.o: parse_timespec.c scheduled.h typesize.h \
 auto-typesize.h taia.h tai.h uint64.h stralloc.h gen_alloc.h \
 attributes.h uogetopt.h bailout.h buffer.h str.h scan.h case.h \
 uotime.h auto-compilew.sh
	$W  -c parse_timespec.c
pathexec_env.o: pathexec_env.c stralloc.h gen_alloc.h alloc.h str.h \
 byte.h env.h pathexec.h auto-compilew.sh
	$W  -c pathexec_env.c
pathexec_run.o: pathexec_run.c error.h stralloc.h gen_alloc.h str.h \
 env.h pathexec.h auto-compilew.sh
	$W  -c pathexec_run.c
print_job.o: print_job.c scheduled.h typesize.h auto-typesize.h taia.h \
 tai.h uint64.h stralloc.h gen_alloc.h attributes.h bailout.h buffer.h \
 fmt.h str.h error.h fmt_tai.h auto-compilew.sh
	$W  -c print_job.c
scan_long.o: scan_long.c gen_scan.h scan.h case.h auto-compilew.sh
	$W  -c scan_long.c
scan_sign.o: scan_sign.c scan.h auto-compilew.sh
	$W  -c scan_sign.c
scan_ulong.o: scan_ulong.c gen_scan.h scan.h case.h auto-compilew.sh
	$W  -c scan_ulong.c
scan_xint64.o: scan_xint64.c gen_scan.h scan.h case.h uint64.h \
 typesize.h auto-typesize.h auto-compilew.sh
	$W  -c scan_xint64.c
sig.o: sig.c sig.h auto-compile.sh
	$C  -c sig.c
sig_catch.o: sig_catch.c sig.h auto-compile.sh
	$C  -c sig_catch.c
str_chr.o: str_chr.c str.h auto-compilew.sh
	$W  -c str_chr.c
str_diff.o: str_diff.c str.h auto-compilew.sh
	$W  -c str_diff.c
str_len.o: str_len.c str.h auto-compilew.sh
	$W  -c str_len.c
str_rchr.o: str_rchr.c str.h auto-compilew.sh
	$W  -c str_rchr.c
str_start.o: str_start.c str.h auto-compilew.sh
	$W  -c str_start.c
stralloc_cat.o: stralloc_cat.c stralloc.h gen_alloc.h auto-compilew.sh
	$W  -c stralloc_cat.c
stralloc_catb.o: stralloc_catb.c stralloc.h gen_alloc.h byte.h auto-compilew.sh
	$W  -c stralloc_catb.c
stralloc_cats.o: stralloc_cats.c str.h stralloc.h gen_alloc.h auto-compilew.sh
	$W  -c stralloc_cats.c
stralloc_copy.o: stralloc_copy.c stralloc.h gen_alloc.h auto-compilew.sh
	$W  -c stralloc_copy.c
stralloc_eady.o: stralloc_eady.c stralloc.h gen_alloc.h gen_alloci.h auto-compilew.sh
	$W  -c stralloc_eady.c
stralloc_free.o: stralloc_free.c stralloc.h gen_alloc.h alloc.h auto-compilew.sh
	$W  -c stralloc_free.c
stralloc_num.o: stralloc_num.c stralloc.h gen_alloc.h fmt.h auto-compilew.sh
	$W  -c stralloc_num.c
stralloc_opyb.o: stralloc_opyb.c stralloc.h gen_alloc.h byte.h auto-compilew.sh
	$W  -c stralloc_opyb.c
stralloc_opys.o: stralloc_opys.c str.h stralloc.h gen_alloc.h auto-compilew.sh
	$W  -c stralloc_opys.c
stralloc_pend.o: stralloc_pend.c stralloc.h gen_alloc.h gen_alloci.h auto-compilew.sh
	$W  -c stralloc_pend.c
stralloc_plus.o: stralloc_plus.c stralloc.h gen_alloc.h gen_alloci.h auto-compilew.sh
	$W  -c stralloc_plus.c
svscan_conf.o: svscan_conf.c buffer.h svscan_conf.h bailout.h str.h \
 open.h fifo.h error.h attributes.h auto-compilew.sh
	$W  -c svscan_conf.c
tai_add.o: tai_add.c tai.h uint64.h typesize.h auto-typesize.h auto-compilew.sh
	$W  -c tai_add.c
tai_now.o: tai_now.c tai.h uint64.h typesize.h auto-typesize.h auto-compile.sh
	$C  -c tai_now.c
tai_sub.o: tai_sub.c tai.h uint64.h typesize.h auto-typesize.h auto-compilew.sh
	$W  -c tai_sub.c
tai_uint.o: tai_uint.c tai.h uint64.h typesize.h auto-typesize.h auto-compilew.sh
	$W  -c tai_uint.c
taia_add.o: taia_add.c taia.h tai.h uint64.h typesize.h \
 auto-typesize.h auto-compilew.sh
	$W  -c taia_add.c
taia_approx.o: taia_approx.c taia.h tai.h uint64.h typesize.h \
 auto-typesize.h auto-compilew.sh
	$W  -c taia_approx.c
taia_frac.o: taia_frac.c taia.h tai.h uint64.h typesize.h \
 auto-typesize.h auto-compilew.sh
	$W  -c taia_frac.c
taia_less.o: taia_less.c taia.h tai.h uint64.h typesize.h \
 auto-typesize.h auto-compilew.sh
	$W  -c taia_less.c
taia_now.o: taia_now.c taia.h tai.h uint64.h typesize.h \
 auto-typesize.h auto-compilew.sh
	$W  -c taia_now.c
taia_sub.o: taia_sub.c taia.h tai.h uint64.h typesize.h \
 auto-typesize.h auto-compilew.sh
	$W  -c taia_sub.c
taia_tai.o: taia_tai.c taia.h tai.h uint64.h typesize.h \
 auto-typesize.h auto-compilew.sh
	$W  -c taia_tai.c
taia_uint.o: taia_uint.c taia.h tai.h uint64.h typesize.h \
 auto-typesize.h auto-compilew.sh
	$W  -c taia_uint.c
uo_now.o: uo_now.c uotime.h auto-compilew.sh
	$W  -c uo_now.c
uo_sec702dt.o: uo_sec702dt.c uotime.h auto-compilew.sh
	$W  -c uo_sec702dt.c
uogetopt.o: uogetopt.c readwrite.h str.h env.h scan.h uogetopt.h \
 attributes.h auto-compilew.sh
	$W  -c uogetopt.c
uolock.o: uolock.c uolock.h auto-compilew.sh
	$W  -c uolock.c
uschedule.o: uschedule.c uogetopt.h scheduled.h typesize.h \
 auto-typesize.h taia.h tai.h uint64.h stralloc.h gen_alloc.h \
 attributes.h bailout.h buffer.h str.h error.h scan.h fmt.h auto-compilew.sh
	$W  -c uschedule.c
uschedulecmd.o: uschedulecmd.c uogetopt.h scheduled.h typesize.h \
 auto-typesize.h taia.h tai.h uint64.h stralloc.h gen_alloc.h \
 attributes.h open.h bailout.h buffer.h env.h str.h scan.h error.h auto-compilew.sh
	$W  -c uschedulecmd.c
uscheduleconf.o: uscheduleconf.c svscan_conf.h buffer.h bailout.h \
 str.h env.h scheduled.h typesize.h auto-typesize.h taia.h tai.h \
 uint64.h stralloc.h gen_alloc.h attributes.h uogetopt.h auto-compilew.sh
	$W  -c uscheduleconf.c
uschedulecp.o: uschedulecp.c uogetopt.h scheduled.h typesize.h \
 auto-typesize.h taia.h tai.h uint64.h stralloc.h gen_alloc.h \
 attributes.h open.h bailout.h buffer.h error.h auto-compilew.sh
	$W  -c uschedulecp.c
uscheduled.o: uscheduled.c scheduled.h typesize.h auto-typesize.h \
 taia.h tai.h uint64.h stralloc.h gen_alloc.h attributes.h sig.h \
 iopause.h auto-iopause.h byte.h str.h uogetopt.h open.h pathexec.h \
 bailout.h buffer.h error.h wait.h fmt.h coe.h env.h uolock.h ndelay.h \
 wrap_stat.h auto-compilew.sh
	$W  -c uscheduled.c
uscheduleedit.o: uscheduleedit.c uogetopt.h scheduled.h typesize.h \
 auto-typesize.h taia.h tai.h uint64.h stralloc.h gen_alloc.h \
 attributes.h open.h wait.h bailout.h buffer.h pathexec.h env.h \
 error.h uolock.h fmt.h wrap_stat.h auto-compilew.sh
	$W  -c uscheduleedit.c
uschedulelist.o: uschedulelist.c scheduled.h typesize.h \
 auto-typesize.h taia.h tai.h uint64.h stralloc.h gen_alloc.h \
 attributes.h uogetopt.h byte.h bailout.h buffer.h env.h str.h alloc.h \
 error.h api_dir.h mssort.h uint32.h wrap_stat.h fmt_tai.h scan.h \
 open.h getln.h auto-compilew.sh
	$W  -c uschedulelist.c
uschedulerm.o: uschedulerm.c attributes.h scheduled.h typesize.h \
 auto-typesize.h taia.h tai.h uint64.h stralloc.h gen_alloc.h \
 uogetopt.h byte.h bailout.h buffer.h iopause.h auto-iopause.h str.h \
 fmt.h error.h open.h wrap_stat.h fmt_tai.h auto-compilew.sh
	$W  -c uschedulerm.c
uscheduleruntimelimit.o: uscheduleruntimelimit.c scheduled.h \
 typesize.h auto-typesize.h taia.h tai.h uint64.h stralloc.h \
 gen_alloc.h attributes.h scan.h fmt.h sig.h auto-compilew.sh
	$W  -c uscheduleruntimelimit.c
wait_nohang.o: wait_nohang.c wait.h auto-compilew.sh
	$W  -c wait_nohang.c
wait_pid.o: wait_pid.c error.h wait.h auto-compilew.sh
	$W  -c wait_pid.c
wrap_stat.o: wrap_stat.c wrap_stat.h typesize.h auto-typesize.h taia.h \
 tai.h uint64.h auto-compilew.sh
	$W  -c wrap_stat.c
rts check: rts-clean  rts-resolve 
rts-clean:
	rm -f rts-failures 
rts-resolve: 
	@if test -f rts-failures ; then echo The following tests failed: ; \
cat rts-failures ; exit 1; fi 
.PRECIOUS: Makefile
remake: clean
	rm -f auto-compile.sh auto-link.sh auto-compilew.sh auto-makelib.sh
packageinfo: 
	@echo version="$(VERSION)"
	@echo package="$(PACKAGE)"
	@echo class="$(CLASS)"

auto-compile.sh auto-compilew.sh auto-makelib.sh auto-link.sh \
auto-systype.lfs: guess-compiler.sh auto-compiler-config
auto-compiler-config: auto-compiler-config.stamp
auto-config: force
	@( for i in conf-cc conf-ld conf-ldflags conf-cflags conf-cflagsw \
	  conf-ar conf-ranlib conf-libs auto-compile.sh auto-compilew.sh \
	  auto-link.sh auto-makelib.sh ; do \
	  echo $$i ; if test -f $$i ; then cat $$i ; fi ;  \
	done ) >$@.t 
	@mv $@.t $@
auto-compiler-config.stamp: auto-config
	@if cmp auto-config $@ ; then : ; else \
	  echo configuration has changed ; \
	  DEFS='$(DEFS)' INCLUDES='$(INCLUDES)' \
	  PREMAKE_DEFS='$(PREMAKE_DEFS)' \
	  sh guess-compiler.sh ; \
	  rm auto-config ; \
	  $(MYMAKE) auto-config && mv auto-config $@ ; \
	fi 

maintainer-clean: distclean
	rm -f Makefile $(MAINTAINER_CLEANFILES)
distclean: clean
	rm -f $(DISTCLEANFILES) auto-dummy # to avoid "no arguments" error
clean spotless: mostlyclean
	rm -f $(TARGETS) auto-*
	rm -rf deps
mostlyclean:
	rm -f *.o *.a $(CLEANFILES)

NEWS-check:
	@if sed 1q NEWS | grep -e "$(VERSION)" > /dev/null; then :; else \
		echo "NEWS not updated; not releasing" 1>&2; \
		exit 1;                                   \
	fi
	@if grep -e "$(VERSION)" ChangeLog > /dev/null; then :; else \
		echo "ChangeLog not updated; not releasing" 1>&2; \
		exit 1;                                   \
	fi
dist:
	rm -f $(PREMAKE_DIST)
	package
bk-dist: NEWS-check
	bk tag `echo "Release-$(VERSION)" | sed 's/\./-/g'`
	$(MYMAKE) dist
gpg-dist: dist
	gpg -b --armor $(PREMAKE_DIST)
force:	

auto-systype: auto-typesize.h $(EXTRA_SYSTYPE) $(PREMAKE_EXTRA_SYSTYPE)
	rm -f $@.t
	@echo  >>$@.t
	@echo "PACKAGE: $(PACKAGE) $(VERSION)" >>$@.t
	@echo "REALNAME: $(REALNAME)" >>$@.t
	@echo "uname-a:" `uname -a` >>$@.t
	@echo "uname-s:" `uname -s` >>$@.t
	@echo "uname-r:" `uname -r` >>$@.t
	@echo "uname-v:" `uname -v` >>$@.t
	@echo "uname-m:" `uname -m` >>$@.t
	@echo "linux-ld.so:" `ls /lib/ld-*.so` >>$@.t
	@echo "linux-libc.so:" `ls /lib/libc-*.so` >>$@.t
	cat auto-typesize.h >>$@.t
	if test "x$(PREMAKE_EXTRA_SYSTYPE)$(EXTRA_SYSTYPE)" = x ; then true ; else \
	  grep -h systype-info $(PREMAKE_EXTRA_SYSTYPE) $(EXTRA_SYSTYPE) >>$@.t \
	      || exit 1 ; \
	fi
	mv $@.t $@

check: all docheck check.exp
	sh ./docheck 2>&1 | cat -v | tee check.out | diff -u check.exp -
