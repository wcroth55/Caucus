.SUFFIXES:     .c .o

#: CR 10/31/05 Caucus 5 release.

OBJS = \
ambiguou.o  prompter.o   \
echoinpu.o  showlice.o  readprom.o   \
inputcon.o  runpromp.o

inout.a: $(OBJS)
	rm -f  inout.a
	ar cq  inout.a  $(OBJS)
	ranlib inout.a

clean:
	rm -f *.o

clobber:
	rm -f *.o inout.a

.c.o:
	cc -O3 -I../include -c $*.c
