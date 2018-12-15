.SUFFIXES:     .c .o

#: CR 10/31/05 Caucus 5 release.

OBJS = \
addxmess.o  chgxresp.o  cusxuser.o  gettruec.o  netargch.o  xfpgloba.o \
addxosub.o  chgxtitl.o  getxphea.o  netoptch.o  xfpprote.o \
auditxac.o  crexport.o  getchn.o    greater.o   netoptse.o  xfuprote.o \
bug.o       cusxattr.o  getcours.o  hnumber.o   putnextx.o \
chgxitem.o  cusxporg.o  getneigh.o  incremen.o  setneigh.o \
chgxpers.o  cusxtext.o  getnextx.o  logx.o      setxphea.o  frzxitem.o \
sendmgr.o   makexpor.o  freexpor.o  makehost.o makehinf.o  freehost.o \
makexact.o  freexact.o  clearxac.o  emptyxpt.o  clearxph.o

LIBS = \
../xaction/xaction.a \
../conf/conf.a     \
../people/people.a \
../inout/inout.a   \
../sys1/sys1.a     \
../sys2/sys2.a     \
../inout/inout.a   \
../unit/unit.a     \
../gen/gen.a       \
../people/people.a \
../resp/resp.a     \
../glyph/glyph.a   \
../sys1/sys1.a     \
../sys2/sys2.a     \
../inout/inout.a   \
../gen/gen.a       \
../unit/unit.a     \
../attach/attach.a \
../chx/chx.a       \
../sys1/sys1.a     \
../sys2/sys2.a     \
../str/str.a       \
../chx/chx.a

all: $(OBJS) xaction.a
	@echo xaction done.

$(OBJS): ../include/xaction.h ../include/sequence.h

xaction.a: $(OBJS)
	rm -f  xaction.a
	ar cq  xaction.a $(OBJS)
	ranlib xaction.a

chn_x: $(OBJS) $(LIBS) chn.o
	cc $(LDFLAGS) $(OPTS) -o chn_x chn.o $(LIBS)

phost: $(OBJS) $(LIBS) phost.o
	cc $(LDFLAGS) $(OPTS) -o phost phost.o $(LIBS)

clean:
	rm -f *.o

clobber:
	rm -f *.o xaction.a chn_x phost

.c.o:
	cc -O3 -I../include -c $*.c
