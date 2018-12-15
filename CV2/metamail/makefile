# Copyright (c) 1991 Bell Communications Research, Inc. (Bellcore)
# 
# Permission to use, copy, modify, and distribute this material 
# for any purpose and without fee is hereby granted, provided 
# that the above copyright notice and this permission notice 
# appear in all copies, and that the name of Bellcore not be 
# used in advertising or publicity pertaining to this 
# material without the specific, prior written permission 
# of an authorized representative of Bellcore.  BELLCORE 
# MAKES NO REPRESENTATIONS ABOUT THE ACCURACY OR SUITABILITY 
# OF THIS MATERIAL FOR ANY PURPOSE.  IT IS PROVIDED "AS IS", 
# WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES.
#
RM=rm

FOOBAR-sun4=-Bstatic

STATICFLAG=FOOBAR${HOST_ARCH}

CFLAGS = -g -I. -I../include ${$(STATICFLAG)} 

LDLIBS =

CONFIGDIR = .

LOCALCFLAGS= -I${CONFIGDIR} ${CFLAGS}

.c.o:
	$(RM) -f $@
	$(CC) -c  $(LOCALCFLAGS) $*.c

#all: metamail mmencode mailto splitmail
all: mmencode splitmail

codes.o:  codes.c config.h

mailto.o: mailto.c config.h

metamail.o: metamail.c config.h

shared.o:  shared.c config.h

splitmail.o:  shared.c config.h

mmencode: mmencode.o codes.o
	$(RM) -f mmencode
	$(CC) $(LOCALCFLAGS) -o mmencode  mmencode.o  codes.o $(LDLIBS)

mailto: mailto.o codes.o shared.o
	$(RM) -f mailto
	$(CC) $(LOCALCFLAGS) -o mailto  mailto.o  codes.o shared.o -lncurses $(LDLIBS)

splitmail: splitmail.o shared.o
	$(RM) -f splitmail
	$(CC) $(LOCALCFLAGS) -o splitmail  splitmail.o shared.o $(LDLIBS)

metamail:  metamail.o codes.o uue.o shared.o putenv.o
	$(RM) -f metamail
	$(CC) $(LOCALCFLAGS) -o metamail  metamail.o  putenv.o codes.o uue.o shared.o $(LDLIBS)

clobber: clean

clean:
	$(RM) -f metamail mmencode mailto splitmail *.o *.BAK
