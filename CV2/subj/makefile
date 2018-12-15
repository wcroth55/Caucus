.SUFFIXES:     .c .o

#: CR 10/31/05 Caucus 5 release.

OBJS = \
subcreat.o  subparse.o   submod.o   submatch.o   subdel.o  \
subuncat.o  subchang.o

subj.a: $(OBJS)
	rm -f  subj.a
	ar cq  subj.a $(OBJS)
	ranlib subj.a

clean:
	rm -f *.o

clobber:
	rm -f *.o subj.a

.c.o:
	cc -O3 -I../include -c $*.c
