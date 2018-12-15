.SUFFIXES:     .c .o

#: CR 10/31/05 Caucus 5 release.

OBJS = \
initnmdi.o  matchnam.o  nlcat.o     nlnode.o    storereg.o \
isregist.o  modnames.o  nlclear.o   nlprint.o   stornmdi.o \
loadnmdi.o  nameinse.o  nlerr.o     nlrdup.o    stornpar.o \
loadnpar.o  nlfree.o    nlsize.o    toomany.o   fixname.o  \
loadreg.o   nladd.o     nlnames.o   personis.o  whatnm.o   \
tomember.o  lastname.o  nluserid.o  nlunique.o  nlcopy.o   \
matchnl.o   nlpunct.o   removeid.o  personfo.o  reguser.o  \
badname.o   loaduser.o  storeuse.o  loguser.o   newuser.o  \
equalid.o   nametone.o  choosena.o  makeaddr.o  getperso.o \
getudict.o  makeuser.o  intoaddr.o  freeuser.o  freeaddr.o \
logfeat.o   makepart.o  freepart.o  copyreg.o   usersize.o \
matchdat.o  moddate.o   delnames.o  nextuser.o  nlfind.o

people.a: $(OBJS)
	rm -f  people.a
	ar cq  people.a  $(OBJS)
	ranlib people.a

clean:
	rm -f *.o

clobber:
	rm -f *.o people.a

.c.o:
	cc -O3 -I../include -c $*.c
