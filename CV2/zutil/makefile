.SUFFIXES:     .c .o

#: CR  9/11/92 12:31 1st carvmark.
#: CR  9/29/92 11:09 Add scryptor.
#: CI 10/04/92 23:19 Chix integration.
#: CR 10/15/92 18:02 Minor fixes.
#: CR 10/21/92 15:35 Give "all:" something to do.
#: CR  1/04/93 14:05 Add 'clean' and 'clobber' elements.
#: CR  1/13/93 13:55 Modify FLIB.
#: CR  1/14/93 14:54 Add uuenc, uudec.
#: CR  2/02/93 14:25 Add testresp.
#: CK  3/03/93 14:14 Add ux2dos, dos2ux.
#: CN  4/14/93 16:57 Remove dos2ux, ux2dos: they're part of TPX_MAKER.
#: CK  5/13/93 13:24 Fix sys -> sys1,sys2.
#: JV  4/08/93 15:10 Add attach.a.
#: JV  6/06/93 12:48 Add addlen.
#: CP  4/14/92 18:06 sys -> sys1, sys2.
#: CN  5/16/93 16:16 Add fdate.
#: CP  7/14/93 17:17 Make things link.
#: CK  8/05/93 16:45 Add availtest.
#: JV  8/20/93 10:32 Add expuser.
#: JZ 10/14/93 19:36 Consistentize expuser.
#: CK 10/13/93 19:16 Remove xfp/nextuser reference.
#: CR 10/17/93 17:09 Add sys2 to availtest.
#: JZ 10/07/93 16:06 Add dtu (DOS to Unix)
#: CP 11/19/93 16:19 Fix 'dtu' target --> 'dtu_x'.
#: CR 04/10/09 Add forwardCaucusMail, turn off all else.

FLIB = \
../cmd2/help.o \
../main/fireafm.o   \
../xmaint/mainter.o \
../api/api.a        \
../people/people.a  \
../conf/conf.a      \
../chg/chg.a        \
../inout/inout.a \
../gen/gen.a \
../unit/unit.a \
../gen/gen.a \
../db/db.a       \
../people/people.a  \
../api/api.a        \
../resp/resp.a       \
../people/people.a  \
../xaction/xaction.a \
../inout/inout.a \
../unit/unit.a   \
../gen/gen.a     \
../glyph/glyph.a \
../sys1/sys1.a   \
../sys2/sys2.a   \
../inout/inout.a \
../attach/attach.a \
../unit/unit.a   \
../chx/chx.a     \
../sys1/sys1.a   \
../sys2/sys2.a   \
../str/str.a

ALIB= \
../api/api.a     \
../conf/conf.a   \
../chg/chg.a     \
../db/db.a       \
../subj/subj.a   \
../db/db.a       \
../ced/ced.a     \
../xaction/xaction.a \
../gen/gen.a     \
../subj/subj.a   \
../people/people.a \
../chg/chg.a     \
../resp/resp.a   \
../inout/inout.a \
../unit/unit.a   \
../sys1/sys1.a   \
../sys2/sys2.a   \
../db/db.a       \
../api/api.a     \
../attach/attach.a   \
../xaction/xaction.a \
../inout/inout.a \
../gen/gen.a     \
../conf/conf.a   \
../unit/unit.a   \
../glyph/glyph.a \
../str/str.a     \
../chx/chx.a     \
../sys1/sys1.a   \
../sys2/sys2.a

#all: addlen fname  fmaster  merge2  scryptor  uudec  uuenc  testresp  fdate_x \
#     availtest  expusr_x dtu_x holdlock lstest forwardCaucusMail
#	@echo zutil done.

all: forwardCaucusMail useridTimeHasher badCharFilter urlDecode
	@echo zutil done.

addlen: addlen.o $(FLIB) makefile
	cc $(LDFLAGS) -o addlen addlen.o $(FLIB) $(NTLIB) $(OPTS)

availtest: availtest.o ../sys1/sys1.a ../sys2/sys2.a  ../unit/unit.a makefile
	cc $(LDFLAGS) -o availtest availtest.o ../sys1/sys1.a \
           ../sys2/sys2.a ../unit/unit.a $(NTLIB)

lstest: lstest.o ../sys1/sys1.a ../sys2/sys2.a ../unit/unit.a  makefile
	cc $(LDFLAGS) -o lstest lstest.o ../sys1/sys1.a \
           ../sys2/sys2.a ../unit/unit.a ../str/str.a

dtu_x: dtu.o
	cc $(LDFLAGS) -o dtu_x dtu.o

fname: fname.o  $(FLIB) makefile
	cc $(LDFLAGS) -o fname fname.o $(FLIB) $(OPTS)

scryptor: scryptor.o   makefile
	cc $(LDFLAGS) -o scryptor scryptor.o

fmaster: fmaster.o
	cc $(LDFLAGS) -o fmaster fmaster.o \
           ../str/str.a ../sys1/sys1.a $(OPTS)

testresp: testresp.o
	cc $(LDFLAGS) -o testresp testresp.o \
          ../str/str.a ../sys1/sys1.a $(OPTS)

merge2: merge2.o
	cc $(LDFLAGS) -o merge2 merge2.o

forwardCaucusMail: forwardCaucusMail.o
	cc $(LDFLAGS) -o forwardCaucusMail forwardCaucusMail.o

useridTimeHasher: useridTimeHasher.o
	cc -lssl $(LDFLAGS) -o useridTimeHasher useridTimeHasher.o \
       ../str/str.a ../sys1/sys1.a 

badCharFilter: badCharFilter.o
	cc -lssl $(LDFLAGS) -o badCharFilter badCharFilter.o

urlDecode: urlDecode.o
	cc -lssl $(LDFLAGS) -o urlDecode urlDecode.o

uudec: uudec.o
	cc $(LDFLAGS) -o uudec uudec.o

uuenc: uuenc.o
	cc $(LDFLAGS) -o uuenc uuenc.o

expusr_x: expuser.o
	cc $(LDFLAGS) -o expusr_x expuser.o  \
            ../api/api.a $(FLIB) $(OPTS)

fdate_x: fdate.o makefile $(ALIB)
	cc $(LDFLAGS) -o fdate_x fdate.o $(ALIB) $(NTLIB) $(OPTS)

holdlock: holdlock.o makefile
	cc $(LDFLAGS) -s -o holdlock holdlock.o  $(ALIB) $(OPTS)

clean:
	rm -f *.o

clobber:
	rm -f *.o fname scryptor fmaster merge2 uuenc uudec availtest expusr_x
	rm -f     addlen testresp fdate_x holdlock lstest

.c.o:
	cc $(CCFLAGS) -I../include -c $*.c
