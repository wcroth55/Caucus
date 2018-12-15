.SUFFIXES:     .c .o

#: CR 10/31/05 Caucus 5 release.

OBJS = \
chxinsid.o chxpunct.o chxquote.o chxstrip.o chxupper.o  tablepart.o \
mycrypt.o  tablefin.o tablemat.o chxwrap.o  chxinser.o  limit10k.o  \
chxtoowi.o colwrap.o

glyph.a: $(OBJS)
	rm -f  glyph.a
	ar cq  glyph.a $(OBJS)
	ranlib glyph.a

clean:
	rm -f *.o

clobber:
	rm -f *.o glyph.a

.c.o:
	cc -O3 -I../include -c $*.c
