.SUFFIXES:     .c .o

#: CR 10/31/05 Caucus 5 release.

OBJS = \
brandnew.o  cnumber.o   listconf.o  printran.o  welcome.o  \
calcstat.o  shownew.o   allowedo.o  rmconf.o    groupmat.o  makeconf.o \
oknewcon.o  allconfs.o  newprint.o  confsize.o  mvconf.o   \
okconf.o    copriv.o    usersbygroup.o

conf.a: $(OBJS)
	rm -f  conf.a
	ar cq  conf.a $(OBJS)
	ranlib conf.a

clean:
	rm -f *.o

clobber:
	rm -f *.o conf.a

.c.o:
	cc -O3 -I../include -c $*.c
