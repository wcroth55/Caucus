.SUFFIXES:     .c .o

#: CR 10/31/05 Caucus 5 release.

OBJS = \
createit.o  chgditem.o  updatepa.o  chixfile.o  iorigtol.o \
chgdresp.o  rorigtol.o  getoresp.o  getoitem.o  getmyhos.o \
chgdtitl.o  adddmess.o  chgdpers.o  scansubj.o  chgdsubj.o \
cusdattr.o  cusdtext.o  isdistri.o  cusdvisi.o  buildpre.o \
cusdporg.o  frzditem.o  cusduser.o  senditem.o  sendsubj.o \
sendconf.o  oktosend.o  andfroze.o  isempty.o

db.a: $(OBJS)
	rm -f  db.a
	ar cq  db.a $(OBJS)
	ranlib db.a

clean:
	rm -f *.o

clobber:
	rm -f *.o db.a

.c.o:
	cc -O3 -I../include -c $*.c
