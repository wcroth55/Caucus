.SUFFIXES:     .c .o

#: CR 10/31/05 Caucus 5 release.

OBJS = \
amakinit.o  afreinit.o  ainit.o     aexit.o     sysainit.o  acache.o \
adebug.o    adicstr.o   \
amakconf.o  afreconf.o  agetconf.o  achgconf.o \
amaktext.o  afretext.o  agettext.o  \
amakcsta.o  afrecsta.o  agetcsta.o  \
amakpers.o  afrepers.o  agetpers.o  aaddpers.o  achgpers.o  adelpers.o \
amakpsta.o  afrepsta.o  agetpsta.o  achgpsta.o  aaddpsta.o  \
amakresp.o  afreresp.o  agetresp.o  adelresp.o  aaddresp.o  achgresp.o \
amakpeop.o  afrepeop.o  agetpeop.o  \
amakrlis.o  afrerlis.o  \
amakname.o  afrename.o  agetname.o  achgname.o  \
amaksubj.o  afresubj.o  agetsubj.o  aaddsubj.o  adelsubj.o  \
acchtbuf.o  acchmast.o  \
modaddps.o  modviewp.o  \
timeofda.o  timeofho.o  \
andrlist.o  rlofsele.o  printrl.o   addtorl.o   rbound.o    rlall.o \
orrlist.o   \
agetvar.o   achgvar.o   amakvar.o   afrevar.o \
oneresp.o   laston.o \
amakvart.o  afrevart.o  agetvart.o  achgvart.o \
vartfind.o  vartadd.o   vartis.o    alispsta.o

api.a: $(OBJS)
	rm -f  api.a
	ar cq  api.a $(OBJS)
	ranlib api.a

clean:
	rm -f *.o

clobber:
	rm -f *.o api.a

.c.o:
	cc -O3 -I../include -c $*.c
