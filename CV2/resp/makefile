.SUFFIXES:     .c .o

#: CR 10/31/05 Caucus 5 release.

OBJS = \
enterres.o  getrespt.o  getline.o   partnum.o   \
entertit.o  loadhead.o  resploop.o  showresp.o  touchmas.o \
itemfree.o  loadtext.o  storemas.o  itemdel.o  \
frozen.o    showhead.o  storetex.o  showform.o  buildlin.o \
clearmas.o  getidhos.o  maketext.o  freetext.o  makemast.o freemast.o

resp.a: $(OBJS)
	rm -f  resp.a
	ar cq  resp.a $(OBJS)
	ranlib resp.a

clean:
	rm -f *.o

clobber:
	rm -f *.o resp.a

.c.o:
	cc -O3 -I../include -c $*.c
