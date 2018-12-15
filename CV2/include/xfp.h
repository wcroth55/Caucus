/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** XFP.H.  Include file for XFP program and related functions. */

/*: JV  3/02/91 12:15 Created file. */
/*: CR  7/01/91 20:52 Added comments, simplification, spelling. */
/*: JV  7/15/91 18:25 Add MAXRPSIZE */
/*: JV  7/29/91 18:08 Changed some elem's in HOST and HOSTINFO. */
/*: CR  8/08/91 21:52 Add tosstime,reshiptime,keep_chunks to hostinfo_t. */
/*: CR  9/18/91 17:52 Lower MINDISK for the moment. */
/*: JV 10/23/91 16:23 Add net_flag_t. */
/*: JV 12/12/91 11:13 Mention new Neighbor Status String. */
/*: JX  6/03/92 14:24 Chixify. */

#include "handicap.h"
#include "chixuse.h"
#include "textbuf.h"
#include "common.h"

/*** Structures that contain info about other hosts on the Caucus Network. */
/* The structure for another host on the net. */
typedef struct host_t {
   Chix  chn;               /* Caucus Host Name */
   Chix  lhname;            /* Local Host Name (ONLY if different from CHN) */
   int   lhnumber;          /* Local Host Number (only for naybors) */
   struct hostinfo_t *info; /* Detailed info about host if a naybor. */
   struct host_t *neighbor; /* Host that is naybor to go thru to get here */
   struct host_t *next;     /* If this is a naybor, ptr to the "next" one */
} HOST, *HOSTPTR;

/* The structure for detailed info about a neighboring Caucus site. */
typedef struct hostinfo_t {
   Chix  mgrid;           /* CaucusID of Caucus Mgr on other host. */
   int   shipmeth;        /* STD # of shipping method. */
   int4  tosstime;        /* max time to leave a transport file around. */
   int4  reshiptime;      /* time between reshipments. */
   int   shipmeth_urg;    /* STD # of urgent shipping method. */
   short status;          /* Status of connection. */
   int4  keep_chunks;     /* Time to keep chunks of partial transport files. */
   int4  seqnum_to_low;   /* # of next xport file to go to this host. */
   int4  seqnum_to_shipd; /* # of last xport file shipped to this host. */
   int4  seqnum_to_hi;    /* # of next xport file to be created for this host */
   int4  seqnum_from;     /* # of next xport file to get from this host. */
   int4  ackseq;          /* Number of next ACK file to go to this host. */
   int4  seqnum_thru_low; /* # of oldest existing XHXP from host hhh
                             that's going on to other hosts. */
   int4  seqnum_thru_hi;  /* # of next XHXP from host hhh that's going
                             to other hosts. */
} Hostinfo, *Hinfoptr;

/*** Certain generally useful structures. */

/* NET_FLAG_T:  structure for the global command-line storage. */
struct net_flag_t {
   Chix optfile;
};

/*** Certain constants */
#define NEWHEADER   1
#define NOADDHEADER 0

/* Constants for sequence numbers. */
#define LOWER 0
#define UPPER 1
#define BOTH

/* Status of neighboring hosts.  See xaction/xfpgloba.c for strings. */
#define UNKNOWN    0
#define UNATTACHED 1
#define PENDING    2
#define ATTACHED   3
#define DETACHED   4

/*** Sizes of things */
/* #define MINDISK 2048 */  /* Assuming .5K per block, this gives us 1MB. */
                            /* Wrong, it's 1K per block. */

#define MINDISK  500
#define MAXREPARSE 10240  /* Max size of a reparse file, in bytes */
