.SUFFIXES:     .c .o

#: CR 10/31/05 Caucus 5 release.

OBJS = \
sqlclass.o

#BG=buildgroups.o sql.a ../str/str.a ../sys1/sys1.a ../gen/gen.a
#RS=runsql.o sql.a ../str/str.a ../gen/gen.a
CAULIB=../str/str.a ../chx/chx.a ../gen/gen.a ../sys1/sys1.a ../sys2/sys2.a


all: sql.a

runsql: runsql.o sql.a
	cc -o runsql runsql.o  sql.a $(CAULIB)  $(SQLLIBS) $(OPTS)

sql.a: $(OBJS)
	rm -f  sql.a
	ar cq  sql.a $(OBJS)
	ranlib sql.a

clean:
	rm -f *.o

clobber:
	rm -f *.o sql.a buildgroups runsql

.c.o:
	cc -O3 -I. -I../include -I$(MYSQLINC) -c $*.c
