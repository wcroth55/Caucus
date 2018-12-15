.SUFFIXES:     .c .o

#: CR 10/31/05 Caucus 5 release.

OBJS = \
sysoneus.o sysparma.o syspass.o  syspcini.o syspdir.o  systimez.o \
syspluck.o sysprv.o   sysputnl.o sysputst.o sysrdfil.o sysrdlin.o \
sysrenam.o sysrmcon.o sysrmdir.o sysscrip.o syssecur.o sysset.o \
syssleep.o sysstat.o  systemid.o systime.o  systurn.o  systype.o \
sysudnum.o sysunlin.o sysuseri.o sysvalid.o sysversi.o sysvmedi.o \
sysymd.o   systmpfi.o systmpdi.o systuser.o sysnt.o    sysopsys.o \
sysmsec.o  systimems.o

sys2.a: $(OBJS)
	rm -f  sys2.a
	ar cq  sys2.a $(OBJS)
	ranlib sys2.a

clean:
	rm -f *.o

clobber:
	rm -f *.o sys2.a

.c.o:
	cc -O3 -I../include -c $*.c
