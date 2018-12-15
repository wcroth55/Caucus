.SUFFIXES:     .c .o

#: CR 10/31/05 Caucus 5 release.

OBJS = \
managert.o  parseinp.o  printit.o   expwarn.o   \
limituse.o  utilglob.o  mainglob.o  trimfile.o  \
parsenum.o  copyfile.o  argchk.o    optchk.o    \
loadprin.o  pluckper.o  reportpa.o  webpw.o     getent.o    \
buglist.o   bugtell.o   parsewor.o  pwfind.o    \
copyprot.o  macsubs.o   plucknam.o  pdirprin.o  \
optset.o    varsubs.o   appendfi.o  parsefie.o  newparse.o  parseora.o \
parse2to.o  collapse.o  mergetok.o  deleteto.o  numtorl.o   \
parserou.o  parserin.o  applyred.o  diskfail.o  \
nrtorlis.o  andisinc.o  andrsinc.o  logger.o    \
mytime.o    socketpath.o bigtime.o

gen.a: $(OBJS)
	rm -f  gen.a
	ar cq  gen.a $(OBJS)
	ranlib gen.a

clean:
	rm -f *.o

clobber:
	rm -f *.o gen.a

.c.o:
	cc -O3 -I../include -c $*.c
