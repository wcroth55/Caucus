.SUFFIXES:     .c .o

#: CR 10/31/05 Caucus 5 release.

OBJS = \
entrytes.o  unitappe.o  unitchec.o  unitclos.o  unitcont.o \
unitglob.o  unitrewi.o  unitkill.o  unitlkfx.o  unitlock.o \
unitmake.o  unitmkse.o  unitmove.o  unitstack.o \
unitname.o  unitshor.o  unitunlk.o  unitview.o  unitwipe.o \
unitwrit.o  unitnewl.o  unitput.o   unitterm.o  unitread.o \
unitbvie.o  unitbmak.o  unitbrea.o  unitbwri.o  unitbcop.o

unit.a: $(OBJS)
	rm -f  unit.a
	ar cq  unit.a $(OBJS)
	ranlib unit.a

clean:
	rm -f *.o

clobber:
	rm -f *.o unit.a

.c.o:
	cc -O3 -I../include -c $*.c
