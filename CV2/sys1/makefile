.SUFFIXES:     .c .o

#: CR 10/31/05 Caucus 5 release.

OBJS = \
sysacces.o sysalarm.o sysavail.o  sysbacku.o sysbrand.o sysisdir.o \
sysbreak.o syscance.o syscheck.o  sysclean.o sysclkch.o sysclrsc.o \
syscmd.o   sysconso.o syscopy.o   sysdayti.o sysdfnam.o \
sysdirch.o sysedito.o sysexit.o   sysflush.o sysfree.o  sysfsize.o \
sysget1k.o sysgetno.o sysgfdir.o  sysgloba.o syshng.o   \
sysmem.o   sysmkcon.o sysmkdir.o  sysmkhos.o syskill.o  syslocker.o \
syshomed.o srvldmas.o syschkpr.o  sysescqu.o sysbpath.o runaway.o \
ntfilena.o ntmkfile.o ntchkfile.o sysoblock.o sysmd5.o  md5.o

sys1.a: $(OBJS)
	rm -f  sys1.a
	ar cq  sys1.a $(OBJS)
	ranlib sys1.a

clean:
	rm -f *.o

clobber:
	rm -f *.o sys1.a

.c.o:
	cc -O3 -I../include -c $*.c
