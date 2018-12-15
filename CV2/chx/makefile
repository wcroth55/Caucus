.SUFFIXES:     .c .o

#: CR 10/31/05 Caucus 5 release.

CHIX = \
ascofchx.o  ascquick.o  chxabr.o    chxalsub.o   \
chxalloc.o  chxclear.o  chxerror.o  chxmem.o     \
chxcheck.o  chxcpy.o    chxfree.o   chxresiz.o   \
chxlen.o    chxcapc.o   chxtype.o   chxcat.o     \
chxscan.o   chxeq.o     chxvalue.o  chxindex.o   \
chxcompa.o  chxcolla.o  chxconc.o   chxcatsu.o   \
chxalter.o  chxcatv.o   chxspec.o   chxtoken.o   \
chxofasc.o  chxquick.o  chxint4.o   chxnum.o     \
jixofeuc.o  jixread.o   eucofjix.o  jixreduc.o   \
jixofsji.o  chxnextl.o  chxbreak.o  chxforma.o   \
chxtrim.o   chxsetva.o  chxrevde.o  jixsingl.o   \
jixwidth.o  sjisofji.o  chxofraw.o  chxsimp.o    \
jixscrsi.o  chxinit.o   memdebug.o  chxtrunc.o   \
chxnextw.o  chxrepla.o  chxsubeq.o  chxrepva.o   \
chxeqf.o    chxchomp.o  chxblank.o  asciifull.o

chx.a: $(CHIX)
	rm -f  chx.a
	ar cq  chx.a $(CHIX)
	ranlib chx.a

clean:
	rm -f *.o

clobber:
	rm -f *.o chx.a

.c.o:
	cc -O3 -I../include -c $*.c
