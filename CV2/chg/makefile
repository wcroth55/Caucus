.SUFFIXES:     .c .o

#: CR 10/31/05 Caucus 5 release.

OBJS = \
change.o    chgname.o   chgresp.o   cuschang.o  editresp.o  set.o \
chgintro.o  chgpass.o   chgrloop.o  cusenter.o  neworgan.o  decodese.o \
chgitem.o   chgphone.o  chgtitle.o  customiz.o  orgconfi.o  chgfull.o \
chgsubj.o   cusvisib.o  valueset.o  cususer.o   cussubj.o   \
freeulis.o  loadulis.o  makeulis.o  storeuli.o

chg.a: $(OBJS)
	rm -f  chg.a
	ar cq  chg.a $(OBJS)
	ranlib chg.a

clean:
	rm -f *.o

clobber:
	rm -f *.o  chg.a

.c.o:
	cc -O3 -I../include -c $*.c
