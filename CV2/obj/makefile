.SUFFIXES:     .c .o

#: CR 10/31/05 Caucus 5 release.

OBJS = \
makeobj.o   freeobj.o  makeobtab.o  freeobtab.o  obtabexp.o \
obtabis.o   loadobj.o  storeobj.o   killobj.o    obtabpr.o

obj.a: $(OBJS)
	rm -f  obj.a
	ar cq  obj.a $(OBJS)
	ranlib obj.a

clean:
	rm -f *.o

clobber:
	rm -f *.o obj.a

.c.o:
	cc -O3 -I../include -c $*.c
