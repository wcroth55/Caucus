.SUFFIXES:     .c .o

#: CR 10/31/05 Caucus 5 release.

OBJS = \
copyatta.o  makeatta.o  freeatta.o  clearatt.o  oktodisp.o  analyzef.o \
parseatt.o  getattad.o  paratthe.o  printade.o  printatt.o  hookatta.o

attach.a: $(OBJS)
	rm -f  attach.a
	ar cq  attach.a $(OBJS)
	ranlib attach.a

clean:
	rm -f *.o

clobber:
	rm -f *.o attach.a

.c.o:
	cc -O3 -I../include -c $*.c
