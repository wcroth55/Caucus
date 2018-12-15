.SUFFIXES:     .c .o

#: CR 10/31/05 Caucus 5 release.

OBJS = \
cfdigit.o   cfupper.o   cfvalue.o   cflower.o   \
stralter.o  strsub.o    streq.o     strtoken.o  strlower.o  \
strconc.o   strindex.o  strnum.o    strscan.o   strshort.o  \
max.o       min.o       str36toi.o  strito36.o  strplain.o  \
strtable.o  strcpytil.o strunescape.o

str.a: $(OBJS)
	rm -f  str.a
	ar cq  str.a $(OBJS)
	ranlib str.a

clean:
	rm -f *.o

clobber:
	rm -f *.o str.a

.c.o:
	cc -O3 -I../include -c $*.c
